// =============================================================================
// ProbingHashTable — Linear Probing (Closed Hashing / Open Addressing)
// =============================================================================
//
// - Scenario: same grade book, different strategy
// - Instead of chains, every entry lives directly in the array
// - When a collision occurs, we probe FORWARD one slot at a time
//   until we find an open slot
// - This file contains TODOs 9-15 for the probing portion of CT10
//

#include "ProbingHashTable.h"
#include <iostream>

// =============================================================================
// Helper — next_prime  (given — not a TODO)
// =============================================================================
int ProbingHashTable::next_prime(int n) {
    if (n <= 2) return 2;
    if (n % 2 == 0) ++n;
    while (true) {
        bool is_prime = true;
        for (int i = 3; i * i <= n; i += 2) {
            if (n % i == 0) { is_prime = false; break; }
        }
        if (is_prime) return n;
        n += 2;
    }
}

// ---------------------------------------------------------------------------
// 9. Constructor
// ---------------------------------------------------------------------------
//
// ! DISCUSSION: The probing table is a flat array of HashSlot structs.
//   - each slot has three fields: key, value, and status
//   - status starts as EMPTY for every slot — no chains, no pointers
//   - default-initializing HashSlot sets status to SlotStatus::EMPTY
//   - contrast with chaining: no pointer-to-pointer, no linked list nodes
//
ProbingHashTable::ProbingHashTable(int capacity)
    : size_(0), capacity_(capacity) {
    table_ = new HashSlot[capacity_];       // default-init → all EMPTY
}

// ---------------------------------------------------------------------------
// 10. insert() — linear probing to find an open slot
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/probing/linear_probe_insert.png — scanning forward on collision
//
// ! DISCUSSION: When the target slot is occupied, probe forward.
//   - start at hash(key), then try (index + 1) % capacity_, and so on
//   - stop when we find EMPTY, DELETED, or a matching key
//   - if the key already exists (OCCUPIED with same key): update the value
//   - if we find EMPTY or DELETED: insert the new entry there
//   - DELETED slots can be reused — this reclaims tombstoned slots
//   - after inserting, check load factor and resize if needed
//   - quadratic probing uses (index + i*i) instead of (index + 1) —
//     reduces clustering but is harder to guarantee visiting all slots
//
void ProbingHashTable::insert(const std::string& key, int value) {
    // Resize BEFORE inserting if we're already at the threshold
    if (static_cast<double>(size_ + 1) / capacity_ > MAX_LOAD_FACTOR) {
        resize();
    }

    size_t index = hash(key);

    // Probe forward until we find a place to insert
    for (int i = 0; i < capacity_; ++i) {
        size_t probe = (index + i) % capacity_;             // linear probing
        // size_t probe = (index + i * i) % capacity_;      // quadratic probing

        if (table_[probe].status == SlotStatus::OCCUPIED &&
            table_[probe].key == key) {
            table_[probe].value = value;    // update existing key
            return;
        }

        if (table_[probe].status == SlotStatus::EMPTY ||
            table_[probe].status == SlotStatus::DELETED) {
            table_[probe].key    = key;
            table_[probe].value  = value;
            table_[probe].status = SlotStatus::OCCUPIED;
            ++size_;
            return;
        }
    }
    // Should never reach here if load factor is managed properly
}

// ---------------------------------------------------------------------------
// 11. search() — linear probing to find a key
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/probing/probe_search.png — skip DELETED, stop at EMPTY
//
// ! DISCUSSION: Search probes the same way as insert.
//   - start at hash(key) and probe forward
//   - OCCUPIED with matching key → found it, return pointer to value
//   - DELETED → skip it, keep probing (the key might be further ahead)
//   - EMPTY → stop, the key was never inserted past this point
//   - this is WHY tombstones exist: if we cleared a slot to EMPTY instead
//     of DELETED, searches for keys that probed past it would fail
//
int* ProbingHashTable::search(const std::string& key) const {
    size_t index = hash(key);

    for (int i = 0; i < capacity_; ++i) {
        size_t probe = (index + i) % capacity_;             // linear probing
        // size_t probe = (index + i * i) % capacity_;      // quadratic probing

        if (table_[probe].status == SlotStatus::EMPTY) {
            return nullptr;                 // hit an empty slot — key not here
        }

        if (table_[probe].status == SlotStatus::OCCUPIED &&
            table_[probe].key == key) {
            return &table_[probe].value;    // found it
        }
        // DELETED or non-matching OCCUPIED → keep probing
    }
    return nullptr;                         // probed entire table, not found
}

// ---------------------------------------------------------------------------
// 12. remove() — tombstone deletion
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/probing/tombstone_delete.png — DELETED vs EMPTY
//
// ! DISCUSSION: We CANNOT just mark a removed slot as EMPTY.
//   - example: Alice hashes to slot 4, Diana also hashes to 4
//   - Diana probes to slot 5 (4 was taken by Alice)
//   - if we remove Alice and mark slot 4 as EMPTY...
//   - searching for Diana starts at slot 4, sees EMPTY, concludes "not found"
//   - but Diana IS in the table at slot 5!
//   - DELETED (tombstone) means "something WAS here — keep probing"
//   - insert() can reuse DELETED slots, so they don't waste space forever
//   - resize() clears all tombstones (only rehashes OCCUPIED entries)
//
bool ProbingHashTable::remove(const std::string& key) {
    size_t index = hash(key);

    for (int i = 0; i < capacity_; ++i) {
        size_t probe = (index + i) % capacity_;             // linear probing
        // size_t probe = (index + i * i) % capacity_;      // quadratic probing

        if (table_[probe].status == SlotStatus::EMPTY) {
            return false;                   // key not in table
        }

        if (table_[probe].status == SlotStatus::OCCUPIED &&
            table_[probe].key == key) {
            table_[probe].status = SlotStatus::DELETED;     // tombstone!
            --size_;
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// 13. resize() — grow the table and rehash (skip tombstones)
// ---------------------------------------------------------------------------
//
// ! DISCUSSION: Resize cleans up tombstones and reduces clustering.
//   - allocate a new, larger table (next prime after doubling)
//   - rehash only OCCUPIED entries — DELETED entries are discarded
//   - this is why resize "cleans up" tombstones — they simply aren't copied
//   - after resize, every entry gets a fresh probe sequence with more room
//
void ProbingHashTable::resize() {
    int old_capacity = capacity_;
    HashSlot* old_table = table_;

    capacity_ = next_prime(old_capacity * 2);
    table_ = new HashSlot[capacity_];       // all slots default to EMPTY
    size_ = 0;                              // insert() will re-increment

    // Rehash only OCCUPIED entries (skip EMPTY and DELETED)
    for (int i = 0; i < old_capacity; ++i) {
        if (old_table[i].status == SlotStatus::OCCUPIED) {
            insert(old_table[i].key, old_table[i].value);
        }
    }
    delete[] old_table;
}

// ---------------------------------------------------------------------------
// 14. Destructor
// ---------------------------------------------------------------------------
//
// ! DISCUSSION: Much simpler than chaining — just delete the array.
//   - no chains to walk, no individual nodes to free
//   - delete[] handles the entire flat array in one call
//   - contrast with ChainingHashTable's nested loop destructor
//
ProbingHashTable::~ProbingHashTable() {
    delete[] table_;
}

// ---------------------------------------------------------------------------
// 15. hash() — same algorithm as ChainingHashTable
// ---------------------------------------------------------------------------
//
// ! DISCUSSION: Both tables use the same hash algorithm.
//   - the hash function itself is independent of the collision strategy
//   - only the modulo divisor changes (each table has its own capacity_)
//   - chaining and probing differ in what happens AFTER the hash — not during
//
size_t ProbingHashTable::hash(const std::string& key) const {
    size_t hash_value = 0;
    for (char c : key) {
        hash_value = hash_value * 31 + c;
    }
    return hash_value % capacity_;
}

// ---------------------------------------------------------------------------
// 16. load_factor()
// ---------------------------------------------------------------------------
//
// ! DISCUSSION: Probing tables must resize MUCH earlier than chaining tables.
//   - chaining can tolerate load factor > 1.0 (chains just get longer)
//   - probing must stay well below 1.0 — we use 0.75 as the threshold
//   - as load factor approaches 1.0, probe sequences get very long (clustering)
//   - at load factor 1.0, the table is completely full — insert loops forever
//
double ProbingHashTable::load_factor() const {
    return static_cast<double>(size_) / capacity_;
}

// ---------------------------------------------------------------------------
// 17. print() — display each slot with its status
// ---------------------------------------------------------------------------
//
// ! DISCUSSION: Print shows the three possible slot states.
//   - OCCUPIED → print the key and value
//   - EMPTY    → print [empty] (never been used)
//   - DELETED  → print [deleted] (tombstone from a removal)
//   - this helps visualize clustering and probe sequences
//
void ProbingHashTable::print() const {
    for (int i = 0; i < capacity_; ++i) {
        std::cout << "  [" << i << "]: ";
        switch (table_[i].status) {
            case SlotStatus::OCCUPIED:
                std::cout << "(" << table_[i].key
                          << ", " << table_[i].value << ")";
                break;
            case SlotStatus::DELETED:
                std::cout << "[deleted]";
                break;
            case SlotStatus::EMPTY:
            default:
                std::cout << "[empty]";
                break;
        }
        std::cout << "\n";
    }
}

#pragma once

#include <string>

// ---------------------------------------------------------------------------
// SlotStatus — tracks the state of each slot in a probing hash table
// ---------------------------------------------------------------------------
//
// - EMPTY    — slot has never been used; safe to stop probing here
// - OCCUPIED — slot holds a live key-value pair
// - DELETED  — slot was occupied but has been removed (tombstone);
//              probing must skip past these, but insert may reuse them
//
enum class SlotStatus { EMPTY, OCCUPIED, DELETED };

// ---------------------------------------------------------------------------
// HashSlot — one slot in the probing table (key + value + status)
// ---------------------------------------------------------------------------
struct HashSlot {
    std::string key;
    int value    = 0;
    SlotStatus status = SlotStatus::EMPTY;
};

// ---------------------------------------------------------------------------
// ProbingHashTable — closed hashing / open addressing (linear probing)
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/probing/probing_hash_table.png — flat slot array with status flags
//
// - Resolves collisions by scanning forward in the array one slot at a time
//   until an open slot is found
// - All data lives inside the array itself — no linked lists, no extra
//   heap allocations per entry
//
// Key design decisions:
//   - std::string keys, int values (same as ChainingHashTable)
//   - Same custom hash function (multiply-and-add with modulo)
//   - Linear probing: next = (index + 1) % capacity_
//   - Tombstone deletion (SlotStatus::DELETED) so probe sequences aren't broken
//   - MAX_LOAD_FACTOR = 0.75 — must stay well below 1.0 to avoid clustering
//
class ProbingHashTable {
public:
    explicit ProbingHashTable(int capacity = 7);    // 7 is prime — reduces hash
                                                    // collisions vs a round number
                                                    // like 8 or 10 (see hash())
    ~ProbingHashTable();

    // Rule of 5: no copy or move
    ProbingHashTable(const ProbingHashTable&) = delete;
    ProbingHashTable& operator=(const ProbingHashTable&) = delete;
    ProbingHashTable(ProbingHashTable&&) = delete;
    ProbingHashTable& operator=(ProbingHashTable&&) = delete;

    void insert(const std::string& key, int value);
    int* search(const std::string& key) const;
    bool remove(const std::string& key);

    size_t hash(const std::string& key) const;
    double load_factor() const;
    void resize();
    void print() const;

    int size() const { return size_; }
    int capacity() const { return capacity_; }
    bool is_empty() const { return size_ == 0; }

private:
    HashSlot* table_;           // flat array of slots
    int size_;                  // number of OCCUPIED slots
    int capacity_;              // total number of slots

    static constexpr double MAX_LOAD_FACTOR = 0.75;
    static int next_prime(int n);
};

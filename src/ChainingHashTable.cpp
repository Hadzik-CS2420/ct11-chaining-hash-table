// =============================================================================
// ChainingHashTable — Separate Chaining (Open Hashing)
// =============================================================================
//
// - Scenario: a professor's grade book
// - Student names are the KEYS, integer grades are the VALUES
// - When two names hash to the same bucket, the entries form a
//   linked-list chain at that index
// - This file contains TODOs 1-9 for CT10
//

#include "ChainingHashTable.h"
#include <iostream>

// =============================================================================
// Helper — next_prime
// =============================================================================
//
// - Returns the smallest prime >= n
// - Used by resize() to pick a new capacity that reduces clustering
//
int ChainingHashTable::next_prime(int n) {
    if (n <= 2) return 2;
    if (n % 2 == 0) ++n;            // start with an odd number
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
// 1. Constructor
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/bucket_array_init.png — empty bucket array, all nullptr
// ? SEE DIAGRAM: images/chaining_hash_table.png — bucket array with chains
//
// ! DISCUSSION: The bucket array — new ChainNode*[capacity_]()
//   - new allocates an array on the heap and returns a pointer to it (ChainNode**)
//   - ChainNode* — each element is a pointer (head of a chain)
//   - [capacity_] — creates one slot per bucket (capacity_ = 7)
//   - () — zero-initializes every element to nullptr
//
// ? SEE DIAGRAM: images/prime_capacity.png — even vs. prime capacity distribution
//
// ! DISCUSSION: Why start with a PRIME capacity (7)?
//   - prime has no common factors with any hash value, so modulo spreads evenly
//   - common choices: 7, 17, 37, 97 — resize() always picks the next prime
//
ChainingHashTable::ChainingHashTable(int capacity)
    : size_(0), capacity_(capacity) {
    buckets_ = new ChainNode*[capacity_]();     // () zero-inits to nullptr
}

// ---------------------------------------------------------------------------
// 2. Destructor
// ---------------------------------------------------------------------------
//
// ! DISCUSSION: We must delete every node in every chain, then the array.
//   - same temp-pointer pattern from Module 4 (SinglyLinkedList destructor)
//   - outer loop walks each bucket; inner loop walks the chain
//   - after all chains are deleted, delete[] the bucket array itself
//
ChainingHashTable::~ChainingHashTable() {
    for (int i = 0; i < capacity_; ++i) {
        ChainNode* current = buckets_[i];
        while (current != nullptr) {
            ChainNode* temp = current;
            current = current->next;
            delete temp;
        }
    }
    delete[] buckets_;
}

// ---------------------------------------------------------------------------
// 3. hash() — custom hash function
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/multiply_and_add.png — multiply-and-add process
//
// ! DISCUSSION: Two steps — see the diagram for the full walkthrough.
//   - Step 1 (multiply-and-add): loop through each character, doing
//     hash_value = hash_value * 31 + char — builds a large number unique to this key
//   - Step 2 (modulo): hash_value % capacity_ maps that number into [0, capacity_)
//   - resizing invalidates old indices — different capacity means different modulo
//
size_t ChainingHashTable::hash(const std::string& key) const {
    // Step 1: multiply-and-add — build a large number from the key
    size_t hash_value = 0;
    for (char c : key) {
        hash_value = hash_value * 31 + c;       // ? each char shifts and adds
    }
    // Step 2: modulo — map into a valid bucket index [0, capacity_)
    return hash_value % capacity_;
}

// ---------------------------------------------------------------------------
// 4. insert() — add or update a key-value pair
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/chaining_insert.png — two cases: update or prepend
// ? SEE DIAGRAM: images/chaining_prepend.png — color-coded breakdown of the prepend one-liner
//
// ! DISCUSSION: Two cases — update existing key or prepend new node.
//   - hash the key, walk the chain, then update or prepend (see diagrams)
//
void ChainingHashTable::insert(const std::string& key, int value) {
    size_t index = hash(key);

    // ── Case 1: UPDATE — walk the chain looking for a matching key ──
    ChainNode* current = buckets_[index];
    while (current != nullptr) {
        if (current->key == key) {
            current->value = value;         // key exists — update its value
            return;                         // done, no new node needed
        }
        current = current->next;
    }

    // ── Case 2: PREPEND — key not found, add new node at head of chain ──
    buckets_[index] = new ChainNode(key, value, buckets_[index]);
    ++size_;

    // Resize if load factor exceeds threshold
    if (load_factor() > MAX_LOAD_FACTOR) {
        resize();
    }
}

// ---------------------------------------------------------------------------
// 5. search() — find a value by key
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/chaining_search.png — found vs. not found
//
// ! DISCUSSION: Search follows the same two-step pattern as insert.
//   - hash the key to find the bucket index
//   - walk the chain comparing keys
//   - if found: return a pointer to the node's value (caller can read it)
//   - if not found: return nullptr
//   - average case O(1) when load factor is low (short chains)
//   - worst case O(n) if every key hashes to the same bucket (one long chain)
//
int* ChainingHashTable::search(const std::string& key) const {
    size_t index = hash(key);

    ChainNode* current = buckets_[index];
    while (current != nullptr) {
        if (current->key == key) {
            return &current->value;  // return int* (address of the value)
        }
        current = current->next;
    }
    return nullptr;                         // key not in table
}

// ---------------------------------------------------------------------------
// 6. remove() — delete a key from its chain
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/chaining_remove_head.png — case 1: key at head
// ? SEE DIAGRAM: images/chaining_remove_middle.png — case 2: key in middle/tail
// ? SEE DIAGRAM: images/chaining_remove_notfound.png — case 3: key not found
//
// ! DISCUSSION: Remove uses the trailing-pointer pattern from CT8.
//   - hash the key to find the bucket, then walk with current and prev
//   - three cases:
//     1. key is at the HEAD of the chain → update buckets_[index]
//     2. key is in the MIDDLE or TAIL → prev->next skips over current
//     3. key is NOT FOUND → return false, do nothing
//   - after unlinking, delete the node and decrement size_
//
bool ChainingHashTable::remove(const std::string& key) {
    size_t index = hash(key);

    ChainNode* current = buckets_[index];
    ChainNode* prev = nullptr;

    while (current != nullptr) {
        if (current->key == key) {
            if (prev == nullptr) {
                buckets_[index] = current->next;    // remove head of chain
            } else {
                prev->next = current->next;         // unlink from middle/tail
            }
            delete current;
            --size_;
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false;                           // key not found
}

// ---------------------------------------------------------------------------
// 7. load_factor()
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/load_factor_resize.png — before/after rehash
//
// ! DISCUSSION: load_factor = size / capacity (cast to double!).
//   - for chaining, load factor CAN exceed 1.0 (chains can be any length)
//   - we resize when it exceeds MAX_LOAD_FACTOR (1.0 = average 1 entry/bucket)
//
double ChainingHashTable::load_factor() const {
    return static_cast<double>(size_) / capacity_;
}

// ---------------------------------------------------------------------------
// 8. resize()
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/load_factor_resize.png — before/after rehash
//
// ! DISCUSSION: Resize is O(n) — every entry must be rehashed (see diagram).
//
void ChainingHashTable::resize() {
    int old_capacity = capacity_;            // save old size before changing
    ChainNode** old_buckets = buckets_;      // save pointer to old array

    capacity_ = next_prime(old_capacity * 2); // new capacity ≈ 2x, always prime
    buckets_ = new ChainNode*[capacity_]();   // allocate new empty bucket array
    size_ = 0;                                // reset — insert() will re-increment

    // Walk every chain in the old table and rehash each entry
    for (int i = 0; i < old_capacity; ++i) {
        ChainNode* current = old_buckets[i];  // head of old chain at bucket i
        while (current != nullptr) {
            insert(current->key, current->value); // rehash with new capacity_
            ChainNode* temp = current;            // save before advancing
            current = current->next;              // advance to next node
            delete temp;                          // free the old node
        }
    }
    delete[] old_buckets;                     // free the old bucket array
}

// ---------------------------------------------------------------------------
// 9. print() — display each bucket's chain
// ---------------------------------------------------------------------------
//
// ! DISCUSSION: print() reveals the internal structure of the hash table.
//   - shows which keys landed in which bucket
//   - collisions are visible as multi-node chains
//   - helps students verify their hash function distributes keys well
//
void ChainingHashTable::print() const {
    for (int i = 0; i < capacity_; ++i) {
        std::cout << "  [" << i << "]: ";
        ChainNode* current = buckets_[i];
        if (current == nullptr) {
            std::cout << "empty";
        }
        while (current != nullptr) {
            std::cout << "(" << current->key << ", " << current->value << ")";
            if (current->next != nullptr) {
                std::cout << " -> ";
            }
            current = current->next;
        }
        std::cout << "\n";
    }
}

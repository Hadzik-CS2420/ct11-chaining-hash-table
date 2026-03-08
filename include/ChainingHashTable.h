#pragma once

#include <string>
#include "ChainNode.h"

// ---------------------------------------------------------------------------
// ChainingHashTable — open hashing / separate chaining
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/chaining/bucket_chain.png — one bucket is just a linked list
// ? SEE DIAGRAM: images/chaining/chaining_hash_table.png — full bucket array with chains
//
// - Resolves collisions by storing a linked list (chain) at each bucket
// - When two keys hash to the same index, the new entry is prepended to
//   that bucket's chain — O(1), same as push_front on a linked list
//
// Key design decisions:
//   - std::string keys, int values (simple key-value pairs)
//   - Custom hash function (multiply-and-add with modulo)
//   - Prime table capacity to reduce clustering
//   - MAX_LOAD_FACTOR = 1.0 — resize when average chain length exceeds 1
//
class ChainingHashTable {
public:
    explicit ChainingHashTable(int capacity = 7);   // 7 is prime — reduces hash
                                                    // collisions vs a round number
                                                    // like 8 or 10 (see hash())
    ~ChainingHashTable();

    // Rule of 5: no copy or move (dynamic resource, not needed for this CT)
    ChainingHashTable(const ChainingHashTable&) = delete;
    ChainingHashTable& operator=(const ChainingHashTable&) = delete;
    ChainingHashTable(ChainingHashTable&&) = delete;
    ChainingHashTable& operator=(ChainingHashTable&&) = delete;

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
    ChainNode** buckets_;       // array of chain head pointers
    int size_;                  // total number of entries across all buckets
    int capacity_;              // number of buckets

    static constexpr double MAX_LOAD_FACTOR = 1.0;
    static int next_prime(int n);
};

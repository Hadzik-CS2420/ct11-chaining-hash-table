#pragma once

#include <string>
#include "ChainNode.h"

// ---------------------------------------------------------------------------
// ChainingHashTable — open hashing / separate chaining
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/bucket_chain.png — one bucket is just a linked list
// ? SEE DIAGRAM: images/chaining_hash_table.png — full bucket array with chains
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

    void insert(const std::string& key, int value);  // add or update a key-value pair
    int* search(const std::string& key) const;        // find value by key (nullptr if missing)
    bool remove(const std::string& key);              // delete a key-value pair from the table

    // ! DISCUSSION: const after the parentheses — a promise not to modify the object.
    //   - search, hash, load_factor, print are const — they only READ data members
    //   - insert, remove, resize are NOT const — they CHANGE size_, buckets_, or capacity_
    //   - the compiler enforces this: a const method cannot modify any data member

    // ! DISCUSSION: size_t — an unsigned integer type for sizes and indices.
    //   - guaranteed large enough to hold any array index or object size
    //   - unsigned means it can never be negative (0 to ~18 quintillion on 64-bit)
    //   - we use it here because hash values should never be negative
    //   - you'll see size_t throughout the C++ standard library (e.g. std::string::size())
    //
    size_t hash(const std::string& key) const;        // convert key to bucket index [0, capacity_)
    double load_factor() const;                       // size_ / capacity_ — average chain length
    void resize();                                    // grow to next prime capacity and rehash all entries
    void print() const;                               // display each bucket's chain for debugging

    int size() const { return size_; }                // total entries across all buckets
    int capacity() const { return capacity_; }        // number of buckets in the array
    bool is_empty() const { return size_ == 0; }      // true when no entries stored

private:
    // ! DISCUSSION: An array of linked list heads (one per bucket).
    ChainNode** buckets_;       // array of chain head pointers
    int size_;                  // total number of entries across all buckets
    int capacity_;              // number of buckets

    // ? SEE DIAGRAM: images/load_factor_resize.png — before/after resize
    // ? load factor = size_ / capacity_ — triggers resize when exceeded
    static constexpr double MAX_LOAD_FACTOR = 1.0;
    static int next_prime(int n);
};

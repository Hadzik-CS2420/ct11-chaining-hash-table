// =============================================================================
// ChainingHashTable — Separate Chaining (Open Hashing)
// =============================================================================
//
// - Scenario: a professor's grade book
// - Student names are the KEYS, integer grades are the VALUES
// - When two names hash to the same bucket, the entries form a
//   linked-list chain at that index
// - This file contains TODOs 1-9 for the chaining portion of CT10
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
    // TODO: Allocate the bucket array on the heap using new ChainNode*[capacity_]()
    //       The () at the end zero-initializes every element to nullptr

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
    // TODO: For each bucket (0 to capacity_-1):
    //         Set current to buckets_[i]
    //         While current is not nullptr:
    //           Save current in a temp pointer
    //           Advance current to current->next
    //           Delete temp

    // TODO: delete[] the bucket array itself

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
    // TODO: Create a size_t variable called hash_value, initialized to 0

    // TODO: For each char c in key:
    //         hash_value = hash_value * 31 + c

    // TODO: Return hash_value % capacity_

    return 0; // placeholder — remove when done
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
    // TODO: Hash the key to get the bucket index

    // ── Case 1: UPDATE — walk the chain looking for a matching key ──
    // TODO: Set current to buckets_[index]
    // TODO: While current is not nullptr:
    //         If current->key == key:
    //           Update current->value and return
    //         Advance current to current->next

    // ── Case 2: PREPEND — key not found, add new node at head of chain ──
    // TODO: buckets_[index] = new ChainNode(key, value, buckets_[index]);
    // TODO: Increment size_

    // TODO: If load_factor() > MAX_LOAD_FACTOR, call resize()

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
    // TODO: Hash the key to get the bucket index

    // TODO: Set current to buckets_[index]
    // TODO: While current is not nullptr:
    //         If current->key == key:
    //           return &current->value  (return int* — address of the value)
    //         Advance current to current->next

    // TODO: Return nullptr — key not found

    return nullptr; // placeholder — remove when done
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
    // TODO: Hash the key to get the bucket index

    // TODO: Create current = buckets_[index] and prev = nullptr

    // TODO: While current is not nullptr:
    //         If current->key == key:
    //           If prev == nullptr:
    //             buckets_[index] = current->next  (remove head)
    //           Else:
    //             prev->next = current->next        (unlink middle/tail)
    //           Delete current, decrement size_, return true
    //         Advance: prev = current, current = current->next

    // TODO: Return false — key not found

    return false; // placeholder — remove when done
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
    // TODO: Return size_ / capacity_ as a double
    //       Hint: use static_cast<double>(size_) to avoid integer division

    return 0.0; // placeholder — remove when done
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
    // TODO: Save old_capacity and old_buckets (we need them to rehash)

    // TODO: Set capacity_ to next_prime(old_capacity * 2)
    // TODO: Allocate a new empty bucket array: new ChainNode*[capacity_]()
    // TODO: Reset size_ to 0 (insert() will re-increment as we rehash)

    // TODO: For each bucket in the OLD table (0 to old_capacity-1):
    //         Walk the chain:
    //           Call insert(current->key, current->value) to rehash
    //           Save current in temp, advance current, delete temp

    // TODO: delete[] old_buckets

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
    // TODO: For each bucket (0 to capacity_-1):
    //         Print "[i]: "
    //         If the bucket is nullptr, print "empty"
    //         Otherwise walk the chain, printing each (key, value) pair
    //         Print a newline

}

#pragma once

#include <string>

// ---------------------------------------------------------------------------
// ChainNode — one link in a separate-chaining bucket
// ---------------------------------------------------------------------------
//
// ? SEE DIAGRAM: images/hash_table_overview.png — big picture: key → hash → index → bucket
// ? SEE DIAGRAM: images/chain_node_structure.png — key/value/next fields
//
// - Each bucket in a ChainingHashTable is a singly linked list of ChainNodes
// - Every node stores a key-value pair and a pointer to the next node
//   in the chain (nullptr if it is the last node)
//
struct ChainNode {
    std::string key;        // the lookup key (e.g. student name)
    int value;              // the stored data (e.g. grade)
    ChainNode* next;        // pointer to the next node in this chain
                            // - a pointer (not a copy) because we need to
                            //   link to an existing node on the heap
                            // - nullptr means this is the last node in the chain

    ChainNode(const std::string& k, int v, ChainNode* n = nullptr)
        : key(k), value(v), next(n) {}
};

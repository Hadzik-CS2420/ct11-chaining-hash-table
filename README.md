# CT10 — Hash Tables

## Overview

An in-class code-together activity introducing hash tables — the first data structure that maps **keys to values** using a hash function. In Part 1, students build a **ChainingHashTable** that resolves collisions by storing linked-list chains at each bucket. In Part 2, students build a **ProbingHashTable** that resolves collisions by scanning forward (linear probing) and uses tombstone deletion. Both parts cover load factor monitoring and resizing with rehashing. The scenario follows a professor's grade book: student names are keys, integer grades are values.

## Learning Objectives

- Explain the **purpose of hash tables**: O(1) average-case insert, search, and delete via key-to-index mapping
- Implement a **custom hash function** that converts string keys to integer indices using character values and modulo
- Implement **separate chaining (open hashing)**: each bucket holds a linked list of key-value pairs
- Implement **linear probing (closed hashing)**: collisions resolved by scanning to the next open slot
- Explain why **tombstone deletion** is required for probing — removing a slot without marking it would break probe sequences
- Calculate **load factor** (`size / capacity`) and trigger a **resize + rehash** when it exceeds a threshold

## Files

| File | Focus | TODOs |
|---|---|---|
| `ChainingHashTable.cpp` | Hash function, insert, search, remove with linked-list chains, resize | 8 |
| `ProbingHashTable.cpp` | Linear probing insert/search, tombstone remove, resize | 9 |
| `main.cpp` | Grade book demo: chaining table (Part 1) + probing table (Part 2) | 8 |

## Supporting Files

| File | Purpose |
|---|---|
| `ChainNode.h` | `ChainNode` struct (`std::string key`, `int value`, `ChainNode* next`) — nodes for chaining buckets |
| `ChainingHashTable.h` | Class with `ChainNode**` bucket array, `size_`, `capacity_` |
| `ProbingHashTable.h` | `HashSlot` struct (key + value + `SlotStatus`), `SlotStatus` enum (`EMPTY`, `OCCUPIED`, `DELETED`), class with `HashSlot*` array |

## Teaching Order

### 1. `ChainingHashTable.cpp` — Separate chaining (8 TODOs)

Start with a diagram showing an array of bucket pointers. Walk through what happens when two keys hash to the same index.

1. **Constructor** — allocate a `ChainNode**` array of size `capacity_`; initialize every bucket to `nullptr`
2. **Destructor** — walk each bucket's chain deleting every node; same temp-pointer pattern as singly linked list
3. **hash()** — iterate through each character of the key, accumulate using multiply-and-add (×31), return `total % capacity_`; discuss why prime capacities reduce clustering
4. **insert()** — hash the key; walk the chain to check for duplicates (update value if found); otherwise prepend a new `ChainNode`; increment `size_`; check load factor and resize if needed
5. **search()** — hash the key; walk the chain comparing keys; return pointer to value if found, `nullptr` otherwise
6. **remove()** — hash the key; walk the chain with a trailing pointer; unlink and delete the matching node; decrement `size_`
7. **load_factor() and resize()** — `load_factor` returns `size_ / capacity_` as a double; `resize` picks a larger prime capacity, allocates a new bucket array, rehashes every entry from the old table, deletes old chains
8. **print()** — loop through each bucket index; print the chain contents or "empty"

### 2. `ProbingHashTable.cpp` — Linear probing (9 TODOs)

Start with a diagram showing a flat array of `HashSlot` structs. Walk through what happens when a collision occurs — scan forward until an open slot is found.

1. **Constructor** — allocate `HashSlot` array; default initialization sets every slot to `EMPTY`
2. **insert()** — hash the key; probe forward (`(index + 1) % capacity_`) until finding `EMPTY`, `DELETED`, or a matching key; insert or update; increment `size_`; resize before inserting if load factor would exceed threshold
3. **search()** — hash the key; probe forward skipping `DELETED` slots; stop at `EMPTY` (key not found) or matching key
4. **remove()** — search for the key; set its status to `DELETED` (tombstone); decrement `size_`; explain why you can't just mark it `EMPTY`
5. **resize()** — allocate new `HashSlot` array with larger prime capacity; rehash only `OCCUPIED` entries (skip `DELETED`); replace old array
6. **Destructor** — `delete[]` the slot array (much simpler than chaining — no chains to walk)
7. **hash()** — same multiply-and-add algorithm as ChainingHashTable
8. **load_factor()** — same formula, but probing uses a lower threshold (0.75 vs 1.0)
9. **print()** — loop through each slot; print the entry, `[empty]`, or `[deleted]`

### 3. `main.cpp` — Professor's Grade Book (8 TODOs)

1. **Part 1 — Insert students** — add 5 student names with grades into the chaining table
2. **Part 1 — Print** — display the bucket layout; notice Alice and Diana collide at bucket 4
3. **Part 1 — Search** — look up an existing student (Charlie) and a missing one (Frank)
4. **Part 1 — Update and remove** — update Bob's grade via insert; remove Diana (dropped the class)
5. **Part 2 — Insert students** — add the same 5 students into the probing table
6. **Part 2 — Print and search** — display the slot layout; notice clustering at slots 4-6
7. **Part 2 — Tombstone demo** — remove Diana (slot 5 → `[deleted]`), then search for Eve (probes past tombstone to slot 6)
8. **Part 2 — Resize demo** — insert Frank and Grace to trigger resize; capacity jumps from 7 to 17, tombstones cleared

## CCD Coverage

Maps CCD 2.6 (Hash Tables) topics to specific sections in this activity.

| CCD Topic | Where It's Covered |
|---|---|
| **Hash tables** — purpose, structure, key-value mapping | `ChainingHashTable.cpp` TODO 1 (constructor), `ChainNode.h` (key-value struct), `main.cpp` TODOs 1-4 (grade book usage) |
| **Hash algorithms** — hash functions, modulo mapping, distribution | `ChainingHashTable.cpp` TODO 3: Step 1 (multiply-and-add), Step 2 (modulo mapping); TODO 1 DISCUSSION (prime capacities); `ProbingHashTable.cpp` TODO 7 (same algorithm) |
| **Array based (closed hashing)** — linear probing, quadratic probing | `ProbingHashTable.cpp` TODOs 1-5 (full probing implementation), `main.cpp` TODOs 5-8 (probing demo); quadratic probing discussed in TODO 2 DISCUSSION but not implemented |
| **Linked list based (open hashing)** — separate chaining | `ChainingHashTable.cpp` TODOs 1-8 (full chaining implementation), `ChainNode.h` (singly linked list node), `main.cpp` TODOs 1-4 (chaining demo) |
| **Optimizing hash tables** — load factor, resizing, choosing hash functions | `ChainingHashTable.cpp` TODO 7 (load_factor + resize), `ProbingHashTable.cpp` TODOs 5, 8 (resize, load_factor); `main.cpp` TODO 8 (resize demo); DISCUSSION covers prime capacities, clustering, threshold differences (1.0 vs 0.75) |

## Key Concepts

- **Hash function**: converts a key (string) to an integer index; a good hash distributes keys uniformly across the array
- **Modulo mapping**: `hash_value % capacity` constrains the index to valid array bounds; prime capacities reduce collision patterns
- **Separate chaining**: each bucket is a linked list — collisions just add to the chain; load factor can exceed 1.0
- **Linear probing**: collisions scan forward one slot at a time; all data lives in the array itself — no pointers needed
- **Primary clustering**: with linear probing, occupied runs grow and merge, making collisions more likely
- **Tombstone deletion**: probing marks removed slots as `DELETED` rather than `EMPTY` so that probe sequences for other keys aren't broken
- **Load factor**: `size / capacity` — chaining typically resizes around 1.0; probing must resize well below 1.0 (0.75 here) to avoid clustering
- **Rehashing**: when resizing, every entry must be re-inserted using the new capacity's modulo — old indices are invalid

## Grading (30 points)

| Category | Points | What is tested |
|---|---|---|
| Build | 2 | Project compiles without errors |
| `hash()` | 3 | Consistent results, values within range |
| Chaining `insert` | 3 | New keys added, duplicates update value, handles collisions |
| Chaining `search` | 3 | Finds existing keys, returns nullptr for missing |
| Chaining `remove` | 3 | Removes from chain, handles missing key |
| Chaining `resize` | 3 | Rehashes all entries into larger table |
| Probing `insert` | 3 | Linear probing finds open slot, handles duplicates |
| Probing `search` | 3 | Probes past tombstones, stops at empty |
| Probing `remove` | 4 | Sets tombstone, doesn't break probe sequences |
| Probing `resize` | 3 | Rehashes occupied slots, clears tombstones |

## Comment Conventions

Uses [Better Comments](https://marketplace.visualstudio.com/items?itemName=OmarRwemi.BetterComments) for VS 2022:

| Prefix | Color | Purpose |
|---|---|---|
| `// !` | Important (red) | `DISCUSSION:` teaching notes for instructor walkthrough |
| `// ?` | Question (blue) | `SEE DIAGRAM:` references to visual aids |
| `// TODO:` | Task (orange) | Student exercises (main branch) |

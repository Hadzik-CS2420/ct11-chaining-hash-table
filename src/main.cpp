// =============================================================================
// CT10: Hash Tables — Grade Book System
// =============================================================================
//
// - Scenario: a professor manages student grades using two hash table implementations
// - Part 1 uses separate chaining (linked-list buckets)
// - Part 2 uses linear probing (flat array with tombstone deletion)
// - Both tables map student names (strings) to grades (ints)
//

#include <iostream>
#include <string>
#include "ChainingHashTable.h"
#include "ProbingHashTable.h"

int main() {
    std::cout << "=== CT10: Hash Tables ===\n";
    std::cout << "=== Scenario: Professor's Grade Book ===\n\n";

    // =========================================================================
    // Part 1: Grade Book (Chaining Hash Table)
    // =========================================================================
    //
    // ! DISCUSSION: Separate chaining stores a linked list at each bucket.
    //   - collisions just add another node to the chain
    //   - the table can hold more entries than it has buckets (load factor > 1)
    //   - trade-off: extra memory for node pointers, but simple to implement
    //
    std::cout << "--- Part 1: Grade Book (Chaining Hash Table) ---\n\n";

    ChainingHashTable grade_book;
    std::cout << "Empty grade book: size=" << grade_book.size()
              << ", capacity=" << grade_book.capacity() << "\n\n";

    // -----------------------------------------------------------------------
    // TODO 1: Insert five student grades into the chaining table
    // -----------------------------------------------------------------------
    std::cout << "Adding student grades...\n";
    grade_book.insert("Alice",   95);
    grade_book.insert("Bob",     82);
    grade_book.insert("Charlie", 91);
    grade_book.insert("Diana",   78);
    grade_book.insert("Eve",     88);
    std::cout << "  Inserted 5 students (size=" << grade_book.size() << ")\n";

    // -----------------------------------------------------------------------
    // TODO 2: Print the table to see the bucket layout
    // -----------------------------------------------------------------------
    //
    // ! DISCUSSION: Notice that Alice and Diana both hash to the same bucket.
    //   - this is a collision — two different keys, same hash index
    //   - chaining handles it by storing both entries in that bucket's chain
    //   - Diana was inserted after Alice, but prepend puts Diana first in chain
    //
    std::cout << "\nGrade Book contents:\n";
    grade_book.print();
    std::cout << "  Load factor: " << grade_book.load_factor() << "\n";

    // -----------------------------------------------------------------------
    // TODO 3: Search for existing and missing students
    // -----------------------------------------------------------------------
    std::cout << "\nSearching for grades...\n";
    int* grade = grade_book.search("Charlie");
    if (grade) {
        std::cout << "  Charlie's grade: " << *grade << "\n";
    }

    grade = grade_book.search("Frank");
    if (grade) {
        std::cout << "  Frank's grade: " << *grade << "\n";
    } else {
        std::cout << "  Frank: not enrolled\n";
    }

    // -----------------------------------------------------------------------
    // TODO 4: Update a grade and remove a student
    // -----------------------------------------------------------------------
    //
    // ! DISCUSSION: Inserting a duplicate key updates the value — no duplicates.
    //   - this is the standard hash table contract: keys are unique
    //   - the same insert() function handles both new keys and updates
    //
    std::cout << "\nBob retook the exam...\n";
    grade_book.insert("Bob", 94);
    grade = grade_book.search("Bob");
    std::cout << "  Bob's updated grade: " << *grade << "\n";

    std::cout << "\nDiana dropped the class...\n";
    grade_book.remove("Diana");
    std::cout << "  Size after removal: " << grade_book.size() << "\n";

    std::cout << "\nUpdated grade book:\n";
    grade_book.print();

    // =========================================================================
    // Part 2: Quick Reference (Probing Hash Table)
    // =========================================================================
    //
    // ! DISCUSSION: Linear probing stores everything in a flat array.
    //   - no linked lists, no extra pointers — all data is in the array itself
    //   - collisions are resolved by scanning forward to the next open slot
    //   - trade-off: better cache performance, but clustering can be a problem
    //
    std::cout << "\n--- Part 2: Quick Reference (Probing Hash Table) ---\n\n";

    ProbingHashTable lookup;
    std::cout << "Empty lookup table: size=" << lookup.size()
              << ", capacity=" << lookup.capacity() << "\n\n";

    // -----------------------------------------------------------------------
    // TODO 5: Insert the same students into the probing table
    // -----------------------------------------------------------------------
    std::cout << "Adding student grades...\n";
    lookup.insert("Alice",   95);
    lookup.insert("Bob",     82);
    lookup.insert("Charlie", 91);
    lookup.insert("Diana",   78);
    lookup.insert("Eve",     88);
    std::cout << "  Inserted 5 students (size=" << lookup.size() << ")\n";

    // -----------------------------------------------------------------------
    // TODO 6: Print and search
    // -----------------------------------------------------------------------
    //
    // ! DISCUSSION: Compare the layout to the chaining table.
    //   - Alice is at slot 4 (her hash index)
    //   - Diana hashes to 4 too, but it's taken — probes to slot 5
    //   - Eve hashes to 5, but Diana is there — probes to slot 6
    //   - this is called PRIMARY CLUSTERING: collisions pile up in runs
    //
    std::cout << "\nLookup table contents:\n";
    lookup.print();
    std::cout << "  Load factor: " << lookup.load_factor() << "\n";

    std::cout << "\nSearching...\n";
    grade = lookup.search("Charlie");
    if (grade) {
        std::cout << "  Charlie's grade: " << *grade << "\n";
    }

    grade = lookup.search("Frank");
    if (grade) {
        std::cout << "  Frank's grade: " << *grade << "\n";
    } else {
        std::cout << "  Frank: not enrolled\n";
    }

    // -----------------------------------------------------------------------
    // TODO 7: Tombstone demo — remove Diana, then search for Eve
    // -----------------------------------------------------------------------
    //
    // ! DISCUSSION: This is the KEY demo for tombstone deletion.
    //   - Diana is at slot 5 (probed from slot 4)
    //   - Eve is at slot 6 (probed from slot 5, which Diana occupied)
    //   - when we remove Diana, slot 5 becomes DELETED (not EMPTY)
    //   - searching for Eve: hash=5 → slot 5 is DELETED → skip → slot 6 → found!
    //   - if we had marked slot 5 as EMPTY, the search would stop there and
    //     incorrectly report that Eve is not in the table
    //
    std::cout << "\nRemoving Diana (tombstone demo)...\n";
    lookup.remove("Diana");
    std::cout << "  After removal:\n";
    lookup.print();

    grade = lookup.search("Eve");
    if (grade) {
        std::cout << "\n  Eve's grade: " << *grade
                  << "  (search worked past tombstone!)\n";
    }

    // -----------------------------------------------------------------------
    // TODO 8: Trigger resize by inserting more students
    // -----------------------------------------------------------------------
    //
    // ! DISCUSSION: Resize clears tombstones and reduces clustering.
    //   - before resize: capacity=7, some slots are DELETED
    //   - after resize: larger prime capacity, all DELETED slots gone
    //   - only OCCUPIED entries are rehashed — tombstones are discarded
    //
    std::cout << "\nAdding more students to trigger resize...\n";
    std::cout << "  Before: capacity=" << lookup.capacity()
              << ", size=" << lookup.size()
              << ", load_factor=" << lookup.load_factor() << "\n";

    lookup.insert("Frank",  76);
    lookup.insert("Grace",  99);

    std::cout << "  After:  capacity=" << lookup.capacity()
              << ", size=" << lookup.size()
              << ", load_factor=" << lookup.load_factor() << "\n";

    std::cout << "\nFinal lookup table:\n";
    lookup.print();

    std::cout << "\n=== CT10 Complete ===\n";
    return 0;
}

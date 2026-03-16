// =============================================================================
// CT10: Chaining Hash Table — Grade Book System
// =============================================================================
//
// - Scenario: a professor manages student grades using a chaining hash table
// - Student names (strings) are KEYS, integer grades are VALUES
// - Separate chaining stores a linked list at each bucket to handle collisions
//

#include <iostream>
#include <string>
#include "ChainingHashTable.h"

int main() {
    std::cout << "=== CT10: Hash Tables ===\n";
    std::cout << "=== Scenario: Professor's Grade Book ===\n\n";

    ChainingHashTable grade_book;
    std::cout << "Empty grade book: size=" << grade_book.size()
              << ", capacity=" << grade_book.capacity() << "\n\n";

    // -----------------------------------------------------------------------
    // TODO 1: Insert five student grades into the chaining table
    // -----------------------------------------------------------------------
    std::cout << "Adding student grades...\n";
    // TODO: insert Alice(95), Bob(82), Charlie(91), Diana(78), Eve(88)

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
    // TODO: print the table
    // TODO: print the load factor

    // -----------------------------------------------------------------------
    // TODO 3: Search for existing and missing students
    // -----------------------------------------------------------------------
    std::cout << "\nSearching for grades...\n";
    // TODO: search for "Charlie" — print grade if found
    // TODO: search for "Frank"   — print "not enrolled" if not found

    // -----------------------------------------------------------------------
    // TODO 4: Update a grade and remove a student
    // -----------------------------------------------------------------------
    //
    // ! DISCUSSION: Inserting a duplicate key updates the value — no duplicates.
    //   - this is the standard hash table contract: keys are unique
    //   - the same insert() function handles both new keys and updates
    //
    std::cout << "\nBob retook the exam...\n";
    // TODO: re-insert Bob with grade 94 (update) and print his new grade

    std::cout << "\nDiana dropped the class...\n";
    // TODO: remove Diana, print size after removal, then print updated table

    std::cout << "\n=== CT10 Complete ===\n";
    return 0;
}

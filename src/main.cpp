// =============================================================================
// CT10: Hash Tables — Grade Book System (Chaining)
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

    std::cout << "\n=== CT10 Complete ===\n";
    return 0;
}

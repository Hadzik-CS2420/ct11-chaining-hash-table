#include <gtest/gtest.h>
#include "ChainingHashTable.h"

// =============================================================================
// Hash Function Tests (3 points)
// =============================================================================

TEST(HashFunction, ConsistentResults) {
    ChainingHashTable table;
    size_t h1 = table.hash("Alice");
    size_t h2 = table.hash("Alice");
    EXPECT_EQ(h1, h2) << "Same key must always produce the same hash";
}

TEST(HashFunction, WithinRange) {
    ChainingHashTable table;   // capacity = 7
    std::string keys[] = {"Alice", "Bob", "Charlie", "Diana", "Eve",
                          "Frank", "Grace", "Hank", "Ivy", "Jack"};
    for (const auto& key : keys) {
        size_t h = table.hash(key);
        EXPECT_GE(h, 0u);
        EXPECT_LT(h, 7u) << "Hash of \"" << key << "\" out of range";
    }
}

// =============================================================================
// Chaining — Insert Tests (3 points)
// =============================================================================

TEST(ChainingInsert, BasicInsert) {
    ChainingHashTable table;
    EXPECT_EQ(table.size(), 0);
    EXPECT_TRUE(table.is_empty());

    table.insert("Alice", 95);
    EXPECT_EQ(table.size(), 1);
    EXPECT_FALSE(table.is_empty());

    table.insert("Bob", 82);
    table.insert("Charlie", 91);
    EXPECT_EQ(table.size(), 3);
}

TEST(ChainingInsert, UpdateExistingKey) {
    ChainingHashTable table;
    table.insert("Alice", 95);
    table.insert("Alice", 99);     // update, not a second entry
    EXPECT_EQ(table.size(), 1);

    int* val = table.search("Alice");
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(*val, 99);
}

TEST(ChainingInsert, HandlesCollisions) {
    ChainingHashTable table;
    // Alice and Diana collide at the same bucket (both hash to 4 with cap 7)
    table.insert("Alice", 95);
    table.insert("Diana", 78);
    EXPECT_EQ(table.size(), 2);

    // Both should be searchable
    int* a = table.search("Alice");
    int* d = table.search("Diana");
    ASSERT_NE(a, nullptr);
    ASSERT_NE(d, nullptr);
    EXPECT_EQ(*a, 95);
    EXPECT_EQ(*d, 78);
}

// =============================================================================
// Chaining — Search Tests (3 points)
// =============================================================================

TEST(ChainingSearch, FindsExistingKey) {
    ChainingHashTable table;
    table.insert("Alice", 95);
    table.insert("Bob", 82);
    table.insert("Charlie", 91);

    int* val = table.search("Bob");
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(*val, 82);
}

TEST(ChainingSearch, ReturnsNullForMissing) {
    ChainingHashTable table;
    table.insert("Alice", 95);

    EXPECT_EQ(table.search("Frank"), nullptr);
    EXPECT_EQ(table.search(""), nullptr);
}

TEST(ChainingSearch, EmptyTable) {
    ChainingHashTable table;
    EXPECT_EQ(table.search("Alice"), nullptr);
}

// =============================================================================
// Chaining — Remove Tests (3 points)
// =============================================================================

TEST(ChainingRemove, RemovesExistingKey) {
    ChainingHashTable table;
    table.insert("Alice", 95);
    table.insert("Bob", 82);

    EXPECT_TRUE(table.remove("Alice"));
    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table.search("Alice"), nullptr);

    // Bob should still be there
    int* val = table.search("Bob");
    ASSERT_NE(val, nullptr);
    EXPECT_EQ(*val, 82);
}

TEST(ChainingRemove, ReturnsFalseForMissing) {
    ChainingHashTable table;
    table.insert("Alice", 95);

    EXPECT_FALSE(table.remove("Frank"));
    EXPECT_EQ(table.size(), 1);
}

TEST(ChainingRemove, RemoveFromCollisionChain) {
    ChainingHashTable table;
    // Alice and Diana collide
    table.insert("Alice", 95);
    table.insert("Diana", 78);

    EXPECT_TRUE(table.remove("Alice"));
    EXPECT_EQ(table.search("Alice"), nullptr);

    // Diana should still be in the same bucket
    int* d = table.search("Diana");
    ASSERT_NE(d, nullptr);
    EXPECT_EQ(*d, 78);
}

// =============================================================================
// Chaining — Resize Tests (3 points)
// =============================================================================

TEST(ChainingResize, TriggersOnHighLoadFactor) {
    ChainingHashTable table;   // capacity 7, max load factor 1.0
    EXPECT_EQ(table.capacity(), 7);

    // Insert 8 entries — load factor exceeds 1.0, triggers resize
    table.insert("A", 1);
    table.insert("B", 2);
    table.insert("C", 3);
    table.insert("D", 4);
    table.insert("E", 5);
    table.insert("F", 6);
    table.insert("G", 7);
    EXPECT_EQ(table.capacity(), 7);    // 7/7 = 1.0, not > 1.0

    table.insert("H", 8);             // 8/7 > 1.0, triggers resize
    EXPECT_GT(table.capacity(), 7);
    EXPECT_EQ(table.size(), 8);
}

TEST(ChainingResize, AllEntriesSurvive) {
    ChainingHashTable table;
    std::string keys[] = {"A", "B", "C", "D", "E", "F", "G", "H"};
    for (int i = 0; i < 8; ++i) {
        table.insert(keys[i], (i + 1) * 10);
    }

    // All entries should be searchable after resize
    for (int i = 0; i < 8; ++i) {
        int* val = table.search(keys[i]);
        ASSERT_NE(val, nullptr) << "Key \"" << keys[i] << "\" lost after resize";
        EXPECT_EQ(*val, (i + 1) * 10);
    }
}

//
// Created by jbetten on 12/5/15.
//

#include <gtest/gtest.h>

#include <jbetten/tree/BPlusTree.hpp>

TEST(BPlusTreeTest, cstor) {
    jbetten::BPlusTree<std::uint64_t, std::uint64_t> tree;
}

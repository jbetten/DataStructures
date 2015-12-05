//
// Created by jbetten on 12/5/15.
//

#ifndef DATASTRUCTURES_BPLUSTREE_HPP
#define DATASTRUCTURES_BPLUSTREE_HPP

#include <algorithm>
#include <memory>
#include <utility>

#include <jbetten/sys/arch.hpp>
#include <jbetten/sys/compiler.hpp>
#include <jbetten/allocator/NewAllocator.hpp>

namespace jbetten {

    template<typename Key, typename Value,
            std::size_t CacheLinesPerBlock = 8,
            typename Allocator = NewAllocator<CacheLinesPerBlock * CacheLineSize>>
    class BPlusTree {

    public:

        static constexpr std::size_t BlockSize = CacheLinesPerBlock * CacheLineSize;

    private:

        template<std::size_t N,
                bool byte = N <= std::numeric_limits<std::uint8_t>::max()>
        struct Counter {
            using type = std::uint8_t;
        };

        template<std::size_t N>
        struct Counter<N, false> {
            using type = std::uint16_t;
            static_assert(N <= std::numeric_limits<type>::max(),
                          "Counter type not large enough");
        };

        template<std::size_t M>
        struct TInternalNode {
            std::array<Key, M - 1> keys;
            typename Counter<M>::type n;
            std::array<typename Allocator::Pointer, M> nodes;

            TInternalNode()
                    : n(0) { }
        } __attribute__ ((packed()));

        template<std::size_t L>
        struct TLeafNode {
            std::array<std::pair<Key, Value>, L> elements;
            typename Allocator::Pointer prev;
            typename Allocator::Pointer next;
            typename Counter<L>::type n;

            TLeafNode()
                    : n(0) { }
        } __attribute__ ((packed()));

        template<template<std::size_t> class Node, std::size_t N = 1,
                bool valid = sizeof(Node<N>) <= BlockSize>
        struct GetMaxSize {
            static constexpr std::size_t value = GetMaxSize<Node, N + 1>::value;
        };

        template<template<std::size_t> class Node, std::size_t N>
        struct GetMaxSize<Node, N, false> {
            static constexpr std::size_t value = N - 1;
        };

    public:

        static_assert(sizeof(TInternalNode<2>) <= BlockSize,
                      "InternalNode must hold at least 2 keys");
        static constexpr std::size_t M = GetMaxSize<TInternalNode>::value;
        using InternalNode = TInternalNode<M>;

        static_assert(sizeof(TLeafNode<1>) <= BlockSize,
                      "LeafNode must hold at least one key / value pair");
        static constexpr std::size_t L = GetMaxSize<TLeafNode>::value;
        using LeafNode = TLeafNode<L>;

    private:

        static inline Key shareChildren(InternalNode &a, InternalNode &b) {
            assert(a.n == M);
            assert(b.n == 0);

            std::move(a.keys.begin() + M / 2, a.keys.end(), b.keys.begin());
            std::move(a.nodes.begin() + M / 2, a.nodes.end(), b.nodes.begin());
            a.n = M / 2;
            b.n = M - M / 2;

            return a.keys[M / 2 - 1];
        }

        static inline Key shareChildren(LeafNode &a, LeafNode &b) {
            assert(a.n == L);
            assert(b.n == 0);

            std::move(a.elements.begin() + L / 2,
                      a.elements.end(),
                      b.elements.begin());
            a.n = L / 2;
            b.n = L - L / 2;
            b.prev = a;
            b.next = a.next;
            a.next = b;

            return a.elements[L / 2 - 1].first;
        }

        template<typename NodeType>
        void splitChild(InternalNode &parent, std::size_t idx, NodeType &childa) {
            auto childb = allocator->template allocate<NodeType>();
            const auto key = shareChildren(childa, childb.second);

            std::move_backward(parent.keys.begin() + idx,
                               parent.keys.begin() + (parent.n - 2),
                               parent.keys.begin() + (parent.n - 1));
            std::move_backward(parent.nodes.begin() + (idx + 1),
                               parent.keys.begin() + (parent.n - 1),
                               parent.nodes.begin() + parent.n);

            parent.keys[idx] = key;
            parent.nodes[idx + 1] = childb.first;
            ++parent.n;
        }

        template<typename NodeType>
        static inline std::size_t getSize(void *n) {
            return reinterpret_cast<NodeType *>(n)->n;
        }

        void insertNonFull(InternalNode *node,
                           std::pair<Key, Value> &&element,
                           std::size_t level) {

        }

        void insertNonFull(LeafNode *leaf, std::pair<Key, Value> &&element) {

        }

        void insert(std::pair<Key, Value> &&element) {
            if ((0 == depth && L == getSize<LeafNode>(root)) ||
                (0 < depth && M == getSize<InternalNode>(root))) {
                auto oldRoot = root;
                auto newRoot = allocator->template allocate<InternalNode>().second;
                newRoot->nodes[0] = allocator->get(oldRoot);
                if (0 == depth) {
                    splitChild<LeafNode>(newRoot, 0, oldRoot);
                } else {
                    splitChild<InternalNode>(newRoot, 0, oldRoot);
                }
                ++depth;
            }

            if (0 == depth) {
                insertNonFull(reinterpret_cast<LeafNode *>(root), element);
            } else {
                insertNonFull(reinterpret_cast<InternalNode *>(root), element, 0);
            }
        }

        std::shared_ptr<Allocator> allocator;
        std::size_t depth;
        void *root;

    public:

        BPlusTree()
                : BPlusTree(std::make_shared<Allocator>()) { }

        explicit BPlusTree(std::shared_ptr<Allocator> alloc)
                : allocator(alloc),
                  depth(0),
                  root((allocator->template allocate<LeafNode>()).second) { }

    private:

    };

};

#endif //DATASTRUCTURES_BPLUSTREE_HPP

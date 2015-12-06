//
// Created by jbetten on 12/5/15.
//

#ifndef DATASTRUCTURES_NEWALLOCATOR_HPP
#define DATASTRUCTURES_NEWALLOCATOR_HPP

#include <cstdint>

namespace jbetten {

    template<std::size_t BlockSize>
    class NewAllocator {

    private:

        struct Block {
            Block *next;
            std::uint8_t pad[BlockSize - sizeof(Block *)];
        };

        Block *free_list;

    public:

        using Pointer = void *;

        NewAllocator()
                : free_list(nullptr) { }

        template<typename NodeType>
        Pointer get(NodeType *const node) {
            return reinterpret_cast<Pointer>(node);
        }

        template<typename NodeType>
        NodeType *get(Pointer ptr) {
            return reinterpret_cast<NodeType *>(ptr);
        }

        template<typename NodeType, typename... Args>
        std::tuple<Pointer, NodeType *> allocate(Args... args) {
            static_assert(sizeof(NodeType) <= BlockSize,
                          "BlockSize is too small for requested type.");

            Block *block;
            if (nullptr == free_list) {
                block = new Block();
            } else {
                block = free_list;
                free_list = free_list->next;
            }
            return std::tuple<Pointer, NodeType *>(block, new(block) NodeType(std::forward<Args>(args)...));
        }

        template<typename NodeType>
        void free(NodeType *const node) {
            node->~NodeType();
            Block *const block = reinterpret_cast<Block *>(node);
            block->next = free_list;
            free_list = block;
        }
    };

};

#endif //DATASTRUCTURES_NEWALLOCATOR_HPP

//
// Created by jbetten on 12/5/15.
//

#include <gtest/gtest.h>

#include <jbetten/allocator/NewAllocator.hpp>
#include <jbetten/sys/x86_64.hpp>

using Allocator = jbetten::NewAllocator<jbetten::CacheLineSize>;

TEST(NewAllocatorTest, simple) {
    bool dstr = false;

    Allocator allocator;

    struct ValueType {
        int value;
        bool &dstr;

        ValueType(bool &d)
                : value(42), dstr(d) {}

        ~ValueType() {
            dstr = true;
        }
    };

    Allocator::Pointer ptr;
    ValueType *value;
    std::tie(ptr, value) = allocator.allocate<ValueType>(std::ref(dstr));

    ASSERT_EQ(42, value->value);
    ASSERT_EQ(ptr, allocator.get(value));
    ASSERT_EQ(value, allocator.get<ValueType>(ptr));
    ASSERT_FALSE(dstr);

    allocator.free(value);

    ASSERT_TRUE(dstr);
}

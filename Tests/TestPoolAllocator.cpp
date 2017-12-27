//
//  TestPoolAllocator.cpp
//  JobRocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 19/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#include <JobRocket/Detail/FixedPoolAllocator.hpp>
#include <JobRocket/Job.hpp>

#include "catch/catch.hpp"

#include <array>
#include <iostream>


TEST_CASE("Test allocator recycles allocated memory in the order it was freed", "[pool]")
{
    jobrocket::FixedPoolAllocator allocator(sizeof(void*), 100);

    std::array<void*, 6> blocks{};

    blocks[0] = allocator.allocate();
    blocks[1] = allocator.allocate();
    blocks[2] = allocator.allocate();
    allocator.free(blocks[0]);
    auto* ptr1 = allocator.allocate();
    allocator.free(blocks[1]);
    auto* ptr2 = allocator.allocate();
    blocks[3] = allocator.allocate();
    blocks[4] = allocator.allocate();
    blocks[5] = allocator.allocate();
    allocator.free(blocks[2]);
    auto* ptr3 = allocator.allocate();

    REQUIRE(blocks[0] != blocks[1]);

    SECTION("Block 1 is recycled")
    {
        REQUIRE(blocks[0] == ptr1);
    }

    SECTION("Block 2 is recycled")
    {
        REQUIRE(blocks[1] == ptr2);
    }

    SECTION("Block 3 is recycled")
    {
        REQUIRE(blocks[2] == ptr3);
    }

    SECTION("All other blocks are different")
    {
        for ( int i = 0; i < blocks.size(); ++i ) {
            for ( int j = 0; j < blocks.size(); ++j ) {
                if ( i != j ) {
                    REQUIRE(blocks[i] != blocks[j]);
                }
            }
        }
    }
}

TEST_CASE("Bad allocations return nullptr", "[pool]")
{
    static constexpr size_t num_blocks = 100;
    jobrocket::FixedPoolAllocator allocator(sizeof(void*), num_blocks);

    for ( int i = 0; i < num_blocks; ++i ) {
        auto* block = allocator.allocate();
        REQUIRE(block != nullptr);
    }

    auto* bad_alloc = allocator.allocate();
    REQUIRE(bad_alloc == nullptr);
}

TEST_CASE("Freeing nullptr outputs error", "[pool]")
{
    jobrocket::FixedPoolAllocator allocator(sizeof(void*), 100);
    void* ptr = nullptr;
    auto success = allocator.free(ptr);
    REQUIRE(!success);
}

TEST_CASE("Freeing too many blocks outputs error", "[pool]")
{
    jobrocket::FixedPoolAllocator allocator(sizeof(void*), 100);
    auto* mem = allocator.allocate();
    auto success = allocator.free(mem);
    REQUIRE(success);
    success = allocator.free(mem);
    REQUIRE(!success);
}

TEST_CASE("Freeing invalid pointer outputs error", "[pool]")
{
    jobrocket::FixedPoolAllocator allocator(sizeof(void*), 100);
    jobrocket::FixedPoolAllocator allocator2(sizeof(void*), 100);

    allocator.allocate();
    auto* mem2 = allocator2.allocate();
    auto success = allocator.free(mem2);

    REQUIRE(!success);
}
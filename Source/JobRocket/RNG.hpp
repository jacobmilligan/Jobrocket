//
//  RNG.hpp
//  JobRocket
//
//  --------------------------------------------------------------
//
//  Created by
//  Jacob Milligan on 11/12/2017
//  Copyright (c) 2016 Jacob Milligan. All rights reserved.
//

#pragma once


#include "JobRocket/NumericTypes.hpp"

namespace sky {


// Xoroshiro128+ Random Number Generator.
// David Blackman & Sebastiano Vigna (2016).
// Available at: http://xoroshiro.di.unimi.it/ [Accessed 11 Dec. 2017].
struct xoroshiro128 {
    xoroshiro128(const u64 seed_a, const u64 seed_b)
        : state{seed_a, seed_b}
    {}

    u64 next() {
        const u64 s0 = state[0];
        u64 s1 = state[1];
        const u64 result = s0 + s1;

        s1 ^= s0;
        state[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14); // a, b
        state[1] = rotl(s1, 36); // c

        return result;
    }

    // This is the jump function for the generator. It is equivalent
    // to 2^64 calls to next(); it can be used to generate 2^64
    // non-overlapping subsequences for parallel computations.
    void jump() {
        static const u64 JUMP[] = { 0xbeac0467eba5facb, 0xd86b048b86aa9922 };

        u64 s0 = 0;
        u64 s1 = 0;
        for ( u64 i : JUMP )
            for(i32 b = 0; b < 64; b++) {
                if ( i & UINT64_C(1) << b) {
                    s0 ^= state[0];
                    s1 ^= state[1];
                }
                next();
            }

        state[0] = s0;
        state[1] = s1;
    }
private:
    u64 state[2];

    static inline u64 rotl(const u64 x, i32 k) {
        return (x << k) | (x >> (64 - k));
    }
};


}
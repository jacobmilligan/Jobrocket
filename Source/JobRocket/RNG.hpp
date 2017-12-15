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


namespace sky {


// Xoroshiro128+ Random Number Generator.
// David Blackman & Sebastiano Vigna (2016).
// Available at: http://xoroshiro.di.unimi.it/ [Accessed 11 Dec. 2017].
struct xoroshiro128 {
    xoroshiro128(const uint64_t seed_a, const uint64_t seed_b)
        : state{seed_a, seed_b}
    {}

    uint64_t next() {
        const uint64_t s0 = state[0];
        uint64_t s1 = state[1];
        const uint64_t result = s0 + s1;

        s1 ^= s0;
        state[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14); // a, b
        state[1] = rotl(s1, 36); // c

        return result;
    }

    // This is the jump function for the generator. It is equivalent
    // to 2^64 calls to next(); it can be used to generate 2^64
    // non-overlapping subsequences for parallel computations.
    void jump() {
        static const uint64_t JUMP[] = { 0xbeac0467eba5facb, 0xd86b048b86aa9922 };

        uint64_t s0 = 0;
        uint64_t s1 = 0;
        for ( uint64_t i : JUMP )
            for(int32_t b = 0; b < 64; b++) {
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
    uint64_t state[2];

    static inline uint64_t rotl(const uint64_t x, int32_t k) {
        return (x << k) | (x >> (64 - k));
    }
};


}
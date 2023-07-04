#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef uint64_t Set;

static const uint32_t mask = 0x00000003f; // Sets are numbered bits 0 through 63

static inline Set empty_set(void) {
    return 0;
}

static inline bool member_set(uint32_t x, Set s) {
    return (s & (0x1 << (x & mask)));
}

static inline Set insert_set(uint32_t x, Set s) {
    return (s | (0x1 << (x & mask)));
}

static inline Set delete_set(uint32_t x, Set s) {
    return (s & ~(0x1 << (x & mask)));
}

static inline Set union_set(Set s, Set t) {
    return (s | t);
}

static inline Set intersect_set(Set s, Set t) {
    return (s & t);
}

static inline Set difference_set(Set s, Set t) {
    return (s & ~t);
}

static inline Set complement_set(Set s) {
    return ~s;
}

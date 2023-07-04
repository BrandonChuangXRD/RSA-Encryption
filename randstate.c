#include <stdint.h>
#include <gmp.h>

#include "randstate.h"

gmp_randstate_t state;

void randstate_init(uint64_t seed) {
    //initiilize the random seed to state (its an extern, so all files should be able to see it
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);
}

void randstate_clear(void) {
    //clears the randstate
    gmp_randclear(state);
}

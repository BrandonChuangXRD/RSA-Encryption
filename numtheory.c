#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>

#include "randstate.h"

//gmp_randstate_t state;
//calculates the greatest common divisor of a and b and stores it in d
void gcd(mpz_t d, mpz_t a, mpz_t b) {
    mpz_t atemp, btemp;
    mpz_inits(atemp, btemp, NULL);

    mpz_set(atemp, a);
    mpz_set(btemp, b);
    while (mpz_cmp_ui(btemp, 0) != 0) {
        mpz_set(d, btemp);
        mpz_mod(btemp, atemp, btemp);
        mpz_set(atemp, d);
    }
    mpz_set(d, atemp);
    mpz_clear(atemp);
    mpz_clear(btemp);
}

//calculates the modular inverse of a and n and stores it in i
void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {
    mpz_t r, ri, t, ti;
    mpz_inits(r, ri, t, ti, NULL);

    // (r, ri) <-(n, a)
    mpz_set(r, n);
    mpz_set(ri, a);

    //(t,t')<-(0, 1)
    mpz_set_ui(t, 0);
    mpz_set_ui(ti, 1);

    while (mpz_cmp_ui(ri, 0) != 0) { //while ri!=0
        //q <- (truncated) r/ri
        mpz_t q;
        mpz_init(q);
        mpz_fdiv_q(q, r,
            ri);

        //(r,ri) <- (ri, r - q x ri)
        mpz_t temp;
        mpz_init(temp);
        mpz_set(temp, r);
        mpz_set(r, ri);

        mpz_t qri;
        mpz_init(qri);
        mpz_mul(qri, q, ri);
        mpz_sub(ri, temp, qri);

        //(t, ti)<-(ti,t - q x ti)mpz_set(d, atemp);
        mpz_set(temp, t);
        mpz_set(t, ti);

        mpz_t qti;
        mpz_init(qti);
        mpz_mul(qti, q, ti);
        mpz_sub(ti, temp, qti);
        mpz_clear(q);
        mpz_clear(temp);
        mpz_clear(qri);
        mpz_clear(qti);
    }
    if (mpz_cmp_ui(r, 1) > 0) { //if r > 1
        mpz_set_ui(i, 0); //return no inverse (set i to 0)
    } else if (mpz_cmp_ui(t, 0) < 0) { //if t < 0
        mpz_add(t, t, n); //t = t+n
        mpz_set(i, t);
    } else {
        mpz_set(i, t);
    }
    mpz_clear(r);
    mpz_clear(ri);
    mpz_clear(t);
    mpz_clear(ti);
}

//does the fast exponentiation
void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
    //v<-1
    mpz_t v;
    mpz_init(v);
    mpz_set_ui(v, 1);

    //p<-a
    mpz_t p;
    mpz_init(p);
    mpz_set(p, base);

    mpz_t d;
    mpz_init(d);
    mpz_set(d, exponent);

    while (mpz_cmp_ui(d, 0) > 0) { //while d > 0
        if (mpz_odd_p(d) != 0) {
            // v = (vxp) % n
            mpz_t vp;
            mpz_init(vp);
            mpz_mul(vp, v, p);
            mpz_mod(v, vp, modulus);
            mpz_clear(vp);
        }
        mpz_mul(p, p, p);
        mpz_mod(p, p, modulus);
        mpz_fdiv_q_ui(d, d, 2);
    }

    mpz_set(out, v);
    mpz_clear(v);
    mpz_clear(p);
    mpz_clear(d);
}

//performs the miller-rabin test
bool is_prime(mpz_t n, uint64_t iters) {
    

    //if the number is even and not 2 then its obviously not prime
    if (mpz_even_p(n) != 0) {
        if (mpz_cmp_ui(n, 2) == 0) {
            return true;
        }
        return false;
    }
    //MANUAL TESTS FOR NUMBERS BELOW 4 AND EVEN NUMBERS
    if (mpz_cmp_ui(n, 0) == 0) {
        return false;
    }
    if (mpz_cmp_ui(n, 1) == 0) {
        return false;
    }
    if (mpz_cmp_ui(n, 3) == 0) {
        return true;
    }
    if (mpz_cmp_ui(n, 5) == 0) {
        return true;
    }

    //figure out s and r such that r is odd
    mpz_t newn;
    mpz_init(newn);
    mpz_sub_ui(newn, n, 1);
    mpz_t s;
    mpz_init(s);
    mpz_set_ui(s, 0);

    mpz_t base;
    mpz_init(base);
    mpz_set_ui(base, 2);

    mpz_t expo;
    mpz_init(expo);
    bool divisible = false;
    bool even = true;

    mpz_t r;
    mpz_init(r);

    do {
        divisible = false;
        even = true;
        //find first conditional (keep going if it is not divisible)
        pow_mod(expo, base, s, n);
        divisible = mpz_divisible_p(newn, expo);

        if (divisible) {
            mpz_fdiv_q(r, newn, expo);
            even = mpz_even_p(r);
        }
        //find second conditonal (keep going if its even)
        mpz_add_ui(s, s, 1);
    } while (!divisible || even);
    mpz_sub_ui(s, s, 1);

    for (uint64_t i = 0; i < iters; i++) {
        mpz_t randnumber;
        mpz_init(randnumber);
        mpz_urandomm(randnumber, state, n);

        mpz_t topcheck;
        mpz_init(topcheck);
        mpz_sub_ui(topcheck, n, 1);
        while (mpz_cmp_ui(randnumber, 2) < 0 || mpz_cmp(randnumber, topcheck) > 0) {
            mpz_urandomm(randnumber, state, n);
        }
        mpz_t y;
        mpz_init(y);
        pow_mod(y, randnumber, r, n);
        if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, newn) != 0) { //y!=1 and y!=n-1
            mpz_t j;
            mpz_init(j);
            mpz_set_ui(j, 1);

            mpz_t firstcmp;
            mpz_init(firstcmp);
            mpz_sub_ui(firstcmp, s, 1);
            //j<= s-1 and y != n-1
            while (mpz_cmp(j, firstcmp) <= 0 && mpz_cmp(y, newn) != 0) {
                pow_mod(y, y, base, n);
                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clear(j);
                    mpz_clear(firstcmp);
                    mpz_clear(randnumber);
                    mpz_clear(y);
                    mpz_clear(topcheck);
                    mpz_clear(newn);
                    mpz_clear(s);
                    mpz_clear(base);
                    mpz_clear(expo);
                    mpz_clear(r);
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }
            if (mpz_cmp(y, newn) != 0) {
                mpz_clear(j);
                mpz_clear(firstcmp);
                mpz_clear(randnumber);
                mpz_clear(y);
                mpz_clear(topcheck);
                mpz_clear(newn);
                mpz_clear(s);
                mpz_clear(base);
                mpz_clear(expo);
                mpz_clear(r);
                return false;
            }
            mpz_clear(j);
            mpz_clear(firstcmp);
        }
        mpz_clear(randnumber);
        mpz_clear(y);
        mpz_clear(topcheck);
    }
    mpz_clear(newn);
    mpz_clear(s);
    mpz_clear(base);
    mpz_clear(expo);
    mpz_clear(r);

    return true;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    while (true) {
        mpz_urandomb(p, state, bits + 1);
        if (is_prime(p, iters)) {
            break;
        }
    }
}

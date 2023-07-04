#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <string.h>

#include "randstate.h"
#include "numtheory.h"

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    //make the primes
    ////p bits in range [nbits/4,(3xnbits)/4)
    srand(65537);
    uint64_t pbits = rand() % ((nbits / 2));
    pbits += nbits / 4;

    ////remaining bits go to q
    uint64_t qbits = nbits - pbits;
    qbits++;
    pbits++;
    ////make the primes
    make_prime(p, pbits, iters);
    make_prime(q, qbits, iters);

    mpz_mul(n, p, q);
    //step two: compute (p-1)(q-1)
    mpz_t greek;
    mpz_init(greek);

    mpz_t pless;
    mpz_init(pless);

    mpz_t qless;
    mpz_init(qless);

    mpz_sub_ui(pless, p, 1);
    mpz_sub_ui(qless, q, 1);
    mpz_mul(greek, pless, qless);

    //find public exponent e
    while (true) {
        //pick random
        do {
            //deviate the amount of bits
            uint64_t dbits = nbits - 1;
            dbits += random() % 3;
            //generate a random number with those amount of bits
            mpz_urandomb(e, state, dbits);
        } while (mpz_cmp_ui(e, 1) < 1); //make sure the number is greater than 1

        mpz_t geeceedee;
        mpz_init(geeceedee);
        gcd(geeceedee, e, greek);

        if (mpz_cmp_ui(geeceedee, 1) == 0) {
            mpz_clear(greek);
            mpz_clear(pless);
            mpz_clear(qless);
            mpz_clear(geeceedee);
            break;
        }
        mpz_clear(geeceedee);
    }
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    //print n e s in hexstrings with trailing newlines
    gmp_fprintf(pbfile, "%Zx\n", n);
    gmp_fprintf(pbfile, "%Zx\n", e);
    gmp_fprintf(pbfile, "%Zx\n", s);
    //print the username with a trailing newline
    fprintf(pbfile, "%s\n", username);
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx", n);
    gmp_fscanf(pbfile, "%Zx", e);
    gmp_fscanf(pbfile, "%Zx", s);
    gmp_fscanf(pbfile, "%255s\n", username);
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    //d = inverse of e mod (p-1)(q-1)
    mpz_t pless;
    mpz_init(pless);
    mpz_sub_ui(pless, p, 1);

    mpz_t qless;
    mpz_init(qless);
    mpz_sub_ui(qless, q, 1);

    mpz_t greek;
    mpz_init(greek);
    mpz_mul(greek, pless, qless);
    mod_inverse(d, e, greek);

    mpz_clear(pless);
    mpz_clear(qless);
    mpz_clear(greek);
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    //print n and d in hexstrings both with trailing newlines
    gmp_fprintf(pvfile, "%Zx\n", n);
    gmp_fprintf(pvfile, "%Zx\n", d);
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx", n);
    gmp_fscanf(pvfile, "%Zx", d);
}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    //calculate block size k=[(log2(n)-1)/8]
    uint64_t k = 0;
    k = mpz_sizeinbase(n, 2);
    k = k - 1;
    k = k / 8;
    //dynamically allocate an aray that can hold k bytes. This array should be of type uint8_t * and will serve as the block
    uint8_t *buffer = calloc(k, sizeof(uint8_t));
    //set the 0th byte of the block to 0xFF (prepends the workaround byte that we need)
    buffer[0] = 0xFF;
    //while there are still unproccessed bytes in infile:
    uint64_t j = 0;
    while ((j = fread(&buffer[1], sizeof(uint8_t), k - 1, infile))
           != 0) { //read k-1 and let j be then number of bytes read
        //use mpz_import() to convert the read bytes including 0xFF to mpz_t m. set the order parameter of mpz_import() to:
        //1 for most significant word first,
        //1 for the endian paramter,
        //0 for the nails parameter
        mpz_t absa;
        mpz_init(absa);
        mpz_import(absa, j + 1, 1, sizeof(uint8_t), 1, 0, buffer);

        //encrypt m with rsa_encrypt(), then write the number to outfile as a hextring with a trailing newline
        mpz_t babsa;
        mpz_init(babsa);
        rsa_encrypt(babsa, absa, e, n);
        gmp_fprintf(outfile, "%Zx\n", babsa);
        mpz_clear(absa);
        mpz_clear(babsa);
    }
    free(buffer);
}

void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n);
}

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    //calculate block size k = [log2(n_-1)/8)] (truncated)
    uint64_t k = mpz_sizeinbase(n, 2);
    k -= 1;
    k /= 8;
    //malloc an array that can hold k bytes
    uint8_t *buffer = calloc(k, sizeof(uint8_t));
    //while theres still unproccessed bytes in infile
    int j = 0;
    mpz_t c;
    mpz_init(c);
    uint64_t h = 0;
    mpz_t prev;
    mpz_init(prev);
    while ((j = gmp_fscanf(infile, "%Zx", c)) != 0) { //scan in a hexstring and save it as mpz_t c
        if (j == EOF) {
            break;
        }
        h++;
        mpz_t m;
        mpz_init(m);
        //decrypt it
        rsa_decrypt(m, c, d, n);

        //do mpz_export
        uint64_t bytes = 0;
        mpz_export(buffer, &bytes, 1, sizeof(uint8_t), 1, 0, m);
        //write out j-1 bytes starting from index 1 of the outfile
        for (uint64_t i = 1; i < bytes; i++) {
            fprintf(outfile, "%c", (char) buffer[i]);
        }
        mpz_clear(m);
    }
    mpz_clear(c);
    free(buffer);
}

void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t;
    mpz_init(t);
    pow_mod(t, s, e, n);
    if (mpz_cmp(m, t) == 0) {
        mpz_clear(t);
        return true;
    } else {
        mpz_clear(t);
        return false;
    }
}

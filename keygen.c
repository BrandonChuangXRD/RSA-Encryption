#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>

#include "set.h"
#include "rsa.h"
#include "randstate.h"
#include "numtheory.h"
#include "helpprints.h"

#define OPTIONS "hb:i:n:d:s:v"
#define PUBDEF  "rsa.pub"
#define PRIVDEF "rsa.priv"
#define PUB     0
#define PRIV    1
#define VERBOSE 2

int main(int argc, char **argv) {
    int opt = 0;
    char *pubout = PUBDEF;
    char *privout = PRIVDEF;

    int num_bits = 256;
    int iters = 50;

    Set commands = empty_set();

    uint64_t seed = (unsigned long) time(NULL);

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': keygenhelp(); return 0;
        case 'b': num_bits = atoi(optarg); break;
        case 'i': iters = atoi(optarg); break;
        case 'n':
            commands = insert_set(PUB, commands);
            pubout = optarg;
            break;
        case 'd':
            commands = insert_set(PRIV, commands);
            privout = optarg;
            break;
        case 's': seed = atoi(optarg); break;
        case 'v': commands = insert_set(VERBOSE, commands); break;
        }
    }
    if (num_bits == 0) {
        printf("Input the amount of bits (has to be greater than 0).\n");
        keygenhelp();
        return 0;
    }
    FILE *pubfile;

    pubfile = fopen(pubout, "w");

    FILE *privfile;
    privfile = fopen(privout, "w");

    int filedesc = fileno(privfile);
    fchmod(filedesc, 0600);

    randstate_init(seed);

    mpz_t pubp;
    mpz_init(pubp);

    mpz_t pubq;
    mpz_init(pubq);

    mpz_t pubn;
    mpz_init(pubn);

    mpz_t pube;
    mpz_init(pube);

    rsa_make_pub(pubp, pubq, pubn, pube, num_bits, iters);

    mpz_t privd;
    mpz_init(privd);
    rsa_make_priv(privd, pube, pubp, pubq);

    //get current user's name with (string) getenv()
    char *username = getenv("USER");

    //convert username to mpz_t with mpz_set_str(), set base to 62. use rsa_sing() to compute the signature
    mpz_t mpz_username;
    mpz_init(mpz_username);
    mpz_set_str(mpz_username, username, 62);

    mpz_t signature;
    mpz_init(signature);
    rsa_sign(signature, mpz_username, privd, pubn);

    mpz_clear(mpz_username);

    rsa_write_pub(pubn, pube, signature, username, pubfile);
    rsa_write_priv(pubn, privd, privfile);

    if (member_set(VERBOSE, commands)) {
        printf("username:           %s\n", username);
        gmp_printf("signature:          %Zd\n", signature);
        gmp_printf("first large prime:  %Zd\n", pubp);
        gmp_printf("second large prime: %Zd\n", pubq);
        gmp_printf("public modulus:     %Zd\n", pubn);
        gmp_printf("public exponent:    %Zd\n", pube);
        gmp_printf("private key:        %Zd\n", privd);
    }

    fclose(pubfile);
    fclose(privfile);
    mpz_clear(pubp);
    mpz_clear(pubq);
    mpz_clear(pubn);
    mpz_clear(pube);
    mpz_clear(privd);
    mpz_clear(signature);

    return 0;
}

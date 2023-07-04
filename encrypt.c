#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "rsa.h"
#include "randstate.h"
#include "numtheory.h"
#include "set.h"
#include "helpprints.h"

#define OPTIONS "hn:i:o:v"
#define PUBKEY  0
#define INFILE  1
#define OUTFILE 2
#define VERBOSE 3

int main(int argc, char **argv) {
    int opt = 0;

    char *infile = " ";
    char *outfile = " ";
    char *pubkey = "rsa.pub";

    bool verbose = false;
    bool has_infile = false;
    bool has_outfile = false;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': encrypthelp(); return 0;
        case 'n': pubkey = optarg; break;
        case 'i':
            has_infile = true;
            infile = optarg;
            break;
        case 'o':
            has_outfile = true;
            outfile = optarg;
            break;
        case 'v': verbose = true; break;
        }
    }

    FILE *pubfile;
    FILE *out;
    FILE *in;
    mpz_t en, ee, es;
    mpz_inits(en, ee, es, NULL);
    char gamertag[256];

    pubfile = fopen(pubkey, "r");
    rsa_read_pub(en, ee, es, gamertag, pubfile);

    mpz_t mpz_gamertag;
    mpz_init(mpz_gamertag);
    mpz_set_str(mpz_gamertag, gamertag, 62);

    if (verbose) {
        printf("username:       %s\n", gamertag);
        gmp_printf("signature:       %Zd\n", es);
        gmp_printf("public modulus:  %Zd\n", en);
        gmp_printf("public exponent: %Zd\n", ee);
    }

    if (!rsa_verify(mpz_gamertag, es, ee, en)) {
        printf("invalid public key\n");
        return 1;
    }
    printf("verified!\n");

    if (has_infile) {
        in = fopen(infile, "r");
    } else {
        in = stdin;
    }

    if (has_outfile) {
        out = fopen(outfile, "w");
    } else {
        out = stdout;
    }

    rsa_encrypt_file(in, out, en, ee);

    mpz_clear(en);
    mpz_clear(ee);
    mpz_clear(es);
    mpz_clear(mpz_gamertag);
    fclose(pubfile);
    fclose(out);
    fclose(in);

    return 0;
}

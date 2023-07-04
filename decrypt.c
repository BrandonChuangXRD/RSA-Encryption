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
    char *privkey = "rsa.priv";

    bool verbose = false;
    bool has_infile = false;
    bool has_outfile = false;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': decrypthelp(); return 0;
        case 'n': privkey = optarg; break;
        case 'i':
            infile = optarg;
            has_infile = true;
            break;
        case 'o':
            outfile = optarg;
            has_outfile = true;
            break;
        case 'v': verbose = true; break;
        }
    }

    FILE *privfile;
    privfile = fopen(privkey, "r");

    //read key, print an error and exit if fails
    mpz_t pn;
    mpz_init(pn);

    mpz_t pd;
    mpz_init(pd);
    rsa_read_priv(pn, pd, privfile);
    if (mpz_cmp_ui(pn, 0) == 0 || mpz_cmp_ui(pd, 0) == 0) {
        printf("invalid private key\n");
        return 0;
    }
    if (verbose) {
        gmp_printf("public modulus: %Zd\n", pn);
        gmp_printf("private key:    %Zd\n", pd);
    }

    //decrypt file using rsa_decrypt_file()
    FILE *in;
    FILE *out;
    if (!has_infile) {
        in = stdin;
    } else {
        in = fopen(infile, "r");
    }

    if (!has_outfile) {
        out = stdout;
    } else {
        out = fopen(outfile, "w");
    }

    rsa_decrypt_file(in, out, pn, pd);

    fclose(privfile);
    fclose(in);
    fclose(out);
    mpz_clear(pn);
    mpz_clear(pd);

    return 0;
}

#include <stdio.h>

#include "helpprints.h"

void keygenhelp(void) {
    printf("SYNOPSIS\n");
    printf("    Creates a public and private key for RSA encryption.\n");
    printf("USAGE\n");
    printf("    ./keygen [-b] [-i] [-n] [-d] [-s] [-v] [-h]\n");
    printf("    -b: specifies the minimum bits needed for the public modulus n.\n");
    printf("    -i: specifies the number of Miller-Rabin iterations for testing primes "
           "(default:50).\n");
    printf("    -n: pbfile: specifies the public key file (defualt: rsa.pub).\n");
    printf("    -d: pvfile: specifies the private key file (default: rsa.priv).\n");
    printf("    -s: specifies the random seed for the random state initialization (default: time "
           "in seconds since the UNIX epoch)\n");
    printf("    -v: enables verbose output.\n");
    printf("    -h: prints help\n");
}

void encrypthelp(void) {
    printf("SYNOPSIS\n");
    printf("    Encrypts a file.\n");
    printf("USAGE\n");
    printf("    ./encrypt [-i] [-o] [-n] [-v] [-h]\n");
    printf("    -i: specifies the input file\n");
    printf("    -o: specifies the output file");
    printf("    -n: specifies the public key file\n");
    printf("    -v: prints the username, signature, public modulus, and the public exponent\n");
    printf("    -h: prints help\n");
}

void decrypthelp(void) {
    printf("SYNOPSIS\n");
    printf("    Decrypts a file.\n");
    printf("USAGE\n");
    printf("    ./decrypt [-i] [-o] [-n] [-v] [-h]\n");
    printf("    -i: specifies the input file\n");
    printf("    -o: specifies the output file\n");
    printf("    -n: specifies the private key file\n");
    printf("    -v: prints the public modulus and the private key\n");
    printf("    -h: prints help\n");
}

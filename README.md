# RSA Encryption/Decryption/Key Generation
An RSA library developed in C for a class I took in UC Santa Cruz.

**Installation**\
A makefile is included, which will do the following with the given flags:
| Flag | Description |
| --- | --- |
| all | creates all files and binaries |
| keygen | creates all files and binaries related to ```keygen``` |
| encrypt | creates all files and binaries related to ```encrypt``` |
| decrypt | creates all files and binaries related to ```decrypt``` |
| clean | remove object (*.o) and executable binary files |
| cleankeys | remove any files with the extension ```.pub``` and ```.priv``` |
| format | formats all code using clang-format |

if it says that ```gmp.h``` is not found, then make sure to run ```sudo apt install pkg-config libgmp-dev```

**Usage**
### keygen:

```./keygen [-b] [-i] [-n] [-d] [-s] [-v] [-h]```
| Flag | Description |
| --- | --- |
| -b | specifies the amount of bits required for a key |
| -i | specifies the number of Miller-Rabin iterations |
| -n | specifies the public key file (default: rsa.pub) |
| -d | specifies the private key file (default: rsa.priv) |
| -s | specifies the seed fo RNG (default: time in seconds since UNIX epoch) |
| -v | enables verbose output |
| -h | prints out help message |

### encrypt/decrypt:

```./encrypt [-i] [-o] [-n] [-v] [-h]```

```./decrypt [-i] [-o] [-n] [-v] [-h]```
| Flag | Description |
| --- | --- |
| -i | specifies the input file |
| -o | specifies the output file |
| -n | specifies the key file (public key for encrypt, private key for decrypt) |
| -v | enables verbose output |
| -h | prints out help message |

**CREDITS:**\
Part of the makefile was made by a TA only noted by "Sloany Liu"

As with the other assignments, this copyright was included in the ```sets.h``` file, so it may be the case that these .h files were created by professor Darell Long:
```
BSD 2-Clause License
Copyright (c) 2021, Darrell Long
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

**KNOWN ERRORS**\
None.

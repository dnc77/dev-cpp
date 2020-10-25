Content summary
---------------

Top level folders:
   inc                        : class definitions (include files)
   src                        : source files
   lib                        : compiled libraries (post compilation only)

Top level categories:
   lib/datastruct             : data presentation, structures, containers etc.
   lib/encode                 : encoding/decoding structures and algorithms
   lib/net                    : network programming libraries
   experimental/net           : experiments related to networking
   experimental/ai            : experiments related to ai techniques

projects:
   lib/datastruct/cycbuf      : a cyclic buffer for reading/writing data
   lib/datastruct/octree      : can be used for 3D collision detection
   lib/encode/becode          : big endian coding for IEEE-754, 16, 32, 64 bit
   lib/encode/elgamal         : an encryption/decryption alg. using mod and exp
   lib/net/tcplib             : a compact TCP/IP library
   experimental/ai/behave     : a human behaviour ai experimental base
   experimental/net/ethframe  : ethframe dumper (experiment)
   experimental/concurrency/* : concurrency testing and reviewing tools

Usage Notes:
*  The inc folder defines common classes used in this repository. Include files
   define the interface to a particular library and is a good way to get an idea
   on how it can be used.
*  Makefiles under the src folder can be used to compile and link.

Compilation Notes:
*  This repository has been compiled and linked on a Raspberry PI using Raspbian
   with the following:
   - gcc (Raspbian 6.3.0-18+rpi1+deb9u1) 6.3.0 20170516
   - GNU ld (GNU Binutils for Raspbian) 2.28
   These should compile and link with no warnings or errors however other tools
   or platforms have not been worked on. In fact some issues may be encountered
   with some 64 bit platforms and more recent compilers.
   Some of these issues include:
   - Compiling C code and C++ code together using a more recent version of gcc

   These issues should be solveable easily however in case of any difficulties,
   please do not hesitate to get in touch!


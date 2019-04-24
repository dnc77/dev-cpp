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

projects:
   lib/datastruct/cycbuf      : a cyclic buffer for reading/writing data
   lib/datastruct/octree      : can be used for 3D collision detection
   lib/encode/becode          : big endian coding for IEEE-754, 16, 32, 64 bit
   lib/encode/elgamal         : an encryption/decryption alg. using mod and exp
   lib/net/tcplib             : a compact TCP/IP library
   experimental/net/ethframe  : ethframe dumper (experiment)
   experimental/concurrency/* : concurrency testing and reviewing tools

Notes:

*  The inc folder defines common classes used in this repository. Include files
   define the interface to a particular library and is a good way to get an idea
   on how it can be used.
*  Makefiles under the src folder can be used to compile and link.

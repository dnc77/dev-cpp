becode

Summary:
becode (big endian code) is an encoding/decoding class which detects the byte
ordering of the local system and encodes data to big endian order.

It also encodes and decodes floating point data (both 32 bit single precision
and 64 bit double precision) in line with the IEEE 754 Standard for
Floating-Point Arithmetic. This is typically useful for network transmission.

How it works:
Internally, the class holds a static variable which is initialized only once on
first constructor and this is used to determine whether the local system is big
or little endian.

Public swap methods exist to swap different data types to big endian provided
that the local system is not big endian already. These guarded swap functions
will not swap back to little endian and they only work in little endian systems.

Public methods exist also to encode floats and doubles to a byte buffer and vice
versa (always in big endian form).

How to use:
1: Create a becode instance.
2: Call swap() functions to swap data of different types.
3: Call IEEE 754 functions for floating point conversions.

Thanks

Duncan Camilleri

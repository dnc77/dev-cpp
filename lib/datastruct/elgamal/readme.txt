elgamal encryption

Summary:
This is a simple encryption algorithm using prime numbers and exponential
computation.

How it works:
The idea is that one picks up a prime number P between 1000 and 10000 and a
generator number G which has an exponent of P - 1. This means that when the
generator is raised to any power, the result is P - 1. Along with that,
a random private number (up to P - 1) should also be used.

The algorithm then determines a public key by applying a modulo/exponent
computation. Using a combination of the numbers given and the modulo
and exponent, two subsequent encoded numbers are generated. The gist
of the algorithm is as follows:

n: message to be encoded
P: prime number
G: generator
a: private key
y: public key
k: any random number which is less than P - 1
m: first coded part of the message
r: second coded part of the message

Step 1: Find public key: y = G ^ a mod P
Step 2: Find r: r = G ^ k mod P
Step 3: Find m: (n * (y ^ k mod P)) mod P

How to use:
1. Choose a prime number, a generator and a private key in line with
   the guidance above.
2. Construct the class passing these as parameters.
3. Choose a message to encode.
4. For each character to encode in a message, an elchar struct is returned
   by the encode function. Consider using the elbug structure.
5. For further details refer to main.cpp.


Thanks

Duncan Camilleri

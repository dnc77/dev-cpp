#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <malloc.h>
#include <time.h>
#include "CElGamal.h"

void helloWorld()
{
   // Elgamal instantiation.
   int prime = 7057;
   int gen = 84;
   int key = 41;
   CElGamal<uint8_t> elg(prime, gen, key);

   // Message to crypt.
   char msg[32];
   strcpy(msg, "Hello World!\0");

   // Encrypted buffer.
   elbuf enc;
   enc.m_charCount = strlen(msg);
   enc.m_bytes = (elchar*)malloc(sizeof(elchar) * enc.m_charCount);
   if (!enc.m_bytes) {
      printf("error: out of memory!\n");
      return;
   }
   memset(enc.m_bytes, 0, sizeof(elchar) * enc.m_charCount);

   // Encode each character first.
   int n = 0;
   for (; n < enc.m_charCount; ++n) {
      enc.m_bytes[n] = elg.encode(msg[n], gen);
   }

   printf("plain text: '%s'\n", msg);
   printf("encoded message:\n");
   for (n = 0; n < enc.m_charCount; ++n) {
      printf("%d %d ", enc.m_bytes[n].msg, enc.m_bytes[n].ret);
   }
   printf("\n");

   // Decode each character.
   uint8_t dec[32];
   memset(dec, 0, 32);
   for (n = 0; n < enc.m_charCount; ++n) {
      dec[n] = elg.decode(&enc.m_bytes[n]);
   }
   printf("decoded message: '%s'\n", (char*)dec);
}

// Basic one number test.
void basicTest()
{
   // Elgamal instantiation.
   int prime = 1297;
   int gen = 6;
   int key = 13;
   CElGamal<uint8_t> elg(prime, gen, key);

   // encode
   uint8_t msg = 24;
   elchar c = elg.encode(msg, gen);
   printf("%d => %d(%d)\n", msg, c.msg, c.ret);

   // decode
   uint8_t dec = elg.decode(&c);
   printf("decoded: %d\n", dec);
}

int main(int argc, char** argv)
{
   // Hello world message.
   helloWorld();

   // Done.
   return 0;
}


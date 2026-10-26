/* NovaOS C-Kernel - Nova Cryptographic RNG / Entropy Pool (Port aus
   adaptive32.inc, NPSPEC-CRYPTO-RNG-0001). Xorshift32-basierte
   CSPRNG-Approximation (kein echter TRNG) fuer ASLR/Nonces/Seeds -
   NICHT zu verwechseln mit crypto.c's RDRAND/Xorshift128-Keystore
   (crypto32.inc), ein eigenstaendiges, schon im alten ASM paralleles
   Subsystem. */
#ifndef NOVA_CSPRNG_H
#define NOVA_CSPRNG_H

#include <stdint.h>

#define CSPRNG_POOL_WORDS 8u

void csprng_initialize(void);
int csprng_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_value gesetzt), -1 wenn Pool nicht bereit. */
int csprng_next(uint32_t *out_value);

void csprng_add_entropy(uint32_t entropy_word);

#endif

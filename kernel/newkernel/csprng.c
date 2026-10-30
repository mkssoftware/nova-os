/* NovaOS C-Kernel - Nova Cryptographic RNG / Entropy Pool (Port aus
   adaptive32.inc). */
#include "csprng.h"
#include "resilience.h"

static uint32_t csprng_pool[CSPRNG_POOL_WORDS];
static int csprng_ready = 0;
static uint32_t csprng_state = 0;
static uint32_t csprng_calls = 0;

static inline uint64_t rdtsc_now(void) {
    uint32_t lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

void csprng_initialize(void) {
    uint32_t tick = resilience_global_tick();
    for (uint32_t i = 0; i < CSPRNG_POOL_WORDS; ++i) {
        uint32_t lo = (uint32_t)rdtsc_now();
        csprng_pool[i] = lo ^ tick ^ i;
    }
    csprng_state = 0xDEADBEEFu;
    csprng_calls = 0;
    csprng_ready = 1;
}

int csprng_next(uint32_t *out_value) {
    if (csprng_ready != 1) return -1;

    uint32_t x = csprng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    uint32_t slot = csprng_calls & (CSPRNG_POOL_WORDS - 1);
    x ^= csprng_pool[slot];
    csprng_pool[slot] = x;
    csprng_state = x;
    ++csprng_calls;

    /* Alle 16 Aufrufe zusaetzlich mit RDTSC durchmischen (Forward-Entropie). */
    if ((csprng_calls & 0xFu) == 0) {
        uint64_t tsc = rdtsc_now();
        csprng_pool[0] ^= (uint32_t)tsc;
        csprng_pool[1] ^= (uint32_t)(tsc >> 32);
    }

    *out_value = x;
    return 0;
}

void csprng_add_entropy(uint32_t entropy_word) {
    if (csprng_ready != 1) return;
    csprng_state ^= entropy_word;
    uint32_t slot = csprng_calls & (CSPRNG_POOL_WORDS - 1);
    csprng_pool[slot] ^= entropy_word;
}

int csprng_self_test(void) {
    uint32_t first, second, third;

    if (csprng_next(&first) != 0) return -1;
    if (first == 0) return -1;

    if (csprng_next(&second) != 0) return -1;
    if (second == first) return -1;

    csprng_add_entropy(0xCAFEBABEu);
    if (csprng_next(&third) != 0) return -1;
    if (third == 0) return -1;

    return 0;
}

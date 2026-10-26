/* NovaOS C-Kernel - Cryptographic Subsystem (Port aus crypto32.inc). */
#include "crypto.h"

typedef struct {
    uint32_t id;
    uint32_t type;
    uint32_t alg;
    uint32_t flags;
    uint32_t len;
    uint32_t generation;
    uint32_t use_count;
    uint8_t material[KEY_MATERIAL_MAX];
} crypto_keyslot_t;

static crypto_keyslot_t crypto_keystore[KEYSTORE_CAPACITY];
static uint32_t crypto_key_count = 0;
static uint32_t crypto_next_key_id = 1;
static uint32_t rng_state[RNG_STATE_SIZE / 4];
static int crypto_ready = 0;

static inline uint64_t read_tsc(void) {
    uint32_t lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

static int rdrand_supported(void) {
    uint32_t eax = 1, ebx, ecx, edx;
    __asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(eax));
    return (ecx >> 30) & 1u;
}

static int rdrand32(uint32_t *out_value) {
    uint32_t value;
    uint8_t ok;
    __asm__ volatile ("rdrand %0\n\tsetc %1" : "=r"(value), "=qm"(ok));
    *out_value = value;
    return ok;
}

/* Befuellt rng_state mit Entropie aus RDRAND (falls verfuegbar) + RDTSC. */
static void crypto_rng_seed(void) {
    uint32_t words = RNG_STATE_SIZE / 4;

    if (rdrand_supported()) {
        for (uint32_t i = 0; i < words; ++i) {
            uint32_t value = 0;
            int ok = 0;
            for (int attempt = 0; attempt < 3; ++attempt) {
                if (rdrand32(&value)) {
                    ok = 1;
                    break;
                }
            }
            if (!ok) {
                uint64_t tsc = read_tsc();
                value = (uint32_t)tsc ^ (uint32_t)(tsc >> 32);
            }
            rng_state[i] = value;
        }
        return;
    }

    uint64_t tsc0 = read_tsc();
    rng_state[0] = (uint32_t)tsc0;
    rng_state[1] = (uint32_t)(tsc0 >> 32);
    uint64_t tsc1 = read_tsc();
    rng_state[2] = (uint32_t)tsc1 ^ (uint32_t)(tsc1 >> 32);

    for (uint32_t i = 3; i < words; ++i) {
        uint64_t tsc = read_tsc();
        uint32_t lo = (uint32_t)tsc;
        uint32_t hi = (uint32_t)(tsc >> 32);
        uint32_t rotated = (lo >> 7) | (lo << 25); /* ror eax,7 */
        rng_state[i] = rotated ^ hi;
    }
}

void crypto_random_bytes(uint8_t *buf, uint32_t len) {
    for (uint32_t i = 0; i < len; ++i) {
        /* Xorshift128-Schritt auf rng_state[0], gemischt mit rng_state[1]. */
        uint32_t x = rng_state[0];
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        x ^= rng_state[1];
        rng_state[0] = x;
        buf[i] = (uint8_t)x;
    }
}

void crypto_initialize(void) {
    for (uint32_t i = 0; i < KEYSTORE_CAPACITY; ++i) {
        crypto_keystore[i] = (crypto_keyslot_t){0};
    }
    crypto_key_count = 0;
    crypto_next_key_id = 1;

    crypto_rng_seed();

    crypto_ready = 1;
}

int crypto_key_generate(uint32_t alg_id, uint32_t key_type, uint32_t key_len_bytes,
                         uint32_t flags, uint32_t *out_key_id) {
    if (crypto_key_count >= KEYSTORE_CAPACITY) {
        return -1;
    }
    uint32_t slot;
    for (slot = 0; slot < KEYSTORE_CAPACITY; ++slot) {
        if (crypto_keystore[slot].id == 0) {
            break;
        }
    }
    if (slot >= KEYSTORE_CAPACITY) {
        return -1;
    }

    crypto_keyslot_t *key = &crypto_keystore[slot];
    key->id = crypto_next_key_id;
    key->type = key_type;
    key->alg = alg_id;
    key->flags = flags;
    key->len = (key_len_bytes <= KEY_MATERIAL_MAX) ? key_len_bytes : KEY_MATERIAL_MAX;
    key->generation = 0;
    key->use_count = 0;

    crypto_random_bytes(key->material, key->len);

    if (out_key_id) {
        *out_key_id = crypto_next_key_id;
    }
    ++crypto_next_key_id;
    ++crypto_key_count;
    return 0;
}

int crypto_key_find(uint32_t key_id, void **out_slot) {
    for (uint32_t i = 0; i < KEYSTORE_CAPACITY; ++i) {
        if (crypto_keystore[i].id == key_id) {
            if (out_slot) {
                *out_slot = &crypto_keystore[i];
            }
            return 0;
        }
    }
    return -1;
}

int crypto_key_delete(uint32_t key_id) {
    void *slot_ptr = 0;
    if (crypto_key_find(key_id, &slot_ptr) != 0) {
        return -1;
    }
    crypto_keyslot_t *key = (crypto_keyslot_t *)slot_ptr;
    crypto_secure_zero(key->material, KEY_MATERIAL_MAX);
    crypto_secure_zero(key, sizeof(*key));
    --crypto_key_count;
    return 0;
}

int crypto_key_rotate(uint32_t key_id) {
    void *slot_ptr = 0;
    if (crypto_key_find(key_id, &slot_ptr) != 0) {
        return -1;
    }
    crypto_keyslot_t *key = (crypto_keyslot_t *)slot_ptr;
    if (!(key->flags & KEYFLAG_ROTATEABLE)) {
        return -1;
    }
    crypto_secure_zero(key->material, KEY_MATERIAL_MAX);
    crypto_random_bytes(key->material, key->len);
    ++key->generation;
    return 0;
}

void crypto_secure_zero(void *buf, uint32_t len) {
    if (len == 0) {
        return;
    }
    uint8_t *p = (uint8_t *)buf;
    for (uint32_t i = 0; i < len; ++i) {
        p[i] = 0;
    }
    __asm__ volatile ("mfence" ::: "memory");
}

int crypto_kdf_derive(uint32_t master_key_id, const uint8_t *context, uint32_t context_len,
                       uint8_t *out, uint32_t out_len) {
    (void)context;
    (void)context_len;
    void *slot_ptr = 0;
    if (crypto_key_find(master_key_id, &slot_ptr) != 0) {
        return -1;
    }
    /* Stub: wie im alten ASM nur Basis-Entropie, keine echte HKDF-Vermischung. */
    crypto_random_bytes(out, out_len);
    return 0;
}

int crypto_self_test(void) {
    if (crypto_ready != 1) return -1;

    uint32_t key_id = 0;
    if (crypto_key_generate(CRYPTO_ALG_AESCBC128, KEYTYPE_SYMMETRIC, 16,
                             KEYFLAG_ROTATEABLE, &key_id) != 0) {
        return -1;
    }

    void *slot_ptr = 0;
    if (crypto_key_find(key_id, &slot_ptr) != 0) return -1;
    if (crypto_key_rotate(key_id) != 0) return -1;
    if (crypto_key_delete(key_id) != 0) return -1;

    return 0;
}

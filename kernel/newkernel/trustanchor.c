/* NovaOS C-Kernel - Nova Trust Anchor Chain (Port aus adaptive32.inc). */
#include "trustanchor.h"
#include "csprng.h"

typedef struct {
    uint32_t id; /* 1-basiert: 0 = frei */
    uint32_t type;
    uint32_t fingerprint;
    uint32_t flags;
} trust_anchor_rec_t;

static trust_anchor_rec_t trust_anchors[TRUST_ANCHOR_COUNT];
static int trust_ready = 0;
static uint32_t trust_count = 0;

int trust_register_anchor(uint32_t anchor_type, uint32_t fingerprint, uint32_t *out_slot) {
    if (trust_count >= TRUST_ANCHOR_COUNT) return -1;

    for (uint32_t i = 0; i < TRUST_ANCHOR_COUNT; ++i) {
        if (trust_anchors[i].id == 0) {
            trust_anchors[i].type = anchor_type;
            trust_anchors[i].fingerprint = fingerprint;
            trust_anchors[i].flags = TRUST_ANC_FLAG_VALID;
            trust_anchors[i].id = i + 1; /* 1-basiert, niemals 0 */
            ++trust_count;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int trust_verify_anchor(uint32_t anchor_type, uint32_t *out_fingerprint) {
    for (uint32_t i = 0; i < TRUST_ANCHOR_COUNT; ++i) {
        if (trust_anchors[i].id == 0) continue;
        if (trust_anchors[i].type != anchor_type) continue;
        if (trust_anchors[i].flags & TRUST_ANC_FLAG_REVOKED) return -1;
        if ((trust_anchors[i].flags & TRUST_ANC_FLAG_VALID) == 0) continue;
        *out_fingerprint = trust_anchors[i].fingerprint;
        return 0;
    }
    return -1;
}

int trust_initialize(void) {
    for (uint32_t i = 0; i < TRUST_ANCHOR_COUNT; ++i) trust_anchors[i] = (trust_anchor_rec_t){0};
    trust_count = 0;

    uint32_t fingerprint, slot;
    if (csprng_next(&fingerprint) != 0) return -1;
    if (trust_register_anchor(TRUST_ANCH_BOOT, fingerprint, &slot) != 0) return -1;

    if (csprng_next(&fingerprint) != 0) return -1;
    if (trust_register_anchor(TRUST_ANCH_SECURITY, fingerprint, &slot) != 0) return -1;

    trust_ready = 1;
    return 0;
}

int trust_self_test(void) {
    if (trust_ready != 1) return -1;

    uint32_t fingerprint;
    if (trust_verify_anchor(TRUST_ANCH_BOOT, &fingerprint) != 0) return -1;
    if (fingerprint == 0) return -1;

    if (trust_verify_anchor(TRUST_ANCH_SECURITY, &fingerprint) != 0) return -1;

    if (trust_verify_anchor(99, &fingerprint) == 0) return -1; /* unbekannter Typ */

    return 0;
}

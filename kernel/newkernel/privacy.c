/* NovaOS C-Kernel - Privacy-Subsystem (Port aus privacy32.inc). */
#include "privacy.h"

typedef struct {
    uint32_t data_id;
    uint32_t label;
    uint32_t retention;
    uint32_t sovereignty_flags;
    uint32_t expiry_ticks;
    uint32_t dlp_action;
    uint32_t noise_epsilon;
    uint32_t access_count;
} priv2_record_t;

static priv2_record_t priv2_table[PRIV_CAPACITY];
static uint32_t priv2_count = 0;
static int priv2_ready = 0;

/* TODO: durch timer_ticks ersetzen, sobald das Time-Subsystem portiert ist. */
static uint32_t current_timer_ticks(void) {
    return 0;
}

void privacy_ext_initialize(void) {
    for (uint32_t i = 0; i < PRIV_CAPACITY; ++i) {
        priv2_table[i] = (priv2_record_t){0};
    }
    priv2_count = 0;
    priv2_ready = 1;
}

int privacy_ext_label_data(uint32_t data_id, uint32_t label, uint32_t retention,
                            uint32_t sovereignty_flags) {
    if (priv2_count >= PRIV_CAPACITY) {
        return -1;
    }
    priv2_record_t *rec = &priv2_table[priv2_count];
    rec->data_id = data_id;
    rec->label = label;
    rec->retention = retention;
    rec->sovereignty_flags = sovereignty_flags;

    uint32_t now = current_timer_ticks();
    switch (retention) {
    case PRIV_RETAIN_SESSION:
        rec->expiry_ticks = now + 360000u;   /* 1h * 100Hz */
        break;
    case PRIV_RETAIN_DAY:
        rec->expiry_ticks = now + 8640000u;  /* 24h * 100Hz */
        break;
    case PRIV_RETAIN_PERMANENT:
        rec->expiry_ticks = 0xFFFFFFFFu;
        break;
    default:
        rec->expiry_ticks = now + 60480000u; /* 7 Tage * 100Hz (Default: Woche) */
        break;
    }

    rec->dlp_action = PRIV_DLP_ALLOW;
    rec->noise_epsilon = 100; /* epsilon=1.0 */
    rec->access_count = 0;
    ++priv2_count;
    return 0;
}

void privacy_ext_check_access(uint32_t data_id, uint32_t *out_dlp_action) {
    for (uint32_t i = 0; i < priv2_count; ++i) {
        priv2_record_t *rec = &priv2_table[i];
        if (rec->data_id != data_id) {
            continue;
        }
        if (rec->expiry_ticks != 0xFFFFFFFFu && current_timer_ticks() >= rec->expiry_ticks) {
            *out_dlp_action = PRIV_DLP_BLOCK; /* abgelaufen */
            return;
        }
        ++rec->access_count; /* Zugriffszaehler (Minimierung) */
        *out_dlp_action = rec->dlp_action;
        return;
    }
    *out_dlp_action = PRIV_DLP_ALLOW; /* unlabeled: erlaubt */
}

int privacy_ext_self_test(void) {
    if (priv2_ready != 1) return -1;

    if (privacy_ext_label_data(0x1234u, PRIV2_LABEL_PERSONAL, PRIV_RETAIN_SESSION,
                                PRIV_SOV_LOCAL_ONLY | PRIV_SOV_AUDITED) != 0) {
        return -1;
    }
    uint32_t dlp_action = 0;
    privacy_ext_check_access(0x1234u, &dlp_action);

    return 0;
}

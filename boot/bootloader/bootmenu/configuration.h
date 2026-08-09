#ifndef NOVA_BOOT_CONFIGURATION_H
#define NOVA_BOOT_CONFIGURATION_H

#include <stdbool.h>
#include <stdint.h>
#include "theme.h"
#include "diagnostics.h"

#define NOVA_CONFIGURATION_VERSION_MAJOR 1u
#define NOVA_CONFIGURATION_VERSION_MINOR 0u
#define NOVA_CONFIGURATION_VERSION_PATCH 0u

typedef enum {
    NOVA_CONFIG_THEME,
    NOVA_CONFIG_REDUCED_MOTION,
    NOVA_CONFIG_TOOLTIPS,
    NOVA_CONFIG_TOOLTIP_DELAY,
    NOVA_CONFIG_SCALE,
    NOVA_CONFIG_QUALITY,
    NOVA_CONFIG_SAFE_MODE,
    NOVA_CONFIG_TEXT_FALLBACK,
    NOVA_CONFIG_AUTO_RECOVERY,
    NOVA_CONFIG_RECOVERY_RETRIES,
    NOVA_CONFIG_WATCHDOG_TIMEOUT,
    NOVA_CONFIG_KEY_REPEAT_DELAY,
    NOVA_CONFIG_KEY_REPEAT_RATE,
    NOVA_CONFIG_DEBUG_OVERLAY,
    NOVA_CONFIG_KEY_COUNT
} nova_configuration_key_t;

typedef struct {
    uint16_t version_major,version_minor,version_patch;
    nova_theme_id_t theme;
    nova_quality_t quality;
    uint16_t scale_milli;
    uint16_t tooltip_delay_ms;
    uint16_t watchdog_timeout_ms;
    uint16_t key_repeat_delay_ms;
    uint16_t key_repeat_rate_ms;
    uint8_t recovery_retries;
    bool reduced_motion;
    bool tooltips;
    bool safe_mode;
    bool text_fallback;
    bool auto_recovery;
    bool debug_overlay;
    uint32_t checksum;
} nova_boot_configuration_t;

typedef struct {
    uint32_t generation;
    uint32_t transactions;
    uint32_t commits;
    uint32_t rollbacks;
    uint32_t resets;
    uint32_t overrides;
    uint32_t validation_errors;
    uint32_t rejected_writes;
    uint32_t notifications;
    bool transaction_active;
} nova_configuration_diagnostics_t;

void nova_configuration_initialize(void);
const nova_boot_configuration_t *nova_configuration_get(void);
const nova_boot_configuration_t *nova_configuration_effective(void);
bool nova_configuration_begin(void);
bool nova_configuration_set(nova_configuration_key_t key,uint32_t value);
bool nova_configuration_commit(void);
bool nova_configuration_rollback(void);
bool nova_configuration_reset_defaults(void);
bool nova_configuration_override(nova_configuration_key_t key,uint32_t value);
bool nova_configuration_clear_overrides(void);
bool nova_configuration_validate(const nova_boot_configuration_t *configuration);
uint32_t nova_configuration_checksum(const nova_boot_configuration_t *configuration);
const nova_configuration_diagnostics_t *nova_configuration_diagnostics(void);

#endif

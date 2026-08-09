#ifndef NOVA_BOOT_RECOVERY_H
#define NOVA_BOOT_RECOVERY_H

#include <stdbool.h>
#include <stdint.h>

#define NOVA_RECOVERY_RECORD_CAPACITY 32u
#define NOVA_RECOVERY_WATCHDOG_COUNT 6u

typedef enum {
    NOVA_UI_ERROR_INFO,
    NOVA_UI_ERROR_WARNING,
    NOVA_UI_ERROR_RECOVERABLE,
    NOVA_UI_ERROR_CRITICAL,
    NOVA_UI_ERROR_FATAL
} nova_ui_error_severity_t;

typedef enum {
    NOVA_UI_SUBSYSTEM_RENDERING,
    NOVA_UI_SUBSYSTEM_LAYOUT,
    NOVA_UI_SUBSYSTEM_INPUT,
    NOVA_UI_SUBSYSTEM_MOTION,
    NOVA_UI_SUBSYSTEM_NAVIGATION,
    NOVA_UI_SUBSYSTEM_RESOURCES
} nova_ui_subsystem_t;

typedef enum {
    NOVA_RECOVERY_LOG_ONLY,
    NOVA_RECOVERY_RETRY,
    NOVA_RECOVERY_LOCAL_FALLBACK,
    NOVA_RECOVERY_REINITIALIZE,
    NOVA_RECOVERY_DISABLE_SUBSYSTEM,
    NOVA_RECOVERY_SAFE_MODE,
    NOVA_RECOVERY_TEXT_MODE
} nova_recovery_level_t;

typedef enum {
    NOVA_RECOVERY_HEALTHY,
    NOVA_RECOVERY_FAILURE,
    NOVA_RECOVERY_RUNNING,
    NOVA_RECOVERY_VALIDATING,
    NOVA_RECOVERY_FALLBACK,
    NOVA_RECOVERY_FATAL
} nova_recovery_state_t;

typedef enum {
    NOVA_RECOVERY_FEATURE_MOTION,
    NOVA_RECOVERY_FEATURE_GLASS,
    NOVA_RECOVERY_FEATURE_BLUR,
    NOVA_RECOVERY_FEATURE_SHADOWS,
    NOVA_RECOVERY_FEATURE_VECTOR_FILTERS,
    NOVA_RECOVERY_FEATURE_COUNT
} nova_recovery_feature_t;

typedef struct {
    uint32_t code;
    uint32_t timestamp_ms;
    nova_ui_subsystem_t subsystem;
    nova_ui_error_severity_t severity;
    nova_recovery_level_t recovery;
    bool successful;
} nova_recovery_record_t;

typedef struct {
    nova_recovery_state_t state;
    nova_recovery_level_t level;
    uint32_t reports;
    uint32_t retries;
    uint32_t fallbacks;
    uint32_t safe_mode_entries;
    uint32_t text_mode_entries;
    uint32_t watchdog_timeouts;
    uint32_t rejected;
    bool safe_mode;
    bool text_mode;
    bool continue_boot;
} nova_recovery_diagnostics_t;

void nova_recovery_initialize(void);
bool nova_recovery_report(uint32_t code, nova_ui_subsystem_t subsystem,
    nova_ui_error_severity_t severity, nova_recovery_level_t preferred,
    uint32_t timestamp_ms);
bool nova_recovery_validate(bool recovered, uint32_t timestamp_ms);
bool nova_recovery_feature_enabled(nova_recovery_feature_t feature);
bool nova_recovery_safe_mode(void);
bool nova_recovery_text_mode(void);
bool nova_recovery_continue_boot(void);
const nova_recovery_record_t *nova_recovery_record(uint32_t chronological_index);
uint16_t nova_recovery_record_count(void);
const nova_recovery_diagnostics_t *nova_recovery_diagnostics(void);
bool nova_recovery_watchdog_configure(nova_ui_subsystem_t subsystem,
    uint32_t timeout_ms, uint32_t now_ms);
bool nova_recovery_watchdog_kick(nova_ui_subsystem_t subsystem, uint32_t now_ms);
bool nova_recovery_watchdog_check(uint32_t now_ms);

#endif

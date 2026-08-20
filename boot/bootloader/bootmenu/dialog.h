#ifndef NOVA_BOOT_DIALOG_H
#define NOVA_BOOT_DIALOG_H

#include <stdint.h>
#include <stdbool.h>

#define NOVA_DIALOG_CAPACITY 4u
#define NOVA_DIALOG_TEXT_CAPACITY 192u
#define NOVA_DIALOG_BUTTON_CAPACITY 3u
#define NOVA_CREDENTIAL_CAPACITY 128u
#define NOVA_DIALOG_TEST_CAPACITY 10u

typedef enum {
    NOVA_DIALOG_INFORMATION,
    NOVA_DIALOG_WARNING,
    NOVA_DIALOG_ERROR,
    NOVA_DIALOG_CONFIRMATION,
    NOVA_DIALOG_PROGRESS,
    NOVA_DIALOG_CREDENTIAL,
    NOVA_DIALOG_RECOVERY,
    NOVA_DIALOG_NETWORK,
    NOVA_DIALOG_FIRMWARE,
    NOVA_DIALOG_CUSTOM
} nova_dialog_type_t;

typedef enum {
    NOVA_DIALOG_CREATED,
    NOVA_DIALOG_INITIALIZED,
    NOVA_DIALOG_OPENING,
    NOVA_DIALOG_VISIBLE,
    NOVA_DIALOG_ACTIVE,
    NOVA_DIALOG_CLOSING,
    NOVA_DIALOG_DESTROYED
} nova_dialog_state_t;

typedef enum {
    NOVA_DIALOG_RESULT_NONE,
    NOVA_DIALOG_RESULT_OK,
    NOVA_DIALOG_RESULT_CANCEL,
    NOVA_DIALOG_RESULT_YES,
    NOVA_DIALOG_RESULT_NO,
    NOVA_DIALOG_RESULT_RETRY,
    NOVA_DIALOG_RESULT_IGNORE,
    NOVA_DIALOG_RESULT_ABORT,
    NOVA_DIALOG_RESULT_CONTINUE
} nova_dialog_result_t;

typedef enum {
    NOVA_CONFIRM_INFORMATION,
    NOVA_CONFIRM_WARNING,
    NOVA_CONFIRM_CRITICAL,
    NOVA_CONFIRM_SECURITY
} nova_confirmation_level_t;

typedef enum { NOVA_WARNING_LOW,NOVA_WARNING_MEDIUM,NOVA_WARNING_HIGH,
    NOVA_WARNING_CRITICAL } nova_warning_level_t;
typedef enum { NOVA_WARNING_GENERAL,NOVA_WARNING_STORAGE,NOVA_WARNING_SECURITY,
    NOVA_WARNING_NETWORK,NOVA_WARNING_MEMORY,NOVA_WARNING_TEMPERATURE,
    NOVA_WARNING_ICON_COUNT } nova_warning_icon_t;

typedef enum {
    NOVA_CREDENTIAL_PASSWORD,
    NOVA_CREDENTIAL_PIN,
    NOVA_CREDENTIAL_PASSPHRASE,
    NOVA_CREDENTIAL_RECOVERY_KEY
} nova_credential_type_t;

typedef struct {
    uint32_t id;
    uint32_t root_view_id;
    uint32_t navigation_context_id;
    uint32_t lifecycle_mask;
    uint64_t opened_ms;
    uint64_t closed_ms;
    nova_dialog_type_t type;
    nova_dialog_state_t state;
    nova_dialog_result_t result;
    nova_confirmation_level_t confirmation_level;
    nova_warning_level_t warning_level;
    nova_warning_icon_t warning_icon;
    uint32_t warning_code;
    char title[64];
    char message[NOVA_DIALOG_TEXT_CAPACITY];
    const char *buttons[NOVA_DIALOG_BUTTON_CAPACITY];
    nova_dialog_result_t button_results[NOVA_DIALOG_BUTTON_CAPACITY];
    uint8_t button_count;
    uint8_t focus;
    uint8_t destructive_button;
    uint8_t destructive_armed;
    uint16_t saved_focus_id;
    uint16_t progress_per_mille;
    uint16_t credential_bytes;
    uint16_t credential_glyphs;
    nova_credential_type_t credential_type;
    char credential[NOVA_CREDENTIAL_CAPACITY];
    char progress_status[96];
    char recommendation[96];
    bool progress_indeterminate;
    bool modal;
    bool cancelable;
    bool secure;
    bool confirmation_validated;
    bool warning_validated;
} nova_dialog_t;

typedef struct {
    uint8_t depth;
    uint8_t peak_depth;
    uint32_t opened;
    uint32_t closed;
    uint32_t cancelled;
    uint32_t results;
    uint32_t focus_moves;
    uint32_t rejected;
    uint32_t lifecycle_transitions;
    uint32_t nonmodal_opened;
    uint32_t stack_errors;
    uint32_t blocked_inputs;
    uint32_t blocked_navigation;
    uint32_t back_events;
    uint32_t focus_traps;
    uint32_t confirmations;
    uint32_t confirmation_decisions;
    uint32_t confirmation_errors;
    uint32_t warnings;
    uint32_t warning_decisions;
    uint32_t warning_errors;
    uint64_t clock_ms;
    nova_confirmation_level_t last_confirmation_level;
    nova_dialog_result_t last_confirmation_result;
    char last_confirmation_title[64];
    nova_warning_level_t last_warning_level;
    nova_warning_icon_t last_warning_icon;
    nova_dialog_result_t last_warning_result;
    uint32_t last_warning_code;
    bool modal_active;
    bool backdrop_active;
} nova_dialog_diagnostics_t;

typedef enum { NOVA_DIALOG_TEST_PASSED,NOVA_DIALOG_TEST_FAILED,
    NOVA_DIALOG_TEST_SKIPPED } nova_dialog_test_outcome_t;
typedef struct {uint32_t dialog_id;nova_dialog_type_t dialog_type;
    nova_dialog_test_outcome_t status;uint64_t duration_us;uint32_t detected_errors;
    uint16_t focus_path;uint16_t event_count;bool modal_trap,keyboard,escape,
    return_focus,security,animation_observed;} nova_dialog_test_result_t;
typedef struct {uint8_t count,passed,failed,skipped;uint32_t reports;
    bool initialized,isolated,deterministic,configuration_unchanged;
} nova_dialog_test_summary_t;

void nova_dialog_initialize(void);
bool nova_dialog_create(nova_dialog_type_t type,bool modal,bool cancelable,
                        uint16_t saved_focus_id,nova_dialog_t **dialog);
bool nova_dialog_show(nova_dialog_t *dialog);
bool nova_dialog_close(nova_dialog_t *dialog,nova_dialog_result_t selected,
                       nova_dialog_result_t *result);
nova_dialog_result_t nova_dialog_result(const nova_dialog_t *dialog);
void nova_dialog_tick(uint32_t elapsed_ms);
bool nova_modal_dialog_active(void);
bool nova_modal_dialog_backdrop_active(void);
bool nova_modal_dialog_input_allowed(uint32_t dialog_id);
bool nova_modal_dialog_navigation_allowed(void);
bool nova_modal_dialog_back(nova_dialog_result_t *result);
nova_dialog_t *nova_dialog_open(nova_dialog_type_t type, const char *title,
                                const char *message, bool modal, bool cancelable,
                                uint16_t saved_focus_id);
nova_dialog_t *nova_confirmation_dialog_show(
    nova_confirmation_level_t level,const char *title,const char *message,
    const char *affirmative_label,nova_dialog_result_t affirmative_result,
    uint16_t saved_focus_id);
nova_dialog_result_t nova_confirmation_dialog_result(void);
nova_dialog_t *nova_warning_dialog_show(
    nova_warning_level_t level,nova_warning_icon_t icon,const char *title,
    const char *message,const char *recommendation,uint32_t warning_code,
    bool modal,uint16_t saved_focus_id);
nova_dialog_result_t nova_warning_dialog_result(void);
bool nova_dialog_add_button(nova_dialog_t *dialog, const char *label,
                            nova_dialog_result_t result, bool destructive);
nova_dialog_t *nova_dialog_active(void);
bool nova_dialog_focus_move(int8_t direction);
bool nova_dialog_activate(nova_dialog_result_t *result);
bool nova_dialog_cancel(nova_dialog_result_t *result);
bool nova_dialog_set_progress(nova_dialog_t *dialog, uint16_t per_mille);
bool nova_dialog_progress_update(nova_dialog_t *dialog, int32_t per_mille,
                                 bool indeterminate, const char *status);
bool nova_dialog_credential_configure(nova_dialog_t *dialog,
                                      nova_credential_type_t type);
bool nova_dialog_credential_input(nova_dialog_t *dialog, uint32_t codepoint);
bool nova_dialog_credential_backspace(nova_dialog_t *dialog);
const char *nova_dialog_credential_value(const nova_dialog_t *dialog);
void nova_dialog_credential_clear(nova_dialog_t *dialog);
const nova_dialog_diagnostics_t *nova_dialog_diagnostics(void);
bool nova_dialog_test_initialize(void);
bool nova_dialog_test_execute(nova_dialog_type_t type);
const nova_dialog_test_result_t *nova_dialog_test_results(void);
const nova_dialog_test_summary_t *nova_dialog_test_summary(void);
bool nova_dialog_test_generate_report(bool authorized,uint8_t *output,
                                      uint32_t capacity,uint32_t *written);

#endif

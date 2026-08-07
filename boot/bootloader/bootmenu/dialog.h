#ifndef NOVA_BOOT_DIALOG_H
#define NOVA_BOOT_DIALOG_H

#include <stdint.h>
#include <stdbool.h>

#define NOVA_DIALOG_CAPACITY 4u
#define NOVA_DIALOG_TEXT_CAPACITY 192u
#define NOVA_DIALOG_BUTTON_CAPACITY 3u
#define NOVA_CREDENTIAL_CAPACITY 128u

typedef enum {
    NOVA_DIALOG_INFORMATION,
    NOVA_DIALOG_WARNING,
    NOVA_DIALOG_ERROR,
    NOVA_DIALOG_CONFIRMATION,
    NOVA_DIALOG_PROGRESS,
    NOVA_DIALOG_CREDENTIAL,
    NOVA_DIALOG_RECOVERY,
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
    NOVA_DIALOG_RESULT_ABORT
} nova_dialog_result_t;

typedef enum {
    NOVA_CREDENTIAL_PASSWORD,
    NOVA_CREDENTIAL_PIN,
    NOVA_CREDENTIAL_PASSPHRASE,
    NOVA_CREDENTIAL_RECOVERY_KEY
} nova_credential_type_t;

typedef struct {
    uint32_t id;
    nova_dialog_type_t type;
    nova_dialog_state_t state;
    nova_dialog_result_t result;
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
    bool progress_indeterminate;
    bool modal;
    bool cancelable;
    bool secure;
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
    bool modal_active;
} nova_dialog_diagnostics_t;

void nova_dialog_initialize(void);
nova_dialog_t *nova_dialog_open(nova_dialog_type_t type, const char *title,
                                const char *message, bool modal, bool cancelable,
                                uint16_t saved_focus_id);
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

#endif

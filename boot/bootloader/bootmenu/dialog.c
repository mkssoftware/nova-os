#include "dialog.h"

static nova_dialog_t dialogs[NOVA_DIALOG_CAPACITY];
static nova_dialog_diagnostics_t diagnostics;
static uint32_t next_id;

static void copy_text(char *target, uint32_t capacity, const char *source)
{
    uint32_t i = 0;
    if (!source) source = "";
    while (i + 1 < capacity && source[i]) { target[i] = source[i]; ++i; }
    target[i] = 0;
}

void nova_dialog_initialize(void)
{
    diagnostics = (nova_dialog_diagnostics_t){0};
    next_id = 1;
}

nova_dialog_t *nova_dialog_open(nova_dialog_type_t type, const char *title,
                                const char *message, bool modal, bool cancelable,
                                uint16_t saved_focus_id)
{
    if (diagnostics.depth >= NOVA_DIALOG_CAPACITY ||
        (modal && diagnostics.modal_active)) {
        ++diagnostics.rejected;
        return 0;
    }
    nova_dialog_t *dialog = &dialogs[diagnostics.depth++];
    *dialog = (nova_dialog_t){0};
    dialog->id = next_id++;
    dialog->type = type;
    dialog->state = NOVA_DIALOG_CREATED;
    copy_text(dialog->title, sizeof(dialog->title), title);
    copy_text(dialog->message, sizeof(dialog->message), message);
    dialog->modal = modal;
    dialog->cancelable = cancelable;
    dialog->secure = type == NOVA_DIALOG_CREDENTIAL;
    dialog->saved_focus_id = saved_focus_id;
    dialog->destructive_button = 0xffu;
    dialog->state = NOVA_DIALOG_INITIALIZED;
    dialog->state = NOVA_DIALOG_OPENING;
    dialog->state = NOVA_DIALOG_VISIBLE;
    dialog->state = NOVA_DIALOG_ACTIVE;
    if (modal) diagnostics.modal_active = true;
    ++diagnostics.opened;
    if (diagnostics.depth > diagnostics.peak_depth)
        diagnostics.peak_depth = diagnostics.depth;
    return dialog;
}

bool nova_dialog_add_button(nova_dialog_t *dialog, const char *label,
                            nova_dialog_result_t result, bool destructive)
{
    if (!dialog || dialog != nova_dialog_active() || !label || !*label ||
        result == NOVA_DIALOG_RESULT_NONE ||
        dialog->button_count >= NOVA_DIALOG_BUTTON_CAPACITY) {
        ++diagnostics.rejected;
        return false;
    }
    uint8_t index = dialog->button_count++;
    dialog->buttons[index] = label;
    dialog->button_results[index] = result;
    if (destructive) dialog->destructive_button = index;
    return true;
}

nova_dialog_t *nova_dialog_active(void)
{
    return diagnostics.depth ? &dialogs[diagnostics.depth - 1] : 0;
}

bool nova_dialog_focus_move(int8_t direction)
{
    nova_dialog_t *dialog = nova_dialog_active();
    if (!dialog || !dialog->button_count || !direction) return false;
    int16_t next = (int16_t)dialog->focus + (direction < 0 ? -1 : 1);
    if (next < 0) next = dialog->button_count - 1;
    if (next >= dialog->button_count) next = 0;
    dialog->focus = (uint8_t)next;
    dialog->destructive_armed = 0;
    ++diagnostics.focus_moves;
    return true;
}

static bool close_active(nova_dialog_result_t selected,
                         nova_dialog_result_t *result, bool cancelled)
{
    nova_dialog_t *dialog = nova_dialog_active();
    if (!dialog || !result || selected == NOVA_DIALOG_RESULT_NONE) return false;
    dialog->state = NOVA_DIALOG_CLOSING;
    dialog->result = selected;
    if (dialog->secure) nova_dialog_credential_clear(dialog);
    dialog->state = NOVA_DIALOG_DESTROYED;
    *result = selected;
    if (cancelled) ++diagnostics.cancelled;
    ++diagnostics.closed;
    ++diagnostics.results;
    --diagnostics.depth;
    diagnostics.modal_active = false;
    for (uint8_t i = 0; i < diagnostics.depth; ++i)
        if (dialogs[i].modal && dialogs[i].state == NOVA_DIALOG_ACTIVE)
            diagnostics.modal_active = true;
    return true;
}

bool nova_dialog_activate(nova_dialog_result_t *result)
{
    nova_dialog_t *dialog = nova_dialog_active();
    if (!dialog || !dialog->button_count || !result) return false;
    if (dialog->focus == dialog->destructive_button && !dialog->destructive_armed) {
        dialog->destructive_armed = 1;
        return false;
    }
    return close_active(dialog->button_results[dialog->focus], result, false);
}

bool nova_dialog_cancel(nova_dialog_result_t *result)
{
    nova_dialog_t *dialog = nova_dialog_active();
    if (!dialog || !dialog->cancelable || !result) return false;
    return close_active(NOVA_DIALOG_RESULT_CANCEL, result, true);
}

bool nova_dialog_set_progress(nova_dialog_t *dialog, uint16_t per_mille)
{
    return nova_dialog_progress_update(dialog,per_mille,false,0);
}

bool nova_dialog_progress_update(nova_dialog_t *dialog, int32_t per_mille,
                                 bool indeterminate, const char *status)
{
    if (!dialog || dialog->type != NOVA_DIALOG_PROGRESS) return false;
    bool valid = per_mille >= 0 && per_mille <= 1000;
    if (per_mille < 0) per_mille = 0;
    if (per_mille > 1000) per_mille = 1000;
    dialog->progress_per_mille = (uint16_t)per_mille;
    dialog->progress_indeterminate = indeterminate;
    if (status) copy_text(dialog->progress_status,sizeof(dialog->progress_status),status);
    if (!valid) ++diagnostics.rejected;
    return valid;
}

bool nova_dialog_credential_configure(nova_dialog_t *dialog,
                                      nova_credential_type_t type)
{
    if (!dialog || dialog->type != NOVA_DIALOG_CREDENTIAL ||
        type > NOVA_CREDENTIAL_RECOVERY_KEY) return false;
    dialog->credential_type = type;
    dialog->secure = true;
    nova_dialog_credential_clear(dialog);
    return true;
}

static uint8_t encode_utf8(uint32_t cp, char bytes[4])
{
    if (cp < 0x20 || cp == 0x7f || cp > 0x10ffff ||
        (cp >= 0xd800 && cp <= 0xdfff)) return 0;
    if (cp < 0x80) { bytes[0]=(char)cp; return 1; }
    if (cp < 0x800) { bytes[0]=(char)(0xc0|(cp>>6)); bytes[1]=(char)(0x80|(cp&63)); return 2; }
    if (cp < 0x10000) { bytes[0]=(char)(0xe0|(cp>>12)); bytes[1]=(char)(0x80|((cp>>6)&63)); bytes[2]=(char)(0x80|(cp&63)); return 3; }
    bytes[0]=(char)(0xf0|(cp>>18)); bytes[1]=(char)(0x80|((cp>>12)&63));
    bytes[2]=(char)(0x80|((cp>>6)&63)); bytes[3]=(char)(0x80|(cp&63)); return 4;
}

bool nova_dialog_credential_input(nova_dialog_t *dialog, uint32_t codepoint)
{
    char encoded[4];
    uint8_t count;
    if (!dialog || dialog != nova_dialog_active() || !dialog->secure ||
        !(count=encode_utf8(codepoint,encoded)) ||
        dialog->credential_bytes + count >= NOVA_CREDENTIAL_CAPACITY) {
        ++diagnostics.rejected;
        return false;
    }
    for (uint8_t i=0;i<count;++i) dialog->credential[dialog->credential_bytes+i]=encoded[i];
    dialog->credential_bytes += count;
    dialog->credential[dialog->credential_bytes]=0;
    ++dialog->credential_glyphs;
    return true;
}

bool nova_dialog_credential_backspace(nova_dialog_t *dialog)
{
    if (!dialog || !dialog->secure || !dialog->credential_bytes) return false;
    uint16_t index = dialog->credential_bytes - 1;
    while (index && ((uint8_t)dialog->credential[index] & 0xc0u) == 0x80u) --index;
    for (uint16_t i=index;i<=dialog->credential_bytes;++i) dialog->credential[i]=0;
    dialog->credential_bytes=index;
    --dialog->credential_glyphs;
    return true;
}

const char *nova_dialog_credential_value(const nova_dialog_t *dialog)
{
    return dialog && dialog->secure ? dialog->credential : 0;
}

void nova_dialog_credential_clear(nova_dialog_t *dialog)
{
    if (!dialog) return;
    volatile char *secret = (volatile char *)dialog->credential;
    for (uint16_t i=0;i<NOVA_CREDENTIAL_CAPACITY;++i) secret[i]=0;
    dialog->credential_bytes=0;
    dialog->credential_glyphs=0;
}

const nova_dialog_diagnostics_t *nova_dialog_diagnostics(void)
{
    return &diagnostics;
}

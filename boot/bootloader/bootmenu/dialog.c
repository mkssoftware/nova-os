#include "dialog.h"
#include "input.h"

static nova_dialog_t dialogs[NOVA_DIALOG_CAPACITY];
static nova_dialog_diagnostics_t diagnostics;
static uint32_t next_id;
static nova_dialog_test_result_t test_results[NOVA_DIALOG_TEST_CAPACITY];
static nova_dialog_test_summary_t test_summary;
static nova_dialog_result_t last_confirmation_result;
static nova_dialog_result_t last_warning_result;

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
    last_confirmation_result=NOVA_DIALOG_RESULT_NONE;
    last_warning_result=NOVA_DIALOG_RESULT_NONE;
    next_id = 1;
}

nova_dialog_t *nova_confirmation_dialog_show(
    nova_confirmation_level_t level,const char *title,const char *message,
    const char *affirmative_label,nova_dialog_result_t affirmative_result,
    uint16_t saved_focus_id)
{
    if(level>NOVA_CONFIRM_SECURITY||!title||!*title||!message||!*message||
       !affirmative_label||!*affirmative_label||
       affirmative_result==NOVA_DIALOG_RESULT_NONE||
       affirmative_result==NOVA_DIALOG_RESULT_CANCEL||
       affirmative_result>NOVA_DIALOG_RESULT_CONTINUE){
        ++diagnostics.rejected;++diagnostics.confirmation_errors;return 0;
    }
    nova_dialog_t *dialog=nova_dialog_open(NOVA_DIALOG_CONFIRMATION,title,message,
                                            true,true,saved_focus_id);
    if(!dialog){++diagnostics.confirmation_errors;return 0;}
    dialog->confirmation_level=level;
    dialog->secure=level==NOVA_CONFIRM_SECURITY;
    /* Information defaults to its harmless acknowledgement. Every level that
       can change state inserts Cancel first, so focus can never land on the
       consequential action merely because the dialog opened. */
    bool buttons_ok=level==NOVA_CONFIRM_INFORMATION?
        (nova_dialog_add_button(dialog,affirmative_label,affirmative_result,false)&&
         nova_dialog_add_button(dialog,"Abbrechen",NOVA_DIALOG_RESULT_CANCEL,false)):
        (nova_dialog_add_button(dialog,"Abbrechen",NOVA_DIALOG_RESULT_CANCEL,false)&&
         nova_dialog_add_button(dialog,affirmative_label,affirmative_result,
                                level>=NOVA_CONFIRM_CRITICAL));
    if(!buttons_ok){
        nova_dialog_result_t ignored=NOVA_DIALOG_RESULT_NONE;
        (void)nova_dialog_close(dialog,NOVA_DIALOG_RESULT_CANCEL,&ignored);
        ++diagnostics.confirmation_errors;return 0;
    }
    dialog->focus=0;dialog->confirmation_validated=true;
    diagnostics.last_confirmation_level=level;
    copy_text(diagnostics.last_confirmation_title,
              sizeof(diagnostics.last_confirmation_title),title);
    last_confirmation_result=NOVA_DIALOG_RESULT_NONE;
    ++diagnostics.confirmations;
    return dialog;
}

nova_dialog_result_t nova_confirmation_dialog_result(void)
{return last_confirmation_result;}

nova_dialog_t *nova_warning_dialog_show(
    nova_warning_level_t level,nova_warning_icon_t icon,const char *title,
    const char *message,const char *recommendation,uint32_t warning_code,
    bool modal,uint16_t saved_focus_id)
{
    bool valid=level<=NOVA_WARNING_CRITICAL&&icon<NOVA_WARNING_ICON_COUNT&&
               title&&*title&&message&&*message;
    if(!valid){
        ++diagnostics.rejected;++diagnostics.warning_errors;
        level=NOVA_WARNING_MEDIUM;icon=NOVA_WARNING_GENERAL;
        title="Warnung";message="Die Warnungsdaten waren unvollständig oder beschädigt.";
        recommendation="Vorgang abbrechen und Diagnose ausführen.";warning_code=0;
    }
    nova_dialog_t *dialog=nova_dialog_open(NOVA_DIALOG_WARNING,title,message,
                                            modal,true,saved_focus_id);
    if(!dialog){++diagnostics.warning_errors;return 0;}
    dialog->warning_level=level;dialog->warning_icon=icon;
    dialog->warning_code=warning_code;
    copy_text(dialog->recommendation,sizeof(dialog->recommendation),recommendation);
    bool buttons_ok;
    if(level==NOVA_WARNING_LOW)
        buttons_ok=nova_dialog_add_button(dialog,"OK",NOVA_DIALOG_RESULT_OK,false);
    else buttons_ok=nova_dialog_add_button(dialog,"Abbrechen",NOVA_DIALOG_RESULT_CANCEL,false)&&
        nova_dialog_add_button(dialog,"Fortfahren",NOVA_DIALOG_RESULT_CONTINUE,
                               level==NOVA_WARNING_CRITICAL);
    if(!buttons_ok){
        nova_dialog_result_t ignored=NOVA_DIALOG_RESULT_NONE;
        (void)nova_dialog_close(dialog,NOVA_DIALOG_RESULT_CANCEL,&ignored);
        ++diagnostics.warning_errors;return 0;
    }
    dialog->focus=0;dialog->warning_validated=true;
    diagnostics.last_warning_level=level;diagnostics.last_warning_icon=icon;
    diagnostics.last_warning_code=warning_code;last_warning_result=NOVA_DIALOG_RESULT_NONE;
    ++diagnostics.warnings;return dialog;
}

nova_dialog_result_t nova_warning_dialog_result(void)
{return last_warning_result;}

void nova_dialog_tick(uint32_t elapsed_ms)
{diagnostics.clock_ms+=elapsed_ms;}

static bool dialog_transition(nova_dialog_t *dialog,nova_dialog_state_t next)
{
    if(!dialog||next>NOVA_DIALOG_DESTROYED||next!=(nova_dialog_state_t)(dialog->state+1)){
        ++diagnostics.rejected;return false;}
    dialog->state=next;dialog->lifecycle_mask|=1u<<(uint32_t)next;
    ++diagnostics.lifecycle_transitions;return true;
}

bool nova_dialog_create(nova_dialog_type_t type,bool modal,bool cancelable,
                        uint16_t saved_focus_id,nova_dialog_t **created)
{
    if(!created||type>NOVA_DIALOG_CUSTOM||diagnostics.depth>=NOVA_DIALOG_CAPACITY||
       (modal&&diagnostics.modal_active)||(!modal&&diagnostics.modal_active)){
        ++diagnostics.rejected;return false;}
    nova_dialog_t *dialog=&dialogs[diagnostics.depth++];
    *dialog=(nova_dialog_t){0};dialog->id=next_id++;dialog->root_view_id=dialog->id;
    dialog->navigation_context_id=dialog->id;dialog->type=type;
    dialog->state=NOVA_DIALOG_CREATED;dialog->lifecycle_mask=1u<<NOVA_DIALOG_CREATED;
    dialog->modal=modal;dialog->cancelable=cancelable;
    dialog->secure=type==NOVA_DIALOG_CREDENTIAL;dialog->saved_focus_id=saved_focus_id;
    dialog->destructive_button=0xffu;*created=dialog;return true;
}

bool nova_dialog_show(nova_dialog_t *dialog)
{
    if(!dialog||dialog!=nova_dialog_active()||dialog->state!=NOVA_DIALOG_CREATED||
       !dialog->root_view_id||!dialog->navigation_context_id){
        ++diagnostics.rejected;return false;}
    if(!dialog_transition(dialog,NOVA_DIALOG_INITIALIZED)||
       !dialog_transition(dialog,NOVA_DIALOG_OPENING)||
       !dialog_transition(dialog,NOVA_DIALOG_VISIBLE)||
       !dialog_transition(dialog,NOVA_DIALOG_ACTIVE))return false;
    dialog->opened_ms=diagnostics.clock_ms;
    if(dialog->modal){diagnostics.modal_active=true;diagnostics.backdrop_active=true;}
    else ++diagnostics.nonmodal_opened;
    ++diagnostics.opened;
    if(diagnostics.depth>diagnostics.peak_depth)diagnostics.peak_depth=diagnostics.depth;
    return true;
}

nova_dialog_t *nova_dialog_open(nova_dialog_type_t type, const char *title,
                                const char *message, bool modal, bool cancelable,
                                uint16_t saved_focus_id)
{
    nova_dialog_t *dialog=0;
    if(!nova_dialog_create(type,modal,cancelable,saved_focus_id,&dialog))return 0;
    /* A new dialog changes the active interaction scope. Captured pointers
       must not continue dragging an obscured control behind the modal layer. */
    nova_input_pointer_cancel_all();
    copy_text(dialog->title, sizeof(dialog->title), title);
    copy_text(dialog->message, sizeof(dialog->message), message);
    if(!nova_dialog_show(dialog)){--diagnostics.depth;return 0;}
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
    if (next < 0) {next = dialog->button_count - 1;++diagnostics.focus_traps;}
    if (next >= dialog->button_count) {next = 0;++diagnostics.focus_traps;}
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
    if(!nova_dialog_close(dialog,selected,result))return false;
    if (cancelled) ++diagnostics.cancelled;
    return true;
}

bool nova_dialog_close(nova_dialog_t *dialog,nova_dialog_result_t selected,
                       nova_dialog_result_t *result)
{
    if(!dialog||dialog!=nova_dialog_active()||!result||selected==NOVA_DIALOG_RESULT_NONE||
       dialog->state!=NOVA_DIALOG_ACTIVE){++diagnostics.rejected;return false;}
    if(!dialog_transition(dialog,NOVA_DIALOG_CLOSING))return false;
    dialog->result=selected;
    if(dialog->type==NOVA_DIALOG_CONFIRMATION){
        last_confirmation_result=selected;
        diagnostics.last_confirmation_result=selected;
        ++diagnostics.confirmation_decisions;
    }
    if(dialog->type==NOVA_DIALOG_WARNING){
        last_warning_result=selected;diagnostics.last_warning_result=selected;
        ++diagnostics.warning_decisions;
    }
    if(dialog->secure)nova_dialog_credential_clear(dialog);
    if(!dialog_transition(dialog,NOVA_DIALOG_DESTROYED))return false;
    dialog->closed_ms=diagnostics.clock_ms;*result=selected;
    ++diagnostics.closed;++diagnostics.results;--diagnostics.depth;
    diagnostics.modal_active=false;diagnostics.backdrop_active=false;
    for(uint8_t i=0;i<diagnostics.depth;++i)
        if(dialogs[i].modal&&dialogs[i].state==NOVA_DIALOG_ACTIVE){
            diagnostics.modal_active=true;diagnostics.backdrop_active=true;}
    return true;
}

nova_dialog_result_t nova_dialog_result(const nova_dialog_t *dialog)
{return dialog?dialog->result:NOVA_DIALOG_RESULT_NONE;}

bool nova_modal_dialog_active(void)
{return diagnostics.modal_active;}

bool nova_modal_dialog_backdrop_active(void)
{return diagnostics.modal_active&&diagnostics.backdrop_active;}

bool nova_modal_dialog_input_allowed(uint32_t dialog_id)
{
    if(!diagnostics.modal_active)return true;
    nova_dialog_t *dialog=nova_dialog_active();
    bool allowed=dialog&&dialog->modal&&dialog->id==dialog_id;
    if(!allowed)++diagnostics.blocked_inputs;
    return allowed;
}

bool nova_modal_dialog_navigation_allowed(void)
{
    if(!diagnostics.modal_active)return true;
    ++diagnostics.blocked_navigation;return false;
}

bool nova_modal_dialog_back(nova_dialog_result_t *result)
{
    ++diagnostics.back_events;
    nova_dialog_t *dialog=nova_dialog_active();
    if(!dialog||!dialog->modal||!result)return false;
    if(!dialog->cancelable){++diagnostics.blocked_navigation;return false;}
    return nova_dialog_cancel(result);
}

bool nova_dialog_activate(nova_dialog_result_t *result)
{
    nova_dialog_t *dialog = nova_dialog_active();
    if (!dialog || !dialog->button_count || !result) return false;
    if(dialog->type==NOVA_DIALOG_CONFIRMATION&&!dialog->confirmation_validated){
        ++diagnostics.rejected;++diagnostics.confirmation_errors;return false;
    }
    if(dialog->type==NOVA_DIALOG_WARNING&&!dialog->warning_validated){
        ++diagnostics.rejected;++diagnostics.warning_errors;return false;
    }
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

bool nova_dialog_test_initialize(void)
{
    for(uint8_t i=0;i<NOVA_DIALOG_TEST_CAPACITY;++i)
        test_results[i]=(nova_dialog_test_result_t){0};
    test_summary=(nova_dialog_test_summary_t){.initialized=true,.isolated=true,
        .deterministic=true,.configuration_unchanged=true};
    return true;
}

bool nova_dialog_test_execute(nova_dialog_type_t type)
{
    if(!test_summary.initialized||test_summary.count==NOVA_DIALOG_TEST_CAPACITY||
       type>NOVA_DIALOG_CUSTOM)return false;
    nova_dialog_t saved[NOVA_DIALOG_CAPACITY];
    for(uint8_t i=0;i<NOVA_DIALOG_CAPACITY;++i)saved[i]=dialogs[i];
    nova_dialog_diagnostics_t saved_diag=diagnostics;uint32_t saved_next=next_id;
    nova_dialog_test_result_t result={.dialog_type=type,.status=NOVA_DIALOG_TEST_FAILED,
        .duration_us=0,.return_focus=true,.animation_observed=true};
    nova_dialog_t *dialog=nova_dialog_open(type,"Dialogtest","Isolierter Dialogtest",
        true,true,77);
    if(!dialog){result.detected_errors|=1u;goto restore;}
    result.dialog_id=dialog->id;result.event_count=1;
    if(dialog->state!=NOVA_DIALOG_ACTIVE||!diagnostics.modal_active||
       !dialog->root_view_id||!dialog->navigation_context_id||
       dialog->lifecycle_mask!=0x1fu)
        result.detected_errors|=2u;
    result.modal_trap=diagnostics.modal_active;
    if(!nova_dialog_add_button(dialog,"Abbrechen",NOVA_DIALOG_RESULT_CANCEL,false)||
       !nova_dialog_add_button(dialog,"OK",NOVA_DIALOG_RESULT_OK,false))
        result.detected_errors|=4u;
    result.event_count+=2;
    if(!nova_dialog_focus_move(1)||dialog->focus!=1||!nova_dialog_focus_move(1)||
       dialog->focus!=0||!nova_dialog_focus_move(-1)||dialog->focus!=1)
        result.detected_errors|=8u;
    else result.focus_path=0x101u;
    result.keyboard=result.focus_path!=0;
    if(type==NOVA_DIALOG_PROGRESS&&
       (!nova_dialog_progress_update(dialog,500,false,"50 Prozent")||
        dialog->progress_per_mille!=500))result.detected_errors|=16u;
    if(type==NOVA_DIALOG_CREDENTIAL){
        if(!nova_dialog_credential_configure(dialog,NOVA_CREDENTIAL_PASSWORD)||
           !nova_dialog_credential_input(dialog,'N')||
           !nova_dialog_credential_input(dialog,0x00e4)||
           dialog->credential_glyphs!=2)result.detected_errors|=32u;
        else result.security=true;
    }else result.security=true;
    nova_dialog_result_t close_result=NOVA_DIALOG_RESULT_NONE;
    if(!nova_dialog_cancel(&close_result)||close_result!=NOVA_DIALOG_RESULT_CANCEL||
       nova_dialog_active()||diagnostics.modal_active||dialog->lifecycle_mask!=0x7fu)
       result.detected_errors|=64u;
    else result.escape=true;
    if(type==NOVA_DIALOG_CREDENTIAL&&dialog->credential_bytes)
        result.detected_errors|=128u;
    result.event_count+=5;
    if(!result.detected_errors)result.status=NOVA_DIALOG_TEST_PASSED;
restore:
    for(uint8_t i=0;i<NOVA_DIALOG_CAPACITY;++i)dialogs[i]=saved[i];
    diagnostics=saved_diag;next_id=saved_next;
    test_results[test_summary.count++]=result;
    if(result.status==NOVA_DIALOG_TEST_PASSED)++test_summary.passed;
    else if(result.status==NOVA_DIALOG_TEST_SKIPPED)++test_summary.skipped;
    else ++test_summary.failed;
    test_summary.configuration_unchanged=diagnostics.depth==saved_diag.depth&&
        diagnostics.modal_active==saved_diag.modal_active&&next_id==saved_next;
    return result.status==NOVA_DIALOG_TEST_PASSED;
}

const nova_dialog_test_result_t *nova_dialog_test_results(void){return test_results;}
const nova_dialog_test_summary_t *nova_dialog_test_summary(void){return &test_summary;}

static bool report_append(uint8_t *out,uint32_t cap,uint32_t *pos,const char *text)
{while(*text){if(*pos+1>=cap)return false;out[(*pos)++]=(uint8_t)*text++;}out[*pos]=0;return true;}
static bool report_u32(uint8_t *out,uint32_t cap,uint32_t *pos,uint32_t value)
{char digits[10];uint8_t n=0;do{digits[n++]=(char)('0'+value%10u);value/=10u;}while(value&&n<10);
 while(n){if(*pos+1>=cap)return false;out[(*pos)++]=(uint8_t)digits[--n];}out[*pos]=0;return true;}
bool nova_dialog_test_generate_report(bool authorized,uint8_t *output,
                                      uint32_t capacity,uint32_t *written)
{
    if(written)*written=0;
    if(!authorized||!output||capacity<128||!written||
       !test_summary.initialized)return false;
    uint32_t p=0;
    if(!report_append(output,capacity,&p,"NOVA_DIALOG_TEST_REPORT\ncount=")||
       !report_u32(output,capacity,&p,test_summary.count)||
       !report_append(output,capacity,&p," passed=")||
       !report_u32(output,capacity,&p,test_summary.passed)||
       !report_append(output,capacity,&p," failed=")||
       !report_u32(output,capacity,&p,test_summary.failed)||
       !report_append(output,capacity,&p,"\n"))return false;
    for(uint8_t i=0;i<test_summary.count;++i){const nova_dialog_test_result_t *r=&test_results[i];
        if(!report_append(output,capacity,&p,"type=")||!report_u32(output,capacity,&p,r->dialog_type)||
           !report_append(output,capacity,&p," status=")||!report_u32(output,capacity,&p,r->status)||
           !report_append(output,capacity,&p," errors=")||!report_u32(output,capacity,&p,r->detected_errors)||
           !report_append(output,capacity,&p," events=")||!report_u32(output,capacity,&p,r->event_count)||
           !report_append(output,capacity,&p,"\n"))return false;}
    *written=p;++test_summary.reports;return true;
}

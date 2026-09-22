#include "boot_control.h"
#include "../bootmenu/graphics.h"

#define EFI_VARIABLE_NON_VOLATILE 1u
#define EFI_VARIABLE_BOOTSERVICE_ACCESS 2u
#define EFI_VARIABLE_RUNTIME_ACCESS 4u

static EFI_RUNTIME_SERVICES *runtime;
static nova_boot_control_record_t current;
static bool persistent;
static bool recovery_required;
static EFI_GUID nova_boot_control_guid={0x4e4f5641u,0x4243u,0x4f4eu,
    {0x54u,0x52u,0x4fu,0x4cu,0x56u,0x31u,0x00u,0x01u}};
static CHAR16 state0_name[]={'N','o','v','a','B','o','o','t','S','t','a','t','e','0',0};
static CHAR16 state1_name[]={'N','o','v','a','B','o','o','t','S','t','a','t','e','1',0};

static void bytes_zero(void *target,UINTN length)
{uint8_t *p=(uint8_t *)target;while(length--)*p++=0;}

static uint32_t record_crc(const nova_boot_control_record_t *record)
{
    const uint8_t *bytes=(const uint8_t *)record;uint32_t crc=0xffffffffu;
    UINTN zero_offset=(UINTN)((const uint8_t *)&record->checksum-bytes);
    for(UINTN i=0;i<sizeof(*record);++i){
        uint8_t value=(i>=zero_offset&&i<zero_offset+4u)?0u:bytes[i];crc^=value;
        for(uint32_t bit=0;bit<8u;++bit)crc=(crc>>1)^((crc&1u)?0xedb88320u:0u);
    }
    return ~crc;
}

static void seal(nova_boot_control_record_t *record)
{record->checksum=0;record->checksum=record_crc(record);}

void nova_boot_control_default(nova_boot_control_record_t *record)
{
    static const uint8_t magic[8]={'N','O','V','A','B','C','T','L'};
    bytes_zero(record,sizeof(*record));
    for(UINTN i=0;i<8u;++i)record->magic[i]=magic[i];
    record->version=NOVA_BOOT_CONTROL_VERSION;record->size=sizeof(*record);
    record->active_slot=0;record->candidate_slot=NOVA_BOOT_CONTROL_NO_SLOT;
    record->known_good_slot=0;record->max_attempts=NOVA_BOOT_CONTROL_DEFAULT_MAX_ATTEMPTS;
    seal(record);
}

bool nova_boot_control_validate(const nova_boot_control_record_t *record)
{
    static const uint8_t magic[8]={'N','O','V','A','B','C','T','L'};
    if(!record)return false;
    for(UINTN i=0;i<8u;++i)if(record->magic[i]!=magic[i])return false;
    if(record->version!=NOVA_BOOT_CONTROL_VERSION||record->size!=sizeof(*record)||
       record->active_slot>1u||record->known_good_slot>1u||
       (record->candidate_slot!=NOVA_BOOT_CONTROL_NO_SLOT&&record->candidate_slot>1u)||
       !record->max_attempts||record->max_attempts>16u||record->attempt_count>record->max_attempts||
       record->last_result>NOVA_BOOT_RESULT_ATTEMPT_LIMIT||
       record->last_milestone>NOVA_BOOT_MILESTONE_OPERATIONAL)return false;
    return record_crc(record)==record->checksum;
}

static bool sequence_is_newer(uint64_t candidate,uint64_t reference)
{
    uint64_t distance=candidate-reference;
    return distance!=0u&&distance<(UINT64_C(1)<<63);
}

bool nova_boot_control_choose_newest(const nova_boot_control_record_t *copy_a,
                                     const nova_boot_control_record_t *copy_b,
                                     nova_boot_control_record_t *selected)
{
    if(!selected)return false;
    bool valid_a=nova_boot_control_validate(copy_a);
    bool valid_b=nova_boot_control_validate(copy_b);
    if(!valid_a&&!valid_b)return false;
    const nova_boot_control_record_t *choice=!valid_a?copy_b:
        (!valid_b||!sequence_is_newer(copy_b->sequence,copy_a->sequence)?copy_a:copy_b);
    *selected=*choice;return true;
}

bool nova_boot_control_prepare_candidate(nova_boot_control_record_t *record,uint32_t slot,
                                         uint32_t max_attempts)
{
    if(!nova_boot_control_validate(record)||slot>1u||slot==record->known_good_slot||
       !max_attempts||max_attempts>16u)return false;
    record->candidate_slot=slot;record->attempt_count=0;record->max_attempts=max_attempts;
    record->last_result=NOVA_BOOT_RESULT_UNKNOWN;record->last_milestone=NOVA_BOOT_MILESTONE_NONE;
    seal(record);return true;
}

uint32_t nova_boot_control_select(nova_boot_control_record_t *record,bool *state_changed)
{
    if(state_changed)*state_changed=false;
    if(!nova_boot_control_validate(record))return 0;
    if(record->candidate_slot!=NOVA_BOOT_CONTROL_NO_SLOT){
        if(record->attempt_count<record->max_attempts)return record->candidate_slot;
        record->candidate_slot=NOVA_BOOT_CONTROL_NO_SLOT;record->active_slot=record->known_good_slot;
        record->last_result=NOVA_BOOT_RESULT_ATTEMPT_LIMIT;record->last_milestone=NOVA_BOOT_MILESTONE_NONE;
        seal(record);if(state_changed)*state_changed=true;
        return record->known_good_slot;
    }
    return record->active_slot;
}

bool nova_boot_control_begin_attempt(nova_boot_control_record_t *record,uint32_t slot)
{
    if(!nova_boot_control_validate(record)||slot>1u)return false;
    if(record->candidate_slot==slot){
        if(record->attempt_count>=record->max_attempts)return false;
        ++record->attempt_count;
    }
    record->active_slot=slot;record->last_result=NOVA_BOOT_RESULT_PENDING;
    record->last_milestone=NOVA_BOOT_MILESTONE_BOOTLOADER_STARTED;seal(record);return true;
}

bool nova_boot_control_artifact_failed(nova_boot_control_record_t *record,uint32_t slot)
{
    if(!nova_boot_control_validate(record)||slot>1u)return false;
    if(record->candidate_slot==slot)record->candidate_slot=NOVA_BOOT_CONTROL_NO_SLOT;
    record->active_slot=record->known_good_slot;record->last_result=NOVA_BOOT_RESULT_ARTIFACT_INVALID;
    record->last_milestone=NOVA_BOOT_MILESTONE_BOOTLOADER_STARTED;seal(record);return true;
}

static bool read_copy(CHAR16 *name,nova_boot_control_record_t *out)
{
    if(!runtime||!runtime->GetVariable)return false;
    UINTN size=sizeof(*out);uint32_t attributes=0;
    return !EFI_ERROR(runtime->GetVariable(name,&nova_boot_control_guid,&attributes,&size,out))&&
           size==sizeof(*out);
}

static bool write_current(void)
{
    if(!runtime||!runtime->SetVariable)return false;
    ++current.sequence;seal(&current);
    CHAR16 *name=(current.sequence&1u)?state1_name:state0_name;
    EFI_STATUS status=runtime->SetVariable(name,&nova_boot_control_guid,
        EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS,
        sizeof(current),&current);
    if(EFI_ERROR(status))return false;
    nova_boot_control_record_t check;
    return read_copy(name,&check)&&nova_boot_control_validate(&check)&&
           check.sequence==current.sequence&&check.checksum==current.checksum;
}

bool uefi_boot_control_initialize(EFI_SYSTEM_TABLE *system_table)
{
    runtime=system_table?system_table->RuntimeServices:0;persistent=false;recovery_required=false;
    nova_boot_control_record_t a,b;bool have_a=read_copy(state0_name,&a),have_b=read_copy(state1_name,&b);
    if(nova_boot_control_choose_newest(have_a?&a:0,have_b?&b:0,&current)){persistent=true;
        if((have_a&&!nova_boot_control_validate(&a))||(have_b&&!nova_boot_control_validate(&b)))
            nova_debug_string("UEFI:BOOT-CONTROL-INVALID-COPY-IGNORED\n");
        nova_debug_string("UEFI:BOOT-CONTROL-RESTORED\n");}
    else if(have_a||have_b){nova_boot_control_default(&current);recovery_required=true;
        nova_debug_string("UEFI:BOOT-CONTROL-CORRUPT-RECOVERY\n");}
    else{nova_boot_control_default(&current);
        bool first_copy=write_current();bool second_copy=first_copy&&write_current();
        persistent=first_copy&&second_copy;
        nova_debug_string(persistent?"UEFI:BOOT-CONTROL-INITIALIZED\n":"UEFI:BOOT-CONTROL-VOLATILE\n");}
    bool changed=false;(void)nova_boot_control_select(&current,&changed);
    if(changed){persistent=write_current();nova_debug_string("UEFI:BOOT-CONTROL-ATTEMPT-LIMIT-ROLLBACK\n");}
    nova_debug_string("UEFI:BOOT-CONTROL-READY\n");return true;
}

const nova_boot_control_record_t *uefi_boot_control_state(void){return &current;}
bool uefi_boot_control_persistent(void){return persistent;}
bool uefi_boot_control_requires_recovery(void){return recovery_required;}
uint32_t uefi_boot_control_select(void){bool changed=false;uint32_t slot=nova_boot_control_select(&current,&changed);if(changed)persistent=write_current();return slot;}
bool uefi_boot_control_begin_attempt(uint32_t slot){if(!nova_boot_control_begin_attempt(&current,slot))return false;persistent=write_current();return true;}
bool uefi_boot_control_artifact_failed(uint32_t slot){if(!nova_boot_control_artifact_failed(&current,slot))return false;persistent=write_current();return true;}

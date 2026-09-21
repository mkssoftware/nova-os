#include "../boot/bootloader/uefi/boot_control.h"
#include <stdio.h>

static uint32_t test_crc(const nova_boot_control_record_t *record)
{
    const uint8_t *bytes=(const uint8_t *)record;uint32_t crc=0xffffffffu;
    size_t zero_offset=(size_t)((const uint8_t *)&record->checksum-bytes);
    for(size_t i=0;i<sizeof(*record);++i){
        uint8_t value=(i>=zero_offset&&i<zero_offset+4u)?0u:bytes[i];crc^=value;
        for(uint32_t bit=0;bit<8u;++bit)crc=(crc>>1)^((crc&1u)?0xedb88320u:0u);
    }
    return ~crc;
}

static void set_sequence(nova_boot_control_record_t *record,uint64_t sequence)
{
    record->sequence=sequence;record->checksum=0;record->checksum=test_crc(record);
}

static int check(bool condition,const char *message)
{
    if(condition)return 0;
    fprintf(stderr,"Boot-Control-Test fehlgeschlagen: %s\n",message);return 1;
}

int main(void)
{
    nova_boot_control_record_t state,copy_a,copy_b,selected;bool changed=false;
    nova_boot_control_default(&state);
    if(check(nova_boot_control_validate(&state),"Defaultzustand ungueltig")||
       check(nova_boot_control_select(&state,&changed)==0&&!changed,"Defaultwahl ist nicht Slot A")||
       check(nova_boot_control_prepare_candidate(&state,1,2),"Slot B nicht als Candidate setzbar")||
       check(nova_boot_control_select(&state,&changed)==1&&!changed,"Candidate B nicht bevorzugt")||
       check(nova_boot_control_begin_attempt(&state,1)&&state.attempt_count==1,"erster Attempt fehlt")||
       check(nova_boot_control_begin_attempt(&state,1)&&state.attempt_count==2,"zweiter Attempt fehlt")||
       check(nova_boot_control_select(&state,&changed)==0&&changed,"Attemptlimit rollt nicht auf A zurueck")||
       check(state.candidate_slot==NOVA_BOOT_CONTROL_NO_SLOT&&
             state.last_result==NOVA_BOOT_RESULT_ATTEMPT_LIMIT,"Rollbackzustand unvollstaendig")||
       check(nova_boot_control_prepare_candidate(&state,1,3),"zweiter Candidate nicht setzbar")||
       check(nova_boot_control_artifact_failed(&state,1),"Artefaktfehler nicht speicherbar")||
       check(state.active_slot==0&&state.candidate_slot==NOVA_BOOT_CONTROL_NO_SLOT&&
             state.last_result==NOVA_BOOT_RESULT_ARTIFACT_INVALID,"Artefaktfehler rollt nicht zurueck"))return 1;
    nova_boot_control_default(&copy_a);copy_b=copy_a;
    set_sequence(&copy_a,41);set_sequence(&copy_b,42);
    if(check(nova_boot_control_choose_newest(&copy_a,&copy_b,&selected)&&selected.sequence==42,
             "neueste redundante Kopie nicht gewaehlt"))return 1;
    copy_b.reserved[0]^=0x5a;
    if(check(nova_boot_control_choose_newest(&copy_a,&copy_b,&selected)&&selected.sequence==41,
             "beschaedigte neuere Kopie verdraengt letzte gueltige Kopie"))return 1;
    copy_a.reserved[0]^=0x5a;
    if(check(!nova_boot_control_choose_newest(&copy_a,&copy_b,&selected),
             "zwei beschaedigte Kopien wurden akzeptiert"))return 1;
    nova_boot_control_default(&copy_a);copy_b=copy_a;
    set_sequence(&copy_a,UINT64_MAX);set_sequence(&copy_b,0);
    if(check(nova_boot_control_choose_newest(&copy_a,&copy_b,&selected)&&selected.sequence==0,
             "Sequenzueberlauf waehlt nicht die neue Kopie"))return 1;
    state.reserved[0]^=0x5a;
    if(check(!nova_boot_control_validate(&state),"CRC erkennt Korruption nicht"))return 1;
    puts("UEFI Boot-Control-Zustandsautomat erfolgreich");return 0;
}

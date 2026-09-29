#include "../boot/bootloader/uefi/boot_control.h"
#include <stdio.h>
#include <string.h>

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

static void reseal(nova_boot_control_record_t *record)
{
    record->checksum=0;record->checksum=test_crc(record);
}

static int check(bool condition,const char *message)
{
    if(condition)return 0;
    fprintf(stderr,"Boot-Control-Test fehlgeschlagen: %s\n",message);return 1;
}

static int check_candidate_cycle(uint32_t known_good,uint32_t candidate,uint32_t max_attempts)
{
    nova_boot_control_record_t state,mirror;nova_boot_control_default(&state);
    state.active_slot=known_good;state.known_good_slot=known_good;reseal(&state);
    if(check(nova_boot_control_prepare_candidate(&state,candidate,max_attempts),
             "Candidate-Zyklus konnte nicht vorbereitet werden"))return 1;
    for(uint32_t attempt=0;attempt<max_attempts;++attempt){
        bool changed_a=false,changed_b=false;mirror=state;
        uint32_t selected_a=nova_boot_control_select(&state,&changed_a);
        uint32_t selected_b=nova_boot_control_select(&mirror,&changed_b);
        if(check(selected_a==candidate&&!changed_a,
                 "Candidate wurde vor dem Versuchslimit nicht ausgewaehlt")||
           check(selected_a==selected_b&&changed_a==changed_b&&memcmp(&state,&mirror,sizeof(state))==0,
                 "identischer Boot-State fuehrt nicht zur deterministischen Auswahl")||
           check(nova_boot_control_begin_attempt(&state,candidate),
                 "gueltiger Candidate-Versuch wurde abgelehnt")||
           check(state.attempt_count==attempt+1u,"Versuch wurde nicht genau einmal gezaehlt"))return 1;
    }
    bool changed=false;
    if(check(nova_boot_control_select(&state,&changed)==known_good&&changed,
             "Versuchslimit rollt nicht auf Known-Good zurueck")||
       check(state.active_slot==known_good&&state.candidate_slot==NOVA_BOOT_CONTROL_NO_SLOT&&
             state.last_result==NOVA_BOOT_RESULT_ATTEMPT_LIMIT,
             "Limit-Rollback hinterlaesst keinen eindeutigen Zustand"))return 1;

    nova_boot_control_default(&state);state.active_slot=known_good;state.known_good_slot=known_good;reseal(&state);
    if(check(nova_boot_control_prepare_candidate(&state,candidate,max_attempts),
             "Artefaktfall konnte nicht vorbereitet werden")||
       check(nova_boot_control_artifact_failed(&state,candidate),
             "eindeutig ungueltiger Candidate wurde nicht deaktiviert")||
       check(state.active_slot==known_good&&state.candidate_slot==NOVA_BOOT_CONTROL_NO_SLOT&&
             state.last_result==NOVA_BOOT_RESULT_ARTIFACT_INVALID,
             "Artefakt-Rollback waehlt nicht Known-Good"))return 1;
    return 0;
}

static int check_health_cycle(void)
{
    nova_boot_control_record_t state,before;nova_boot_control_default(&state);
    nova_boot_health_policy_t policy={NOVA_BOOT_HEALTH_REQUIRED_DESKTOP,false};
    if(check(nova_boot_control_prepare_candidate(&state,1,3),"Health-Candidate fehlt")||
       check(state.slot_generation[1]==2,"Candidate-Generation ist nicht eindeutig")||
       check(nova_boot_control_begin_attempt(&state,1),"Health-Attempt fehlt"))return 1;
    nova_boot_health_evidence_t evidence={
        .slot=1,.generation=state.slot_generation[1],.boot_attempt=state.attempt_count,
        .reached_milestones=NOVA_BOOT_MILESTONE_BIT(NOVA_BOOT_MILESTONE_BOOTLOADER_STARTED)|
            NOVA_BOOT_MILESTONE_BIT(NOVA_BOOT_MILESTONE_KERNEL_ENTERED),
        .failed_milestone=NOVA_BOOT_MILESTONE_NONE,.status=NOVA_BOOT_HEALTH_PENDING,
        .trust_verified=true
    };
    if(check(nova_boot_control_apply_health(&state,&policy,&evidence,true),
             "autorisiertes KernelEntered-Milestone abgelehnt")||
       check(state.last_milestone==NOVA_BOOT_MILESTONE_KERNEL_ENTERED&&
             state.candidate_slot==1,"Kernel Entry hat Candidate falsch committed"))return 1;
    before=state;evidence.status=NOVA_BOOT_HEALTH_HEALTHY;
    if(check(!nova_boot_control_apply_health(&state,&policy,&evidence,true),
             "unvollstaendige Milestones wurden als Healthy akzeptiert")||
       check(memcmp(&state,&before,sizeof(state))==0,"abgelehnter Health-Commit mutiert Zustand"))return 1;
    evidence.reached_milestones=NOVA_BOOT_MILESTONE_BIT(NOVA_BOOT_MILESTONE_BOOTLOADER_STARTED)|
        NOVA_BOOT_HEALTH_REQUIRED_DESKTOP;
    if(check(!nova_boot_control_apply_health(&state,&policy,&evidence,false),
             "nicht autorisierter Health-Commit wurde akzeptiert")||
       check(memcmp(&state,&before,sizeof(state))==0,"Autorisierungsfehler mutiert Zustand"))return 1;
    evidence.boot_attempt++;
    if(check(!nova_boot_control_apply_health(&state,&policy,&evidence,true),
             "Evidence eines anderen Attempts wurde wiederverwendet"))return 1;
    evidence.boot_attempt--;evidence.generation++;
    if(check(!nova_boot_control_apply_health(&state,&policy,&evidence,true),
             "Evidence einer anderen Generation wurde wiederverwendet"))return 1;
    evidence.generation--;evidence.trust_verified=false;
    if(check(!nova_boot_control_apply_health(&state,&policy,&evidence,true),
             "Health ohne Trust wurde committed"))return 1;
    evidence.trust_verified=true;
    if(check(nova_boot_control_apply_health(&state,&policy,&evidence,true),
             "vollstaendiger autorisierter Health-Commit abgelehnt")||
       check(state.known_good_slot==1&&state.active_slot==1&&
             state.candidate_slot==NOVA_BOOT_CONTROL_NO_SLOT&&
             state.last_result==NOVA_BOOT_RESULT_HEALTH_CONFIRMED&&
             state.last_milestone==NOVA_BOOT_MILESTONE_HEALTH_CONFIRMED,
             "Health-Commit erzeugt keinen eindeutigen Known-Good-Zustand"))return 1;

    if(check(nova_boot_control_prepare_candidate(&state,0,2),"zweiter Health-Candidate fehlt")||
       check(state.slot_generation[0]==3,"neue Candidate-Generation wurde wiederverwendet")||
       check(nova_boot_control_begin_attempt(&state,0),"zweiter Health-Attempt fehlt"))return 1;
    evidence=(nova_boot_health_evidence_t){
        .slot=0,.generation=state.slot_generation[0],.boot_attempt=state.attempt_count,
        .reached_milestones=NOVA_BOOT_MILESTONE_BIT(NOVA_BOOT_MILESTONE_BOOTLOADER_STARTED)|
            NOVA_BOOT_MILESTONE_BIT(NOVA_BOOT_MILESTONE_KERNEL_ENTERED),
        .failed_milestone=NOVA_BOOT_MILESTONE_KERNEL_INITIALIZED,
        .status=NOVA_BOOT_HEALTH_TIMED_OUT,.trust_verified=true
    };
    if(check(nova_boot_control_apply_health(&state,&policy,&evidence,true),
             "Health-Timeout wurde nicht gespeichert")||
       check(state.last_result==NOVA_BOOT_RESULT_HEALTH_TIMED_OUT&&
             state.last_milestone==NOVA_BOOT_MILESTONE_KERNEL_INITIALIZED&&
             state.candidate_slot==0,"Timeout hat Candidate unkontrolliert committed"))return 1;
    if(check(nova_boot_control_begin_attempt(&state,0),
             "Retry nach Health-Timeout wurde abgelehnt"))return 1;
    evidence.boot_attempt=state.attempt_count;
    evidence.status=NOVA_BOOT_HEALTH_DEGRADED;
    evidence.failed_milestone=NOVA_BOOT_MILESTONE_NONE;
    evidence.reached_milestones=NOVA_BOOT_MILESTONE_BIT(NOVA_BOOT_MILESTONE_BOOTLOADER_STARTED)|
        NOVA_BOOT_HEALTH_REQUIRED_DESKTOP;
    if(check(!nova_boot_control_apply_health(&state,&policy,&evidence,true),
             "Degraded wurde entgegen Policy committed"))return 1;
    policy.degraded_may_commit=true;
    if(check(nova_boot_control_apply_health(&state,&policy,&evidence,true),
             "policyerlaubter Degraded-Commit wurde abgelehnt")||
       check(state.last_result==NOVA_BOOT_RESULT_HEALTH_DEGRADED&&state.known_good_slot==0,
             "Degraded-Commit wurde nicht nachvollziehbar gespeichert"))return 1;
    return 0;
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
    for(uint32_t max_attempts=1;max_attempts<=16u;++max_attempts){
        if(check_candidate_cycle(0,1,max_attempts)||check_candidate_cycle(1,0,max_attempts))return 1;
    }
    if(check_health_cycle())return 1;
    nova_boot_control_default(&state);copy_a=state;
    if(check(!nova_boot_control_prepare_candidate(&state,0,2),
             "Known-Good wurde unzulaessig als Candidate akzeptiert")||
       check(!nova_boot_control_prepare_candidate(&state,2,2),
             "ungueltiger Slot wurde als Candidate akzeptiert")||
       check(!nova_boot_control_prepare_candidate(&state,1,0),
             "Versuchslimit null wurde akzeptiert")||
       check(!nova_boot_control_prepare_candidate(&state,1,17),
             "zu grosses Versuchslimit wurde akzeptiert")||
       check(!nova_boot_control_begin_attempt(&state,2),
             "Versuch fuer ungueltigen Slot wurde akzeptiert")||
       check(memcmp(&state,&copy_a,sizeof(state))==0,
             "abgelehnter Uebergang hat den Zustand veraendert"))return 1;
    nova_boot_control_default(&copy_a);copy_b=copy_a;
    set_sequence(&copy_a,41);set_sequence(&copy_b,42);
    if(check(nova_boot_control_choose_newest(&copy_a,&copy_b,&selected)&&selected.sequence==42,
             "neueste redundante Kopie nicht gewaehlt"))return 1;
    copy_b.slot_generation[0]^=0x5a;
    if(check(nova_boot_control_choose_newest(&copy_a,&copy_b,&selected)&&selected.sequence==41,
             "beschaedigte neuere Kopie verdraengt letzte gueltige Kopie"))return 1;
    copy_a.slot_generation[0]^=0x5a;
    if(check(!nova_boot_control_choose_newest(&copy_a,&copy_b,&selected),
             "zwei beschaedigte Kopien wurden akzeptiert"))return 1;
    nova_boot_control_default(&copy_a);copy_b=copy_a;
    set_sequence(&copy_a,UINT64_MAX);set_sequence(&copy_b,0);
    if(check(nova_boot_control_choose_newest(&copy_a,&copy_b,&selected)&&selected.sequence==0,
             "Sequenzueberlauf waehlt nicht die neue Kopie"))return 1;
    state.slot_generation[0]^=0x5a;
    if(check(!nova_boot_control_validate(&state),"CRC erkennt Korruption nicht"))return 1;
    puts("UEFI Boot-Control-Zustandsautomat erfolgreich");return 0;
}

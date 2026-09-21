#include "../boot/bootloader/uefi/boot_control.h"
#include <stdio.h>

static int check(bool condition,const char *message)
{
    if(condition)return 0;
    fprintf(stderr,"Boot-Control-Test fehlgeschlagen: %s\n",message);return 1;
}

int main(void)
{
    nova_boot_control_record_t state;bool changed=false;
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
    state.reserved[0]^=0x5a;
    if(check(!nova_boot_control_validate(&state),"CRC erkennt Korruption nicht"))return 1;
    puts("UEFI Boot-Control-Zustandsautomat erfolgreich");return 0;
}

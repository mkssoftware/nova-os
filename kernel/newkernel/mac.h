/* NovaOS C-Kernel - Nova Mandatory Access Control (Port aus security32.inc,
   NPSPEC-SECURITY-MAC-0001). Sensitivity-Labels fuer Subjects/Objects;
   Dominanz-Check: Subject muss Objekt dominieren (Label >= Objekt-Label). */
#ifndef NOVA_MAC_H
#define NOVA_MAC_H

#include <stdint.h>

#define MAC_SUBJECT_COUNT 8u
#define MAC_OBJECT_COUNT  8u

#define MAC_LABEL_PUBLIC       0u
#define MAC_LABEL_INTERNAL     1u
#define MAC_LABEL_CONFIDENTIAL 2u
#define MAC_LABEL_SECRET       3u

void mac_initialize(void);
int mac_self_test(void); /* 0 = ok */

int mac_register_subject(uint32_t subject_id, uint32_t label); /* 0 = Erfolg, -1 wenn voll */
int mac_register_object(uint32_t object_id, uint32_t label);   /* 0 = Erfolg, -1 wenn voll */

/* 0 = erlaubt (subject_label >= object_label), -1 = verboten/unbekannt. */
int mac_check(uint32_t subject_id, uint32_t object_id);

#endif

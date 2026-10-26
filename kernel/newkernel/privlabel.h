/* NovaOS C-Kernel - Nova Privacy Label Policy (Port aus adaptive32.inc,
   NPSPEC-PRIVACY-LABEL-0001). Datenobjekte tragen Privacy-Labels
   (PUBLIC/SENSITIVE/PRIVATE/CONFIDENTIAL), ergaenzen MAC-Labels um eine
   Datenschutz-Dimension. Privacy Label != Access Control Label - getrennte
   Policy-Ebene, NICHT zu verwechseln mit mac.c's MAC-Labels. */
#ifndef NOVA_PRIVLABEL_H
#define NOVA_PRIVLABEL_H

#include <stdint.h>

#define PRIV_OBJ_COUNT 16u

#define PRIV_LABEL_PUBLIC    0u
#define PRIV_LABEL_SENSITIVE 1u
#define PRIV_LABEL_PRIVATE   2u
#define PRIV_LABEL_CONF      3u

void privlabel_initialize(void);
int privlabel_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn label > PRIV_LABEL_CONF oder Tabelle voll. */
int privlabel_register(uint32_t obj_id, uint32_t label, uint32_t retention);

/* 0 = erlaubt (Label <= required_label), -1 = verweigert oder obj_id
   unbekannt. */
int privlabel_check(uint32_t obj_id, uint32_t required_label);

#endif

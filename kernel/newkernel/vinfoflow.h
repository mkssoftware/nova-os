/* NovaOS C-Kernel - Nova Information Flow Verifier (Port aus
   adaptive32.inc, NPSPEC-VERIFY-INFORMATIONFLOW-0001). Label-basierte
   Informationsflusskontrolle: jeder Kanal hat ein Sicherheitslabel
   (0=public, hoeher=sensibler); Schreibvorgaenge von hoeherem Label zu
   niedrigerem werden verweigert und ueber `security_audit_log` gemeldet
   (echte Cross-Modul-Verdrahtung mit audit.c, 1:1 wie im alten ASM). */
#ifndef NOVA_VINFOFLOW_H
#define NOVA_VINFOFLOW_H

#include <stdint.h>

#define VINFO_CAPACITY 8u

void vinfoflow_initialize(void);
int vinfoflow_self_test(void); /* 0 = ok */

/* 0 = Erfolg, -1 wenn chan_id==0 oder Tabelle voll. */
int vinfoflow_register(uint32_t chan_id, uint32_t label, uint32_t owner);

/* 0 = erlaubt (src.label <= dst.label), -1 = verweigert (Violation
   gezaehlt, security_audit_log aufgerufen). */
int vinfoflow_check(uint32_t src_chan_id, uint32_t dst_chan_id);

#endif

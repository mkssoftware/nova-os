/* NovaOS C-Kernel - Nova Self-Protection (Port aus adaptive32.inc,
   NPSPEC-AUTONOMY-SELFPROTECTION-0001). Erkennt und isoliert Bedrohungen
   durch Verfolgung von Threat-Events je Quelle; ueberschreitet die
   Anzahl einen Schwellwert, wird die Quelle quarantiert. */
#ifndef NOVA_SELFPROT_H
#define NOVA_SELFPROT_H

#include <stdint.h>

#define SELFPROT_CAPACITY 8u

#define SELFPROT_INTRUSION 1u
#define SELFPROT_OVERFLOW  2u
#define SELFPROT_INJECTION 3u
#define SELFPROT_TAMPERING 4u

void selfprot_initialize(void);
int selfprot_self_test(void); /* 0 = ok */

/* Zeichnet ein Threat-Event fuer source_id auf (legt bei Bedarf einen
   neuen Eintrag an). 0 = gezaehlt, noch nicht quarantiert; -1 = Schwelle
   erreicht (jetzt quarantiert) ODER war bereits quarantiert ODER
   source_id==0 ODER Tabelle voll. */
int selfprot_threat(uint32_t source_id, uint32_t threat_type, uint32_t threshold);

/* 0 = Erfolg (out_count/out_quarantined gesetzt), -1 wenn source_id
   unbekannt. */
int selfprot_query(uint32_t source_id, uint32_t *out_count, uint32_t *out_quarantined);

#endif

/* NovaOS C-Kernel - Nova Self-Diagnosis (Port aus adaptive32.inc,
   NPSPEC-AUTONOMY-SELFDIAGNOSIS-0001). Zeichnet beobachtete Symptome auf
   und bildet sie auf Diagnosen ab. */
#ifndef NOVA_SDIAG_H
#define NOVA_SDIAG_H

#include <stdint.h>

#define SDIAG_CAPACITY 8u

#define SDIAG_SYM_LATENCY 1u
#define SDIAG_SYM_MEMORY  2u
#define SDIAG_SYM_CPU     3u
#define SDIAG_SYM_CRASH   4u
#define SDIAG_SYM_SILENT  5u

#define SDIAG_DIAG_UNKNOWN    0u
#define SDIAG_DIAG_OVERLOAD   1u
#define SDIAG_DIAG_LEAK       2u
#define SDIAG_DIAG_CORRUPTION 3u
#define SDIAG_DIAG_HANG       4u

void sdiag_initialize(void);
int sdiag_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn diag_id==0 oder Tabelle voll. */
int sdiag_register(uint32_t diag_id, uint32_t symptom_type, uint32_t diagnosis, uint32_t *out_slot);

/* 0 = Diagnose gefunden (out_diagnosis, out_count gesetzt, Count erhoeht),
   -1 = kein registriertes Symptom (out_diagnosis = SDIAG_DIAG_UNKNOWN,
   out_count = 0). */
int sdiag_diagnose(uint32_t symptom_type, uint32_t *out_diagnosis, uint32_t *out_count);

#endif

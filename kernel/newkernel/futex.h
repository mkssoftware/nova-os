/* NovaOS C-Kernel - Nova Fast Userspace Mutex (Port aus sync32.inc,
   NPSPEC-SYNC-FUTEX-0001). Kernel-seitige Futex-Infrastruktur: eine
   Warte-Queue pro Futex-Adresse. Bootstrap: kein echtes Blockieren
   (Single-CPU, kein Scheduler-Sleep) - `futex_wait` zaehlt nur den
   Waiter-Zaehler hoch, `futex_wake` zaehlt ihn wieder herunter. */
#ifndef NOVA_FUTEX_H
#define NOVA_FUTEX_H

#include <stdint.h>

#define FUTEX_CAPACITY 16u

#define FTX_FLAG_PRIVATE 0x01u
#define FTX_FLAG_SHARED  0x02u
#define FTX_FLAG_RT      0x04u

void futex_initialize(void);
int futex_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt - neu registriert oder bereits
   vorhanden), -1 wenn Tabelle voll. */
int futex_register(uint32_t futex_addr, uint32_t flags, uint32_t *out_slot);

/* 0 = Erfolg (waiter_count erhoeht), -1 wenn slot ungueltig/inaktiv. */
int futex_wait(uint32_t slot);

/* Weckt bis zu max_wake Waiter (dekrementiert waiter_count entsprechend).
   0 = immer Erfolg (auch wenn slot ungueltig/inaktiv oder keine Waiter
   vorhanden sind - matched altes ASM-Verhalten 1:1). */
int futex_wake(uint32_t slot, uint32_t max_wake);

#endif

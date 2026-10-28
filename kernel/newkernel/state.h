/* NovaOS C-Kernel - Nova Global-State- und Transaction-Bootstrapkern
   (Port aus state32.inc). Legt die stabile ABI-Kante fuer versionierte,
   verantwortete Teilzustaende und kontrollierte Transitions an, auf der
   spaetere Services aufbauen - implementiert noch keinen universellen
   Zustandsspeicher.

   WICHTIG (Vereinfachung ggue. dem alten ASM): Version/Generation/
   Previous-Version sind hier plain uint32_t statt echter 64-Bit-Felder -
   im Original bleibt das jeweils hoehere Dword in jedem tatsaechlich
   ausgefuehrten Pfad (Create/Self-Test) immer 0, ein echter Ueberlauf
   ueber 2^32 Transitions ist fuer diesen Bootstrap-Kern irrelevant. */
#ifndef NOVA_STATE_H
#define NOVA_STATE_H

#include <stdint.h>

#define STATE_RECORD_CAPACITY 8u

#define STATE_VALIDITY_UNKNOWN 0u
#define STATE_VALIDITY_VALID   1u
#define STATE_VALIDITY_INVALID 2u
#define STATE_VALIDITY_STALE   3u

#define STATE_KIND_KERNEL  1u
#define STATE_KIND_SERVICE 5u

#define STATE_BOOT_ID_HIGH 0x4E4F5641u /* "NOVA" */
#define STATE_BOOT_ID_LOW  0x53544154u /* "STAT" */

void state_manager_initialize(void);
int state_manager_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt, Validity -> UNKNOWN, Version=1), -1 wenn
   ein Parameter 0 ist oder die Tabelle voll ist. */
int state_create(uint32_t id_high, uint32_t id_low, uint32_t kind, uint32_t owner,
                  uint32_t *out_slot);

/* Kontrollierte Transition: nur erlaubt, wenn expected_version mit der
   aktuellen Version des Records uebereinstimmt (optimistische
   Nebenlaeufigkeitskontrolle) - sonst wird die Validity auf STALE gesetzt
   und -1 zurueckgegeben (Konflikt), OHNE die Version zu erhoehen. Bei
   Erfolg: previous_version=aktuelle Version, Version++, Validity=
   new_validity. 0 = Erfolg, -1 wenn slot unbekannt, new_validity >
   STATE_VALIDITY_STALE, oder Versions-Konflikt. */
int state_transition(uint32_t slot, uint32_t expected_version, uint32_t new_validity);

/* Fuer transaction.c's transaction_commit (Commit-Generation-Snapshot) -
   der Zaehler selbst wird in diesem Bootstrap-Kern von niemandem
   erhoeht, bleibt also konstant 1 (siehe state.c). */
uint32_t state_generation_counter_value(void);

#define TRANSACTION_CAPACITY 4u

#define TRANSACTION_ACTIVE    2u
#define TRANSACTION_PREPARED  5u
#define TRANSACTION_COMMITTED 7u
#define TRANSACTION_COMPLETED 9u
#define TRANSACTION_FAILED    12u

#define TRANSACTION_SCOPE_OBJECT  1u
#define TRANSACTION_SCOPE_SERVICE 6u

void transaction_manager_initialize(void);
int transaction_manager_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_txn_id gesetzt, Zustand -> ACTIVE), -1 wenn owner/scope
   0 sind oder die Tabelle voll ist. */
int transaction_begin(uint32_t owner, uint32_t scope, uint32_t expected_version,
                       uint32_t *out_txn_id);

/* Erfolg nur wenn Zustand ACTIVE UND expected_version mit dem bei
   transaction_begin() hinterlegten Wert uebereinstimmt - sonst Konflikt
   (Zustand -> FAILED). 0 = Erfolg (Zustand -> PREPARED), -1 bei Konflikt
   oder unbekannter/falscher Zustand. */
int transaction_prepare(uint32_t txn_id, uint32_t expected_version);

/* Nur aus PREPARED erlaubt. 0 = Erfolg (Zustand -> COMMITTED, Commit-
   Generation = state_generation_counter_value()), -1 sonst. */
int transaction_commit(uint32_t txn_id);

/* Nur aus COMMITTED erlaubt (Commit und Verification sind bewusst
   getrennte Schritte). 0 = Erfolg (Zustand -> COMPLETED), -1 sonst. */
int transaction_verify(uint32_t txn_id);

#endif

/* NovaOS C-Kernel - Nova Autonomous Self-Healing (NPSPEC-AUTONOMY-
   SELFHEALING-0001, Port aus adaptive32.inc). Observe->Detect->
   Diagnose->Repair->Verify-Pipeline: jeder Heal-Slot ueberwacht ein
   Subsystem; ein Reparaturversuch meldet den Fehler an die bereits
   vorhandene resilience_fault_pipeline() (integration.c) und liest
   danach den Containment-Status zurueck.

   War im alten ASM bis 2026-10-26 bewusst UEBERSPRUNGEN (siehe
   project_c_kernel_migration_status.md "Bewusst uebersprungen"): haengt
   an resilience_fault_pipeline()/containment_check() aus dem damals
   noch unportierten Resilience/Containment-Block - beide sind
   inzwischen vorhanden (integration.c, resilience.c), die
   Abhaengigkeit ist aufgeloest.

   ANPASSUNG gegenueber dem alten ASM (bewusst, kein 1:1-Fehler): das
   alte `selfheal_repair` rief `containment_check(subsystem_id)` DIREKT
   mit der Subsystem-ID als Slot-Index auf. Das neue `containment.c`-
   Aequivalent (resilience.c) trennt Subsystem-ID und Containment-Slot
   explizit (`containment_find_by_subsystem` noetig, siehe
   resilience_fault_pipeline()s eigene Implementierung in integration.c,
   die genau dieses Muster bereits verwendet). Diese Portierung uebernimmt
   deshalb dasselbe Muster: nach dem Pipeline-Aufruf wird der Containment-
   Slot per containment_find_by_subsystem() gesucht; existiert keiner
   (Subsystem war nie bei containment_register() angemeldet), gilt der
   Reparaturversuch als erfolgreich (kein Containment-Datensatz = nichts
   isoliert) - exakt das alte ASM-Verhalten fuer den Normalfall
   (unregistrierte Subsysteme werden auch dort nie isoliert). */
#ifndef NOVA_SELFHEAL_H
#define NOVA_SELFHEAL_H

#include <stdint.h>

#define HEAL_CAPACITY 8u

#define HEAL_STATE_OK       0u
#define HEAL_STATE_DEGRADED 1u
#define HEAL_STATE_HEALING  2u
#define HEAL_STATE_FAILED   3u

typedef struct {
    uint32_t id;           /* 1-basiert, 0 = freier Slot */
    uint32_t subsystem_id;
    uint32_t state;
    uint32_t attempts;
} selfheal_record_t;

void selfheal_initialize(void);
int selfheal_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt, Anfangszustand HEAL_STATE_OK),
   -1 = HEAL_CAPACITY erschoepft. */
int selfheal_register(uint32_t subsystem_id, uint32_t *out_slot);

/* Meldet fault_type ueber resilience_fault_pipeline() (siehe
   integration.h) und liest danach den Containment-Status des
   Subsystems zurueck (siehe Anpassungs-Hinweis oben).
   0 = Reparatur erfolgreich (Zustand -> HEAL_STATE_OK),
   -1 = ungueltiger/leerer Slot ODER das Subsystem ist jetzt per
   Containment isoliert (Zustand -> HEAL_STATE_FAILED). */
int selfheal_repair(uint32_t slot, uint32_t fault_type);

uint32_t selfheal_total_repairs(void);
int selfheal_record_at(uint32_t slot, const selfheal_record_t **out);

#endif

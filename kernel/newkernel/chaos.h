/* NovaOS C-Kernel - Nova Chaos Engine (Port aus adaptive32.inc,
   NPSPEC-RESILIENCE-CHAOS-0001). Plant und verfolgt strukturierte
   Fehler-Szenarien fuer Resilience-Tests: verbindet fi_inject
   (fi.h) mit rverif_check (rverif.h) ueber Fault-Injection ->
   Chaos-Schedule -> Recovery-Verifikation. */
#ifndef NOVA_CHAOS_H
#define NOVA_CHAOS_H

#include <stdint.h>

#define CHAOS_CAPACITY 8u

void chaos_initialize(void);
int chaos_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_slot gesetzt), -1 wenn scenario_id==0 oder Tabelle
   voll. */
int chaos_register_scenario(uint32_t scenario_id, uint32_t target_subsystem,
                             uint32_t fault_type, uint32_t *out_slot);
/* Injiziert den Fehler via fi_inject und erhoeht den Runs-Zaehler. 0 =
   Erfolg, -1 wenn scenario_id unbekannt/ungueltig. */
int chaos_run_scenario(uint32_t scenario_id);
/* Erhoeht den Recovered-Zaehler (nach erfolgreicher rverif_check-Pruefung
   aufzurufen). 0 = Erfolg, -1 wenn scenario_id unbekannt. */
int chaos_mark_recovered(uint32_t scenario_id);

#endif

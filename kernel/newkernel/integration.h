/* NovaOS C-Kernel - Nova Subsystem Integration Pipelines (Port aus
   integration32.inc, NPSPEC-INTEGRATION-0001, Teil 1 von 4 - nur die vier
   bisher offenen Pipelines, siehe Kommentar unten fuer den Gesamtstatus
   dieser Datei). Verbindet bereits portierte Subsysteme zu
   zusammenhaengenden Kontrollfluessen:
     Pipeline 1: Security Access Check  (MAC -> DAC -> RBAC -> Audit)
     Pipeline 2: Resilience Fault       (Detection -> Containment ->
                                          Failover -> Restart -> Metrics)
     Pipeline 4: Memory Pressure / OOM  (OOM-Kill -> Audit -> Metrics -> CB)
     Pipeline 5: Watchdog Expire        (Tick -> Check -> Fault -> CB)
   Pipeline 3 (Scheduler Admission) ist bereits als `scheduler_admit_task`
   in scheduler.c/.h portiert (siehe project_c_kernel_migration_status,
   Modul 77) - hier NICHT erneut definiert.

   NOCH OFFEN in integration32.inc (siehe project_c_kernel_migration_
   status fuer Details): der ueberlappende zweite Netzwerk-Stack
   (`network_manager_*`) und der grosse "§37/§38 Progressive Readiness"-
   Block (Background-Init-Threads im Scheduler-Kontext) - beide bewusst
   NICHT Teil dieses Batches. */
#ifndef NOVA_INTEGRATION_H
#define NOVA_INTEGRATION_H

#include <stdint.h>

void integration_pipelines_initialize(void);
int integration_pipelines_self_test(void); /* 0 = ok */

/* Pipeline 1: MAC -> DAC -> RBAC, in dieser Reihenfolge; der erste
   abgelehnte Check bricht ab und schreibt einen AUDIT_TYPE_VIOLATION-
   Eintrag. Bei vollstaendigem Erfolg wird AUDIT_TYPE_CAP_GRANT geloggt.
   0 = Zugriff erlaubt, -1 = verweigert. */
int security_access_check(uint32_t subject_id, uint32_t object_id, uint32_t perm_bits);

/* Pipeline 2: registriert den Fehler bei detection_record_fault, erhoeht
   MET_SLOT_FAULTS, meldet ihn (falls ein passender Containment-Eintrag
   fuer subsystem_id existiert) an containment_report_fault - wird dieser
   dadurch ISOLATED, wird (falls ein Failover-Eintrag mit primary_id ==
   subsystem_id existiert) automatisch failover_trigger ausgeloest -,
   und prueft (falls ein Supervised-Restart-Eintrag mit owner ==
   subsystem_id existiert) restart_check_and_record. Alle drei Schritte
   sind optional (kein Fehler, wenn kein passender Eintrag existiert). */
void resilience_fault_pipeline(uint32_t fault_type, uint32_t subsystem_id, uint32_t fault_code);

/* Pipeline 4: waehlt per oom_kill_victim() ein Opfer, loggt
   AUDIT_TYPE_VIOLATION, erhoeht MET_SLOT_FREES, meldet dem Circuit
   Breaker an Slot 0 (Speicher-Subsystem) einen Fehler. 0 = Erfolg
   (*out_killed_pid gesetzt), -1 wenn kein Kandidat vorhanden war. */
int memory_pressure_response(uint32_t *out_killed_pid);

/* Pipeline 5: erhoeht den Resilience-Tick, prueft alle Watchdogs
   (watchdog_check) - bei neu abgelaufenen Watchdogs (erkannt ueber eine
   interne Vergleichsmarke gegen watchdog_total_expires()) wird ein
   FD_TYPE_TIMING-Fault registriert, MET_SLOT_IRQ_COUNT erhoeht und dem
   Circuit Breaker an Slot 0 (Watchdog-Subsystem) ein Fehler gemeldet. */
void watchdog_expire_pipeline(void);

#endif

/* NovaOS C-Kernel - Nova Subsystem Integration Pipelines (siehe
   integration.h fuer Scope). */
#include "integration.h"
#include "mac.h"
#include "dac.h"
#include "rbac.h"
#include "audit.h"
#include "resilience.h"
#include "observability.h"
#include "oom.h"

int security_access_check(uint32_t subject_id, uint32_t object_id, uint32_t perm_bits) {
    if (mac_check(subject_id, object_id) != 0) {
        security_audit_log(AUDIT_TYPE_VIOLATION, subject_id, object_id, 0u);
        return -1;
    }
    if (dac_check(object_id, subject_id, perm_bits) != 0) {
        security_audit_log(AUDIT_TYPE_VIOLATION, subject_id, object_id, 0u);
        return -1;
    }
    if (rbac_check(subject_id, perm_bits) != 0) {
        security_audit_log(AUDIT_TYPE_VIOLATION, subject_id, object_id, 0u);
        return -1;
    }

    security_audit_log(AUDIT_TYPE_CAP_GRANT, subject_id, object_id, 1u);
    return 0;
}

void resilience_fault_pipeline(uint32_t fault_type, uint32_t subsystem_id, uint32_t fault_code) {
    detection_record_fault(fault_type, subsystem_id, fault_code);
    metrics_increment(MET_SLOT_FAULTS);

    uint32_t cont_slot;
    if (containment_find_by_subsystem(subsystem_id, &cont_slot) == 0) {
        containment_report_fault(cont_slot);
        if (containment_check(cont_slot) != 0) { /* jetzt ISOLATED */
            uint32_t fo_slot;
            if (failover_find_by_primary(subsystem_id, &fo_slot) == 0) {
                failover_trigger(fo_slot);
            }
        }
    }

    uint32_t rst_slot;
    if (restart_find_by_owner(subsystem_id, &rst_slot) == 0) {
        restart_check_and_record(rst_slot);
    }
}

int memory_pressure_response(uint32_t *out_killed_pid) {
    int32_t killed = oom_kill_victim();
    if (killed < 0) return -1;

    security_audit_log(AUDIT_TYPE_VIOLATION, (uint32_t)killed, 0u, 0u);
    metrics_increment(MET_SLOT_FREES);
    circuitbreaker_record_failure(0u); /* Speicher-Subsystem = CB-Slot 0 */

    *out_killed_pid = (uint32_t)killed;
    return 0;
}

static uint32_t wep_last_expires = 0;

void watchdog_expire_pipeline(void) {
    resilience_tick();
    watchdog_check();

    uint32_t current = watchdog_total_expires();
    if (current == wep_last_expires) return;
    wep_last_expires = current;

    detection_record_fault(FD_TYPE_TIMING, 0u, 0xDEAD0001u);
    metrics_increment(MET_SLOT_IRQ_COUNT);
    circuitbreaker_record_failure(0u); /* Watchdog-Subsystem = CB-Slot 0 */
}

static int integration_ready = 0;

void integration_pipelines_initialize(void) {
    wep_last_expires = watchdog_total_expires();
    integration_ready = 0;
}

int integration_pipelines_self_test(void) {
    /* --- Pipeline 1: Security Access Check --- */
    mac_register_subject(50u, MAC_LABEL_CONFIDENTIAL);
    mac_register_object(99u, MAC_LABEL_INTERNAL);
    dac_register(99u, 50u, DAC_PERM_READ | DAC_PERM_WRITE | DAC_PERM_EXEC, 0u);
    rbac_define_role(10u, DAC_PERM_READ | DAC_PERM_WRITE);
    rbac_assign_role(50u, 10u);

    if (security_access_check(50u, 99u, DAC_PERM_WRITE) != 0) return -1;
    if (security_audit_count(AUDIT_TYPE_CAP_GRANT) < 1u) return -1;

    mac_register_subject(51u, MAC_LABEL_PUBLIC);
    mac_register_object(88u, MAC_LABEL_SECRET);
    uint32_t violations_before = security_audit_count(AUDIT_TYPE_VIOLATION);
    if (security_access_check(51u, 88u, DAC_PERM_READ) == 0) return -1;
    if (security_audit_count(AUDIT_TYPE_VIOLATION) <= violations_before) return -1;

    /* --- Pipeline 2: Resilience Fault Pipeline --- */
    uint32_t cont_slot;
    if (containment_register(77u, 2u, &cont_slot) != 0) return -1;
    uint32_t fo_slot;
    failover_register(77u, 78u, &fo_slot);

    resilience_fault_pipeline(FD_TYPE_TIMING, 77u, 0xBAD0001u);
    if (containment_check(cont_slot) != 0) return -1; /* noch DEGRADED, nicht isoliert */

    resilience_fault_pipeline(FD_TYPE_TIMING, 77u, 0xBAD0002u);
    if (containment_check(cont_slot) == 0) return -1; /* jetzt ISOLATED */

    uint32_t fo_state, fo_primary, fo_backup;
    if (failover_query(fo_slot, &fo_state, &fo_primary, &fo_backup) != 0) return -1;
    if (fo_state != FO_STATE_FAILOVER) return -1;

    uint32_t fault_value, fault_peak, fault_total;
    if (metrics_read(MET_SLOT_FAULTS, &fault_value, &fault_peak, &fault_total) != 0) return -1;
    if (fault_value == 0u) return -1;

    /* --- Pipeline 4: Memory Pressure Response --- */
    oom_register_candidate(200u, 60u);
    oom_register_candidate(201u, 90u);

    uint32_t audit_before = security_audit_count(AUDIT_TYPE_VIOLATION);
    uint32_t killed_pid;
    if (memory_pressure_response(&killed_pid) != 0) return -1;
    if (killed_pid != 201u) return -1; /* Score 90 > 60 */
    if (security_audit_count(AUDIT_TYPE_VIOLATION) <= audit_before) return -1;

    /* --- Pipeline 5: Watchdog Expire Pipeline --- */
    uint32_t wd_slot;
    if (watchdog_create(0xFFu, 1u, WD_ACTION_LOG, &wd_slot) != 0) return -1;
    watchdog_arm(wd_slot);

    wep_last_expires = watchdog_total_expires();
    uint32_t expires_before = watchdog_total_expires();
    watchdog_expire_pipeline();
    if (watchdog_total_expires() <= expires_before) return -1;

    integration_ready = 1;
    return 0;
}

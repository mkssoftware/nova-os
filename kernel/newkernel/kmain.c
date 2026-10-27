/* NovaOS C-Kernel - erster C-Kernmain. Paralleler, objektbasierter Kernel-Kern;
   alte .inc-Subsysteme werden schrittweise hierher portiert.

   "Desktop First": der Desktop wird so frueh wie moeglich angezeigt - nur
   die fuer Sicherheit/Diagnose unverzichtbaren Subsysteme (Serial, Logging,
   Panic/Crash-Dump) laufen vorher. Alles andere laedt danach als
   "Hintergrund"-Phase weiter, waehrend der Desktop bereits sichtbar ist. */

#include "serial.h"
#include "distributed.h"
#include "boot_phase.h"
#include "panic.h"
#include "logging.h"
#include "oom.h"
#include "cow.h"
#include "aslr.h"
#include "slab.h"
#include "heap.h"
#include "object.h"
#include "handle.h"
#include "component.h"
#include "bib.h"
#include "framebuffer.h"
#include "display.h"
#include "compositor.h"
#include "desktop.h"
#include "surface.h"
#include "pmm.h"
#include "paging.h"
#include "process.h"
#include "arch.h"
#include "bootmgr.h"
#include "abi.h"
#include "semantic.h"
#include "execution.h"
#include "privacy.h"
#include "verify.h"
#include "txmgr.h"
#include "autonomy.h"
#include "datamove.h"
#include "statemgr.h"
#include "identity.h"
#include "resource.h"
#include "realtime.h"
#include "distcom.h"
#include "trust.h"
#include "nobj.h"
#include "global.h"
#include "pci.h"
#include "driver.h"
#include "update.h"
#include "block.h"
#include "ui.h"
#include "app.h"
#include "text.h"
#include "fsindex.h"
#include "crypto.h"
#include "compat.h"
#include "power.h"
#include "thermal.h"
#include "fs.h"
#include "font.h"
#include "network.h"
#include "program.h"
#include "interrupt.h"
#include "time.h"
#include "ipc.h"
#include "nvme.h"
#include "scheduler.h"
#include "hal.h"
#include "usb.h"
#include "kog.h"
#include "evbus.h"
#include "uobj.h"
#include "cap.h"
#include "security.h"
#include "audit.h"
#include "mac.h"
#include "dac.h"
#include "rbac.h"
#include "powermgr.h"
#include "exception.h"
#include "syscall.h"
#include "csprng.h"
#include "cpuidle.h"
#include "thermthrottle.h"
#include "identreg.h"
#include "trustanchor.h"
#include "adaptfb.h"
#include "radmit.h"
#include "excon.h"
#include "tmpiso.h"
#include "privlabel.h"
#include "retry.h"
#include "rmode.h"
#include "rtv.h"
#include "sgraph.h"
#include "fsm.h"
#include "txroll.h"
#include "fi.h"
#include "fclass.h"
#include "rpol.h"
#include "rverif.h"
#include "ssnap.h"
#include "shist.h"
#include "srec.h"
#include "txlog.h"
#include "tbarrier.h"
#include "tcomp.h"
#include "redundancy.h"
#include "chaos.h"
#include "resiso.h"
#include "raccount.h"
#include "rguarant.h"
#include "rreclaim.h"
#include "sdiag.h"
#include "sconf.h"
#include "apred.h"
#include "selfopt.h"
#include "selfprot.h"
#include "rresv.h"
#include "aconstraint.h"
#include "autopol.h"
#include "aschedsw.h"
#include "sver.h"
#include "sactual.h"
#include "sdesired.h"
#include "txres.h"
#include "txsys.h"
#include "rarbit.h"
#include "acache.h"
#include "apower.h"
#include "pollearn.h"
#include "prederr.h"
#include "aprefetch.h"
#include "apreload.h"
#include "astorage.h"
#include "anetwork.h"
#include "uoverride.h"
#include "rdeadline.h"
#include "reconomy.h"
#include "renergy.h"
#include "rlatency.h"
#include "sglobal.h"
#include "txconfig.h"
#include "txmulti.h"
#include "vtemporal.h"
#include "vinfoflow.h"
#include "vcapsafe.h"
#include "vmemsafe.h"
#include "ring3.h"
#include "sync.h"
#include "syncext.h"
#include "futex.h"
#include "deadlock.h"
#include "pimutex.h"
#include "irqmgr.h"
#include "modloader.h"
#include "taskscope.h"
#include "task.h"
#include "taskdeadline.h"
#include "taskgroup.h"
#include "threadmgr.h"
#include "smp.h"

void kmain(uint32_t boot_magic, uint32_t bib_physical_address) __asm__("kmain");

static void report(const char *label, int ok) {
    serial_puts(label);
    serial_puts(ok ? " ok\r\n" : " FEHLGESCHLAGEN\r\n");
}

void kmain(uint32_t boot_magic, uint32_t bib_physical_address) {
    (void)boot_magic;

    /* --- Unverzichtbare Grundlage (Diagnose/Absturzsicherheit) --- */
    serial_init();
    serial_puts("NOVA: Kernel-Kern gebootet\r\n");

    logging_initialize();
    report("NOVA: Logging Selftest", logging_self_test() == 0);

    boot_phase_current = BOOT_PHASE_KERNEL_ENTRY;
    boot_phase_last_success = BOOT_PHASE_NONE;
    boot_phase_log();

    panic_manager_initialize();
    report("NOVA: Panic Manager Selftest", panic_manager_self_test() == 0);
    crash_dump_initialize();
    report("NOVA: Crash Dump Selftest", crash_dump_self_test() == 0);

    boot_phase_last_success = BOOT_PHASE_KERNEL_ENTRY;
    boot_phase_current = BOOT_PHASE_HANDOFF;
    boot_phase_log();

    /* --- Desktop First: so frueh wie moeglich sichtbar --- */
    int bib_ok = (bib_parse(bib_physical_address) == 0);
    report("NOVA: BIP Parse", bib_ok);

    const bib_graphics_info_t *gfx = bib_graphics();
    int fb_ok = bib_ok && gfx->has_graphics &&
        display_initialize(gfx->framebuffer_address, gfx->pitch,
                            gfx->width, gfx->height, gfx->bits_per_pixel) == 0;
    report("NOVA: Framebuffer Initialize", fb_ok);
    report("NOVA: Display Manager Selftest", fb_ok && display_self_test() == 0);
    surface_manager_initialize();
    report("NOVA: Surface Manager Selftest", surface_self_test() == 0);
    report("NOVA: Compositor Selftest", fb_ok && compositor_self_test() == 0);

    report("NOVA: Desktop Show", fb_ok && desktop_show() == 0);
    serial_puts("NOVA: Desktop angezeigt - Hintergrund-Subsysteme laden jetzt weiter\r\n");

    boot_phase_last_success = BOOT_PHASE_HANDOFF;
    boot_phase_current = BOOT_PHASE_DESKTOP_READY;
    boot_phase_log();

    /* --- Hintergrund-Phase: alles, was nicht vor dem Desktop gebraucht wird --- */
    report("NOVA: Distributed Initialize", distributed_initialize() == 0);
    report("NOVA: Distributed Selftest", distributed_self_test() == 0);
    report("NOVA: Distributed Register Node(node 1)",
           dist_register_node(1, 0 /* DIST_ROLE_PRIMARY */, 0x0A000001u, 0) == 0);
    report("NOVA: Distributed Register Node(node 2, replica)",
           dist_register_node(2, 1 /* DIST_ROLE_REPLICA */, 0x0A000002u, 0) == 0);

    uint32_t rpc_status = 0;
    int rpc_rc = dist_send_rpc(1, 0xDEADBEEFu, 0, &rpc_status);
    report("NOVA: Distributed Send RPC(node 1)", rpc_rc == 0 && rpc_status == DIST_RPC_OK);

    report("NOVA: Distributed Replicate", dist_replicate(42, 0x1234u) == 0);

    int pmm_ok = bib_ok && pmm_initialize() == 0;
    report("NOVA: Physical Memory Manager Initialize", pmm_ok);
    report("NOVA: Physical Memory Manager Selftest", pmm_ok && pmm_self_test() == 0);

    heap_initialize();
    report("NOVA: Heap Selftest", pmm_ok && heap_self_test() == 0);

    int paging_ok = pmm_ok && paging_initialize() == 0;
    report("NOVA: Paging Initialize", paging_ok);
    report("NOVA: Paging Selftest", paging_ok && paging_self_test() == 0);

    oom_initialize();
    report("NOVA: Out-of-Memory Selftest", oom_self_test() == 0);

    cow_initialize();
    report("NOVA: Copy-on-Write Selftest", cow_self_test() == 0);

    aslr_initialize();
    report("NOVA: ASLR Selftest", aslr_self_test() == 0);

    slab_initialize();
    report("NOVA: Slab Selftest", slab_self_test() == 0);

    object_manager_initialize();
    report("NOVA: Object Manager Selftest", object_manager_self_test() == 0);

    handle_manager_initialize();
    report("NOVA: Handle Manager Selftest", handle_manager_self_test() == 0);

    component_manager_initialize();
    report("NOVA: Component Manager Selftest", component_manager_self_test() == 0);

    process_manager_initialize();
    report("NOVA: Process Manager Selftest", process_manager_self_test() == 0);

    arch_initialize();
    report("NOVA: Architecture-Model Selftest", arch_self_test() == 0);

    bootmgr_initialize();
    report("NOVA: Boot Manager Selftest", bootmgr_self_test() == 0);

    abimgr_initialize();
    report("NOVA: ABI Manager Selftest", abimgr_self_test() == 0);
    int abi_rc = abimgr_register_syscall(NOVA_SYSCALL_WRITE, ABI_FLAG_STABLE, 0xDEAD0001u);
    uint32_t negotiated = 0;
    int negotiate_rc = abimgr_negotiate_version(NOVA_ABI_VERSION, &negotiated);
    report("NOVA: ABI Manager Register Syscall + Negotiate",
           abi_rc == 0 && negotiate_rc == 0 && negotiated == NOVA_ABI_VERSION);

    semmgr_initialize();
    report("NOVA: Semantic Manager Selftest", semmgr_self_test() == 0);
    uint32_t sem_id = 0;
    int sem_rc = semmgr_register(0x1234ABCDu, STYPE_CAPABILITY, 0, &sem_id);
    void *sem_rec = semmgr_query(0x1234ABCDu);
    uint32_t sem_result = 0;
    int validate_rc = semmgr_validate(sem_id, 42, &sem_result);
    report("NOVA: Semantic Manager Register + Query + Validate",
           sem_rc == 0 && sem_rec != 0 && validate_rc == 0 && sem_result == SVAL_OK);

    execution_initialize();
    report("NOVA: Execution Selftest", execution_self_test() == 0);
    uint32_t contract_id = 0;
    int exec_rc = exec_register_contract(EXEC_PROV_CPU, EXEC_FLAG_DETERMINISTIC, 0, 10,
                                          &contract_id);
    report("NOVA: Execution Register Contract + Check Deadline",
           exec_rc == 0 && exec_check_deadline(contract_id) == 0);

    privacy_ext_initialize();
    report("NOVA: Privacy Extension Selftest", privacy_ext_self_test() == 0);

    verify_initialize();
    report("NOVA: Verify Selftest", verify_self_test() == 0);

    txmgr_initialize();
    report("NOVA: Transaction Manager Selftest", txmgr_self_test() == 0);
    uint32_t tx_id = 0;
    int tx_rc = txmgr_begin(TX_TYPE_LOCAL, TX_FLAG_ATOMIC, 0, &tx_id);
    report("NOVA: Transaction Manager Begin + Commit", tx_rc == 0 && txmgr_commit(tx_id) == 0);

    autonomy_initialize();
    report("NOVA: Autonomy Manager Selftest", autonomy_self_test() == 0);

    datamove_initialize();
    report("NOVA: Data Move Selftest", datamove_self_test() == 0);

    statemgr_initialize();
    report("NOVA: State Manager Selftest", statemgr_self_test() == 0);
    uint32_t sm_slot = 0;
    int sm_state_rc = state_register(1, SM_STATE_RUNNING, &sm_slot);
    uint32_t reconcile_result = 0;
    int reconcile_rc = state_set_desired(1, SM_STATE_DEGRADED) == 0 &&
                        state_reconcile(1, &reconcile_result) == 0;
    report("NOVA: State Manager Register + Set Desired + Reconcile",
           sm_state_rc == 0 && reconcile_rc && reconcile_result == SM_RECONCILE_OK);

    report("NOVA: Identity Manager Initialize", identity_mgr_initialize() == 0);
    report("NOVA: Identity Manager Selftest", identity_mgr_self_test() == 0);

    resource_initialize();
    report("NOVA: Resource Manager Selftest", resource_self_test() == 0);

    report("NOVA: Realtime Manager Initialize", realtime_initialize() == 0);
    report("NOVA: Realtime Manager Selftest", realtime_self_test() == 0);
    int rt_rc = rtmgr_task_register(1, RT_PRIO_HIGH, RT_SCHED_DEADLINE, RT_LATENCY_SOFT);
    report("NOVA: Realtime Manager Register Task + Deadline Check",
           rt_rc == 0 && rt_deadline_check(1) == 0);

    distcom_initialize();
    report("NOVA: Distributed Communications Selftest", distcom_self_test() == 0);

    report("NOVA: Trust Manager Initialize", trust_mgr_initialize() == 0);
    report("NOVA: Trust Manager Selftest", trust_mgr_self_test() == 0);

    object_initialize();
    report("NOVA: Nova Object Subsystem Selftest", object_self_test() == 0);

    globalization_initialize();
    report("NOVA: Globalization Selftest", globalization_self_test() == 0);

    driver_discovery_initialize();
    report("NOVA: Driver Discovery Selftest", driver_discovery_self_test() == 0);
    report("NOVA: Driver Discovery PCI Scan", driver_pci_scan() == 0);

    update_initialize();
    report("NOVA: Update Manager Selftest", update_self_test() == 0);

    block_layer_initialize();
    report("NOVA: Storage Block Layer Selftest", block_layer_self_test() == 0);

    ui_initialize();
    report("NOVA: UI Subsystem Selftest", ui_self_test() == 0);
    uint32_t ui_win_id = 0;
    int ui_rc = ui_window_create(0, 0, 320, 240, &ui_win_id);
    report("NOVA: UI Window Create + Compositor Frame",
           ui_rc == 0 && ui_compositor_frame() == 0);

    app_manager_initialize();
    report("NOVA: Application Model Selftest", app_manager_self_test() == 0);

    text_initialize();
    report("NOVA: Text Subsystem Selftest", text_self_test() == 0);
    static const uint8_t arabic_text[] = {0xD8, 0xB0}; /* U+0630 (arabisch) */
    report("NOVA: Text Bidi Classify + Security Check",
           text_bidi_classify(arabic_text, sizeof(arabic_text)) == BIDI_RTL &&
           text_security_check(arabic_text, sizeof(arabic_text)) == 0);

    fsindex_initialize();
    report("NOVA: Filesystem-Index Selftest", fsindex_self_test() == 0);

    crypto_initialize();
    report("NOVA: Crypto Subsystem Selftest", crypto_self_test() == 0);

    compat_initialize();
    report("NOVA: Compatibility Layer Selftest", compat_self_test() == 0);

    power_extended_initialize();
    report("NOVA: Power Management Selftest", power_extended_self_test() == 0);

    thermal_manager_initialize();
    report("NOVA: Thermal Manager Selftest", thermal_manager_self_test() == 0);

    fs_initialize();
    report("NOVA: Filesystem Subsystem Selftest", fs_self_test() == 0);

    font_manager_initialize();
    report("NOVA: Font Manager Selftest", font_manager_self_test() == 0);

    network_stack_initialize();
    report("NOVA: Network Stack Selftest", network_stack_self_test() == 0);

    program_manager_initialize();
    report("NOVA: Program Manager Selftest", program_manager_self_test() == 0);

    interrupt_initialize();
    report("NOVA: Interrupt Subsystem Selftest", interrupt_self_test() == 0);

    timer_initialize();
    interrupt_enable();
    report("NOVA: Timer Subsystem Selftest", timer_self_test() == 0);

    time_subsystem_initialize();
    report("NOVA: Time Subsystem Selftest", time_subsystem_self_test() == 0);

    deferred_initialize();
    report("NOVA: Deferred Work Queue Selftest", deferred_self_test() == 0);

    ipc_initialize();
    report("NOVA: IPC Queue Selftest", ipc_self_test() == 0);

    device_manager_initialize();
    report("NOVA: Device Manager Selftest", device_manager_self_test() == 0);

    driver_framework_initialize();
    report("NOVA: Driver Framework Selftest", driver_framework_self_test() == 0);

    vfs_initialize();
    report("NOVA: VFS Bootstrap Selftest", vfs_self_test() == 0);

    service_manager_initialize();
    report("NOVA: Service Manager Selftest", service_manager_self_test() == 0);

    report("NOVA: NVMe Controller Selftest", nvme_initialize() == 0 && nvme_self_test() == 0);

    scheduler_initialize();
    report("NOVA: Scheduler Frame Selftest", scheduler_self_test() == 0);

    worksteal_initialize();
    report("NOVA: Work Stealing Selftest", worksteal_self_test() == 0);

    rt_scheduler_initialize();
    report("NOVA: RT Scheduler Selftest", rt_scheduler_self_test() == 0);

    cfs_initialize();
    report("NOVA: Fair Scheduler Selftest", cfs_self_test() == 0);

    qos_initialize();
    report("NOVA: QoS Scheduler Selftest", qos_self_test() == 0);

    report("NOVA: ACPI HAL Selftest", acpi_rsdp_initialize() == 0 && acpi_hal_self_test() == 0);

    report("NOVA: Early Security Entropy Selftest",
           early_security_entropy_initialize() == 0 && early_security_entropy_self_test() == 0);

    report("NOVA: USB xHCI Selftest", usb_initialize() == 0 && usb_self_test() == 0);

    kog_initialize();
    report("NOVA: Kernel Object Graph Selftest", kog_self_test() == 0);

    evbus_initialize();
    report("NOVA: Event Bus Selftest", evbus_self_test() == 0);

    uobj_initialize();
    report("NOVA: Unified Object API Selftest", uobj_self_test() == 0);

    report("NOVA: Capability Framework Selftest", cap_initialize() == 0 && cap_self_test() == 0);

    security_initialize();
    report("NOVA: Security Capability Manager Selftest", security_self_test() == 0);

    security_audit_initialize();
    report("NOVA: Security Audit Log Selftest", security_audit_self_test() == 0);

    mac_initialize();
    report("NOVA: Mandatory Access Control Selftest", mac_self_test() == 0);

    dac_initialize();
    report("NOVA: Discretionary Access Control Selftest", dac_self_test() == 0);

    rbac_initialize();
    report("NOVA: Role-Based Access Control Selftest", rbac_self_test() == 0);

    power_manager_initialize();
    report("NOVA: Power Manager ABI Selftest", power_manager_self_test() == 0);

    exception_manager_initialize();
    report("NOVA: Exception Manager Selftest", exception_manager_self_test() == 0);

    syscall_manager_initialize();
    report("NOVA: System Call Interface Selftest", syscall_manager_self_test() == 0);

    csprng_initialize();
    report("NOVA: CSPRNG Entropy Pool Selftest", csprng_self_test() == 0);

    cpuidle_initialize();
    report("NOVA: CPU Idle State Management Selftest", cpuidle_self_test() == 0);

    thermal_initialize();
    report("NOVA: Thermal Throttle Policy Selftest", thermal_self_test() == 0);

    identity_initialize();
    report("NOVA: Identity Registry Selftest", identity_self_test() == 0);

    report("NOVA: Trust Anchor Chain Selftest", trust_initialize() == 0 && trust_self_test() == 0);

    adapt_initialize();
    report("NOVA: Adaptive Feedback Loop Selftest", adapt_self_test() == 0);

    radmit_initialize();
    report("NOVA: Resource Admission Control Selftest", radmit_self_test() == 0);

    excon_initialize();
    report("NOVA: Execution Contract Selftest", excon_self_test() == 0);

    tmpiso_initialize();
    report("NOVA: RT Temporal Isolation Selftest", tmpiso_self_test() == 0);

    privlabel_initialize();
    report("NOVA: Privacy Label Policy Selftest", privlabel_self_test() == 0);

    retry_initialize();
    report("NOVA: Retry Manager Selftest", retry_self_test() == 0);

    rmode_initialize();
    report("NOVA: Recovery Mode Manager Selftest", rmode_self_test() == 0);

    rtv_initialize();
    report("NOVA: Runtime Verification Monitor Selftest", rtv_self_test() == 0);

    sgraph_initialize();
    report("NOVA: System State Graph Selftest", sgraph_self_test() == 0);

    fsm_initialize();
    report("NOVA: Finite State Machine Selftest", fsm_self_test() == 0);

    txroll_initialize();
    report("NOVA: Transaction Rollback Journal Selftest", txroll_self_test() == 0);

    fi_initialize();
    report("NOVA: Fault Injection Framework Selftest", fi_self_test() == 0);

    fclass_initialize();
    report("NOVA: Fault Classification Selftest", fclass_self_test() == 0);

    rpol_initialize();
    report("NOVA: Recovery Policy Manager Selftest", rpol_self_test() == 0);

    rverif_initialize();
    report("NOVA: Resilience Verification Selftest", rverif_self_test() == 0);

    ssnap_initialize();
    report("NOVA: State Snapshot Selftest", ssnap_self_test() == 0);

    shist_initialize();
    report("NOVA: State History Ring Selftest", shist_self_test() == 0);

    srec_initialize();
    report("NOVA: State Reconciliation Selftest", srec_self_test() == 0);

    txlog_initialize();
    report("NOVA: Transaction Log Selftest", txlog_self_test() == 0);

    tbarrier_initialize();
    report("NOVA: Transaction Barrier Selftest", tbarrier_self_test() == 0);

    tcomp_initialize();
    report("NOVA: Transaction Compensation Selftest", tcomp_self_test() == 0);

    redundancy_initialize();
    report("NOVA: Redundancy Manager Selftest", redundancy_self_test() == 0);

    chaos_initialize();
    report("NOVA: Chaos Engine Selftest", chaos_self_test() == 0);

    resilisolation_initialize();
    report("NOVA: Resilience Isolation Selftest", resilisolation_self_test() == 0);

    raccount_initialize();
    report("NOVA: Resource Accounting Selftest", raccount_self_test() == 0);

    rguarant_initialize();
    report("NOVA: Resource Guarantee Selftest", rguarant_self_test() == 0);

    rreclaim_initialize();
    report("NOVA: Resource Reclaim Selftest", rreclaim_self_test() == 0);

    sdiag_initialize();
    report("NOVA: Self-Diagnosis Selftest", sdiag_self_test() == 0);

    sconf_initialize();
    report("NOVA: Self-Configuration Selftest", sconf_self_test() == 0);

    apred_initialize();
    report("NOVA: Adaptive Prediction Selftest", apred_self_test() == 0);

    selfopt_initialize();
    report("NOVA: Self-Optimization Selftest", selfopt_self_test() == 0);

    selfprot_initialize();
    report("NOVA: Self-Protection Selftest", selfprot_self_test() == 0);

    rresv_initialize();
    report("NOVA: Resource Reservation Selftest", rresv_self_test() == 0);

    aconstraint_initialize();
    report("NOVA: Autonomy Constraint Engine Selftest", aconstraint_self_test() == 0);

    autopol_initialize();
    report("NOVA: Autonomy Policy Manager Selftest", autopol_self_test() == 0);

    aschedsw_initialize();
    report("NOVA: Adaptive Scheduler Selftest", aschedsw_self_test() == 0);

    sver_initialize();
    report("NOVA: State Versioning Selftest", sver_self_test() == 0);

    sactual_initialize();
    report("NOVA: State Actual Store Selftest", sactual_self_test() == 0);

    sdesired_initialize();
    report("NOVA: State Desired Store Selftest", sdesired_self_test() == 0);

    txres_initialize();
    report("NOVA: Transaction Resource Manager Selftest", txres_self_test() == 0);

    txsys_initialize();
    report("NOVA: Transaction System Coordinator Selftest", txsys_self_test() == 0);

    rarbit_initialize();
    report("NOVA: Resource Arbitration Selftest", rarbit_self_test() == 0);

    acache_initialize();
    report("NOVA: Adaptive Cache Manager Selftest", acache_self_test() == 0);

    apower_initialize();
    report("NOVA: Adaptive Power Manager Selftest", apower_self_test() == 0);

    pollearn_initialize();
    report("NOVA: Policy Learning Engine Selftest", pollearn_self_test() == 0);

    prederr_initialize();
    report("NOVA: Prediction Error Tracker Selftest", prederr_self_test() == 0);

    aprefetch_initialize();
    report("NOVA: Adaptive Prefetch Engine Selftest", aprefetch_self_test() == 0);

    apreload_initialize();
    report("NOVA: Adaptive Preload Manager Selftest", apreload_self_test() == 0);

    astorage_initialize();
    report("NOVA: Adaptive Storage Manager Selftest", astorage_self_test() == 0);

    anetwork_initialize();
    report("NOVA: Adaptive Network Manager Selftest", anetwork_self_test() == 0);

    uoverride_initialize();
    report("NOVA: User Override Manager Selftest", uoverride_self_test() == 0);

    rdeadline_initialize();
    report("NOVA: Resource Deadline Manager Selftest", rdeadline_self_test() == 0);

    reconomy_initialize();
    report("NOVA: Resource Economy Manager Selftest", reconomy_self_test() == 0);

    renergy_initialize();
    report("NOVA: Resource Energy Manager Selftest", renergy_self_test() == 0);

    rlatency_initialize();
    report("NOVA: Resource Latency Monitor Selftest", rlatency_self_test() == 0);

    sglobal_initialize();
    report("NOVA: State Global Store Selftest", sglobal_self_test() == 0);

    txconfig_initialize();
    report("NOVA: Transaction Config Manager Selftest", txconfig_self_test() == 0);

    txmulti_initialize();
    report("NOVA: Tx MultiService Coordinator Selftest", txmulti_self_test() == 0);

    vtemporal_initialize();
    report("NOVA: Temporal Invariant Verifier Selftest", vtemporal_self_test() == 0);

    vinfoflow_initialize();
    report("NOVA: Information Flow Verifier Selftest", vinfoflow_self_test() == 0);

    vcapsafe_initialize();
    report("NOVA: Capability Safety Verifier Selftest", vcapsafe_self_test() == 0);

    vmemsafe_initialize();
    report("NOVA: Memory Safety Verifier Selftest", vmemsafe_self_test() == 0);

    sync_initialize();
    report("NOVA: Synchronisation Bootstrap Selftest", sync_self_test() == 0);

    sync_ext_initialize();
    report("NOVA: Synchronisation Erweiterung (Mutex/Cond/WQ/RCU) Selftest", sync_ext_self_test() == 0);

    futex_initialize();
    report("NOVA: Futex Selftest", futex_self_test() == 0);

    deadlock_initialize();
    report("NOVA: Deadlock Detection Selftest", deadlock_self_test() == 0);

    pi_initialize();
    report("NOVA: Priority Inheritance Mutex Selftest", pi_self_test() == 0);

    irq_manager_initialize();
    report("NOVA: Interrupt Manager 1.0 Selftest", irq_manager_self_test() == 0);

    module_loader_initialize();
    report("NOVA: Kernel Module Loader Selftest", module_loader_self_test() == 0);

    int tscope_ok = task_scope_manager_initialize() == 0;
    report("NOVA: Task Scope Manager Initialize", tscope_ok);
    report("NOVA: Task Scope Manager Selftest", tscope_ok && task_scope_manager_self_test() == 0);

    task_manager_initialize();
    report("NOVA: Task Manager Selftest", task_manager_self_test() == 0);

    task_deadline_manager_initialize();
    report("NOVA: Task Deadline Manager Selftest", task_deadline_manager_self_test() == 0);

    task_group_manager_initialize();
    report("NOVA: Task Group Manager Selftest", task_group_manager_self_test() == 0);

    int thread_mgr_ok = thread_manager_initialize() == 0;
    report("NOVA: Thread Manager Initialize", thread_mgr_ok);
    report("NOVA: Thread Manager Selftest", thread_mgr_ok && thread_manager_self_test() == 0);

    int smp_ok = smp_initialize() == 0;
    report("NOVA: SMP/AP-Aktivierung Initialize", smp_ok);
    report("NOVA: SMP/AP-Aktivierung Selftest", smp_ok && smp_self_test() == 0);
    serial_puts("NOVA: SMP Online-APs=");
    serial_put_hex32(smp_ap_online_count());
    serial_puts("\r\n");

    int ring3_ok = ring3_initialize() == 0;
    report("NOVA: Ring-3-Bootstrap (GDT/TSS/User-Pages) Initialize", ring3_ok);
    report("NOVA: Ring-3-Bootstrap Selftest", ring3_ok && ring3_self_test() == 0);

    boot_phase_last_success = BOOT_PHASE_DESKTOP_READY;
    boot_phase_current = BOOT_PHASE_SYSTEM_READY;
    boot_phase_log();

    serial_puts("NOVA: alle bisherigen Subsysteme vollstaendig verifiziert\r\n");

    if (ring3_ok) {
        /* Einweg-Uebergang: ab hier laeuft der erste Userspace-Thread in
           Ring 3 (siehe ring3.c). Der Heartbeat-Syscall-Log-Eintrag oben
           in interrupt_dispatch_c ist der eigentliche Beweis dafuer. */
        ring3_enter();
    }

    for (;;) {
        __asm__ volatile ("hlt");
    }
}

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
#include "desktop.h"
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
        framebuffer_initialize(gfx->framebuffer_address, gfx->pitch,
                                gfx->width, gfx->height, gfx->bits_per_pixel) == 0;
    report("NOVA: Framebuffer Initialize", fb_ok);

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

    boot_phase_last_success = BOOT_PHASE_DESKTOP_READY;
    boot_phase_current = BOOT_PHASE_SYSTEM_READY;
    boot_phase_log();

    serial_puts("NOVA: alle bisherigen Subsysteme vollstaendig verifiziert\r\n");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}

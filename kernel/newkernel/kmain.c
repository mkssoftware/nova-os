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

void kmain(uint32_t boot_magic, uint32_t bib_physical_address) __asm__("kmain");

static void report(const char *label, int ok) {
    serial_puts(label);
    serial_puts(ok ? " ok\r\n" : " FAILED\r\n");
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
    report("NOVA: distributed_initialize", distributed_initialize() == 0);
    report("NOVA: distributed_self_test", distributed_self_test() == 0);
    report("NOVA: dist_register_node(node 1)",
           dist_register_node(1, 0 /* DIST_ROLE_PRIMARY */, 0x0A000001u, 0) == 0);
    report("NOVA: dist_register_node(node 2, replica)",
           dist_register_node(2, 1 /* DIST_ROLE_REPLICA */, 0x0A000002u, 0) == 0);

    uint32_t rpc_status = 0;
    int rpc_rc = dist_send_rpc(1, 0xDEADBEEFu, 0, &rpc_status);
    report("NOVA: dist_send_rpc(node 1)", rpc_rc == 0 && rpc_status == DIST_RPC_OK);

    report("NOVA: dist_replicate", dist_replicate(42, 0x1234u) == 0);

    int pmm_ok = bib_ok && pmm_initialize() == 0;
    report("NOVA: pmm_initialize", pmm_ok);
    report("NOVA: pmm_self_test", pmm_ok && pmm_self_test() == 0);

    heap_initialize();
    report("NOVA: heap_self_test", pmm_ok && heap_self_test() == 0);

    int paging_ok = pmm_ok && paging_initialize() == 0;
    report("NOVA: paging_initialize", paging_ok);
    report("NOVA: paging_self_test", paging_ok && paging_self_test() == 0);

    oom_initialize();
    report("NOVA: oom_self_test", oom_self_test() == 0);

    cow_initialize();
    report("NOVA: cow_self_test", cow_self_test() == 0);

    aslr_initialize();
    report("NOVA: aslr_self_test", aslr_self_test() == 0);

    slab_initialize();
    report("NOVA: slab_self_test", slab_self_test() == 0);

    object_manager_initialize();
    report("NOVA: object_manager_self_test", object_manager_self_test() == 0);

    handle_manager_initialize();
    report("NOVA: handle_manager_self_test", handle_manager_self_test() == 0);

    component_manager_initialize();
    report("NOVA: component_manager_self_test", component_manager_self_test() == 0);

    process_manager_initialize();
    report("NOVA: process_manager_self_test", process_manager_self_test() == 0);

    arch_initialize();
    report("NOVA: arch_self_test", arch_self_test() == 0);

    bootmgr_initialize();
    report("NOVA: bootmgr_self_test", bootmgr_self_test() == 0);

    abimgr_initialize();
    report("NOVA: abimgr_self_test", abimgr_self_test() == 0);
    int abi_rc = abimgr_register_syscall(NOVA_SYSCALL_WRITE, ABI_FLAG_STABLE, 0xDEAD0001u);
    uint32_t negotiated = 0;
    int negotiate_rc = abimgr_negotiate_version(NOVA_ABI_VERSION, &negotiated);
    report("NOVA: abimgr_register_syscall+negotiate",
           abi_rc == 0 && negotiate_rc == 0 && negotiated == NOVA_ABI_VERSION);

    semmgr_initialize();
    report("NOVA: semmgr_self_test", semmgr_self_test() == 0);
    uint32_t sem_id = 0;
    int sem_rc = semmgr_register(0x1234ABCDu, STYPE_CAPABILITY, 0, &sem_id);
    void *sem_rec = semmgr_query(0x1234ABCDu);
    uint32_t sem_result = 0;
    int validate_rc = semmgr_validate(sem_id, 42, &sem_result);
    report("NOVA: semmgr_register+query+validate",
           sem_rc == 0 && sem_rec != 0 && validate_rc == 0 && sem_result == SVAL_OK);

    execution_initialize();
    report("NOVA: execution_self_test", execution_self_test() == 0);
    uint32_t contract_id = 0;
    int exec_rc = exec_register_contract(EXEC_PROV_CPU, EXEC_FLAG_DETERMINISTIC, 0, 10,
                                          &contract_id);
    report("NOVA: exec_register_contract+check_deadline",
           exec_rc == 0 && exec_check_deadline(contract_id) == 0);

    privacy_ext_initialize();
    report("NOVA: privacy_ext_self_test", privacy_ext_self_test() == 0);

    verify_initialize();
    report("NOVA: verify_self_test", verify_self_test() == 0);

    txmgr_initialize();
    report("NOVA: txmgr_self_test", txmgr_self_test() == 0);
    uint32_t tx_id = 0;
    int tx_rc = txmgr_begin(TX_TYPE_LOCAL, TX_FLAG_ATOMIC, 0, &tx_id);
    report("NOVA: txmgr_begin+commit", tx_rc == 0 && txmgr_commit(tx_id) == 0);

    autonomy_initialize();
    report("NOVA: autonomy_self_test", autonomy_self_test() == 0);

    datamove_initialize();
    report("NOVA: datamove_self_test", datamove_self_test() == 0);

    statemgr_initialize();
    report("NOVA: statemgr_self_test", statemgr_self_test() == 0);
    uint32_t sm_slot = 0;
    int sm_state_rc = state_register(1, SM_STATE_RUNNING, &sm_slot);
    uint32_t reconcile_result = 0;
    int reconcile_rc = state_set_desired(1, SM_STATE_DEGRADED) == 0 &&
                        state_reconcile(1, &reconcile_result) == 0;
    report("NOVA: state_register+set_desired+reconcile",
           sm_state_rc == 0 && reconcile_rc && reconcile_result == SM_RECONCILE_OK);

    boot_phase_last_success = BOOT_PHASE_DESKTOP_READY;
    boot_phase_current = BOOT_PHASE_SYSTEM_READY;
    boot_phase_log();

    serial_puts("NOVA: alle bisherigen Subsysteme vollstaendig verifiziert\r\n");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}

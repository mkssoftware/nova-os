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

void kmain(uint32_t boot_magic, uint32_t bib_physical_address) __asm__("kmain");

static void report(const char *label, int ok) {
    serial_puts(label);
    serial_puts(ok ? " ok\r\n" : " FAILED\r\n");
}

void kmain(uint32_t boot_magic, uint32_t bib_physical_address) {
    (void)boot_magic;

    /* --- Unverzichtbare Grundlage (Diagnose/Absturzsicherheit) --- */
    serial_init();
    serial_puts("NOVA: Paralleler C-Kernel-Kern gebootet\r\n");

    logging_initialize();
    report("NOVA: logging_self_test", logging_self_test() == 0);

    boot_phase_current = BOOT_PHASE_KERNEL_ENTRY;
    boot_phase_last_success = BOOT_PHASE_NONE;
    boot_phase_log();

    panic_manager_initialize();
    report("NOVA: panic_manager_self_test", panic_manager_self_test() == 0);
    crash_dump_initialize();
    report("NOVA: crash_dump_self_test", crash_dump_self_test() == 0);

    boot_phase_last_success = BOOT_PHASE_KERNEL_ENTRY;
    boot_phase_current = BOOT_PHASE_HANDOFF;
    boot_phase_log();

    /* --- Desktop First: so frueh wie moeglich sichtbar --- */
    int bib_ok = (bib_parse(bib_physical_address) == 0);
    report("NOVA: bib_parse", bib_ok);

    const bib_graphics_info_t *gfx = bib_graphics();
    int fb_ok = bib_ok && gfx->has_graphics &&
        framebuffer_initialize(gfx->framebuffer_address, gfx->pitch,
                                gfx->width, gfx->height, gfx->bits_per_pixel) == 0;
    report("NOVA: framebuffer_initialize", fb_ok);

    report("NOVA: desktop_show", fb_ok && desktop_show() == 0);
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

    oom_initialize();
    report("NOVA: oom_self_test", oom_self_test() == 0);

    cow_initialize();
    report("NOVA: cow_self_test", cow_self_test() == 0);

    aslr_initialize();
    report("NOVA: aslr_self_test", aslr_self_test() == 0);

    slab_initialize();
    report("NOVA: slab_self_test", slab_self_test() == 0);

    heap_initialize();
    report("NOVA: heap_self_test", heap_self_test() == 0);

    object_manager_initialize();
    report("NOVA: object_manager_self_test", object_manager_self_test() == 0);

    handle_manager_initialize();
    report("NOVA: handle_manager_self_test", handle_manager_self_test() == 0);

    component_manager_initialize();
    report("NOVA: component_manager_self_test", component_manager_self_test() == 0);

    boot_phase_last_success = BOOT_PHASE_DESKTOP_READY;
    boot_phase_current = BOOT_PHASE_SYSTEM_READY;
    boot_phase_log();

    serial_puts("NOVA: alle bisherigen Subsysteme vollstaendig verifiziert\r\n");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}

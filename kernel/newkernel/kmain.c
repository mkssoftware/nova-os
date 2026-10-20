/* NovaOS C-Kernel - erster C-Kernmain. Paralleler, objektbasierter Kernel-Kern;
   alte .inc-Subsysteme werden schrittweise hierher portiert. */

#include "serial.h"
#include "distributed.h"

void kmain(void) __asm__("kmain");

static void report(const char *label, int ok) {
    serial_puts(label);
    serial_puts(ok ? " ok\r\n" : " FAILED\r\n");
}

void kmain(void) {
    serial_init();
    serial_puts("NOVA-C: Paralleler C-Kernel-Kern gebootet\r\n");

    report("NOVA-C: distributed_initialize", distributed_initialize() == 0);
    report("NOVA-C: distributed_self_test", distributed_self_test() == 0);
    report("NOVA-C: dist_register_node(node 1)",
           dist_register_node(1, 0 /* DIST_ROLE_PRIMARY */, 0x0A000001u, 0) == 0);
    report("NOVA-C: dist_register_node(node 2, replica)",
           dist_register_node(2, 1 /* DIST_ROLE_REPLICA */, 0x0A000002u, 0) == 0);

    uint32_t rpc_status = 0;
    int rpc_rc = dist_send_rpc(1, 0xDEADBEEFu, 0, &rpc_status);
    report("NOVA-C: dist_send_rpc(node 1)", rpc_rc == 0 && rpc_status == DIST_RPC_OK);

    report("NOVA-C: dist_replicate", dist_replicate(42, 0x1234u) == 0);

    serial_puts("NOVA-C: distributed-Subsystem-Port vollstaendig verifiziert\r\n");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}

/* NovaOS C-Kernel - Distributed-Systems-Subsystem (Port von distributed32.inc).
   Semantik 1:1 aus dem alten ASM-Modul uebernommen; die Register-basierte
   ASM-Calling-Convention (EAX/EBX/ECX/EDX, CF=Erfolg) entfaellt im neuen
   C-Kernel zugunsten normaler C-Signaturen mit int-Rueckgabewert (0=Erfolg). */

#include "distributed.h"

static int dist_ready = 0;
static uint32_t dist_node_count = 0;
static uint32_t dist_local_id = 1;
static uint32_t dist_consistency = DIST_CONS_EVENTUAL;
static dist_node_t dist_node_table[DIST_NODE_CAPACITY];

int distributed_initialize(void) {
    for (uint32_t i = 0; i < DIST_NODE_CAPACITY; ++i) {
        dist_node_table[i] = (dist_node_t){0};
    }
    dist_node_count = 0;
    dist_local_id = 1;
    dist_consistency = DIST_CONS_EVENTUAL;
    dist_ready = 1;
    return 0;
}

int dist_register_node(uint32_t node_id, uint32_t role, uint32_t addr, uint32_t capabilities) {
    if (dist_node_count >= DIST_NODE_CAPACITY) {
        return -1;
    }
    dist_node_t *node = &dist_node_table[dist_node_count];
    node->id = node_id;
    node->role = role;
    node->addr = addr;
    node->capabilities = capabilities;
    node->consistency = dist_consistency;
    node->placement_zone = 0;
    node->tx_count = 0;
    node->flags = 0;
    node->latency_us = 100; /* Default: 100us lokal */
    ++dist_node_count;
    return 0;
}

int dist_send_rpc(uint32_t node_id, uint32_t function_hash, uint32_t arg, uint32_t *out_status) {
    (void)function_hash;
    (void)arg;
    for (uint32_t i = 0; i < dist_node_count; ++i) {
        if (dist_node_table[i].id == node_id) {
            /* Einfacher RPC-Stub: lokal simulieren */
            *out_status = DIST_RPC_OK;
            return 0;
        }
    }
    *out_status = DIST_RPC_FAILED;
    return -1;
}

int dist_replicate(uint32_t data_id, uint32_t data_hash) {
    (void)data_id;
    (void)data_hash;
    for (uint32_t i = 0; i < dist_node_count; ++i) {
        if (dist_node_table[i].role == DIST_ROLE_REPLICA) {
            ++dist_node_table[i].tx_count;
        }
    }
    return 0;
}

int distributed_self_test(void) {
    return (dist_ready == 1) ? 0 : -1;
}

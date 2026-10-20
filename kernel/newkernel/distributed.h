/* NovaOS C-Kernel - Distributed-Systems-Subsystem (Port von distributed32.inc).
   NPSPEC-DISTRIBUTED-CLUSTER-0001, -CONSISTENCY-0001, -EXECUTION-0001,
   -IPC-0001, -LOCATION-0001, -PLACEMENT-0001, -REMOTECAPABILITY-0001,
   -REPLICATION-0001, -SCHEDULER-0001, -STORAGE-0001, -TRANSACTION-0001. */
#ifndef NOVA_DISTRIBUTED_H
#define NOVA_DISTRIBUTED_H

#include <stdint.h>

/* Konsistenz-Modelle (NPSPEC-DISTRIBUTED-CONSISTENCY-0001) */
#define DIST_CONS_EVENTUAL   0
#define DIST_CONS_CAUSAL     1
#define DIST_CONS_SEQUENTIAL 2
#define DIST_CONS_STRONG     3

/* Knoten-Rolle (NPSPEC-DISTRIBUTED-CLUSTER-0001) */
#define DIST_ROLE_PRIMARY 0
#define DIST_ROLE_REPLICA 1 /* NPSPEC-DISTRIBUTED-REPLICATION-0001 */
#define DIST_ROLE_WORKER  2 /* NPSPEC-DISTRIBUTED-SCHEDULER-0001 */
#define DIST_ROLE_STORAGE 3 /* NPSPEC-DISTRIBUTED-STORAGE-0001 */

/* Remote-Aufruf-Status (NPSPEC-DISTRIBUTED-IPC-0001) */
#define DIST_RPC_PENDING 0
#define DIST_RPC_OK      1
#define DIST_RPC_TIMEOUT 2
#define DIST_RPC_FAILED  3

#define DIST_NODE_CAPACITY 8

typedef struct {
    uint32_t id;
    uint32_t role;          /* DIST_ROLE_* */
    uint32_t consistency;   /* DIST_CONS_* */
    uint32_t addr;          /* IP/Node-Adresse (vereinfacht) */
    uint32_t capabilities;  /* Bitmask Remote-Caps (NPSPEC-DISTRIBUTED-REMOTECAPABILITY-0001) */
    uint32_t placement_zone;
    uint32_t tx_count;      /* laufende dist. Transaktionen */
    uint32_t flags;
    uint32_t latency_us;    /* NPSPEC-DISTRIBUTED-LOCATION-0001 */
    uint32_t reserved;
} dist_node_t;

int distributed_initialize(void);
int distributed_self_test(void);

/* 0 = Erfolg */
int dist_register_node(uint32_t node_id, uint32_t role, uint32_t addr, uint32_t capabilities);

/* 0 = Erfolg, *out_status erhaelt DIST_RPC_* */
int dist_send_rpc(uint32_t node_id, uint32_t function_hash, uint32_t arg, uint32_t *out_status);

/* 0 = Erfolg (an alle Replika-Knoten gesendet) */
int dist_replicate(uint32_t data_id, uint32_t data_hash);

#endif

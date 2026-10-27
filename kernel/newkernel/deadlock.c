/* NovaOS C-Kernel - Nova Deadlock Detection (Port aus sync32.inc). */
#include "deadlock.h"

static uint32_t dl_wfg[DL_NODE_COUNT * DL_NODE_COUNT];
static uint32_t dl_owner[DL_NODE_COUNT];
static uint32_t dl_visited[DL_NODE_COUNT];
static uint32_t dl_in_stack[DL_NODE_COUNT];
static int dl_ready = 0;
static uint32_t dl_detected = 0;

void deadlock_initialize(void) {
    for (uint32_t i = 0; i < DL_NODE_COUNT * DL_NODE_COUNT; ++i) dl_wfg[i] = 0;
    for (uint32_t i = 0; i < DL_NODE_COUNT; ++i) dl_owner[i] = 0;
    dl_ready = 1;
    dl_detected = 0;
}

/* DFS von Knoten node; 0 = kein Zyklus, -1 = Zyklus gefunden. Nutzt
   dl_visited/dl_in_stack als temporaere Felder (vom Aufrufer
   zurueckgesetzt). */
static int deadlock_dfs_node(uint32_t node) {
    dl_visited[node] = 1;
    dl_in_stack[node] = 1;

    for (uint32_t neighbor = 0; neighbor < DL_NODE_COUNT; ++neighbor) {
        if (dl_wfg[node * DL_NODE_COUNT + neighbor] == 0) continue;

        if (dl_visited[neighbor] == 0) {
            if (deadlock_dfs_node(neighbor) != 0) {
                dl_in_stack[node] = 0;
                return -1;
            }
        } else if (dl_in_stack[neighbor] == 1) {
            dl_in_stack[node] = 0;
            return -1;
        }
    }

    dl_in_stack[node] = 0;
    return 0;
}

int deadlock_detect(void) {
    for (uint32_t i = 0; i < DL_NODE_COUNT; ++i) {
        dl_visited[i] = 0;
        dl_in_stack[i] = 0;
    }

    for (uint32_t start = 0; start < DL_NODE_COUNT; ++start) {
        if (dl_visited[start] != 0) continue;
        if (deadlock_dfs_node(start) != 0) return -1;
    }
    return 0;
}

int deadlock_add_wait(uint32_t task_id, uint32_t waits_on_id) {
    if (task_id >= DL_NODE_COUNT || waits_on_id >= DL_NODE_COUNT) return -1;
    if (task_id == waits_on_id) return -1;

    dl_wfg[task_id * DL_NODE_COUNT + waits_on_id] = 1;

    if (deadlock_detect() != 0) {
        ++dl_detected;
        return -1;
    }
    return 0;
}

void deadlock_remove_wait(uint32_t task_id, uint32_t waits_on_id) {
    if (task_id >= DL_NODE_COUNT || waits_on_id >= DL_NODE_COUNT) return;
    dl_wfg[task_id * DL_NODE_COUNT + waits_on_id] = 0;
}

int deadlock_self_test(void) {
    if (!dl_ready) return -1;

    /* 0->1, 1->2: kein Zyklus */
    if (deadlock_add_wait(0, 1) != 0) return -1;
    if (deadlock_add_wait(1, 2) != 0) return -1;

    /* 2->0: schliesst den Zyklus, muss erkannt werden */
    if (deadlock_add_wait(2, 0) == 0) return -1;
    if (dl_detected != 1) return -1;

    /* Kante entfernen -> kein Zyklus mehr */
    deadlock_remove_wait(2, 0);
    if (deadlock_detect() != 0) return -1;

    return 0;
}

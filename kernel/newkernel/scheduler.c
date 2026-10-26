/* NovaOS C-Kernel - Scheduler-Familie (Port aus scheduler32.inc). Siehe
   scheduler.h fuer den Hinweis zum bewusst fehlenden echten Taskwechsel. */
#include "scheduler.h"
#include "pmm.h"
#include "ipc.h"

#define CODE_SEGMENT 0x08u
#define DATA_SEGMENT 0x10u

/* --------------------------------------------------------------------- */
/* Haupt-Scheduler (nur Frame-Aufbau, kein echter Taskwechsel)            */
/* --------------------------------------------------------------------- */

static uint32_t scheduler_contexts[SCHEDULER_THREAD_COUNT];
static int scheduler_enabled = 0;
static uint32_t scheduler_thread1_runs = 0;
static uint32_t scheduler_thread2_runs = 0;

/* Bewusst niemals aufgerufen ohne echten Taskwechsel (siehe scheduler.h) -
   faehrt fort, sobald der Frame tatsaechlich geladen wird. */
static void scheduler_thread1(void) {
    ++scheduler_thread1_runs;
    ipc_message_t msg = {{1, 0, 0x4E4F5641u, 0}};
    ipc_send(&msg);
    for (;;) {
        __asm__ volatile ("pause");
    }
}

static void scheduler_thread2(void) {
    ++scheduler_thread2_runs;
    ipc_message_t msg;
    ipc_receive(&msg);
    for (;;) {
        __asm__ volatile ("pause");
    }
}

uint32_t scheduler_create_frame(uint32_t entry_point) {
    uint32_t numa;
    uint32_t page = pmm_alloc_page(&numa);
    if (page == 0) {
        return 0;
    }
    for (uint32_t i = 0; i < PMM_PAGE_SIZE; ++i) {
        ((uint8_t *)page)[i] = 0;
    }

    uint32_t frame = page + PMM_PAGE_SIZE - SCHEDULER_FRAME_SIZE;
    uint32_t *f = (uint32_t *)frame;
    f[0] = DATA_SEGMENT; /* DS */
    f[1] = DATA_SEGMENT; /* ES */
    f[2] = DATA_SEGMENT; /* FS */
    f[3] = DATA_SEGMENT; /* GS */
    f[4] = 0;  /* EDI */
    f[5] = 0;  /* ESI */
    f[6] = 0;  /* EBP */
    f[7] = 0;  /* ESP-Platzhalter von PUSHAD */
    f[8] = 0;  /* EBX */
    f[9] = 0;  /* EDX */
    f[10] = 0; /* ECX */
    f[11] = 0; /* EAX */
    f[12] = 32; /* normalisierter Vektor */
    f[13] = 0;  /* Fehlercode */
    f[14] = entry_point; /* EIP */
    f[15] = CODE_SEGMENT; /* CS */
    f[16] = 0x00000202u;  /* EFLAGS, IF gesetzt */

    return frame;
}

void scheduler_initialize(void) {
    scheduler_enabled = 0;
    for (uint32_t i = 0; i < SCHEDULER_THREAD_COUNT; ++i) {
        scheduler_contexts[i] = 0;
    }
    scheduler_thread1_runs = 0;
    scheduler_thread2_runs = 0;

    uint32_t frame1 = scheduler_create_frame((uint32_t)scheduler_thread1);
    if (frame1 == 0) return;
    scheduler_contexts[1] = frame1;

    uint32_t frame2 = scheduler_create_frame((uint32_t)scheduler_thread2);
    if (frame2 == 0) return;
    scheduler_contexts[2] = frame2;

    scheduler_enabled = 1;
}

int scheduler_self_test(void) {
    if (scheduler_enabled != 1) return -1;
    if (scheduler_contexts[1] == 0 || scheduler_contexts[2] == 0) return -1;

    /* Frame-Inhalt pruefen (EIP/CS/EFLAGS an den erwarteten Offsets). */
    uint32_t *f1 = (uint32_t *)scheduler_contexts[1];
    if (f1[14] != (uint32_t)scheduler_thread1) return -1;
    if (f1[15] != CODE_SEGMENT) return -1;
    if (f1[16] != 0x00000202u) return -1;

    return 0;
}

/* --------------------------------------------------------------------- */
/* Work-Stealing-Scheduler                                                */
/* --------------------------------------------------------------------- */

typedef struct {
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    uint32_t cpu_mask;
    uint32_t flags;
    uint32_t steal_count;
} ws_slot_t;

static ws_slot_t ws_slot_queues[WS_SLOT_COUNT];
static ws_task_entry_t ws_task_storage[WS_SLOT_COUNT][WS_QUEUE_DEPTH];
static int ws_initialized = 0;
static uint32_t ws_total_steals = 0;

void worksteal_initialize(void) {
    for (uint32_t i = 0; i < WS_SLOT_COUNT; ++i) {
        ws_slot_queues[i] = (ws_slot_t){0};
        ws_slot_queues[i].cpu_mask = 0xFFFFFFFFu; /* keine harte Affinitaet bekannt */
        for (uint32_t j = 0; j < WS_QUEUE_DEPTH; ++j) {
            ws_task_storage[i][j] = (ws_task_entry_t){0};
        }
    }
    ws_initialized = 1;
    ws_total_steals = 0;
}

int worksteal_enqueue(uint32_t slot, const ws_task_entry_t *task) {
    if (slot >= WS_SLOT_COUNT) return -1;
    ws_slot_t *q = &ws_slot_queues[slot];
    if (q->count >= WS_QUEUE_DEPTH) return -1;

    ws_task_storage[slot][q->tail] = *task;
    q->tail = (q->tail + 1 >= WS_QUEUE_DEPTH) ? 0 : q->tail + 1;
    ++q->count;
    return 0;
}

int worksteal_try_steal(uint32_t victim_slot, uint32_t thief_slot, ws_task_entry_t *out_task) {
    if (victim_slot >= WS_SLOT_COUNT || thief_slot >= WS_SLOT_COUNT) return -1;
    if (victim_slot == thief_slot) return -1;

    ws_slot_t *victim = &ws_slot_queues[victim_slot];
    if (victim->count == 0) return -1;

    ws_task_entry_t *task = &ws_task_storage[victim_slot][victim->head];

    if (task->flags & WS_TASK_FLAG_RT) return -1; /* RT-Bit: niemals stehlen */
    if (task->flags & WS_TASK_FLAG_HARD_AFF) {
        if ((ws_slot_queues[thief_slot].cpu_mask & task->cpu_mask) == 0) return -1;
    }

    *out_task = *task;
    victim->head = (victim->head + 1 >= WS_QUEUE_DEPTH) ? 0 : victim->head + 1;
    --victim->count;
    ++victim->steal_count;
    ++ws_total_steals;
    return 0;
}

int worksteal_self_test(void) {
    if (ws_initialized != 1) return -1;
    if (ws_slot_queues[0].count != 0) return -1;

    ws_task_entry_t normal_task = {(uint32_t)scheduler_thread1 /* Platzhalter-Entry */, 1, 0xFFFFFFFFu, 0};
    if (worksteal_enqueue(0, &normal_task) != 0) return -1;
    if (ws_slot_queues[0].count != 1) return -1;

    ws_task_entry_t rt_task = {(uint32_t)scheduler_thread2, 1, 0xFFFFFFFFu, WS_TASK_FLAG_RT};
    if (worksteal_enqueue(1, &rt_task) != 0) return -1;

    ws_task_entry_t stolen;
    if (worksteal_try_steal(1, 0, &stolen) == 0) return -1; /* RT-Schutz: muss fehlschlagen */

    if (worksteal_try_steal(0, 1, &stolen) != 0) return -1;

    if (ws_slot_queues[0].count != 0) return -1;

    return 0;
}

/* --------------------------------------------------------------------- */
/* RT-Scheduler (EDF / FIFO)                                             */
/* --------------------------------------------------------------------- */

#define RT_STATE_FREE 0u
#define RT_STATE_READY 1u

typedef struct {
    uint32_t id;
    uint32_t state;
    uint32_t owner;
    uint32_t deadline;
    uint32_t priority;
    uint32_t policy;
} rt_record_t;

static rt_record_t rt_table[RT_CAPACITY];
static uint32_t rt_next_id = 0;
static uint32_t rt_miss_count = 0;
static int rt_ready = 0;

void rt_scheduler_initialize(void) {
    for (uint32_t i = 0; i < RT_CAPACITY; ++i) {
        rt_table[i] = (rt_record_t){0};
    }
    rt_next_id = 0;
    rt_miss_count = 0;
    rt_ready = 1;
}

int rt_task_register(uint32_t owner, uint32_t deadline_ticks, uint32_t priority,
                      uint32_t policy, uint32_t *out_slot) {
    for (uint32_t i = 0; i < RT_CAPACITY; ++i) {
        if (rt_table[i].state == RT_STATE_FREE) {
            rt_table[i].id = rt_next_id++;
            rt_table[i].owner = owner;
            rt_table[i].state = RT_STATE_READY;
            rt_table[i].deadline = deadline_ticks;
            rt_table[i].priority = priority;
            rt_table[i].policy = policy;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int rt_select_next(uint32_t *out_slot) {
    int32_t best_slot = -1;
    uint32_t best_deadline = 0xFFFFFFFFu;

    for (uint32_t i = 0; i < RT_CAPACITY; ++i) {
        if (rt_table[i].state != RT_STATE_READY) continue;
        if (rt_table[i].policy == RT_POLICY_EDF) {
            if (rt_table[i].deadline < best_deadline) {
                best_deadline = rt_table[i].deadline;
                best_slot = (int32_t)i;
            }
        } else {
            /* FIFO: erster gefundener READY-Slot gewinnt. */
            if (best_slot == -1) {
                best_slot = (int32_t)i;
            }
        }
    }

    if (best_slot == -1) return -1;
    *out_slot = (uint32_t)best_slot;
    return 0;
}

int rt_check_deadline(uint32_t slot, uint32_t current_tick) {
    if (slot >= RT_CAPACITY) return -1;
    if (rt_table[slot].state == RT_STATE_FREE) return -1;
    if (current_tick <= rt_table[slot].deadline) return 0;
    ++rt_miss_count;
    return -1;
}

int rt_scheduler_self_test(void) {
    if (rt_ready != 1) return -1;

    uint32_t slot_a;
    if (rt_task_register(1, 1000, 0, RT_POLICY_EDF, &slot_a) != 0) return -1;

    uint32_t slot_b;
    if (rt_task_register(2, 500, 0, RT_POLICY_EDF, &slot_b) != 0) return -1;

    uint32_t selected;
    if (rt_select_next(&selected) != 0) return -1;
    if (selected != slot_b) return -1; /* fruehere Deadline gewinnt */

    if (rt_check_deadline(slot_b, 300) != 0) return -1; /* 300 <= 500: ok */
    if (rt_check_deadline(slot_b, 600) == 0) return -1; /* 600 > 500: miss */
    if (rt_miss_count != 1) return -1;

    return 0;
}

/* --------------------------------------------------------------------- */
/* Fair-Scheduler (CFS-Analogon)                                          */
/* --------------------------------------------------------------------- */

#define CFS_STATE_FREE     0u
#define CFS_STATE_RUNNABLE 1u

typedef struct {
    uint32_t id;
    uint32_t state;
    uint32_t owner;
    uint32_t vruntime;
    uint32_t weight;
    uint32_t last_run;
} cfs_record_t;

static cfs_record_t cfs_table[CFS_CAPACITY];
static uint32_t cfs_next_id = 0;
static int cfs_ready = 0;

/* TODO: durch wd_global_tick ersetzen, sobald das Watchdog-Subsystem
   portiert ist (wie in aslr.c). */
static uint32_t wd_global_tick_stub(void) {
    return 0;
}

void cfs_initialize(void) {
    for (uint32_t i = 0; i < CFS_CAPACITY; ++i) {
        cfs_table[i] = (cfs_record_t){0};
    }
    cfs_next_id = 0;
    cfs_ready = 1;
}

int cfs_task_enqueue(uint32_t owner, uint32_t weight, uint32_t *out_slot) {
    for (uint32_t i = 0; i < CFS_CAPACITY; ++i) {
        if (cfs_table[i].state == CFS_STATE_FREE) {
            cfs_table[i].id = cfs_next_id++;
            cfs_table[i].state = CFS_STATE_RUNNABLE;
            cfs_table[i].owner = owner;
            cfs_table[i].vruntime = 0;
            cfs_table[i].weight = weight;
            cfs_table[i].last_run = 0;
            *out_slot = i;
            return 0;
        }
    }
    return -1;
}

int cfs_pick_next(uint32_t *out_slot) {
    uint32_t best_vruntime = 0xFFFFFFFFu;
    int32_t best_slot = -1;

    for (uint32_t i = 0; i < CFS_CAPACITY; ++i) {
        if (cfs_table[i].state != CFS_STATE_RUNNABLE) continue;
        if (cfs_table[i].vruntime < best_vruntime) {
            best_vruntime = cfs_table[i].vruntime;
            best_slot = (int32_t)i;
        }
    }

    if (best_slot == -1) return -1;
    *out_slot = (uint32_t)best_slot;
    return 0;
}

void cfs_update_vruntime(uint32_t slot, uint32_t elapsed_ticks) {
    if (slot >= CFS_CAPACITY) return;
    if (cfs_table[slot].state == CFS_STATE_FREE) return;

    uint32_t weight = cfs_table[slot].weight;
    uint32_t delta = (weight != 0) ? (elapsed_ticks / weight) : elapsed_ticks;
    cfs_table[slot].vruntime += delta;
    cfs_table[slot].last_run = wd_global_tick_stub();
}

int cfs_self_test(void) {
    if (cfs_ready != 1) return -1;

    uint32_t slot_a, slot_b;
    if (cfs_task_enqueue(1, 1, &slot_a) != 0) return -1;
    if (cfs_task_enqueue(2, 2, &slot_b) != 0) return -1;

    uint32_t selected;
    if (cfs_pick_next(&selected) != 0) return -1; /* beide vruntime=0, kleinster Index */

    cfs_update_vruntime(slot_a, 10); /* 10 / weight=1 = 10 */
    if (cfs_pick_next(&selected) != 0) return -1;
    if (selected != slot_b) return -1; /* vruntime 0 < 10 */

    return 0;
}

/* --------------------------------------------------------------------- */
/* QoS-Scheduler                                                         */
/* --------------------------------------------------------------------- */

typedef struct {
    uint32_t id;
    uint32_t qos_class;
    uint32_t budget;
    uint32_t max_budget;
    uint32_t used;
} qos_record_t;

static qos_record_t qos_table[QOS_CLASS_COUNT];
static int qos_ready = 0;
static uint32_t qos_epoch = 0;
static uint32_t qos_throttled = 0;

void qos_initialize(void) {
    static const uint32_t budgets[QOS_CLASS_COUNT] = {100, 70, 40, 10};
    for (uint32_t i = 0; i < QOS_CLASS_COUNT; ++i) {
        qos_table[i].id = i;
        qos_table[i].qos_class = i;
        qos_table[i].budget = budgets[i];
        qos_table[i].max_budget = budgets[i];
        qos_table[i].used = 0;
    }
    qos_ready = 1;
    qos_epoch = 0;
    qos_throttled = 0;
}

int qos_consume(uint32_t qos_class) {
    if (qos_class >= QOS_CLASS_COUNT) return -1;
    qos_record_t *q = &qos_table[qos_class];
    if (q->budget == 0) {
        ++qos_throttled;
        return -1;
    }
    --q->budget;
    ++q->used;
    return 0;
}

void qos_new_epoch(void) {
    for (uint32_t i = 0; i < QOS_CLASS_COUNT; ++i) {
        qos_table[i].budget = qos_table[i].max_budget;
    }
    ++qos_epoch;
}

int qos_query(uint32_t qos_class, uint32_t *out_budget, uint32_t *out_max) {
    if (qos_class >= QOS_CLASS_COUNT) return -1;
    *out_budget = qos_table[qos_class].budget;
    *out_max = qos_table[qos_class].max_budget;
    return 0;
}

int qos_self_test(void) {
    if (qos_ready != 1) return -1;

    uint32_t budget, max;
    if (qos_query(QOS_CLASS_RT, &budget, &max) != 0) return -1;
    if (budget != 100) return -1;

    for (uint32_t i = 0; i < 100; ++i) {
        qos_consume(QOS_CLASS_RT);
    }
    if (qos_consume(QOS_CLASS_RT) == 0) return -1; /* ausgeschoepft */
    if (qos_throttled != 1) return -1;

    qos_new_epoch();
    if (qos_query(QOS_CLASS_RT, &budget, &max) != 0) return -1;
    if (budget != 100) return -1;
    if (qos_epoch != 1) return -1;

    return 0;
}

#define SCHED_RT_THRESHOLD 200u

int scheduler_admit_task(uint32_t owner, uint32_t priority, uint32_t qos_class, uint32_t *out_slot) {
    if (priority >= SCHED_RT_THRESHOLD) {
        uint32_t deadline = wd_global_tick_stub() + 500u;
        return rt_task_register(owner, deadline, priority, RT_POLICY_EDF, out_slot);
    }

    if (qos_consume(qos_class) != 0) return -1;

    uint32_t weight = (priority >= 256u) ? 1u : (256u - priority);
    if (weight < 1u) weight = 1u;

    return cfs_task_enqueue(owner, weight, out_slot);
}

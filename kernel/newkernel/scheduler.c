/* NovaOS C-Kernel - Scheduler-Familie (Port aus scheduler32.inc). Siehe
   scheduler.h fuer den Hinweis zum jetzt verdrahteten, aber bewusst erst
   spaet aktivierten echten Taskwechsel. */
#include "scheduler.h"
#include "pmm.h"
#include "ipc.h"
#include "serial.h"

#define CODE_SEGMENT 0x08u
#define DATA_SEGMENT 0x10u

/* --------------------------------------------------------------------- */
/* Haupt-Scheduler (Frame-Aufbau + echter Round-Robin-Taskwechsel)        */
/* --------------------------------------------------------------------- */

static uint32_t scheduler_contexts[SCHEDULER_THREAD_COUNT];
static int32_t thread_cpu_affinity[SCHEDULER_THREAD_COUNT]; /* -1 = beliebige CPU */
static int32_t thread_running_on[SCHEDULER_THREAD_COUNT];   /* -1 = frei, 0 = CPU0 */
static int32_t scheduler_current_thread = -1; /* Pendant zu per_cpu_current_thread[0] - nur 1 CPU */
static int scheduler_frames_ready = 0; /* Frames gueltig aufgebaut (fuer scheduler_self_test) */
static int scheduler_enabled = 0;      /* echter Taskwechsel aktiv (siehe scheduler_enable) */
static uint32_t scheduler_thread1_runs = 0;
static uint32_t scheduler_thread2_runs = 0;

static uint32_t ipc_sent = 0;
static uint32_t ipc_received = 0;
static uint32_t ipc_error = 0;
static uint32_t ipc_last_sequence = 0;
static int preempt_thread1_logged = 0;
static int preempt_thread2_logged = 0;
static uint32_t scheduler_dynamic_runs = 0;
static int preempt_dynamic_logged = 0;

/* Laufen erst, sobald scheduler_on_tick sie tatsaechlich einlastet (siehe
   scheduler_enable) - bis dahin bleiben ihre Frames unbenutztes, aber
   gueltiges Datenmaterial fuer scheduler_self_test. Nicht static, da
   threadmgr.c ihre Adresse fuer thread_register braucht. */
void scheduler_thread1(void) {
    for (;;) {
        ++scheduler_thread1_runs;
        if (!preempt_thread1_logged) {
            preempt_thread1_logged = 1;
            serial_puts("NOVA: Praeemptiver Taskwechsel aktiv - Thread 1 lief\r\n");
        }
        uint32_t seq = ipc_sent + 1;
        ipc_message_t msg = {{1, seq, 0x4E4F5641u, 0}};
        if (ipc_send(&msg) != 0) {
            ++ipc_sent;
        }
        __asm__ volatile ("pause");
    }
}

void scheduler_thread2(void) {
    for (;;) {
        ++scheduler_thread2_runs;
        if (!preempt_thread2_logged) {
            preempt_thread2_logged = 1;
            serial_puts("NOVA: Praeemptiver Taskwechsel aktiv - Thread 2 lief\r\n");
        }
        ipc_message_t msg;
        if (ipc_receive(&msg) != 0) {
            if (msg.words[0] != 1 || msg.words[2] != 0x4E4F5641u) {
                ipc_error = 1;
            } else if (msg.words[1] > ipc_last_sequence) {
                ipc_last_sequence = msg.words[1];
                ++ipc_received;
            } else {
                ipc_error = 1;
            }
        }
        __asm__ volatile ("pause");
    }
}

/* Einstiegspunkt fuer dynamisch erzeugte Threads (threadmgr.c's
   thread_create_dynamic) - minimaler Platzhalter-Koerper, 1:1 wie im
   alten ASM (nur Lebenszeichen, kein eigener Zweck). */
void scheduler_dynamic_thread(void) {
    for (;;) {
        ++scheduler_dynamic_runs;
        if (!preempt_dynamic_logged) {
            preempt_dynamic_logged = 1;
            serial_puts("NOVA: Praeemptiver Taskwechsel aktiv - dynamischer Thread lief\r\n");
        }
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
    scheduler_frames_ready = 0;
    scheduler_current_thread = -1;
    for (uint32_t i = 0; i < SCHEDULER_THREAD_COUNT; ++i) {
        scheduler_contexts[i] = 0;
        thread_cpu_affinity[i] = -1;
        thread_running_on[i] = -1;
    }
    /* §133: Slot 0 (BSP-Idle/Kernel-Thread) auf CPU 0 pinnen - belegt
       reserviert diesen Slot permanent, scheduler_on_tick wird ihn daher
       nie als Ziel waehlen. */
    thread_cpu_affinity[0] = 0;
    thread_running_on[0] = 0;

    scheduler_thread1_runs = 0;
    scheduler_thread2_runs = 0;

    uint32_t frame1 = scheduler_create_frame((uint32_t)scheduler_thread1);
    if (frame1 == 0) return;
    scheduler_contexts[1] = frame1;

    uint32_t frame2 = scheduler_create_frame((uint32_t)scheduler_thread2);
    if (frame2 == 0) return;
    scheduler_contexts[2] = frame2;

    scheduler_frames_ready = 1;
}

/* EINWEG-Freigabe des echten Taskwechsels (siehe scheduler.h) - erst
   aufzurufen, wenn nichts im Aufrufer mehr sequentiell weiterlaufen
   muss, danach kann jeder Timer-Tick die aktuelle Ausfuehrung permanent
   zugunsten von scheduler_thread1/2 verdraengen. */
void scheduler_enable(void) {
    scheduler_enabled = 1;
}

uint32_t scheduler_on_tick(uint32_t frame_ptr) {
    if (!scheduler_enabled) return frame_ptr;

    int32_t previous_slot;
    int32_t current = scheduler_current_thread;

    if (current == -1) {
        /* CPU war idle - Slot 0 nur als Suchstartpunkt, kein Frame zu sichern. */
        previous_slot = 0;
    } else {
        scheduler_contexts[current] = frame_ptr;
        thread_running_on[current] = -1;
        previous_slot = current;
    }

    int32_t search_start = previous_slot;
    int32_t selected = -1;
    int32_t slot = previous_slot;
    for (;;) {
        slot = (slot + 1 >= (int32_t)SCHEDULER_THREAD_COUNT) ? 0 : slot + 1;
        if (slot == search_start) break; /* alle Slots geprueft, keiner passt */

        if (thread_cpu_affinity[slot] != -1 && thread_cpu_affinity[slot] != 0) continue;
        if (thread_running_on[slot] != -1) continue;
        if (scheduler_contexts[slot] == 0) continue;

        selected = slot;
        break;
    }

    if (selected != -1) {
        scheduler_current_thread = selected;
        thread_running_on[selected] = 0;
        return scheduler_contexts[selected];
    }

    /* Kein kompatibler Slot gefunden. */
    if (current == -1) {
        return frame_ptr; /* war idle, bleibt idle */
    }
    thread_running_on[previous_slot] = 0;
    scheduler_current_thread = previous_slot;
    return frame_ptr; /* gleicher Thread laeuft unveraendert weiter */
}

int scheduler_claim_dynamic_slot(uint32_t entry_point, int32_t cpu_affinity, uint32_t *out_slot) {
    for (uint32_t slot = 1; slot < SCHEDULER_THREAD_COUNT; ++slot) {
        if (scheduler_contexts[slot] != 0) continue;

        uint32_t context = scheduler_create_frame(entry_point);
        if (context == 0) return -1;

        scheduler_contexts[slot] = context;
        thread_cpu_affinity[slot] = cpu_affinity;
        thread_running_on[slot] = -1;
        *out_slot = slot;
        return 0;
    }
    return -1;
}

void scheduler_release_dynamic_slot(uint32_t slot) {
    if (slot == 0 || slot >= SCHEDULER_THREAD_COUNT) return;
    scheduler_contexts[slot] = 0;
    thread_cpu_affinity[slot] = -1;
    thread_running_on[slot] = -1;
}

uint32_t scheduler_context_at(uint32_t slot) {
    if (slot >= SCHEDULER_THREAD_COUNT) return 0;
    return scheduler_contexts[slot];
}

int32_t scheduler_affinity_at(uint32_t slot) {
    if (slot >= SCHEDULER_THREAD_COUNT) return -1;
    return thread_cpu_affinity[slot];
}

int32_t scheduler_running_on_at(uint32_t slot) {
    if (slot >= SCHEDULER_THREAD_COUNT) return -1;
    return thread_running_on[slot];
}

int scheduler_self_test(void) {
    if (scheduler_frames_ready != 1) return -1;
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

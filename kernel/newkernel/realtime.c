/* NovaOS C-Kernel - Realtime-Subsystem (Port aus realtime32.inc). */
#include "realtime.h"

typedef struct {
    uint32_t task_id;
    uint32_t priority;
    uint32_t sched_class;
    uint32_t latency_class;
    uint32_t period_us;
    uint32_t deadline_us;
    uint32_t wcet_us;
    uint32_t budget_us;
    uint32_t jitter_max_us;
    uint32_t miss_count;
    uint32_t exec_count;
    uint32_t flags;
} rt_task_t;

static rt_task_t rt_task_table[RT_TASK_CAPACITY];
static uint32_t rt_task_count = 0;
static uint32_t rt_jitter_global_us = 0;
static uint32_t rt_deadline_miss_total = 0;
static int rtmgr_ready = 0;

int rtmgr_task_register(uint32_t task_id, uint32_t priority, uint32_t sched_class,
                         uint32_t latency_class) {
    if (rt_task_count >= RT_TASK_CAPACITY) {
        return -1;
    }
    rt_task_t *task = &rt_task_table[rt_task_count];
    task->task_id = task_id;
    task->priority = priority;
    task->sched_class = sched_class;
    task->latency_class = latency_class;
    task->period_us = 10000;   /* 10ms Default */
    task->deadline_us = 10000;
    task->wcet_us = 1000;      /* 1ms WCET */
    task->budget_us = 5000;    /* 5ms Budget */
    task->jitter_max_us = 0;
    task->miss_count = 0;
    task->exec_count = 0;
    task->flags = RT_FLAG_ACTIVE;
    ++rt_task_count;
    return 0;
}

int realtime_initialize(void) {
    for (uint32_t i = 0; i < RT_TASK_CAPACITY; ++i) {
        rt_task_table[i] = (rt_task_t){0};
    }
    rt_task_count = 0;
    rt_jitter_global_us = 0;
    rt_deadline_miss_total = 0;

    if (rtmgr_task_register(0, RT_PRIO_BEST_EFFORT, RT_SCHED_IDLE, RT_LATENCY_BEST_EFF) != 0) {
        return -1;
    }

    rtmgr_ready = 1;
    return 0;
}

static rt_task_t *rtmgr_find(uint32_t task_id) {
    for (uint32_t i = 0; i < rt_task_count; ++i) {
        if (rt_task_table[i].task_id == task_id) {
            return &rt_task_table[i];
        }
    }
    return 0;
}

int rt_set_params(uint32_t task_id, uint32_t period_us, uint32_t deadline_us, uint32_t wcet_us) {
    rt_task_t *task = rtmgr_find(task_id);
    if (!task) {
        return -1;
    }
    task->period_us = period_us;
    task->deadline_us = deadline_us;
    task->wcet_us = wcet_us;
    return 0;
}

int rt_record_jitter(uint32_t task_id, uint32_t jitter_us) {
    rt_task_t *task = rtmgr_find(task_id);
    if (task && jitter_us > task->jitter_max_us) {
        task->jitter_max_us = jitter_us;
    }
    if (jitter_us > rt_jitter_global_us) {
        rt_jitter_global_us = jitter_us;
    }
    return 0;
}

int rt_deadline_check(uint32_t task_id) {
    rt_task_t *task = rtmgr_find(task_id);
    if (!task) {
        return -1;
    }
    ++task->exec_count;

    /* Vereinfacht: Jitter-Check gegen halbe Deadline. */
    if (task->jitter_max_us <= task->deadline_us / 2) {
        return 0;
    }

    ++task->miss_count;
    ++rt_deadline_miss_total;
    return (task->latency_class == RT_LATENCY_HARD) ? -1 : 0;
}

int realtime_self_test(void) {
    return (rtmgr_ready == 1) ? 0 : -1;
}

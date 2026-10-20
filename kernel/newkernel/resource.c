/* NovaOS C-Kernel - Resource-Management (Port aus resource32.inc). */
#include "resource.h"

typedef struct {
    uint32_t type;
    uint32_t total;
    uint32_t reserved;
    uint32_t used;
    uint32_t guarantee;
    uint32_t economy_cost;
    uint32_t admit_threshold;   /* Prozent */
    uint32_t reclaim_threshold; /* Prozent */
} res_bucket_t;

static res_bucket_t res_buckets[RES_TYPE_COUNT];
static int res_ready = 0;

void resource_initialize(void) {
    for (uint32_t i = 0; i < RES_TYPE_COUNT; ++i) {
        res_buckets[i] = (res_bucket_t){0};
    }

    res_buckets[RES_TYPE_CPU].type = RES_TYPE_CPU;
    res_buckets[RES_TYPE_CPU].total = 100;
    res_buckets[RES_TYPE_CPU].guarantee = RES_GUAR_SOFT;
    res_buckets[RES_TYPE_CPU].admit_threshold = 80;
    res_buckets[RES_TYPE_CPU].reclaim_threshold = 90;

    res_buckets[RES_TYPE_MEMORY].type = RES_TYPE_MEMORY;
    res_buckets[RES_TYPE_MEMORY].total = 1024; /* Pages, vereinfacht */
    res_buckets[RES_TYPE_MEMORY].guarantee = RES_GUAR_HARD;
    res_buckets[RES_TYPE_MEMORY].admit_threshold = 85;
    res_buckets[RES_TYPE_MEMORY].reclaim_threshold = 95;

    res_buckets[RES_TYPE_IO].type = RES_TYPE_IO;
    res_buckets[RES_TYPE_IO].total = 10000; /* IOPS */
    res_buckets[RES_TYPE_IO].guarantee = RES_GUAR_SOFT;
    res_buckets[RES_TYPE_IO].admit_threshold = 70;
    res_buckets[RES_TYPE_IO].reclaim_threshold = 85;

    res_buckets[RES_TYPE_NETWORK].type = RES_TYPE_NETWORK;
    res_buckets[RES_TYPE_NETWORK].total = 1000; /* Mbps */
    res_buckets[RES_TYPE_NETWORK].guarantee = RES_GUAR_BEST_EFFORT;
    res_buckets[RES_TYPE_NETWORK].admit_threshold = 80;
    res_buckets[RES_TYPE_NETWORK].reclaim_threshold = 90;

    res_ready = 1;
}

int res_request(uint32_t type, uint32_t amount) {
    if (type >= RES_TYPE_COUNT) {
        return -1;
    }
    res_bucket_t *bucket = &res_buckets[type];
    uint32_t max_admit = (bucket->total * bucket->admit_threshold) / 100;
    if (bucket->used + amount > max_admit) {
        return -1;
    }
    bucket->used += amount;
    return 0;
}

int res_release(uint32_t type, uint32_t amount) {
    if (type >= RES_TYPE_COUNT) {
        return -1;
    }
    res_bucket_t *bucket = &res_buckets[type];
    if (bucket->used < amount) {
        bucket->used = 0; /* Underflow-Schutz wie im ASM */
    } else {
        bucket->used -= amount;
    }
    return 0;
}

int res_reserve(uint32_t type, uint32_t amount, uint32_t guarantee) {
    if (type >= RES_TYPE_COUNT) {
        return -1;
    }
    res_bucket_t *bucket = &res_buckets[type];
    uint32_t free_capacity = bucket->total - bucket->reserved;
    if (amount > free_capacity) {
        return -1;
    }
    bucket->reserved += amount;
    bucket->guarantee = guarantee;
    return 0;
}

int resource_self_test(void) {
    if (res_ready != 1) return -1;

    if (res_request(RES_TYPE_CPU, 20) != 0) return -1;
    if (res_release(RES_TYPE_CPU, 20) != 0) return -1;
    if (res_reserve(RES_TYPE_MEMORY, 64, RES_GUAR_HARD) != 0) return -1;

    return 0;
}

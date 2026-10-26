/* NovaOS C-Kernel - Storage Block Layer (Port aus block32.inc). */
#include "block.h"

typedef struct {
    uint32_t id;
    uint32_t flags;
    uint32_t block_size;
    uint32_t block_count_lo;
    uint32_t block_count_hi;
    uint32_t transport;
    uint32_t driver_handle;
    uint32_t reads;
    uint32_t writes;
    uint32_t errors;
    uint32_t health;
    block_io_fn_t read_fn;
    block_io_fn_t write_fn;
    block_flush_fn_t flush_fn;
    block_offline_fn_t offline_fn;
} block_record_t;

static block_record_t block_records[BLOCK_CAPACITY];
static uint32_t block_count = 0;
static int block_layer_ready = 0;

void block_layer_initialize(void) {
    for (uint32_t i = 0; i < BLOCK_CAPACITY; ++i) {
        block_records[i] = (block_record_t){0};
    }
    block_count = 0;
    block_layer_ready = 1;
}

int block_device_register(const block_register_params_t *params, block_io_fn_t read_fn,
                           block_io_fn_t write_fn, block_flush_fn_t flush_fn,
                           block_offline_fn_t offline_fn, void **out_record) {
    if (block_count >= BLOCK_CAPACITY) {
        return -1;
    }

    uint32_t slot;
    for (slot = 0; slot < BLOCK_CAPACITY; ++slot) {
        if (!(block_records[slot].flags & BLOCK_FLAG_ONLINE)) {
            break;
        }
    }
    if (slot >= BLOCK_CAPACITY) {
        return -1;
    }

    block_record_t *rec = &block_records[slot];
    rec->id = params->id;
    rec->flags = params->flags | BLOCK_FLAG_ONLINE;
    rec->block_size = params->block_size;
    rec->block_count_lo = params->block_count_lo;
    rec->block_count_hi = params->block_count_hi;
    rec->transport = params->transport;
    rec->driver_handle = params->driver_handle;
    rec->reads = 0;
    rec->writes = 0;
    rec->errors = 0;
    rec->health = BLOCK_HEALTH_OK;
    rec->read_fn = read_fn;
    rec->write_fn = write_fn;
    rec->flush_fn = flush_fn;
    rec->offline_fn = offline_fn;

    ++block_count;
    if (out_record) {
        *out_record = rec;
    }
    return 0;
}

void block_device_offline(void *record) {
    if (!record) {
        return;
    }
    block_record_t *rec = (block_record_t *)record;
    rec->flags &= ~BLOCK_FLAG_ONLINE;
    rec->flags |= BLOCK_FLAG_DEGRADED;
    rec->health = BLOCK_HEALTH_FAIL;
}

int block_io_submit(void *record, uint32_t lba_lo, uint32_t lba_hi, uint32_t count,
                     void *buffer, uint32_t direction) {
    if (!record) {
        return -1;
    }
    block_record_t *rec = (block_record_t *)record;

    if (!(rec->flags & BLOCK_FLAG_ONLINE)) {
        return -1;
    }
    if (direction == BLK_IO_WRITE && (rec->flags & BLOCK_FLAG_READONLY)) {
        return -1;
    }

    /* Einfache Bereichspruefung (vereinfacht fuer Boot-Kontext). */
    if (lba_hi == 0) {
        uint32_t end = lba_lo + count;
        if (end < lba_lo || end > rec->block_count_lo) {
            ++rec->errors;
            rec->health = BLOCK_HEALTH_WARN;
            return -1;
        }
    } else if (lba_hi >= rec->block_count_hi) {
        return -1;
    }

    int io_rc;
    switch (direction) {
    case BLK_IO_READ:
        if (!rec->read_fn) return -1;
        io_rc = rec->read_fn(rec, lba_lo, lba_hi, count, buffer);
        if (io_rc != 0) {
            ++rec->errors;
            rec->health = BLOCK_HEALTH_WARN;
            return -1;
        }
        ++rec->reads;
        return 0;
    case BLK_IO_WRITE:
        if (!rec->write_fn) return -1;
        io_rc = rec->write_fn(rec, lba_lo, lba_hi, count, buffer);
        if (io_rc != 0) {
            ++rec->errors;
            rec->health = BLOCK_HEALTH_WARN;
            return -1;
        }
        ++rec->writes;
        return 0;
    case BLK_IO_FLUSH:
        if (!rec->flush_fn) {
            return 0; /* kein Flush verfuegbar: kein Fehler */
        }
        io_rc = rec->flush_fn(rec);
        if (io_rc != 0) {
            ++rec->errors;
            rec->health = BLOCK_HEALTH_WARN;
            return -1;
        }
        return 0;
    default:
        return -1;
    }
}

int block_device_lookup_by_handle(uint32_t driver_handle, void **out_record) {
    for (uint32_t i = 0; i < BLOCK_CAPACITY; ++i) {
        if (block_records[i].driver_handle == driver_handle) {
            if (out_record) {
                *out_record = &block_records[i];
            }
            return 0;
        }
    }
    return -1;
}

int block_layer_self_test(void) {
    if (block_layer_ready != 1) return -1;
    if (block_count != 0) return -1;

    block_register_params_t params = {
        .id = 0,
        .flags = BLOCK_FLAG_ONLINE | BLOCK_FLAG_FLUSH_CAP,
        .block_size = 512,
        .block_count_lo = 1, /* 1 Block als Dummy */
        .block_count_hi = 0,
        .transport = BLOCK_TRANSPORT_VIRT,
        .driver_handle = 0,
    };
    void *record = 0;
    if (block_device_register(&params, 0, 0, 0, 0, &record) != 0) {
        return -1;
    }
    if (block_count != 1) {
        return -1;
    }

    /* Aufraeumen. */
    block_count = 0;
    for (uint32_t i = 0; i < BLOCK_CAPACITY; ++i) {
        block_records[i] = (block_record_t){0};
    }

    return 0;
}

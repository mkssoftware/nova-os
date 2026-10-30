/* NovaOS C-Kernel - Nova Security Audit Log (Port aus security32.inc). */
#include "audit.h"
#include "resilience.h"

typedef struct {
    uint32_t seq;
    uint32_t tick;
    uint32_t type;
    uint32_t subject;
    uint32_t object;
    uint32_t result;
    uint32_t flags;
} audit_event_t;

static audit_event_t audit_log_buf[AUDIT_CAPACITY];
static uint32_t audit_write_pos = 0;
static uint32_t audit_seq = 0;
static uint32_t audit_total = 0;
static uint32_t audit_violations = 0;
static int audit_ready = 0;

void security_audit_initialize(void) {
    for (uint32_t i = 0; i < AUDIT_CAPACITY; ++i) audit_log_buf[i] = (audit_event_t){0};
    audit_write_pos = 0;
    audit_seq = 0;
    audit_total = 0;
    audit_violations = 0;
    audit_ready = 1;
}

int security_audit_log(uint32_t type, uint32_t subject, uint32_t object, uint32_t result) {
    audit_event_t *entry = &audit_log_buf[audit_write_pos];
    entry->seq = audit_seq++;
    entry->tick = resilience_global_tick();
    entry->type = type;
    entry->subject = subject;
    entry->object = object;
    entry->result = result;
    entry->flags = 0;

    audit_write_pos = (audit_write_pos + 1 >= AUDIT_CAPACITY) ? 0 : audit_write_pos + 1;
    ++audit_total;

    if (type == AUDIT_TYPE_VIOLATION) {
        ++audit_violations;
    }
    return 0;
}

uint32_t security_audit_count(uint32_t type) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < AUDIT_CAPACITY; ++i) {
        if (audit_log_buf[i].type == type) ++count;
    }
    return count;
}

int security_audit_self_test(void) {
    if (audit_ready != 1) return -1;

    security_audit_log(AUDIT_TYPE_CAP_GRANT, 1, 0x00000001u, 1);
    security_audit_log(AUDIT_TYPE_VIOLATION, 2, 0xDEADBEEFu, 0);

    if (audit_total != 2) return -1;
    if (audit_violations != 1) return -1;
    if (security_audit_count(AUDIT_TYPE_VIOLATION) != 1) return -1;
    if (security_audit_count(AUDIT_TYPE_CAP_GRANT) != 1) return -1;

    for (uint32_t i = 0; i < 31; ++i) {
        security_audit_log(AUDIT_TYPE_SYSCALL, 0, 0, 0);
    }
    /* write_pos sollte bei (2+31) mod 32 = 1 stehen */
    if (audit_write_pos != 1) return -1;

    return 0;
}

/* NovaOS C-Kernel - Nova Mandatory Access Control (Port aus security32.inc). */
#include "mac.h"

typedef struct {
    uint32_t id;
    uint32_t label;
    uint32_t flags;
} mac_record_t;

static mac_record_t mac_subjects[MAC_SUBJECT_COUNT];
static mac_record_t mac_objects[MAC_OBJECT_COUNT];
static int mac_ready = 0;
static uint32_t mac_denials = 0;

void mac_initialize(void) {
    for (uint32_t i = 0; i < MAC_SUBJECT_COUNT; ++i) mac_subjects[i] = (mac_record_t){0};
    for (uint32_t i = 0; i < MAC_OBJECT_COUNT; ++i) mac_objects[i] = (mac_record_t){0};
    mac_ready = 1;
    mac_denials = 0;
}

int mac_register_subject(uint32_t subject_id, uint32_t label) {
    for (uint32_t i = 0; i < MAC_SUBJECT_COUNT; ++i) {
        if (mac_subjects[i].id == 0) {
            mac_subjects[i].id = subject_id;
            mac_subjects[i].label = label;
            mac_subjects[i].flags = 1;
            return 0;
        }
    }
    return -1;
}

int mac_register_object(uint32_t object_id, uint32_t label) {
    for (uint32_t i = 0; i < MAC_OBJECT_COUNT; ++i) {
        if (mac_objects[i].id == 0) {
            mac_objects[i].id = object_id;
            mac_objects[i].label = label;
            mac_objects[i].flags = 1;
            return 0;
        }
    }
    return -1;
}

int mac_check(uint32_t subject_id, uint32_t object_id) {
    mac_record_t *subject = 0;
    for (uint32_t i = 0; i < MAC_SUBJECT_COUNT; ++i) {
        if (mac_subjects[i].id == subject_id) { subject = &mac_subjects[i]; break; }
    }
    if (!subject) { ++mac_denials; return -1; }

    mac_record_t *object = 0;
    for (uint32_t i = 0; i < MAC_OBJECT_COUNT; ++i) {
        if (mac_objects[i].id == object_id) { object = &mac_objects[i]; break; }
    }
    if (!object) { ++mac_denials; return -1; }

    if (subject->label < object->label) {
        ++mac_denials;
        return -1;
    }
    return 0;
}

int mac_self_test(void) {
    if (mac_ready != 1) return -1;

    mac_register_subject(1, MAC_LABEL_CONFIDENTIAL);
    mac_register_object(10, MAC_LABEL_INTERNAL);
    if (mac_check(1, 10) != 0) return -1; /* 2>=1 erlaubt */

    mac_register_subject(2, MAC_LABEL_PUBLIC);
    mac_register_object(20, MAC_LABEL_SECRET);
    if (mac_check(2, 20) == 0) return -1; /* 0<3 verboten */

    if (mac_denials != 1) return -1;

    return 0;
}

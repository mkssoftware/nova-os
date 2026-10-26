/* NovaOS C-Kernel - Nova Privacy Label Policy (Port aus adaptive32.inc). */
#include "privlabel.h"

typedef struct {
    uint32_t obj_id; /* kein 1-basiertes id-Feld im alten ASM; obj_id==0 =
                         frei ist bewusst uebernommen (0 ist keine gueltige
                         Objekt-ID in diesem Subsystem). */
    uint32_t has_entry;
    uint32_t label;
    uint32_t retention;
} priv_rec_t;

static priv_rec_t priv_table[PRIV_OBJ_COUNT];
static int privlabel_ready = 0;
static uint32_t privlabel_denials = 0;

void privlabel_initialize(void) {
    for (uint32_t i = 0; i < PRIV_OBJ_COUNT; ++i) priv_table[i] = (priv_rec_t){0};
    privlabel_denials = 0;
    privlabel_ready = 1;
}

int privlabel_register(uint32_t obj_id, uint32_t label, uint32_t retention) {
    if (label > PRIV_LABEL_CONF) return -1;

    for (uint32_t i = 0; i < PRIV_OBJ_COUNT; ++i) {
        if (priv_table[i].has_entry == 0) {
            priv_table[i].obj_id = obj_id;
            priv_table[i].has_entry = 1;
            priv_table[i].label = label;
            priv_table[i].retention = retention;
            return 0;
        }
    }
    return -1;
}

int privlabel_check(uint32_t obj_id, uint32_t required_label) {
    for (uint32_t i = 0; i < PRIV_OBJ_COUNT; ++i) {
        if (priv_table[i].has_entry != 0 && priv_table[i].obj_id == obj_id) {
            if (priv_table[i].label <= required_label) return 0;
            ++privlabel_denials;
            return -1;
        }
    }
    return -1;
}

int privlabel_self_test(void) {
    if (privlabel_register(100, PRIV_LABEL_PUBLIC, 0) != 0) return -1;
    if (privlabel_check(100, PRIV_LABEL_SENSITIVE) != 0) return -1; /* PUBLIC<=SENSITIVE */

    if (privlabel_register(101, PRIV_LABEL_CONF, 0) != 0) return -1;
    if (privlabel_check(101, PRIV_LABEL_PRIVATE) == 0) return -1; /* CONF>PRIVATE: verweigert */

    return 0;
}

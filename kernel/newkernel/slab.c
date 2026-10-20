/* NovaOS C-Kernel - Slab Allocator (Port aus memory32.inc). Freie Objekte
   bilden eine intrusive Linked List: die ersten 4 Byte jedes freien Objekts
   zeigen auf das naechste freie Objekt (0 = Listenende), identisch zum ASM. */
#include "slab.h"

#define SLAB_CLASS_COUNT  4
#define SLAB_OBJECTS_PER  16u

typedef struct {
    uint32_t obj_size;
    void *free_head;
    uint32_t alloc_count;
    uint32_t free_count;
    uint32_t total;
} slab_class_t;

static slab_class_t slab_classes[SLAB_CLASS_COUNT];
static uint8_t slab_pool_0[SLAB_SIZE_0 * SLAB_OBJECTS_PER];
static uint8_t slab_pool_1[SLAB_SIZE_1 * SLAB_OBJECTS_PER];
static uint8_t slab_pool_2[SLAB_SIZE_2 * SLAB_OBJECTS_PER];
static uint8_t slab_pool_3[SLAB_SIZE_3 * SLAB_OBJECTS_PER];
static int slab_ready = 0;

static void slab_get_pool_base(int class_index, uint8_t **base, uint32_t *obj_size) {
    switch (class_index) {
    case 0: *base = slab_pool_0; *obj_size = SLAB_SIZE_0; break;
    case 1: *base = slab_pool_1; *obj_size = SLAB_SIZE_1; break;
    case 2: *base = slab_pool_2; *obj_size = SLAB_SIZE_2; break;
    default: *base = slab_pool_3; *obj_size = SLAB_SIZE_3; break;
    }
}

void slab_initialize(void) {
    slab_classes[0].obj_size = SLAB_SIZE_0;
    slab_classes[1].obj_size = SLAB_SIZE_1;
    slab_classes[2].obj_size = SLAB_SIZE_2;
    slab_classes[3].obj_size = SLAB_SIZE_3;

    for (int class_index = 0; class_index < SLAB_CLASS_COUNT; ++class_index) {
        uint8_t *base;
        uint32_t obj_size;
        slab_get_pool_base(class_index, &base, &obj_size);

        uint8_t *cur = base;
        for (uint32_t i = 0; i < SLAB_OBJECTS_PER - 1; ++i) {
            uint8_t *next = cur + obj_size;
            *(void **)cur = next;
            cur = next;
        }
        *(void **)cur = 0; /* letzter Eintrag: Listenende */

        slab_class_t *cls = &slab_classes[class_index];
        cls->free_head = base;
        cls->free_count = SLAB_OBJECTS_PER;
        cls->alloc_count = 0;
        cls->total = 0;
    }

    slab_ready = 1;
}

static int slab_class_for_size(uint32_t size) {
    if (size <= SLAB_SIZE_0) return 0;
    if (size <= SLAB_SIZE_1) return 1;
    if (size <= SLAB_SIZE_2) return 2;
    if (size <= SLAB_SIZE_3) return 3;
    return -1;
}

void *slab_alloc(uint32_t size) {
    int class_index = slab_class_for_size(size);
    if (class_index < 0) {
        return 0;
    }
    slab_class_t *cls = &slab_classes[class_index];
    void *obj = cls->free_head;
    if (!obj) {
        return 0; /* Klasse erschoepft */
    }
    cls->free_head = *(void **)obj;
    --cls->free_count;
    ++cls->alloc_count;
    ++cls->total;
    return obj;
}

void slab_free(void *ptr, uint32_t size) {
    int class_index = slab_class_for_size(size);
    if (class_index < 0) {
        class_index = SLAB_CLASS_COUNT - 1; /* wie ASM: faellt auf Klasse 3 zurueck */
    }
    slab_class_t *cls = &slab_classes[class_index];
    *(void **)ptr = cls->free_head;
    cls->free_head = ptr;
    ++cls->free_count;
    --cls->alloc_count;
}

int slab_self_test(void) {
    if (slab_ready != 1) return -1;

    void *obj0 = slab_alloc(16);
    if (!obj0) return -1;
    if (slab_classes[0].free_count != 15) return -1;

    void *obj2 = slab_alloc(64);
    if (!obj2) return -1;

    slab_free(obj0, 16);
    if (slab_classes[0].free_count != 16) return -1;

    slab_free(obj2, 64);
    if (slab_classes[2].free_count != 16) return -1;

    void *obj3 = slab_alloc(128);
    if (!obj3) return -1;
    slab_free(obj3, 128);

    return 0;
}

#include "kalloc.h"
#include "pmm.h"
#include "sync.h"
#include <stddef.h>

typedef struct kalloc_block {
    uint32_t size;    /* Groesse des Nutzbereichs NACH diesem Header, in Byte */
    uint32_t in_use;
    struct kalloc_block *prev; /* Nachbar mit niedrigerer Adresse (oder NULL) */
    struct kalloc_block *next; /* Nachbar mit hoeherer Adresse (oder NULL) */
} kalloc_block_t;

#define KALLOC_HEADER_SIZE ((uint32_t)sizeof(kalloc_block_t))
/* Nur splitten, wenn der Rest selbst wieder mindestens Header + diese
   Mindestnutzgroesse ergibt - sonst bleibt der ganze Block beim
   Anfordernden (etwas interne Fragmentierung statt winziger, praktisch
   nutzloser Freilisten-Splitter). */
#define KALLOC_MIN_SPLIT_PAYLOAD 16u

static kalloc_block_t *kalloc_head = NULL; /* nach Adresse sortiert */
static volatile uint32_t kalloc_lock = SYNC_SPIN_UNLOCKED;
static uint32_t kalloc_bytes_in_use_count = 0;
static uint32_t kalloc_allocation_count_value = 0;

void kalloc_initialize(void) {
    kalloc_head = NULL;
    sync_spin_init(&kalloc_lock);
    kalloc_bytes_in_use_count = 0;
    kalloc_allocation_count_value = 0;
}

static uint32_t kalloc_round_up(uint32_t size, uint32_t alignment) {
    uint32_t rounded = (size + alignment - 1u) & ~(alignment - 1u);
    return (rounded < size) ? 0u : rounded; /* 0 signalisiert Ueberlauf */
}

static uint8_t *kalloc_block_end(kalloc_block_t *b) {
    return (uint8_t *)b + KALLOC_HEADER_SIZE + b->size;
}

/* Fuegt einen neuen, freien Block (von einer frischen PMM-Allokation) an
   der richtigen Stelle in die nach Adresse sortierte Liste ein und
   verschmilzt ihn opportunistisch mit physisch direkt angrenzenden
   Nachbarn, falls vorhanden (pmm_alloc_contiguous() kann, muss aber
   nicht, an eine vorherige Allokation anschliessen). */
static void kalloc_insert_free_block(kalloc_block_t *block) {
    if (!kalloc_head || block < kalloc_head) {
        block->next = kalloc_head;
        block->prev = NULL;
        if (kalloc_head) kalloc_head->prev = block;
        kalloc_head = block;
    } else {
        kalloc_block_t *cur = kalloc_head;
        while (cur->next && cur->next < block) cur = cur->next;
        block->next = cur->next;
        block->prev = cur;
        if (cur->next) cur->next->prev = block;
        cur->next = block;
    }

    if (block->prev && !block->prev->in_use && kalloc_block_end(block->prev) == (uint8_t *)block) {
        kalloc_block_t *prev = block->prev;
        prev->size += KALLOC_HEADER_SIZE + block->size;
        prev->next = block->next;
        if (block->next) block->next->prev = prev;
        block = prev;
    }
    if (block->next && !block->next->in_use && kalloc_block_end(block) == (uint8_t *)block->next) {
        kalloc_block_t *next = block->next;
        block->size += KALLOC_HEADER_SIZE + next->size;
        block->next = next->next;
        if (next->next) next->next->prev = block;
    }
}

/* Holt per pmm_alloc_contiguous() genug Seiten fuer mindestens
   need_payload Byte Nutzbereich und reiht sie als einen neuen freien
   Block ein. 0 = Erfolg, -1 = PMM erschoepft. */
static int kalloc_grow(uint32_t need_payload) {
    uint32_t need_total = need_payload + KALLOC_HEADER_SIZE;
    uint32_t pages = (need_total + PMM_PAGE_SIZE - 1u) / PMM_PAGE_SIZE;

    uint32_t first_page;
    if (pmm_alloc_contiguous(pages, &first_page) != 0) return -1;

    kalloc_block_t *block = (kalloc_block_t *)(uintptr_t)first_page;
    block->size = pages * PMM_PAGE_SIZE - KALLOC_HEADER_SIZE;
    block->in_use = 0;
    block->prev = NULL;
    block->next = NULL;

    kalloc_insert_free_block(block);
    return 0;
}

static kalloc_block_t *kalloc_find_free(uint32_t payload_size) {
    for (kalloc_block_t *b = kalloc_head; b; b = b->next) {
        if (!b->in_use && b->size >= payload_size) return b;
    }
    return NULL;
}

void *kalloc(uint32_t size) {
    if (size == 0) return NULL;
    uint32_t aligned_size = kalloc_round_up(size, KALLOC_ALIGNMENT);
    if (aligned_size == 0) return NULL; /* Ueberlauf */

    uint32_t flags = sync_spin_lock_irqsave(&kalloc_lock);

    kalloc_block_t *block = kalloc_find_free(aligned_size);
    if (!block) {
        if (kalloc_grow(aligned_size) != 0) {
            sync_spin_unlock_irqrestore(&kalloc_lock, flags);
            return NULL;
        }
        block = kalloc_find_free(aligned_size);
        if (!block) { /* sollte nach kalloc_grow() nie passieren */
            sync_spin_unlock_irqrestore(&kalloc_lock, flags);
            return NULL;
        }
    }

    if (block->size >= aligned_size + KALLOC_HEADER_SIZE + KALLOC_MIN_SPLIT_PAYLOAD) {
        kalloc_block_t *remainder = (kalloc_block_t *)(uint8_t *)((uint8_t *)block +
                                      KALLOC_HEADER_SIZE + aligned_size);
        remainder->size = block->size - aligned_size - KALLOC_HEADER_SIZE;
        remainder->in_use = 0;
        remainder->prev = block;
        remainder->next = block->next;
        if (block->next) block->next->prev = remainder;
        block->next = remainder;
        block->size = aligned_size;
    }

    block->in_use = 1;
    kalloc_bytes_in_use_count += block->size;
    ++kalloc_allocation_count_value;

    void *result = (void *)((uint8_t *)block + KALLOC_HEADER_SIZE);
    sync_spin_unlock_irqrestore(&kalloc_lock, flags);
    return result;
}

void kfree(void *ptr) {
    if (!ptr) return;

    uint32_t flags = sync_spin_lock_irqsave(&kalloc_lock);

    kalloc_block_t *block = (kalloc_block_t *)((uint8_t *)ptr - KALLOC_HEADER_SIZE);
    block->in_use = 0;
    kalloc_bytes_in_use_count -= block->size;
    --kalloc_allocation_count_value;

    if (block->next && !block->next->in_use && kalloc_block_end(block) == (uint8_t *)block->next) {
        kalloc_block_t *next = block->next;
        block->size += KALLOC_HEADER_SIZE + next->size;
        block->next = next->next;
        if (next->next) next->next->prev = block;
    }
    if (block->prev && !block->prev->in_use && kalloc_block_end(block->prev) == (uint8_t *)block) {
        kalloc_block_t *prev = block->prev;
        prev->size += KALLOC_HEADER_SIZE + block->size;
        prev->next = block->next;
        if (block->next) block->next->prev = prev;
    }

    sync_spin_unlock_irqrestore(&kalloc_lock, flags);
}

void *kcalloc(uint32_t count, uint32_t size) {
    if (count == 0 || size == 0) return NULL;
    if (count > 0xFFFFFFFFu / size) return NULL; /* Ueberlauf count*size */
    uint32_t total = count * size;

    void *ptr = kalloc(total);
    if (!ptr) return NULL;
    uint8_t *bytes = (uint8_t *)ptr;
    for (uint32_t i = 0; i < total; ++i) bytes[i] = 0;
    return ptr;
}

void *krealloc(void *ptr, uint32_t new_size) {
    if (!ptr) return kalloc(new_size);
    if (new_size == 0) { kfree(ptr); return NULL; }

    kalloc_block_t *old_block = (kalloc_block_t *)((uint8_t *)ptr - KALLOC_HEADER_SIZE);
    uint32_t old_size = old_block->size;

    void *new_ptr = kalloc(new_size);
    if (!new_ptr) return NULL; /* alter Block bleibt unveraendert gueltig */

    uint32_t copy_len = (old_size < new_size) ? old_size : new_size;
    uint8_t *src = (uint8_t *)ptr;
    uint8_t *dst = (uint8_t *)new_ptr;
    for (uint32_t i = 0; i < copy_len; ++i) dst[i] = src[i];

    kfree(ptr);
    return new_ptr;
}

uint32_t kalloc_bytes_in_use(void) { return kalloc_bytes_in_use_count; }
uint32_t kalloc_allocation_count(void) { return kalloc_allocation_count_value; }

int kalloc_self_test(void) {
    if (kalloc_allocation_count() != 0) return -1;
    if (kalloc_bytes_in_use() != 0) return -1;

    /* Grundzyklus: Allokation, Beschreibung, Inhalt bleibt bei weiteren
       Allokationen unberuehrt. */
    uint32_t *a = (uint32_t *)kalloc(64);
    if (!a) return -1;
    if (((uintptr_t)a & (KALLOC_ALIGNMENT - 1u)) != 0) return -1;
    a[0] = 0x4E4F5641u;
    a[15] = 0x4B414C4Cu;

    uint32_t *b = (uint32_t *)kalloc(128);
    if (!b) return -1;
    if (((uintptr_t)b & (KALLOC_ALIGNMENT - 1u)) != 0) return -1;
    if (a[0] != 0x4E4F5641u || a[15] != 0x4B414C4Cu) return -1;

    if (kalloc_allocation_count() != 2) return -1;

    /* Freigabe + Wiederverwendung: nach Freigabe von a sollte eine neue,
       gleich grosse Anfrage wieder exakt denselben Block treffen
       (First-Fit auf den soeben freigewordenen Platz, keine neue
       PMM-Seite noetig). */
    kfree(a);
    if (kalloc_allocation_count() != 1) return -1;

    uint32_t *c = (uint32_t *)kalloc(64);
    if (!c) return -1;
    if (c != a) return -1; /* exakt derselbe wiederverwendete Block */

    /* Coalescing: zwei benachbarte, freigegebene Bloecke verschmelzen zu
       einem groesseren - beweisbar, indem danach eine Anfrage erfolgreich
       ist, die GROESSER als jeder einzelne der beiden urspruenglichen
       Bloecke ist, aber in deren SUMME passt, ohne dass kalloc_grow()
       dafuer eine neue PMM-Seite anfordern muesste (nicht direkt
       beobachtbar von aussen, aber die anschliessende Allokation an
       GENAU der Adresse von c beweist es indirekt - siehe unten). */
    kfree(c);
    kfree(b);
    if (kalloc_allocation_count() != 0) return -1;
    if (kalloc_bytes_in_use() != 0) return -1;

    uint32_t *merged = (uint32_t *)kalloc(64u + 128u + 16u);
    if (!merged) return -1;
    if ((void *)merged != (void *)c) return -1; /* beweist: c und b wurden zu einem Block verschmolzen */
    kfree(merged);

    /* Groessere Allokation als eine einzelne PMM-Seite - beweist, dass
       kalloc_grow() mehrere zusammenhaengende Seiten anfordern kann. */
    uint8_t *large = (uint8_t *)kalloc(PMM_PAGE_SIZE + 512u);
    if (!large) return -1;
    for (uint32_t i = 0; i < PMM_PAGE_SIZE + 512u; i += 64u) large[i] = (uint8_t)(i & 0xFFu);
    for (uint32_t i = 0; i < PMM_PAGE_SIZE + 512u; i += 64u) {
        if (large[i] != (uint8_t)(i & 0xFFu)) return -1;
    }
    kfree(large);

    /* kcalloc: Nullinitialisierung. */
    uint8_t *zeroed = (uint8_t *)kcalloc(32u, 4u);
    if (!zeroed) return -1;
    for (uint32_t i = 0; i < 128u; ++i) {
        if (zeroed[i] != 0) return -1;
    }
    zeroed[0] = 0xAAu;

    /* krealloc: Inhalt bleibt bis min(alt,neu) erhalten, wachsen UND
       schrumpfen. */
    uint8_t *grown = (uint8_t *)krealloc(zeroed, 256u);
    if (!grown) return -1;
    if (grown[0] != 0xAAu) return -1;
    uint8_t *shrunk = (uint8_t *)krealloc(grown, 8u);
    if (!shrunk) return -1;
    if (shrunk[0] != 0xAAu) return -1;
    kfree(shrunk);

    /* Randfaelle. */
    if (kalloc(0) != NULL) return -1;
    if (kcalloc(0xFFFFFFFFu, 2u) != NULL) return -1; /* Ueberlauf count*size */
    kfree(NULL); /* darf nicht abstuerzen */
    if (krealloc(NULL, 0) != NULL) return -1;

    return 0;
}

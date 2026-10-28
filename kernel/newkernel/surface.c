/* NovaOS C-Kernel - Nova Surface Manager (siehe surface.h fuer Scope). */
#include "surface.h"

typedef struct {
    uint32_t handle; /* 0 = frei */
    uint32_t state;
    uint32_t owner_pid;
    uint32_t x, y, w, h;
    uint32_t damage_x, damage_y, damage_w, damage_h;
    int damage_valid;
    uint32_t buffer_addr;
    uint32_t commit_seq;
    uint32_t consumed_seq;
} surface_slot_t;

static void surface_damage_union(surface_slot_t *slot, uint32_t x, uint32_t y,
                                  uint32_t w, uint32_t h) {
    if (!slot->damage_valid) {
        slot->damage_x = x;
        slot->damage_y = y;
        slot->damage_w = w;
        slot->damage_h = h;
        slot->damage_valid = 1;
        return;
    }

    uint32_t old_x_end = slot->damage_x + slot->damage_w;
    uint32_t old_y_end = slot->damage_y + slot->damage_h;
    uint32_t new_x_end = x + w;
    uint32_t new_y_end = y + h;

    uint32_t union_x = (x < slot->damage_x) ? x : slot->damage_x;
    uint32_t union_y = (y < slot->damage_y) ? y : slot->damage_y;
    uint32_t union_x_end = (new_x_end > old_x_end) ? new_x_end : old_x_end;
    uint32_t union_y_end = (new_y_end > old_y_end) ? new_y_end : old_y_end;

    slot->damage_x = union_x;
    slot->damage_y = union_y;
    slot->damage_w = union_x_end - union_x;
    slot->damage_h = union_y_end - union_y;
}

static surface_slot_t surface_slots[SURFACE_CAPACITY];
static uint32_t surface_next_handle = 1;

static int surface_find_slot(uint32_t handle) {
    if (handle == 0) return -1;
    for (uint32_t i = 0; i < SURFACE_CAPACITY; ++i) {
        if (surface_slots[i].handle == handle) return (int)i;
    }
    return -1;
}

void surface_manager_initialize(void) {
    for (uint32_t i = 0; i < SURFACE_CAPACITY; ++i) {
        surface_slots[i] = (surface_slot_t){0};
    }
    surface_next_handle = 1;
}

int surface_create(uint32_t owner_pid, uint32_t x, uint32_t y, uint32_t w,
                    uint32_t h, uint32_t *out_handle) {
    for (uint32_t i = 0; i < SURFACE_CAPACITY; ++i) {
        if (surface_slots[i].handle == 0) {
            surface_slots[i].handle = surface_next_handle++;
            surface_slots[i].state = SURFACE_STATE_CREATED;
            surface_slots[i].owner_pid = owner_pid;
            surface_slots[i].x = x;
            surface_slots[i].y = y;
            surface_slots[i].w = w;
            surface_slots[i].h = h;
            surface_slots[i].damage_valid = 0;
            surface_slots[i].buffer_addr = 0;
            surface_slots[i].commit_seq = 0;
            surface_slots[i].consumed_seq = 0;
            surface_damage_union(&surface_slots[i], x, y, w, h);
            *out_handle = surface_slots[i].handle;
            return 0;
        }
    }
    return -1;
}

int surface_configure(uint32_t handle, uint32_t x, uint32_t y, uint32_t w,
                      uint32_t h) {
    int i = surface_find_slot(handle);
    if (i < 0) return -1;
    uint32_t state = surface_slots[i].state;
    if (state != SURFACE_STATE_CREATED && state != SURFACE_STATE_CONFIGURED) return -1;

    surface_damage_union(&surface_slots[i], surface_slots[i].x, surface_slots[i].y,
                         surface_slots[i].w, surface_slots[i].h);
    surface_slots[i].x = x;
    surface_slots[i].y = y;
    surface_slots[i].w = w;
    surface_slots[i].h = h;
    surface_damage_union(&surface_slots[i], x, y, w, h);
    surface_slots[i].state = SURFACE_STATE_CONFIGURED;
    return 0;
}

int surface_attach_buffer(uint32_t handle, uint32_t buffer_addr) {
    if (buffer_addr == 0) return -1;
    int i = surface_find_slot(handle);
    if (i < 0) return -1;
    uint32_t state = surface_slots[i].state;
    if (state != SURFACE_STATE_CONFIGURED && state != SURFACE_STATE_ACTIVE) return -1;

    surface_slots[i].buffer_addr = buffer_addr;
    surface_slots[i].state = SURFACE_STATE_ATTACHED;
    return 0;
}

uint32_t surface_buffer_addr(uint32_t handle) {
    int i = surface_find_slot(handle);
    if (i < 0) return 0;
    return surface_slots[i].buffer_addr;
}

int surface_commit(uint32_t handle) {
    int i = surface_find_slot(handle);
    if (i < 0) return -1;
    uint32_t state = surface_slots[i].state;
    if (state != SURFACE_STATE_ATTACHED && state != SURFACE_STATE_HIDDEN) return -1;

    /* Konservativ: volles aktuelles Rechteck als beschaedigt markieren
       (siehe surface.h - echte Teilbereichs-Verfolgung braucht Buffer-
       Attach, folgt in einem spaeteren Modul). */
    surface_damage_union(&surface_slots[i], surface_slots[i].x, surface_slots[i].y,
                         surface_slots[i].w, surface_slots[i].h);
    surface_slots[i].state = SURFACE_STATE_ACTIVE;
    ++surface_slots[i].commit_seq;
    return 0;
}

int surface_hide(uint32_t handle) {
    int i = surface_find_slot(handle);
    if (i < 0) return -1;
    if (surface_slots[i].state != SURFACE_STATE_ACTIVE) return -1;

    surface_slots[i].state = SURFACE_STATE_HIDDEN;
    return 0;
}

int surface_destroy(uint32_t handle) {
    int i = surface_find_slot(handle);
    if (i < 0) return -1;

    surface_slots[i].state = SURFACE_STATE_DESTROYING;
    surface_slots[i] = (surface_slot_t){0};
    return 0;
}

int surface_resize(uint32_t handle, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    int i = surface_find_slot(handle);
    if (i < 0) return -1;
    uint32_t state = surface_slots[i].state;
    if (state == SURFACE_STATE_FREE || state == SURFACE_STATE_DESTROYING) return -1;

    surface_damage_union(&surface_slots[i], surface_slots[i].x, surface_slots[i].y,
                         surface_slots[i].w, surface_slots[i].h);
    surface_slots[i].x = x;
    surface_slots[i].y = y;
    surface_slots[i].w = w;
    surface_slots[i].h = h;
    surface_damage_union(&surface_slots[i], x, y, w, h);
    return 0;
}

uint32_t surface_state(uint32_t handle) {
    int i = surface_find_slot(handle);
    if (i < 0) return SURFACE_STATE_FREE;
    return surface_slots[i].state;
}

int surface_rect(uint32_t handle, uint32_t *out_x, uint32_t *out_y,
                  uint32_t *out_w, uint32_t *out_h) {
    int i = surface_find_slot(handle);
    if (i < 0) return -1;
    *out_x = surface_slots[i].x;
    *out_y = surface_slots[i].y;
    *out_w = surface_slots[i].w;
    *out_h = surface_slots[i].h;
    return 0;
}

int surface_damage_rect(uint32_t handle, uint32_t *out_x, uint32_t *out_y,
                         uint32_t *out_w, uint32_t *out_h) {
    int i = surface_find_slot(handle);
    if (i < 0) return -1;
    if (!surface_slots[i].damage_valid) return -1;
    *out_x = surface_slots[i].damage_x;
    *out_y = surface_slots[i].damage_y;
    *out_w = surface_slots[i].damage_w;
    *out_h = surface_slots[i].damage_h;
    return 0;
}

int surface_clear_damage(uint32_t handle) {
    int i = surface_find_slot(handle);
    if (i < 0) return -1;
    surface_slots[i].damage_valid = 0;
    return 0;
}

uint32_t surface_commit_seq(uint32_t handle) {
    int i = surface_find_slot(handle);
    if (i < 0) return 0;
    return surface_slots[i].commit_seq;
}

int surface_consume_if_dirty(uint32_t handle) {
    int i = surface_find_slot(handle);
    if (i < 0) return -1;
    if (surface_slots[i].consumed_seq == surface_slots[i].commit_seq) return -1;

    surface_slots[i].consumed_seq = surface_slots[i].commit_seq;
    return 0;
}

uint32_t surface_capacity(void) {
    return SURFACE_CAPACITY;
}

uint32_t surface_handle_at_slot(uint32_t slot_index) {
    if (slot_index >= SURFACE_CAPACITY) return 0;
    return surface_slots[slot_index].handle;
}

uint32_t surface_owner_pid(uint32_t handle) {
    int i = surface_find_slot(handle);
    if (i < 0) return 0;
    return surface_slots[i].owner_pid;
}

uint32_t surface_count_in_use(void) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < SURFACE_CAPACITY; ++i) {
        if (surface_slots[i].handle != 0) ++count;
    }
    return count;
}

int surface_self_test(void) {
    surface_manager_initialize();

    uint32_t h1, h2;
    if (surface_create(1u, 10u, 20u, 100u, 50u, &h1) != 0) return -1;
    if (surface_state(h1) != SURFACE_STATE_CREATED) return -1;

    /* Ungueltiger Uebergang: Commit vor Attach muss fehlschlagen. */
    if (surface_commit(h1) == 0) return -1;

    if (surface_configure(h1, 10u, 20u, 100u, 50u) != 0) return -1;
    if (surface_state(h1) != SURFACE_STATE_CONFIGURED) return -1;

    /* Erneutes Configure (noch vor dem ersten Attach) auf eine versetzte
       Flaeche muss die Vereinigung aus altem und neuem Rechteck als Damage
       markieren statt nur das neue Rechteck. */
    if (surface_configure(h1, 60u, 20u, 100u, 50u) != 0) return -1;
    uint32_t union_x, union_y, union_w, union_h;
    if (surface_damage_rect(h1, &union_x, &union_y, &union_w, &union_h) != 0) return -1;
    if (union_x != 10u || union_y != 20u || union_w != 150u || union_h != 50u) return -1;
    /* Zurueck auf die ursprueungliche Geometrie fuer den Rest des Tests. */
    if (surface_configure(h1, 10u, 20u, 100u, 50u) != 0) return -1;
    surface_clear_damage(h1);

    /* Attach ohne Buffer-Adresse (0) muss fehlschlagen. */
    if (surface_attach_buffer(h1, 0u) == 0) return -1;

    static uint32_t test_pixels[4] = {0xFF0000u, 0x00FF00u, 0x0000FFu, 0xFFFFFFu};
    uint32_t buffer_addr = (uint32_t)(uintptr_t)test_pixels;
    if (surface_attach_buffer(h1, buffer_addr) != 0) return -1;
    if (surface_state(h1) != SURFACE_STATE_ATTACHED) return -1;
    if (surface_buffer_addr(h1) != buffer_addr) return -1;

    /* Vor dem ersten Commit darf noch kein Commit-Sequenzwechsel vorliegen. */
    if (surface_commit_seq(h1) != 0u) return -1;
    if (surface_consume_if_dirty(h1) == 0) return -1;

    if (surface_commit(h1) != 0) return -1;
    if (surface_state(h1) != SURFACE_STATE_ACTIVE) return -1;
    if (surface_commit_seq(h1) != 1u) return -1;

    /* Erster consume_if_dirty nach einem Commit muss Erfolg melden, der
       zweite (ohne neuen Commit dazwischen) muss fehlschlagen. */
    if (surface_consume_if_dirty(h1) != 0) return -1;
    if (surface_consume_if_dirty(h1) == 0) return -1;

    /* Erneuter Commit (ueber den erlaubten ACTIVE->HIDDEN->ACTIVE-Pfad, da
       ein direkter Re-Commit aus ACTIVE kein gueltiger Zustandsuebergang
       ist) erhoeht die Sequenz weiter und macht die Surface wieder als
       'dirty' sichtbar. */
    if (surface_hide(h1) != 0) return -1;
    if (surface_commit(h1) != 0) return -1;
    if (surface_commit_seq(h1) != 2u) return -1;
    if (surface_consume_if_dirty(h1) != 0) return -1;

    uint32_t x, y, w, h;
    if (surface_rect(h1, &x, &y, &w, &h) != 0) return -1;
    if (x != 10u || y != 20u || w != 100u || h != 50u) return -1;
    if (surface_owner_pid(h1) != 1u) return -1;

    /* Damage-Rechteck muss nach Create+Configure+Commit das volle
       Surface-Rechteck abdecken, und nach clear() wieder leer sein. */
    uint32_t dx, dy, dw, dh;
    if (surface_damage_rect(h1, &dx, &dy, &dw, &dh) != 0) return -1;
    if (dx != 10u || dy != 20u || dw != 100u || dh != 50u) return -1;
    if (surface_clear_damage(h1) != 0) return -1;
    if (surface_damage_rect(h1, &dx, &dy, &dw, &dh) == 0) return -1;

    if (surface_hide(h1) != 0) return -1;
    if (surface_state(h1) != SURFACE_STATE_HIDDEN) return -1;

    /* Re-Commit aus HIDDEN muss direkt wieder ACTIVE erlauben (kein erneutes
       Attach notwendig - entspricht einem Client, der nur sichtbar/unsichtbar
       geschaltet wird). */
    if (surface_commit(h1) != 0) return -1;
    if (surface_state(h1) != SURFACE_STATE_ACTIVE) return -1;

    /* surface_resize() muss (im Gegensatz zu surface_configure()) auch auf
       einer ACTIVE Surface funktionieren, ohne deren Zustand zu aendern -
       Grundlage fuer Runtime-Resize (window.c Maximize/Restore). */
    if (surface_resize(h1, 0u, 0u, 200u, 100u) != 0) return -1;
    if (surface_state(h1) != SURFACE_STATE_ACTIVE) return -1;
    if (surface_rect(h1, &x, &y, &w, &h) != 0) return -1;
    if (x != 0u || y != 0u || w != 200u || h != 100u) return -1;

    /* Zweite, unabhaengige Surface parallel offen halten. */
    if (surface_create(2u, 0u, 0u, 10u, 10u, &h2) != 0) return -1;
    if (surface_count_in_use() != 2u) return -1;

    /* Iteration ueber alle Slots (fuer den Compositor) muss genau h1 und
       h2 liefern, sonst nur freie Slots (Handle 0). */
    if (surface_capacity() != SURFACE_CAPACITY) return -1;
    int found_h1 = 0, found_h2 = 0;
    for (uint32_t slot = 0; slot < surface_capacity(); ++slot) {
        uint32_t slot_handle = surface_handle_at_slot(slot);
        if (slot_handle == h1) found_h1 = 1;
        if (slot_handle == h2) found_h2 = 1;
    }
    if (!found_h1 || !found_h2) return -1;
    if (surface_handle_at_slot(surface_capacity()) != 0u) return -1;

    if (surface_destroy(h1) != 0) return -1;
    if (surface_state(h1) != SURFACE_STATE_FREE) return -1;
    if (surface_count_in_use() != 1u) return -1;

    /* Unbekanntes Handle muss ueberall sauber fehlschlagen. */
    if (surface_configure(999u, 0u, 0u, 1u, 1u) == 0) return -1;
    if (surface_destroy(999u) == 0) return -1;

    surface_destroy(h2);
    return 0;
}

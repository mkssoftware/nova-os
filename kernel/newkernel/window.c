/* NovaOS C-Kernel - Nova Window Manager (siehe window.h fuer Scope). */
#include "window.h"
#include "surface.h"
#include "display.h"

typedef struct {
    uint32_t handle; /* == Surface-Handle; 0 = Slot frei */
    uint32_t parent_handle;
    uint32_t z_order;
    int decorated;
    uint32_t mode;
    uint32_t restore_x, restore_y, restore_w, restore_h;
} window_record_t;

static window_record_t window_table[WINDOW_CAPACITY];
static uint32_t window_next_z = 1;
static uint32_t window_focused = 0;

static int window_find_slot(uint32_t handle) {
    if (handle == 0) return -1;
    for (uint32_t i = 0; i < WINDOW_CAPACITY; ++i) {
        if (window_table[i].handle == handle) return (int)i;
    }
    return -1;
}

void window_manager_initialize(void) {
    for (uint32_t i = 0; i < WINDOW_CAPACITY; ++i) window_table[i] = (window_record_t){0};
    window_next_z = 1;
    window_focused = 0;
}

int window_create(uint32_t owner_pid, uint32_t x, uint32_t y, uint32_t w,
                   uint32_t h, int decorated, uint32_t *out_handle) {
    int slot = -1;
    for (uint32_t i = 0; i < WINDOW_CAPACITY; ++i) {
        if (window_table[i].handle == 0) { slot = (int)i; break; }
    }
    if (slot < 0) return -1;

    uint32_t handle;
    if (surface_create(owner_pid, x, y, w, h, &handle) != 0) return -1;
    if (surface_configure(handle, x, y, w, h) != 0) {
        surface_destroy(handle);
        return -1;
    }

    window_table[slot].handle = handle;
    window_table[slot].parent_handle = 0;
    window_table[slot].z_order = window_next_z++;
    window_table[slot].decorated = decorated ? 1 : 0;
    window_table[slot].mode = WINDOW_MODE_NORMAL;
    window_table[slot].restore_x = x;
    window_table[slot].restore_y = y;
    window_table[slot].restore_w = w;
    window_table[slot].restore_h = h;

    *out_handle = handle;
    return 0;
}

uint32_t window_mode(uint32_t handle) {
    int slot = window_find_slot(handle);
    if (slot < 0) return WINDOW_MODE_NORMAL;
    return window_table[slot].mode;
}

int window_minimize(uint32_t handle) {
    int slot = window_find_slot(handle);
    if (slot < 0) return -1;
    if (window_table[slot].mode == WINDOW_MODE_MINIMIZED) return 0;

    uint32_t x, y, w, h;
    if (surface_rect(handle, &x, &y, &w, &h) != 0) return -1;
    if (surface_hide(handle) != 0) return -1;

    window_table[slot].restore_x = x;
    window_table[slot].restore_y = y;
    window_table[slot].restore_w = w;
    window_table[slot].restore_h = h;
    window_table[slot].mode = WINDOW_MODE_MINIMIZED;
    return 0;
}

static int window_maximize_like(uint32_t handle, uint32_t target_mode) {
    int slot = window_find_slot(handle);
    if (slot < 0) return -1;
    if (window_table[slot].mode == WINDOW_MODE_MINIMIZED) return -1; /* zuerst restore() */
    if (window_table[slot].mode == target_mode) return 0;

    uint32_t display_w = display_width();
    uint32_t display_h = display_height();
    if (display_w == 0 || display_h == 0) return -1;

    if (window_table[slot].mode == WINDOW_MODE_NORMAL) {
        uint32_t x, y, w, h;
        if (surface_rect(handle, &x, &y, &w, &h) != 0) return -1;
        window_table[slot].restore_x = x;
        window_table[slot].restore_y = y;
        window_table[slot].restore_w = w;
        window_table[slot].restore_h = h;
    }

    if (surface_resize(handle, 0u, 0u, display_w, display_h) != 0) return -1;
    window_table[slot].mode = target_mode;
    return 0;
}

int window_maximize(uint32_t handle) {
    return window_maximize_like(handle, WINDOW_MODE_MAXIMIZED);
}

int window_set_fullscreen(uint32_t handle) {
    return window_maximize_like(handle, WINDOW_MODE_FULLSCREEN);
}

int window_restore(uint32_t handle) {
    int slot = window_find_slot(handle);
    if (slot < 0) return -1;

    if (window_table[slot].mode == WINDOW_MODE_NORMAL) return 0;

    if (window_table[slot].mode == WINDOW_MODE_MINIMIZED) {
        if (surface_commit(handle) != 0) return -1;
        window_table[slot].mode = WINDOW_MODE_NORMAL;
        return 0;
    }

    /* MAXIMIZED oder FULLSCREEN -> urspruengliches Rechteck wiederherstellen. */
    if (surface_resize(handle, window_table[slot].restore_x, window_table[slot].restore_y,
                       window_table[slot].restore_w, window_table[slot].restore_h) != 0) {
        return -1;
    }
    window_table[slot].mode = WINDOW_MODE_NORMAL;
    return 0;
}

int window_destroy(uint32_t handle) {
    int slot = window_find_slot(handle);
    if (slot < 0) return -1;

    /* Verwaiste Kinder behalten ihre Parent-Referenz nicht - sie werden
       zu eigenstaendigen Top-Level-Fenstern (kein rekursives Zerstoeren). */
    for (uint32_t i = 0; i < WINDOW_CAPACITY; ++i) {
        if (window_table[i].parent_handle == handle) window_table[i].parent_handle = 0;
    }
    if (window_focused == handle) window_focused = 0;

    window_table[slot] = (window_record_t){0};
    surface_destroy(handle);
    return 0;
}

int window_is_visible(uint32_t handle) {
    return surface_state(handle) == SURFACE_STATE_ACTIVE;
}

int window_is_decorated(uint32_t handle) {
    int slot = window_find_slot(handle);
    if (slot < 0) return 0;
    return window_table[slot].decorated;
}

int window_set_parent(uint32_t handle, uint32_t parent_handle) {
    int slot = window_find_slot(handle);
    if (slot < 0) return -1;
    if (parent_handle == handle) return -1;
    if (parent_handle != 0 && window_find_slot(parent_handle) < 0) return -1;

    window_table[slot].parent_handle = parent_handle;
    return 0;
}

uint32_t window_parent(uint32_t handle) {
    int slot = window_find_slot(handle);
    if (slot < 0) return 0;
    return window_table[slot].parent_handle;
}

int window_raise(uint32_t handle) {
    int slot = window_find_slot(handle);
    if (slot < 0) return -1;
    window_table[slot].z_order = window_next_z++;
    return 0;
}

int window_lower(uint32_t handle) {
    int slot = window_find_slot(handle);
    if (slot < 0) return -1;

    uint32_t min_z = window_table[slot].z_order;
    for (uint32_t i = 0; i < WINDOW_CAPACITY; ++i) {
        if (window_table[i].handle != 0 && window_table[i].z_order < min_z) {
            min_z = window_table[i].z_order;
        }
    }
    window_table[slot].z_order = min_z - 1u;
    return 0;
}

uint32_t window_z_order(uint32_t handle) {
    int slot = window_find_slot(handle);
    if (slot < 0) return 0;
    return window_table[slot].z_order;
}

int window_focus(uint32_t handle) {
    if (handle == 0) {
        window_focused = 0;
        return 0;
    }
    if (window_find_slot(handle) < 0) return -1;
    window_focused = handle;
    return 0;
}

uint32_t window_focused_handle(void) {
    return window_focused;
}

uint32_t window_capacity(void) {
    return WINDOW_CAPACITY;
}

uint32_t window_handle_at_slot(uint32_t slot_index) {
    if (slot_index >= WINDOW_CAPACITY) return 0;
    return window_table[slot_index].handle;
}

uint32_t window_count_in_use(void) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < WINDOW_CAPACITY; ++i) {
        if (window_table[i].handle != 0) ++count;
    }
    return count;
}

uint32_t window_hit_test(uint32_t x, uint32_t y) {
    uint32_t best_handle = 0;
    uint32_t best_z = 0;
    for (uint32_t i = 0; i < WINDOW_CAPACITY; ++i) {
        uint32_t handle = window_table[i].handle;
        if (handle == 0) continue;
        if (!window_is_visible(handle)) continue;

        uint32_t wx, wy, ww, wh;
        if (surface_rect(handle, &wx, &wy, &ww, &wh) != 0) continue;
        if (x < wx || x >= wx + ww || y < wy || y >= wy + wh) continue;

        if (best_handle == 0 || window_table[i].z_order > best_z) {
            best_handle = handle;
            best_z = window_table[i].z_order;
        }
    }
    return best_handle;
}

uint32_t window_titlebar_hit_test(uint32_t x, uint32_t y) {
    uint32_t best_handle = 0;
    uint32_t best_z = 0;
    for (uint32_t i = 0; i < WINDOW_CAPACITY; ++i) {
        uint32_t handle = window_table[i].handle;
        if (handle == 0) continue;
        if (!window_table[i].decorated) continue;
        if (!window_is_visible(handle)) continue;

        uint32_t wx, wy, ww, wh;
        if (surface_rect(handle, &wx, &wy, &ww, &wh) != 0) continue;
        if (wy < WINDOW_TITLEBAR_HEIGHT) continue; /* kein Platz oberhalb, siehe compositor.c */
        uint32_t bar_y = wy - WINDOW_TITLEBAR_HEIGHT;

        if (x < wx || x >= wx + ww || y < bar_y || y >= wy) continue;

        if (best_handle == 0 || window_table[i].z_order > best_z) {
            best_handle = handle;
            best_z = window_table[i].z_order;
        }
    }
    return best_handle;
}

int window_move(uint32_t handle, uint32_t x, uint32_t y) {
    if (window_find_slot(handle) < 0) return -1;
    uint32_t cx, cy, cw, ch;
    if (surface_rect(handle, &cx, &cy, &cw, &ch) != 0) return -1;
    return surface_resize(handle, x, y, cw, ch);
}

int window_self_test(void) {
    surface_manager_initialize();
    window_manager_initialize();

    uint32_t w1, w2, w3;
    if (window_create(1u, 0u, 0u, 10u, 10u, 1, &w1) != 0) return -1;
    if (!window_is_decorated(w1)) return -1;
    if (window_is_visible(w1)) return -1; /* vor Attach+Commit nicht ACTIVE */

    if (window_create(1u, 5u, 5u, 10u, 10u, 0, &w2) != 0) return -1;
    if (window_is_decorated(w2)) return -1;
    if (window_count_in_use() != 2u) return -1;

    /* Z-Order: zuletzt erzeugtes Fenster liegt zunaechst oben. */
    if (window_z_order(w2) <= window_z_order(w1)) return -1;

    /* Raise von w1 muss es ueber w2 bringen. */
    if (window_raise(w1) != 0) return -1;
    if (window_z_order(w1) <= window_z_order(w2)) return -1;

    /* Lower von w1 muss es wieder unter w2 bringen. */
    if (window_lower(w1) != 0) return -1;
    if (window_z_order(w1) >= window_z_order(w2)) return -1;

    /* Parent/Child. */
    if (window_create(1u, 1u, 1u, 2u, 2u, 0, &w3) != 0) return -1;
    if (window_set_parent(w3, w1) != 0) return -1;
    if (window_parent(w3) != w1) return -1;
    if (window_set_parent(w3, w3) == 0) return -1; /* Selbstreferenz verboten */
    if (window_set_parent(w3, 0xFFFFu) == 0) return -1; /* unbekannter Parent */

    /* Fokus. */
    if (window_focused_handle() != 0) return -1;
    if (window_focus(w2) != 0) return -1;
    if (window_focused_handle() != w2) return -1;
    if (window_focus(0xFFFFu) == 0) return -1; /* unbekannt */
    if (window_focused_handle() != w2) return -1; /* unveraendert nach Fehlversuch */

    /* Minimize/Maximize/Fullscreen/Restore (NPSPEC-WINDOW-MANAGEMENT-
       0004/-0009) - braucht eine ACTIVE Surface (Attach+Commit). */
    static uint32_t w1_pixels[100];
    for (uint32_t i = 0; i < 100u; ++i) w1_pixels[i] = 0x808080u;
    if (surface_attach_buffer(w1, (uint32_t)(uintptr_t)w1_pixels) != 0) return -1;
    if (surface_commit(w1) != 0) return -1;
    if (!window_is_visible(w1)) return -1;
    if (window_mode(w1) != WINDOW_MODE_NORMAL) return -1;

    if (window_minimize(w1) != 0) return -1;
    if (window_mode(w1) != WINDOW_MODE_MINIMIZED) return -1;
    if (window_is_visible(w1)) return -1; /* HIDDEN */
    if (window_minimize(w1) != 0) return -1; /* idempotent */

    /* Maximize waehrend minimiert ist unzulaessig - zuerst restore(). */
    if (window_maximize(w1) == 0) return -1;

    if (window_restore(w1) != 0) return -1;
    if (window_mode(w1) != WINDOW_MODE_NORMAL) return -1;
    if (!window_is_visible(w1)) return -1; /* wieder ACTIVE, kein erneutes Attach noetig */

    uint32_t rx, ry, rw, rh;
    if (surface_rect(w1, &rx, &ry, &rw, &rh) != 0) return -1;
    if (rx != 0u || ry != 0u || rw != 10u || rh != 10u) return -1; /* unveraendert */

    if (window_maximize(w1) != 0) return -1;
    if (window_mode(w1) != WINDOW_MODE_MAXIMIZED) return -1;
    if (surface_rect(w1, &rx, &ry, &rw, &rh) != 0) return -1;
    if (rx != 0u || ry != 0u || rw != display_width() || rh != display_height()) return -1;
    if (!window_is_visible(w1)) return -1; /* ACTIVE bleibt ACTIVE, nur Geometrie aendert sich */

    if (window_restore(w1) != 0) return -1;
    if (window_mode(w1) != WINDOW_MODE_NORMAL) return -1;
    if (surface_rect(w1, &rx, &ry, &rw, &rh) != 0) return -1;
    if (rx != 0u || ry != 0u || rw != 10u || rh != 10u) return -1; /* urspr. Rechteck zurueck */

    if (window_set_fullscreen(w1) != 0) return -1;
    if (window_mode(w1) != WINDOW_MODE_FULLSCREEN) return -1;
    if (surface_rect(w1, &rx, &ry, &rw, &rh) != 0) return -1;
    if (rx != 0u || ry != 0u || rw != display_width() || rh != display_height()) return -1;

    if (window_restore(w1) != 0) return -1;
    if (window_mode(w1) != WINDOW_MODE_NORMAL) return -1;

    /* Hit-Testing (NPSPEC-WINDOW-MANAGEMENT-0005): w1 ist wieder bei
       (0,0,10,10) und ACTIVE. Ein Punkt innerhalb trifft w1, ausserhalb
       trifft nichts (w2/w3 sind an dieser Stelle nicht ACTIVE). */
    if (window_hit_test(5u, 5u) != w1) return -1;
    if (window_hit_test(20u, 20u) != 0u) return -1;

    /* Ueberlappende, beide sichtbare Fenster: das topmost (hoeherer
       Z-Order) muss gewinnen. */
    uint32_t hit_a, hit_b;
    if (window_create(1u, 0u, 0u, 20u, 20u, 0, &hit_a) != 0) return -1;
    if (window_create(1u, 5u, 5u, 20u, 20u, 0, &hit_b) != 0) return -1;
    static uint32_t hit_pixels[400];
    if (surface_attach_buffer(hit_a, (uint32_t)(uintptr_t)hit_pixels) != 0) return -1;
    if (surface_commit(hit_a) != 0) return -1;
    if (surface_attach_buffer(hit_b, (uint32_t)(uintptr_t)hit_pixels) != 0) return -1;
    if (surface_commit(hit_b) != 0) return -1;
    /* (10,10) liegt in beiden Rechtecken - hit_b (zuletzt erzeugt, hoeherer
       Z-Order) muss gewinnen. */
    if (window_hit_test(10u, 10u) != hit_b) return -1;
    if (window_raise(hit_a) != 0) return -1; /* jetzt gewinnt hit_a */
    if (window_hit_test(10u, 10u) != hit_a) return -1;
    window_destroy(hit_a);
    window_destroy(hit_b);

    /* Titelleisten-Hit-Test (NPSPEC-WINDOW-DECORATION-0002) + window_move
       (Grundlage fuer input.c's Fenster-Drag per Titelleiste). */
    uint32_t drag_win;
    if (window_create(1u, 30u, 40u, 50u, 30u, 1, &drag_win) != 0) return -1; /* decorated */
    static uint32_t drag_pixels[50 * 30];
    if (surface_attach_buffer(drag_win, (uint32_t)(uintptr_t)drag_pixels) != 0) return -1;
    if (surface_commit(drag_win) != 0) return -1;

    /* Punkt in der Titelleiste (oberhalb des Fensterrechtecks) trifft. */
    if (window_titlebar_hit_test(40u, 25u) != drag_win) return -1;
    /* Punkt im Fensterinhalt selbst ist KEIN Titelleisten-Treffer. */
    if (window_titlebar_hit_test(40u, 50u) != 0u) return -1;

    if (window_move(drag_win, 60u, 70u) != 0) return -1;
    uint32_t mx, my, mw, mh;
    if (surface_rect(drag_win, &mx, &my, &mw, &mh) != 0) return -1;
    if (mx != 60u || my != 70u || mw != 50u || mh != 30u) return -1; /* Groesse unveraendert */
    if (window_move(0xFFFFu, 0u, 0u) == 0) return -1; /* unbekanntes Handle */

    window_destroy(drag_win);

    /* Unbekanntes Handle muss bei allen vier Operationen fehlschlagen. */
    if (window_minimize(0xFFFFu) == 0) return -1;
    if (window_maximize(0xFFFFu) == 0) return -1;
    if (window_set_fullscreen(0xFFFFu) == 0) return -1;
    if (window_restore(0xFFFFu) == 0) return -1;

    /* Zerstoeren von w1: w3 (Kind) muss seine Parent-Referenz verlieren,
       Fokus auf w2 bleibt unberuehrt. */
    if (window_destroy(w1) != 0) return -1;
    if (window_parent(w3) != 0) return -1;
    if (window_focused_handle() != w2) return -1;
    if (window_count_in_use() != 2u) return -1;

    /* Fokus auf das zerstoerte Fenster selbst muss aufgehoben werden. */
    if (window_focus(w3) != 0) return -1;
    if (window_destroy(w3) != 0) return -1;
    if (window_focused_handle() != 0) return -1;

    if (window_destroy(w2) != 0) return -1;
    if (window_count_in_use() != 0u) return -1;

    /* Unbekanntes Handle muss ueberall sauber fehlschlagen. */
    if (window_destroy(0xFFFFu) == 0) return -1;
    if (window_raise(0xFFFFu) == 0) return -1;

    return 0;
}

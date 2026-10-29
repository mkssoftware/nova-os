/* NovaOS C-Kernel - Nova Input System (siehe input.h fuer Scope). */
#include "input.h"
#include "ps2.h"
#include "window.h"
#include "surface.h"
#include "compositor.h"
#include "display.h"
#include "threadmgr.h"
#include "interrupt.h"
#include "desktop_shell.h"

static input_event_t input_ring[INPUT_RING_CAPACITY];
static uint32_t input_ring_head = 0; /* naechster Schreibindex */
static uint32_t input_ring_tail = 0; /* naechster Leseindex */
static uint32_t input_ring_count = 0;
static uint32_t input_dropped = 0;

static uint32_t input_cursor_x_value = 0;
static uint32_t input_cursor_y_value = 0;

/* Fenster-Drag per Titelleiste (NPSPEC-WINDOW-DECORATION-0002): 0 = kein
   Drag aktiv. offset_x/y sind VORZEICHENBEHAFTET, da ein Klick in der
   Titelleiste (oberhalb des Fensterrechtecks) einen negativen Y-Offset
   gegenueber dem Fensterursprung ergibt. */
static uint32_t input_drag_handle = 0;
static int32_t input_drag_offset_x = 0;
static int32_t input_drag_offset_y = 0;

/* Zweite, von input_ring getrennte Warteschlange fuer den Ring-3-Client
   (siehe input.h Scope-Hinweis zu SYSCALL_SERVICE_INPUT). */
static input_event_t input_client_queue[INPUT_CLIENT_QUEUE_CAPACITY];
static uint32_t input_client_head = 0;
static uint32_t input_client_tail = 0;
static uint32_t input_client_count = 0;
static uint32_t input_client_dropped = 0;

/* Mindestabstand zwischen zwei Ringpuffer-Drain-Durchlaeufen des Input-
   Threads, in echten Timer-Ticks (gleiches Muster wie compositor.c's
   COMPOSITOR_THREAD_INTERVAL_TICKS). Kuerzer als der Compositor-Takt, da
   Mausbewegung sich sonst sichtbar ruckelig anfuehlt. */
#define INPUT_THREAD_INTERVAL_TICKS 2u

void input_initialize(void) {
    for (uint32_t i = 0; i < INPUT_RING_CAPACITY; ++i) input_ring[i] = (input_event_t){0};
    input_ring_head = 0;
    input_ring_tail = 0;
    input_ring_count = 0;
    input_dropped = 0;

    for (uint32_t i = 0; i < INPUT_CLIENT_QUEUE_CAPACITY; ++i) input_client_queue[i] = (input_event_t){0};
    input_client_head = 0;
    input_client_tail = 0;
    input_client_count = 0;
    input_client_dropped = 0;

    uint32_t width = display_width();
    uint32_t height = display_height();
    input_cursor_x_value = (width > 0u) ? width / 2u : 0u;
    input_cursor_y_value = (height > 0u) ? height / 2u : 0u;

    input_drag_handle = 0;
    input_drag_offset_x = 0;
    input_drag_offset_y = 0;

    ps2_controller_initialize();
}

void input_push_event(const input_event_t *evt) {
    if (input_ring_count >= INPUT_RING_CAPACITY) {
        ++input_dropped;
        return;
    }
    input_ring[input_ring_head] = *evt;
    input_ring_head = (input_ring_head + 1u) % INPUT_RING_CAPACITY;
    ++input_ring_count;
}

uint32_t input_dropped_count(void) {
    return input_dropped;
}

int input_pop_event(input_event_t *out) {
    if (input_ring_count == 0u) return -1;
    *out = input_ring[input_ring_tail];
    input_ring_tail = (input_ring_tail + 1u) % INPUT_RING_CAPACITY;
    --input_ring_count;
    return 0;
}

uint32_t input_pending_count(void) {
    return input_ring_count;
}

uint32_t input_cursor_x(void) {
    return input_cursor_x_value;
}

uint32_t input_cursor_y(void) {
    return input_cursor_y_value;
}

static void input_client_push(const input_event_t *evt) {
    if (input_client_count >= INPUT_CLIENT_QUEUE_CAPACITY) {
        ++input_client_dropped;
        return;
    }
    input_client_queue[input_client_head] = *evt;
    input_client_head = (input_client_head + 1u) % INPUT_CLIENT_QUEUE_CAPACITY;
    ++input_client_count;
}

int input_client_poll(input_event_t *out) {
    if (input_client_count == 0u) return -1;
    *out = input_client_queue[input_client_tail];
    input_client_tail = (input_client_tail + 1u) % INPUT_CLIENT_QUEUE_CAPACITY;
    --input_client_count;
    return 0;
}

uint32_t input_client_pending_count(void) {
    return input_client_count;
}

uint32_t input_client_dropped_count(void) {
    return input_client_dropped;
}

/* Alt+Tab-aehnliches Fokus-Zyklieren (NPSPEC-INPUT-FOCUS-0002, reduziert
   auf reine Fenster-Ebene - kein Nova-UI-Steuerelement-Baum vorhanden,
   siehe input.h Scope-Hinweis). Ausgeloest von VKEY_TAB (siehe unten):
   sucht ab dem aktuell fokussierten Fenster das naechste sichtbare
   Fenster (Slot-Reihenfolge, mit Umlauf) und fokussiert+hebt es an.
   Kein-Op, wenn hoechstens ein sichtbares Fenster existiert. */
static void input_cycle_focus(void) {
    uint32_t capacity = window_capacity();
    if (capacity == 0u) return;

    uint32_t current = window_focused_handle();
    int start_slot = -1;
    if (current != 0u) {
        /* Nur suchen, wenn ueberhaupt etwas fokussiert ist - sonst wuerde
           die Suche einen beliebigen FREIEN Slot (Handle ebenfalls 0)
           faelschlich als "aktuelle Position" treffen. */
        for (uint32_t i = 0; i < capacity; ++i) {
            if (window_handle_at_slot(i) == current) {
                start_slot = (int)i;
                break;
            }
        }
    }

    for (uint32_t step = 1; step <= capacity; ++step) {
        uint32_t idx = (uint32_t)(((start_slot + (int)step) % (int)capacity + (int)capacity) % (int)capacity);
        uint32_t handle = window_handle_at_slot(idx);
        if (handle != 0u && handle != current && window_is_visible(handle)) {
            window_focus(handle);
            window_raise(handle);
            return;
        }
    }
}

static void input_dispatch_one(const input_event_t *evt) {
    if (evt->type == INPUT_EVENT_MOUSE_MOVE) {
        uint32_t width = display_width();
        uint32_t height = display_height();

        int32_t next_x = (int32_t)input_cursor_x_value + evt->mouse_dx;
        int32_t next_y = (int32_t)input_cursor_y_value + evt->mouse_dy;

        if (next_x < 0) next_x = 0;
        else if (width > 0u && next_x >= (int32_t)width) next_x = (int32_t)width - 1;
        if (next_y < 0) next_y = 0;
        else if (height > 0u && next_y >= (int32_t)height) next_y = (int32_t)height - 1;

        input_cursor_x_value = (uint32_t)next_x;
        input_cursor_y_value = (uint32_t)next_y;
        compositor_set_cursor(input_cursor_x_value, input_cursor_y_value, 0xFFFFFFu);

        /* Laufender Fenster-Drag per Titelleiste (siehe MOUSE_BUTTON_DOWN
           unten): das Fenster folgt dem Cursor minus dem beim Drag-Start
           gemerkten Versatz. */
        if (input_drag_handle != 0u) {
            int32_t new_x = (int32_t)input_cursor_x_value - input_drag_offset_x;
            int32_t new_y = (int32_t)input_cursor_y_value - input_drag_offset_y;
            if (new_x < 0) new_x = 0;
            if (new_y < 0) new_y = 0;
            window_move(input_drag_handle, (uint32_t)new_x, (uint32_t)new_y);
        }

        input_client_push(evt);
        return;
    }

    /* Linksklick (MOUSE_BUTTON_DOWN): zuerst die Titelleiste pruefen
       (NPSPEC-WINDOW-DECORATION-0002, window_titlebar_hit_test()) - ein
       Treffer fokussiert/hebt das Fenster an UND startet einen Drag
       (merkt den Versatz Cursor<->Fensterursprung fuer die obige MOUSE_
       MOVE-Verarbeitung). Sonst echtes Hit-Testing auf den Fensterinhalt
       (NPSPEC-INPUT-EVENT-0002, window_hit_test()) + Fokus-Wechsel/Raise
       - das war bis zum Input-System der Grund, warum window.h Klick-
       Fokuswechsel als BLOCKED dokumentierte; gilt jetzt nicht mehr
       (siehe dortigen Kommentar). Trifft der Klick die Taskbar (Desktop-
       Shell-Chrome), uebernimmt desktop_shell_handle_click() die Deutung
       (z.B. Start-Button) UND unterdrueckt den Fokus-Wechsel - reines
       Chrome soll nie fokussiert werden. */
    if (evt->type == INPUT_EVENT_MOUSE_BUTTON_DOWN && (evt->mouse_buttons & INPUT_MOUSE_BUTTON_LEFT)) {
        uint32_t titlebar_hit = window_titlebar_hit_test(input_cursor_x_value, input_cursor_y_value);
        if (titlebar_hit != 0u) {
            uint32_t wx, wy, ww, wh;
            if (surface_rect(titlebar_hit, &wx, &wy, &ww, &wh) == 0) {
                window_focus(titlebar_hit);
                window_raise(titlebar_hit);
                input_drag_handle = titlebar_hit;
                input_drag_offset_x = (int32_t)input_cursor_x_value - (int32_t)wx;
                input_drag_offset_y = (int32_t)input_cursor_y_value - (int32_t)wy;
            }
        } else {
            uint32_t hit = window_hit_test(input_cursor_x_value, input_cursor_y_value);
            if (hit != 0) {
                uint32_t wx, wy, ww, wh;
                if (surface_rect(hit, &wx, &wy, &ww, &wh) == 0) {
                    uint32_t local_x = input_cursor_x_value - wx;
                    uint32_t local_y = input_cursor_y_value - wy;
                    if (!desktop_shell_handle_click(hit, local_x, local_y)) {
                        window_focus(hit);
                        window_raise(hit);
                    }
                }
            }
        }
    }

    /* Linke Maustaste losgelassen: einen laufenden Drag beenden (egal ob
       er gerade erst hier oder schon frueher begann). */
    if (evt->type == INPUT_EVENT_MOUSE_BUTTON_UP && (evt->mouse_buttons & INPUT_MOUSE_BUTTON_LEFT)) {
        input_drag_handle = 0u;
    }

    /* VKEY_TAB zyklisiert den Fokus (siehe input_cycle_focus oben) - der
       erste Fall, in dem ein Tastatur-Event selbst (nicht nur Maus) eine
       echte Wirkung im Kernel hat, statt nur an die Client-Warteschlange
       durchgereicht zu werden. */
    if (evt->type == INPUT_EVENT_KEY_DOWN && evt->key_vkey == VKEY_TAB) {
        input_cycle_focus();
    }

    /* Tastatur-/Maus-Tasten-Events: an das aktuell fokussierte Fenster
       "geroutet" (NPSPEC-INPUT-FOCUS-0001, reduziert - siehe input.h
       Scope-Hinweis). Die eigentliche Zustellung an den Ring-3-Client
       passiert unabhaengig vom Fokus-Abfrageergebnis ueber die Client-
       Warteschlange (SYSCALL_SERVICE_INPUT pollt sie, siehe syscall.c) -
       es gibt nur einen Client, daher keine fokusbasierte Filterung. */
    (void)window_focused_handle();
    input_client_push(evt);
}

void input_drain_pending(void) {
    input_event_t evt;
    while (input_pop_event(&evt) == 0) {
        input_dispatch_one(&evt);
    }
}

void input_thread_body(void) {
    uint32_t last_tick = timer_get_ticks();
    for (;;) {
        uint32_t now = timer_get_ticks();
        if ((now - last_tick) >= INPUT_THREAD_INTERVAL_TICKS) {
            last_tick = now;
            input_drain_pending();
        }
        __asm__ volatile ("pause");
    }
}

int input_thread_start(void) {
    uint32_t tid;
    if (thread_create_dynamic((uint32_t)(uintptr_t)input_thread_body, 1u, -1, &tid) != 0) {
        return -1;
    }
    return 0;
}

int input_self_test(void) {
    input_initialize();
    if (ps2_controller_self_test() != 0) return -1;

    input_event_t evt = {0};
    evt.type = INPUT_EVENT_KEY_DOWN;
    evt.key_vkey = 'A';
    input_push_event(&evt);
    if (input_pending_count() != 1u) return -1;

    input_event_t out;
    if (input_pop_event(&out) != 0) return -1;
    if (out.type != INPUT_EVENT_KEY_DOWN || out.key_vkey != (uint16_t)'A') return -1;
    if (input_pending_count() != 0u) return -1;
    if (input_pop_event(&out) == 0) return -1; /* Ring jetzt leer */

    /* Ueberlauf: mehr als INPUT_RING_CAPACITY Events fuellen den Ring
       nicht ueber, sondern zaehlen Drops (kein Blockieren/Wachsen im
       ISR-Kontext moeglich). */
    for (uint32_t i = 0; i < INPUT_RING_CAPACITY + 4u; ++i) {
        input_push_event(&evt);
    }
    if (input_pending_count() != INPUT_RING_CAPACITY) return -1;
    if (input_dropped_count() != 4u) return -1;
    while (input_pop_event(&out) == 0) { }

    /* Mausbewegung bewegt den Cursor um genau dx/dy. */
    input_initialize(); /* Reset nach dem Ueberlauf-Test */
    uint32_t start_x = input_cursor_x();
    uint32_t start_y = input_cursor_y();
    input_event_t move = {0};
    move.type = INPUT_EVENT_MOUSE_MOVE;
    move.mouse_dx = 10;
    move.mouse_dy = -4;
    input_dispatch_one(&move);
    if (input_cursor_x() != start_x + 10u) return -1;
    if (input_cursor_y() + 4u != start_y) return -1;

    /* input_dispatch_one() reiht jedes verarbeitete Event zusaetzlich in
       die Client-Warteschlange ein (siehe SYSCALL_SERVICE_INPUT in
       syscall.c) - unabhaengig vom rohen ISR-Ringpuffer oben. */
    if (input_client_pending_count() != 1u) return -1;
    input_event_t client_out;
    if (input_client_poll(&client_out) != 0) return -1;
    if (client_out.type != INPUT_EVENT_MOUSE_MOVE || client_out.mouse_dx != 10) return -1;
    if (input_client_pending_count() != 0u) return -1;
    if (input_client_poll(&client_out) == 0) return -1; /* Warteschlange jetzt leer */

    /* Grenzen-Clamping: eine riesige negative Bewegung darf nicht unter
       Koordinate 0 gehen. */
    input_event_t big_move = {0};
    big_move.type = INPUT_EVENT_MOUSE_MOVE;
    big_move.mouse_dx = -1000000;
    big_move.mouse_dy = -1000000;
    input_dispatch_one(&big_move);
    if (input_cursor_x() != 0u || input_cursor_y() != 0u) return -1;
    (void)input_client_poll(&client_out); /* aufraeumen */

    /* Ueberlauf der Client-Warteschlange (getrennter Zaehler vom rohen
       Ringpuffer). */
    for (uint32_t i = 0; i < INPUT_CLIENT_QUEUE_CAPACITY + 3u; ++i) {
        input_dispatch_one(&move);
    }
    if (input_client_pending_count() != INPUT_CLIENT_QUEUE_CAPACITY) return -1;
    if (input_client_dropped_count() != 3u) return -1;
    while (input_client_poll(&client_out) == 0) { }

    /* input_drain_pending() (vom Input-Thread genutzt) verarbeitet alle
       im rohen Ringpuffer wartenden Events deterministisch in einem
       Rutsch - fuer Selbsttests AUSSERHALB von input.c (z.B. syscall.c),
       die keinen echten Thread-Lauf abwarten koennen/wollen. */
    input_push_event(&move);
    input_push_event(&move);
    input_drain_pending();
    if (input_pending_count() != 0u) return -1;
    if (input_client_pending_count() != 2u) return -1;
    while (input_client_poll(&client_out) == 0) { }

    /* Linksklick-Routing (window_hit_test() + window_focus()/window_raise(),
       NPSPEC-INPUT-EVENT-0002) - eigenes, einfaches Testfenster statt
       einer echten Desktop-Shell-Taskbar (die wird separat in
       desktop_shell_self_test ueber desktop_shell_handle_click geprueft).
       Sicher an dieser Stelle im Boot: window.c/surface.c tragen noch
       keine echten Fenster (die frueheste "Desktop First"-Phase nutzt
       weiterhin nur die alte compositor_window_create-Rechteck-API). */
    surface_manager_initialize();
    window_manager_initialize();
    uint32_t click_win;
    if (window_create(1u, 50u, 50u, 100u, 80u, 0, &click_win) != 0) return -1;
    static uint32_t click_win_pixels[100 * 80];
    if (surface_attach_buffer(click_win, (uint32_t)(uintptr_t)click_win_pixels) != 0) return -1;
    if (surface_commit(click_win) != 0) return -1;
    if (window_focused_handle() != 0) return -1;

    input_cursor_x_value = 75u;
    input_cursor_y_value = 70u; /* liegt innerhalb von click_win */
    input_event_t click = {0};
    click.type = INPUT_EVENT_MOUSE_BUTTON_DOWN;
    click.mouse_buttons = INPUT_MOUSE_BUTTON_LEFT;
    input_dispatch_one(&click);
    if (window_focused_handle() != click_win) return -1;

    /* Klick ausserhalb jedes Fensters darf den Fokus nicht aendern. */
    input_cursor_x_value = 5u;
    input_cursor_y_value = 5u;
    input_dispatch_one(&click);
    if (window_focused_handle() != click_win) return -1;

    window_destroy(click_win);

    /* Fenster-Drag per Titelleiste (window_titlebar_hit_test() +
       window_move(), siehe input_dispatch_one's MOUSE_BUTTON_DOWN/
       MOUSE_MOVE-Behandlung). */
    uint32_t drag_win;
    if (window_create(1u, 100u, 100u, 60u, 40u, 1, &drag_win) != 0) return -1; /* decorated */
    static uint32_t drag_win_pixels[60 * 40];
    if (surface_attach_buffer(drag_win, (uint32_t)(uintptr_t)drag_win_pixels) != 0) return -1;
    if (surface_commit(drag_win) != 0) return -1;

    input_cursor_x_value = 110u;
    input_cursor_y_value = 90u; /* liegt in der Titelleiste: y in [80,100) */
    input_event_t drag_down = {0};
    drag_down.type = INPUT_EVENT_MOUSE_BUTTON_DOWN;
    drag_down.mouse_buttons = INPUT_MOUSE_BUTTON_LEFT;
    input_dispatch_one(&drag_down);
    if (window_focused_handle() != drag_win) return -1;

    input_event_t drag_move = {0};
    drag_move.type = INPUT_EVENT_MOUSE_MOVE;
    drag_move.mouse_dx = 20;
    drag_move.mouse_dy = 15;
    input_dispatch_one(&drag_move);
    uint32_t dx, dy, dw, dh;
    if (surface_rect(drag_win, &dx, &dy, &dw, &dh) != 0) return -1;
    if (dx != 120u || dy != 115u) return -1; /* 100+20, 100+15 - Versatz blieb konstant */
    if (dw != 60u || dh != 40u) return -1; /* Groesse unveraendert */

    input_event_t drag_up = {0};
    drag_up.type = INPUT_EVENT_MOUSE_BUTTON_UP;
    drag_up.mouse_buttons = INPUT_MOUSE_BUTTON_LEFT;
    input_dispatch_one(&drag_up);

    /* Nach dem Loslassen bewegt eine weitere Mausbewegung das Fenster
       NICHT mehr. */
    input_dispatch_one(&drag_move);
    if (surface_rect(drag_win, &dx, &dy, &dw, &dh) != 0) return -1;
    if (dx != 120u || dy != 115u) return -1; /* unveraendert */

    window_destroy(drag_win);

    /* Tab-Fokus-Zyklus (input_cycle_focus): drei sichtbare Fenster,
       Umlauf vorwaerts, und ein verschwindendes fokussiertes Fenster
       darf den Zyklus nicht blockieren. */
    uint32_t ta, tb, tc;
    if (window_create(1u, 0u, 0u, 20u, 20u, 0, &ta) != 0) return -1;
    if (window_create(1u, 30u, 0u, 20u, 20u, 0, &tb) != 0) return -1;
    if (window_create(1u, 60u, 0u, 20u, 20u, 0, &tc) != 0) return -1;
    static uint32_t cycle_pixels[400];
    if (surface_attach_buffer(ta, (uint32_t)(uintptr_t)cycle_pixels) != 0) return -1;
    if (surface_commit(ta) != 0) return -1;
    if (surface_attach_buffer(tb, (uint32_t)(uintptr_t)cycle_pixels) != 0) return -1;
    if (surface_commit(tb) != 0) return -1;
    if (surface_attach_buffer(tc, (uint32_t)(uintptr_t)cycle_pixels) != 0) return -1;
    if (surface_commit(tc) != 0) return -1;

    if (window_focused_handle() != 0u) return -1;
    input_event_t tab_evt = {0};
    tab_evt.type = INPUT_EVENT_KEY_DOWN;
    tab_evt.key_vkey = VKEY_TAB;
    input_dispatch_one(&tab_evt); /* kein Fokus -> erstes sichtbares Fenster */
    if (window_focused_handle() != ta) return -1;
    input_dispatch_one(&tab_evt);
    if (window_focused_handle() != tb) return -1;
    input_dispatch_one(&tab_evt);
    if (window_focused_handle() != tc) return -1;
    input_dispatch_one(&tab_evt); /* Umlauf zurueck zu ta */
    if (window_focused_handle() != ta) return -1;

    /* Verschwindet das fokussierte Fenster, zyklisiert Tab zum naechsten
       verbleibenden sichtbaren Fenster, statt steckenzubleiben. */
    window_destroy(tb);
    input_dispatch_one(&tab_evt);
    if (window_focused_handle() != tc) return -1;

    window_destroy(ta);
    window_destroy(tc);

    return 0;
}

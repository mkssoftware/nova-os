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
#include "syscall.h"

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
/* Pointer-Capture gilt auch fuer undekorierte Client-Fenster, die ihr
   Verschieben selbst ueber GUI_OP_MOVE umsetzen: Nach BUTTON_DOWN bleiben
   MOVE/UP beim urspruenglichen Fenster, selbst wenn der Cursor dessen
   Rechteck waehrend des Ziehens kurz verlaesst. */
static uint32_t input_pointer_capture_handle = 0;

/* Fenster-Resize per Eckgriff (window_resize_corner_hit_test, GUI-
   Design-Spec "Eingabe": "Resize"): 0 = kein Resize aktiv.
   start_w/h+start_cursor_x/y merken den Zustand bei BUTTON_DOWN, jede
   folgende MOUSE_MOVE berechnet die neue Groesse relativ dazu (NICHT
   relativ zum letzten MOVE - vermeidet Rundungsdrift ueber viele kleine
   Events). Obere Grenze ist window_max_size() (die Spawn-Groesse, siehe
   window.h) - der Client-GUI-Puffer ist nie fuer mehr Flaeche physisch
   allokiert; untere Grenze eine feste Mindestgroesse, unter der ein
   Fenster nicht mehr sinnvoll bedienbar waere. */
#define WINDOW_RESIZE_MIN_W 120u
#define WINDOW_RESIZE_MIN_H 80u
static uint32_t input_resize_handle = 0;
static uint32_t input_resize_start_w = 0;
static uint32_t input_resize_start_h = 0;
static uint32_t input_resize_start_cursor_x = 0;
static uint32_t input_resize_start_cursor_y = 0;

/* Super(allein)->Startmenue vs. Super+E->Explorer (siehe input_dispatch_
   one unten): 1 = waehrend der aktuell gehaltenen Super-Taste wurde schon
   eine Kombo ausgeloest, also beim Loslassen von Super NICHT zusaetzlich
   das Startmenue toggeln. */
static int input_super_combo_used = 0;

/* Zweite, von input_ring getrennte Warteschlangen-FAMILIE fuer Ring-3-
   Clients (siehe input.h Scope-Hinweis zu SYSCALL_SERVICE_INPUT und zur
   NPSPEC-INPUT-EVENT-0001-Nachruestung). Slot 0 = Legacy/Fallback, immer
   gebunden (pid-Feld bleibt 0, "bound" bleibt 1). Slots 1..N-1 werden
   lazy an die erste PID gebunden, die dort pollt. */
typedef struct {
    int bound;
    uint32_t pid;
    input_event_t queue[INPUT_CLIENT_QUEUE_CAPACITY];
    uint32_t head, tail, count, dropped;
} input_client_slot_t;

static input_client_slot_t input_client_slots[INPUT_CLIENT_SLOTS];

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

    for (uint32_t s = 0; s < INPUT_CLIENT_SLOTS; ++s) {
        input_client_slot_t *slot = &input_client_slots[s];
        for (uint32_t i = 0; i < INPUT_CLIENT_QUEUE_CAPACITY; ++i) slot->queue[i] = (input_event_t){0};
        slot->head = 0;
        slot->tail = 0;
        slot->count = 0;
        slot->dropped = 0;
        slot->pid = 0;
        slot->bound = 0;
    }
    input_client_slots[0].bound = 1; /* Legacy-Slot immer gebunden */

    uint32_t width = display_width();
    uint32_t height = display_height();
    input_cursor_x_value = (width > 0u) ? width / 2u : 0u;
    input_cursor_y_value = (height > 0u) ? height / 2u : 0u;

    input_drag_handle = 0;
    input_drag_offset_x = 0;
    input_drag_offset_y = 0;
    input_pointer_capture_handle = 0;

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

static void input_slot_push(input_client_slot_t *slot, const input_event_t *evt) {
    if (slot->count >= INPUT_CLIENT_QUEUE_CAPACITY) {
        ++slot->dropped;
        return;
    }
    slot->queue[slot->head] = *evt;
    slot->head = (slot->head + 1u) % INPUT_CLIENT_QUEUE_CAPACITY;
    ++slot->count;
}

static int input_slot_poll(input_client_slot_t *slot, input_event_t *out) {
    if (slot->count == 0u) return -1;
    *out = slot->queue[slot->tail];
    slot->tail = (slot->tail + 1u) % INPUT_CLIENT_QUEUE_CAPACITY;
    --slot->count;
    return 0;
}

/* Slot 0 protokolliert weiterhin jedes Event fuer Kernel-Selbsttests.
   Ein echter Client bekommt das Event dagegen nur, wenn seine PID dem
   per Fokus/Hit-Test bestimmten Ziel entspricht. */
static void input_client_push_to(uint32_t target_pid, const input_event_t *evt) {
    input_slot_push(&input_client_slots[0], evt);
    if (target_pid == 0u) return;
    for (uint32_t s = 1; s < INPUT_CLIENT_SLOTS; ++s) {
        if (input_client_slots[s].bound && input_client_slots[s].pid == target_pid) {
            input_slot_push(&input_client_slots[s], evt);
            return;
        }
    }
}

static uint32_t input_pointer_target_handle(void) {
    if (input_pointer_capture_handle != 0u &&
        surface_state(input_pointer_capture_handle) != SURFACE_STATE_FREE) {
        return input_pointer_capture_handle;
    }
    uint32_t handle = window_titlebar_hit_test(input_cursor_x_value, input_cursor_y_value);
    return (handle != 0u) ? handle : window_hit_test(input_cursor_x_value, input_cursor_y_value);
}

static uint32_t input_owner_of_handle(uint32_t handle) {
    return (handle != 0u) ? surface_owner_pid(handle) : 0u;
}

int input_client_poll(input_event_t *out) {
    return input_slot_poll(&input_client_slots[0], out);
}

uint32_t input_client_pending_count(void) {
    return input_client_slots[0].count;
}

uint32_t input_client_dropped_count(void) {
    return input_client_slots[0].dropped;
}

int input_client_poll_for(uint32_t pid, input_event_t *out) {
    if (pid == 0u) {
        return input_client_poll(out);
    }
    for (uint32_t s = 1; s < INPUT_CLIENT_SLOTS; ++s) {
        if (input_client_slots[s].bound && input_client_slots[s].pid == pid) {
            return input_slot_poll(&input_client_slots[s], out);
        }
    }
    for (uint32_t s = 1; s < INPUT_CLIENT_SLOTS; ++s) {
        if (!input_client_slots[s].bound) {
            input_client_slots[s].bound = 1;
            input_client_slots[s].pid = pid;
            return input_slot_poll(&input_client_slots[s], out); /* frisch gebunden, leer */
        }
    }
    /* Slot-Tabelle voll - lieber am gemeinsamen Legacy-Slot 0 teilnehmen
       als Events fuer diesen Client stillschweigend zu verlieren. */
    return input_client_poll(out);
}

void input_client_drain_all(void) {
    input_event_t tmp;
    for (uint32_t s = 0; s < INPUT_CLIENT_SLOTS; ++s) {
        while (input_slot_poll(&input_client_slots[s], &tmp) == 0) { }
    }
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

        /* Laufender Resize per Eckgriff (siehe MOUSE_BUTTON_DOWN unten) -
           relativ zur beim Drag-Start gemerkten Start-Groesse/-Position,
           nicht zum letzten MOVE (sonst wuerde sich Rundung ueber viele
           kleine Events aufsummieren). Obere Grenze: window_max_size()
           (die Spawn-Groesse - der Client-Puffer ist nie fuer mehr
           physisch allokiert, siehe window.h). Untere Grenze: feste
           WINDOW_RESIZE_MIN_W/_H. */
        if (input_resize_handle != 0u) {
            int32_t dx = (int32_t)input_cursor_x_value - (int32_t)input_resize_start_cursor_x;
            int32_t dy = (int32_t)input_cursor_y_value - (int32_t)input_resize_start_cursor_y;
            int32_t new_w = (int32_t)input_resize_start_w + dx;
            int32_t new_h = (int32_t)input_resize_start_h + dy;

            uint32_t max_w, max_h;
            if (window_max_size(input_resize_handle, &max_w, &max_h) == 0) {
                if (new_w > (int32_t)max_w) new_w = (int32_t)max_w;
                if (new_h > (int32_t)max_h) new_h = (int32_t)max_h;
            }
            if (new_w < (int32_t)WINDOW_RESIZE_MIN_W) new_w = (int32_t)WINDOW_RESIZE_MIN_W;
            if (new_h < (int32_t)WINDOW_RESIZE_MIN_H) new_h = (int32_t)WINDOW_RESIZE_MIN_H;

            window_resize(input_resize_handle, (uint32_t)new_w, (uint32_t)new_h);
        }

        uint32_t target = input_pointer_target_handle();
        input_client_push_to(input_owner_of_handle(target), evt);
        return;
    }

    uint32_t delivery_handle = 0u;
    if (evt->type == INPUT_EVENT_MOUSE_BUTTON_DOWN) {
        delivery_handle = input_pointer_target_handle();
        if ((evt->mouse_buttons & INPUT_MOUSE_BUTTON_LEFT) != 0u) {
            input_pointer_capture_handle = delivery_handle;
        }
    } else if (evt->type == INPUT_EVENT_MOUSE_BUTTON_UP) {
        delivery_handle = input_pointer_target_handle();
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
        /* Klick ausserhalb des sichtbaren Startmenues schliesst es. Die
           Taskbar-Zone ist ausgenommen, damit der Startbutton sein Toggle
           selbst eindeutig ausfuehren kann. Geometrie entspricht dem
           Ring-3-Startmenue (280x144) und der 44px-Dockhoehe. */
        if (syscall_start_menu_visible_value() != 0) {
            uint32_t screen_h = display_height();
            uint32_t menu_h = 144u;
            uint32_t dock_h = 44u;
            uint32_t menu_y = (screen_h > menu_h + dock_h) ? screen_h - menu_h - dock_h : 0u;
            int outside = input_cursor_x_value >= 280u || input_cursor_y_value < menu_y ||
                          input_cursor_y_value >= menu_y + menu_h;
            if (outside && input_cursor_y_value < screen_h - dock_h) {
                syscall_toggle_start_menu_from_kernel();
            }
        }

        uint32_t resize_hit = window_resize_corner_hit_test(input_cursor_x_value, input_cursor_y_value);
        uint32_t titlebar_hit = (resize_hit == 0u)
            ? window_titlebar_hit_test(input_cursor_x_value, input_cursor_y_value) : 0u;
        if (resize_hit != 0u) {
            /* Eckgriff hat Vorrang vor Titelleiste/Inhalt - ein Treffer
               fokussiert/hebt das Fenster wie jede andere Interaktion an
               UND startet den Resize-Zustandsautomaten (siehe MOUSE_MOVE
               oben), analog zum Titelleisten-Drag. */
            window_focus(resize_hit);
            window_raise(resize_hit);
            uint32_t cx, cy, cw, ch;
            if (surface_rect(resize_hit, &cx, &cy, &cw, &ch) == 0) {
                (void)cx;
                (void)cy;
                input_resize_handle = resize_hit;
                input_resize_start_w = cw;
                input_resize_start_h = ch;
                input_resize_start_cursor_x = input_cursor_x_value;
                input_resize_start_cursor_y = input_cursor_y_value;
            }
        } else if (titlebar_hit != 0u) {
            uint32_t wx, wy, ww, wh;
            if (surface_rect(titlebar_hit, &wx, &wy, &ww, &wh) == 0) {
                window_focus(titlebar_hit);
                window_raise(titlebar_hit);
                uint32_t control = window_control_hit_test(titlebar_hit, input_cursor_x_value,
                                                           input_cursor_y_value);
                if (control == WINDOW_CONTROL_CLOSE) {
                    window_destroy(titlebar_hit);
                } else if (control == WINDOW_CONTROL_MINIMIZE) {
                    window_minimize(titlebar_hit);
                } else if (control == WINDOW_CONTROL_MAXIMIZE) {
                    if (window_mode(titlebar_hit) == WINDOW_MODE_MAXIMIZED) {
                        window_restore(titlebar_hit);
                    } else {
                        window_maximize(titlebar_hit);
                    }
                } else {
                    input_drag_handle = titlebar_hit;
                    input_drag_offset_x = (int32_t)input_cursor_x_value - (int32_t)wx;
                    input_drag_offset_y = (int32_t)input_cursor_y_value - (int32_t)wy;
                }
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
        input_resize_handle = 0u;
        input_pointer_capture_handle = 0u;
    }

    /* Alt+Tab zyklisiert den Fokus (siehe input_cycle_focus oben und die
       GUI-Design-Spec "Eingabe": "Alt+Tab (Fensterwechsel)") - der erste
       Fall, in dem ein Tastatur-Event selbst (nicht nur Maus) eine echte
       Wirkung im Kernel hat, statt nur an die Client-Warteschlange
       durchgereicht zu werden. Alt-Pflicht erst nachtraeglich ergaenzt
       (2026-11-20): ohne sie wuerde JEDES Tab in einem kuenftigen Client
       mit mehreren Eingabefeldern faelschlich die Fensterfokussierung
       der Desktop Shell ausloesen, statt dem Client zum Feldwechsel zu
       dienen - das Event wird trotzdem IMMER an den Client durchgereicht
       (siehe unten), nur die WM-eigene Sonderbehandlung braucht Alt. */
    if (evt->type == INPUT_EVENT_KEY_DOWN && evt->key_vkey == VKEY_TAB &&
        (evt->modifiers & (INPUT_MOD_LALT | INPUT_MOD_RALT)) != 0u) {
        input_cycle_focus();
    }

    /* Alt+F4 schliesst das fokussierte Fenster (GUI-Design-Spec
       "Eingabe") - Alt wird bereits als Modifier-Bit im Event gefuehrt
       (ps2.c), kein eigener Haltezustand hier noetig. */
    if (evt->type == INPUT_EVENT_KEY_DOWN && evt->key_vkey == VKEY_F4 &&
        (evt->modifiers & (INPUT_MOD_LALT | INPUT_MOD_RALT)) != 0u) {
        uint32_t focused = window_focused_handle();
        if (focused != 0u) window_destroy(focused);
    }

    /* Super(allein)->Startmenue, Super+E->Explorer (GUI-Design-Spec
       "Eingabe"). Die Super-Taste selbst toggelt ERST bei ihrem eigenen
       Loslassen (KEY_UP) - so kann eine waehrenddessen gedrueckte Kombo-
       Taste (E) das per input_super_combo_used unterdruecken, exakt wie
       das uebliche Windows-Taste-Verhalten (Super+E oeffnet NUR den
       Explorer, nicht zusaetzlich das Startmenue). */
    if (evt->type == INPUT_EVENT_KEY_DOWN && evt->key_vkey == 'E' &&
        (evt->modifiers & INPUT_MOD_LSUPER) != 0u) {
        syscall_toggle_explorer_from_kernel();
        input_super_combo_used = 1;
    }
    if (evt->type == INPUT_EVENT_KEY_UP && evt->key_vkey == VKEY_LSUPER) {
        if (!input_super_combo_used) {
            syscall_toggle_start_menu_from_kernel();
        }
        input_super_combo_used = 0;
    }

    /* Escape schliesst das Startmenue, falls offen (GUI-Design-Spec
       "Eingabe": "Escape (Menues schliessen)") - bewusst nur SCHLIESSEN,
       nie oeffnen (anders als Super/Super+E oben, die reine Toggles
       sind): ein Escape-Druck bei bereits geschlossenem Startmenue bleibt
       ein No-Op. Einziges vorhandene "Menue" in diesem Entwicklungsstand
       - Kontextmenues existieren noch nicht. */
    if (evt->type == INPUT_EVENT_KEY_DOWN && evt->key_vkey == VKEY_ESCAPE) {
        if (syscall_start_menu_visible_value() != 0) {
            syscall_toggle_start_menu_from_kernel();
        }
    }

    /* Tasten gehen ausschliesslich an den Besitzer des fokussierten
       Fensters; Pointer-Tasten an das vor der WM-Aktion bestimmte oberste
       Fenster. Damit reagieren verdeckte Fenster nicht mehr auf denselben
       Klick. Slot 0 erhaelt das Event zusaetzlich nur fuer Selbsttests. */
    if (evt->type == INPUT_EVENT_KEY_DOWN || evt->type == INPUT_EVENT_KEY_UP) {
        delivery_handle = window_focused_handle();
    }
    input_client_push_to(input_owner_of_handle(delivery_handle), evt);
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

    /* Tab OHNE Alt darf die Fokus-Zyklierung NICHT ausloesen (siehe
       input_dispatch_one-Kommentar oben - sonst wuerde ein Client mit
       mehreren Eingabefeldern nie ungestoert per Tab wechseln koennen). */
    input_event_t tab_plain = {0};
    tab_plain.type = INPUT_EVENT_KEY_DOWN;
    tab_plain.key_vkey = VKEY_TAB;
    input_dispatch_one(&tab_plain);
    if (window_focused_handle() != 0u) return -1; /* unveraendert */

    input_event_t tab_evt = {0};
    tab_evt.type = INPUT_EVENT_KEY_DOWN;
    tab_evt.key_vkey = VKEY_TAB;
    tab_evt.modifiers = INPUT_MOD_LALT; /* jetzt echtes Alt+Tab, siehe oben */
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

    /* Mehrclient-Routing: Zwei ueberlappende Fenster, nur der Besitzer
       des hoeheren Fensters darf den Klick erhalten. Vorher ging dasselbe
       Event per Broadcast an beide PID-Warteschlangen. */
    input_client_drain_all();
    input_event_t routed_out;
    (void)input_client_poll_for(11u, &routed_out); /* Slots vor Event binden */
    (void)input_client_poll_for(12u, &routed_out);
    uint32_t lower, upper;
    if (window_create(11u, 100u, 100u, 80u, 60u, 0, &lower) != 0) return -1;
    if (window_create(12u, 110u, 110u, 80u, 60u, 0, &upper) != 0) return -1;
    static uint32_t route_pixels[80u * 60u];
    if (surface_attach_buffer(lower, (uint32_t)(uintptr_t)route_pixels) != 0) return -1;
    if (surface_attach_buffer(upper, (uint32_t)(uintptr_t)route_pixels) != 0) return -1;
    if (surface_commit(lower) != 0 || surface_commit(upper) != 0) return -1;
    input_cursor_x_value = 120u;
    input_cursor_y_value = 120u;
    input_event_t routed_click = {0};
    routed_click.type = INPUT_EVENT_MOUSE_BUTTON_DOWN;
    routed_click.mouse_buttons = INPUT_MOUSE_BUTTON_LEFT;
    input_dispatch_one(&routed_click);
    if (input_client_poll_for(11u, &routed_out) == 0) return -1;
    if (input_client_poll_for(12u, &routed_out) != 0) return -1;
    if (routed_out.type != INPUT_EVENT_MOUSE_BUTTON_DOWN) return -1;
    input_event_t routed_up = {0};
    routed_up.type = INPUT_EVENT_MOUSE_BUTTON_UP;
    routed_up.mouse_buttons = INPUT_MOUSE_BUTTON_LEFT;
    input_dispatch_one(&routed_up);
    window_destroy(lower);
    window_destroy(upper);

    /* Alt+F4 schliesst das fokussierte Fenster (siehe input_dispatch_one
       oben). Synthetischer Rundweg wie bei den obigen Faellen - QEMU-
       Tastatur-Injektion ueber QMP/input-send-event erreicht die PS/2-
       IRQ1 in der headless (-display none) Testumgebung nachweislich
       NICHT (per TEMP-DEBUG-Scancode-Trace waehrend der Entwicklung
       bestaetigt: 0 ankommende Bytes trotz mehrfacher QMP-Injektion) -
       ein echter Boot-/Klick-Test kann dieses Feature deshalb nicht
       pruefen, der Selbsttest hier schon. */
    uint32_t f4_win;
    if (window_create(21u, 5u, 5u, 40u, 40u, 0, &f4_win) != 0) return -1;
    window_focus(f4_win);
    if (window_focused_handle() != f4_win) return -1;
    input_event_t f4_evt = {0};
    f4_evt.type = INPUT_EVENT_KEY_DOWN;
    f4_evt.key_vkey = VKEY_F4;
    f4_evt.modifiers = INPUT_MOD_LALT;
    input_dispatch_one(&f4_evt);
    if (window_focused_handle() != 0u) return -1; /* Fenster weg -> kein Fokus mehr */
    if (window_destroy(f4_win) == 0) return -1; /* bereits zerstoert -> muss fehlschlagen */

    /* F4 OHNE Alt darf NICHTS schliessen (sonst wuerde jede normale F4-
       Eingabe in einem Client versehentlich dessen Fenster killen). */
    uint32_t f4_win2;
    if (window_create(22u, 5u, 5u, 40u, 40u, 0, &f4_win2) != 0) return -1;
    window_focus(f4_win2);
    input_event_t f4_plain = {0};
    f4_plain.type = INPUT_EVENT_KEY_DOWN;
    f4_plain.key_vkey = VKEY_F4;
    input_dispatch_one(&f4_plain);
    if (window_focused_handle() != f4_win2) return -1; /* unveraendert */
    window_destroy(f4_win2);

    /* Super (allein) -> Startmenue-Flag toggeln. */
    int sm_before = syscall_start_menu_visible_value();
    input_event_t super_down = {0};
    super_down.type = INPUT_EVENT_KEY_DOWN;
    super_down.key_vkey = VKEY_LSUPER;
    input_dispatch_one(&super_down);
    input_event_t super_up = {0};
    super_up.type = INPUT_EVENT_KEY_UP;
    super_up.key_vkey = VKEY_LSUPER;
    input_dispatch_one(&super_up);
    if (syscall_start_menu_visible_value() == sm_before) return -1;
    syscall_toggle_start_menu_from_kernel(); /* Flag wieder in Ausgangszustand */

    /* Super+E -> nur Explorer toggeln, NICHT zusaetzlich das Startmenue
       (siehe input_super_combo_used-Kommentar oben). */
    int sm_before2 = syscall_start_menu_visible_value();
    int ex_before = syscall_explorer_visible_value();
    input_event_t super_down2 = {0};
    super_down2.type = INPUT_EVENT_KEY_DOWN;
    super_down2.key_vkey = VKEY_LSUPER;
    input_dispatch_one(&super_down2);
    input_event_t e_down = {0};
    e_down.type = INPUT_EVENT_KEY_DOWN;
    e_down.key_vkey = 'E';
    e_down.modifiers = INPUT_MOD_LSUPER;
    input_dispatch_one(&e_down);
    if (syscall_explorer_visible_value() == ex_before) return -1;
    input_event_t super_up2 = {0};
    super_up2.type = INPUT_EVENT_KEY_UP;
    super_up2.key_vkey = VKEY_LSUPER;
    input_dispatch_one(&super_up2);
    if (syscall_start_menu_visible_value() != sm_before2) return -1; /* unveraendert */
    syscall_toggle_explorer_from_kernel(); /* Flag wieder in Ausgangszustand */

    /* Escape schliesst das Startmenue, falls offen - aber toggelt es NIE
       auf, wenn es bereits geschlossen ist (reiner Schliess-Befehl, kein
       Toggle wie Super). */
    if (syscall_start_menu_visible_value() != 0) return -1; /* Vorbedingung */
    input_event_t escape_closed = {0};
    escape_closed.type = INPUT_EVENT_KEY_DOWN;
    escape_closed.key_vkey = VKEY_ESCAPE;
    input_dispatch_one(&escape_closed);
    if (syscall_start_menu_visible_value() != 0) return -1; /* weiterhin unveraendert (No-Op) */

    syscall_toggle_start_menu_from_kernel(); /* jetzt offen */
    input_dispatch_one(&escape_closed);
    if (syscall_start_menu_visible_value() != 0) return -1; /* jetzt wieder geschlossen */

    /* Fenster-Resize per Eckgriff (2026-11-20, GUI-Design-Spec "Eingabe":
       "Resize") - synthetischer Rundweg wie bei allen Maus-Tests oben. */
    uint32_t resize_win;
    if (window_create(32u, 0u, 0u, 200u, 150u, 0, &resize_win) != 0) return -1;
    static uint32_t resize_pixels[200u * 150u];
    if (surface_attach_buffer(resize_win, (uint32_t)(uintptr_t)resize_pixels) != 0) return -1;
    if (surface_commit(resize_win) != 0) return -1;

    /* Eckgriff-Zone liegt bei [190,140) bis (200,150) - (195,145) trifft. */
    input_cursor_x_value = 195u;
    input_cursor_y_value = 145u;
    input_event_t resize_down = {0};
    resize_down.type = INPUT_EVENT_MOUSE_BUTTON_DOWN;
    resize_down.mouse_buttons = INPUT_MOUSE_BUTTON_LEFT;
    input_dispatch_one(&resize_down);

    /* Versuch, ueber die Erzeugungsgroesse (200x150) hinaus zu
       vergroessern - muss auf genau diese Groesse gekappt werden. */
    input_event_t resize_grow = {0};
    resize_grow.type = INPUT_EVENT_MOUSE_MOVE;
    resize_grow.mouse_dx = 100;
    resize_grow.mouse_dy = 100;
    input_dispatch_one(&resize_grow); /* Cursor jetzt (295,245) */
    uint32_t grx, gry, grw, grh;
    if (surface_rect(resize_win, &grx, &gry, &grw, &grh) != 0) return -1;
    if (grw != 200u || grh != 150u) return -1; /* bei max_w/max_h gekappt */

    /* Von dort aus weit verkleinern - muss auf WINDOW_RESIZE_MIN_W/_H
       (120x80) gekappt werden, nicht auf die rechnerische Zielgroesse. */
    input_event_t resize_shrink = {0};
    resize_shrink.type = INPUT_EVENT_MOUSE_MOVE;
    resize_shrink.mouse_dx = -200;
    resize_shrink.mouse_dy = -200;
    input_dispatch_one(&resize_shrink); /* Cursor jetzt (95,45) */
    uint32_t srx, sry, srw, srh;
    if (surface_rect(resize_win, &srx, &sry, &srw, &srh) != 0) return -1;
    if (srw != WINDOW_RESIZE_MIN_W || srh != WINDOW_RESIZE_MIN_H) return -1;

    input_event_t resize_up = {0};
    resize_up.type = INPUT_EVENT_MOUSE_BUTTON_UP;
    resize_up.mouse_buttons = INPUT_MOUSE_BUTTON_LEFT;
    input_dispatch_one(&resize_up);

    /* Nach dem Loslassen darf weitere Mausbewegung die Groesse NICHT
       mehr aendern (Resize-Zustandsautomat muss beendet sein). */
    input_event_t resize_after = {0};
    resize_after.type = INPUT_EVENT_MOUSE_MOVE;
    resize_after.mouse_dx = -10;
    resize_after.mouse_dy = -10;
    input_dispatch_one(&resize_after);
    uint32_t arx, ary, arw, arh;
    if (surface_rect(resize_win, &arx, &ary, &arw, &arh) != 0) return -1;
    if (arw != WINDOW_RESIZE_MIN_W || arh != WINDOW_RESIZE_MIN_H) return -1; /* unveraendert */

    window_destroy(resize_win);

    return 0;
}

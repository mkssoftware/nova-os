/* NovaOS C-Kernel - Nova Input System (Port aus docs/NPSPEC/
   sysarchitecture/049-GUI/005-Inputsystem/NPSPEC-INPUT-ARCH-0001..0004
   (Architektur), NPSPEC-INPUT-EVENT-0001..0004 (Event-Modell/Dispatcher/
   Routing), NPSPEC-INPUT-CURSOR-0001 (Cursor-Position), NPSPEC-INPUT-
   FOCUS-0001 (Fokus-Modell). Hoeherwertige Schicht ueber ps2.c (siehe
   ps2.h fuer die reine Hardware-Dekodierung) - kennt Events/Fokus/Cursor,
   aber keine PS/2-Portadressen.

   WICHTIG (Scope dieser Sitzung): EIN globaler Ringpuffer (fest 64
   Eintraege), KEINE getrennten Warteschlangen pro Empfaenger (NPSPEC-
   INPUT-EVENT-0001 Abschnitt 5, BLOCKED - es gibt bisher nur einen
   Konsumenten-Thread). Echtes Hit-Testing (NPSPEC-INPUT-EVENT-0002
   Abschnitt 6) IST umgesetzt: Linksklick loest window_hit_test()/
   window_titlebar_hit_test() + window_focus()/window_raise() aus (Klick-
   Fokuswechsel ist NICHT mehr blockiert, siehe window.h), ein Klick auf
   die Titelleiste startet zusaetzlich einen Fenster-Drag ueber mehrere
   MOUSE_MOVE-Events hinweg (window_move()) bis zum MOUSE_BUTTON_UP.
   VKEY_TAB zyklisiert den Fokus auf reiner Fenster-Ebene
   (input_cycle_focus, NPSPEC-INPUT-FOCUS-0002 TEILWEISE umgesetzt - nur
   Fenster-Reihenfolge, KEIN Nova-UI-Steuerelement-Baum/Tab-Order
   innerhalb eines Fensters, der existiert nicht). KEIN Pointer Capture
   (NPSPEC-INPUT-EVENT-0003/NPSPEC-INPUT-FOCUS-0003, BLOCKED - der Drag-
   Automat oben ist eine feste Spezialbehandlung fuer Titelleisten, kein
   allgemeiner Capture-Mechanismus fuer beliebige Fensterinhalte), KEINE
   Geste/Mehrfachberuehrung (NPSPEC-INPUT-EVENT-0005, BLOCKED: kein
   Touch-Geraet), KEINE globalen Tastenkombinationen (NPSPEC-INPUT-EVENT-
   0006, BLOCKED: braucht eine Registry+Capability-Pruefung, die noch
   nicht existiert), KEIN Clipboard/Drag&Drop von DATEN (NPSPEC-INPUT-
   DATA-0001..0004, BLOCKED: braucht IPC-Datenaustausch zwischen
   Prozessen, der hier fehlt - nicht zu verwechseln mit dem oben
   umgesetzten reinen Fenster-Verschieben), KEIN Rahmen-Resize per Ziehen
   (kein Resize-Handle-Hit-Testing umgesetzt).

   Zustellung an einen Ring-3-Client: SYSCALL_SERVICE_INPUT (syscall.c,
   analog zu SYSCALL_SERVICE_VFS) pollt input_client_poll() - eine ZWEITE,
   vom rohen ISR-Ringpuffer (input_push_event/input_pop_event) getrennte
   Warteschlange, in die input_dispatch_one() jedes verarbeitete Event
   zusaetzlich einreiht. Da es weiterhin nur den EINEN Ring-3-Client gibt
   (SYSCALL_SINGLE_CLIENT_PID, siehe syscall.h), bekommt dieser Client
   ALLE Events ungefiltert (kein Hit-Testing/keine Fokus-basierte
   Filterung vor der Zustellung - NPSPEC-INPUT-EVENT-0001 Abschnitt 5
   "getrennte Warteschlangen pro Empfaenger" bleibt mit nur einem
   Empfaenger eine triviale Reduktion, keine echte Umsetzung). */
#ifndef NOVA_INPUT_H
#define NOVA_INPUT_H

#include <stdint.h>

#define INPUT_MOD_LSHIFT 0x01u
#define INPUT_MOD_RSHIFT 0x02u
#define INPUT_MOD_LCTRL  0x04u
#define INPUT_MOD_RCTRL  0x08u
#define INPUT_MOD_LALT   0x10u
#define INPUT_MOD_RALT   0x20u

#define INPUT_MOUSE_BUTTON_LEFT   0x01u
#define INPUT_MOUSE_BUTTON_RIGHT  0x02u
#define INPUT_MOUSE_BUTTON_MIDDLE 0x04u

typedef enum {
    INPUT_EVENT_KEY_DOWN = 0u,
    INPUT_EVENT_KEY_UP,
    INPUT_EVENT_MOUSE_MOVE,
    INPUT_EVENT_MOUSE_BUTTON_DOWN,
    INPUT_EVENT_MOUSE_BUTTON_UP
} input_event_type_t;

/* NPSPEC-INPUT-EVENT-0001's Event-Grundmodell, reduziert (kein Source-
   Device-Handle, keine Event-ID/-Prioritaet - siehe Scope-Hinweis). */
typedef struct {
    input_event_type_t type;
    uint32_t timestamp_ticks; /* timer_get_ticks() */
    uint16_t key_vkey;        /* nur bei KEY_DOWN/KEY_UP gueltig, siehe ps2.h VKEY_* */
    uint8_t modifiers;        /* Bitmaske INPUT_MOD_*, Momentaufnahme bei Tastatur-Events */
    int32_t mouse_dx, mouse_dy; /* nur bei MOUSE_MOVE gueltig (relative Bewegung) */
    uint8_t mouse_buttons;     /* bei MOUSE_MOVE: aktueller Tastenzustand; bei
                                  MOUSE_BUTTON_DOWN/UP: genau EIN gesetztes Bit
                                  (welche Taste den Wechsel ausgeloest hat) */
} input_event_t;

#define INPUT_RING_CAPACITY 64u

/* Setzt Ringpuffer/Cursorposition zurueck (Cursor startet in der
   Bildschirmmitte, siehe display.h) und ruft ps2_controller_initialize()
   (siehe ps2.h) - legt KEINE IDT-Gates/PIC-Masken an, das bleibt
   interrupt.c's Aufgabe. Mehrfacher Aufruf ist sicher. */
void input_initialize(void);

/* 0 = ps2_controller_self_test() UND die reinen Ringpuffer-/Cursor-
   Grenzpruefungen (synthetische Events, kein echter IRQ noetig) waren
   erfolgreich. */
int input_self_test(void);

/* ISR-Kontext (IF=0, von ps2.c aufgerufen): nur Ringpuffer-Push, kein
   Blockieren/Allozieren. Verwirft das Event stillschweigend bei vollem
   Ring (siehe input_dropped_count fuer die Diagnose-Zaehlung). */
void input_push_event(const input_event_t *evt);
uint32_t input_dropped_count(void);

/* 0 = Erfolg (*out gesetzt), -1 = Ring leer. Fuer den Input-Thread
   (siehe input_thread_body), aber auch direkt fuer Selbsttests nutzbar. */
int input_pop_event(input_event_t *out);
uint32_t input_pending_count(void);

uint32_t input_cursor_x(void);
uint32_t input_cursor_y(void);

#define INPUT_CLIENT_QUEUE_CAPACITY 16u

/* NPSPEC-INPUT-EVENT-0001 Abschnitt 5 ("getrennte Warteschlangen pro
   Empfaenger") - bis 2026-11-04 bewusst NICHT umgesetzt (siehe
   Dateikommentar oben, mittlerweile VERALTET: es gibt seit ushell_process.c
   laengst DREI echte, gleichzeitig laufende Ring-3-Clients, nicht mehr nur
   einen). Echter Fund: alle drei konkurrierten um dieselbe EINE globale
   Warteschlange - wer zuerst pollte, "gewann" jedes Event, unabhaengig
   davon, fuer wen es per Cursor-Position eigentlich gedacht war (siehe
   [[feedback_input_single_queue_race]]). Jetzt: bis zu INPUT_CLIENT_SLOTS
   eigene, per PID gebundene Warteschlangen - jedes dispatchte Event wird
   per Broadcast in ALLE aktuell gebundenen Slots UND in den Legacy-Slot 0
   eingereiht (jeder Client filtert ohnehin selbst per Cursor-Position/
   Hit-Test, ein Broadcast ist daher sicher und einfacher als echtes
   Vorab-Routing). */
#define INPUT_CLIENT_SLOTS 4u

/* Legacy-Verhalten (Slot 0, IMMER gebunden) - fuer Selbsttests und als
   Fallback, wenn der Aufrufer-PID nicht aufloesbar ist (frueh im Boot,
   bevor irgendein Prozess existiert). 0 = Erfolg (*out gesetzt), -1 =
   Warteschlange leer. */
int input_client_poll(input_event_t *out);
uint32_t input_client_pending_count(void);
uint32_t input_client_dropped_count(void);

/* Wie input_client_poll(), aber aus der PER-PID gebundenen Warteschlange
   (erster Aufruf mit einer neuen PID bindet lazy einen freien Slot - der
   Client bekommt ab dann alle KUENFTIGEN Broadcasts, keine rueckwirkenden).
   pid==0 ist gleichbedeutend mit input_client_poll() (Slot 0). Faellt bei
   voller Slot-Tabelle (alle INPUT_CLIENT_SLOTS-1 echten Slots belegt) auf
   Slot 0 zurueck, statt das Event zu verlieren - EIN gemeinsamer
   Notbehelf-Empfaenger ist besser als ein stillschweigend blinder Client.
   0 = Erfolg (*out gesetzt), -1 = Warteschlange leer. */
int input_client_poll_for(uint32_t pid, input_event_t *out);

/* Fuer Selbsttests (siehe syscall.c): leert ALLE Slots (0 und alle aktuell
   gebundenen), unabhaengig davon, welcher Slot vom naechsten echten Poll-
   Aufruf tatsaechlich genutzt wuerde. */
void input_client_drain_all(void);

/* Verarbeitet alle aktuell im rohen ISR-Ringpuffer wartenden Events
   (Cursor-Update/Fokus-Abfrage/Einreihen in die Client-Warteschlange,
   siehe input_dispatch_one in input.c) - faktorisiert aus
   input_thread_body heraus, damit Selbsttests (input.c UND syscall.c)
   das Draining deterministisch ohne echten Thread-Lauf ausloesen
   koennen. */
void input_drain_pending(void);

/* Einziger Einstiegspunkt eines dauerhaft laufenden Kernel-Threads (siehe
   input_thread_start), analog zu compositor_thread_body/desktop_shell_
   thread_body: drained in festem Taktabstand den Ringpuffer (MOUSE_MOVE
   aktualisiert Cursor+compositor_set_cursor(), alle Events werden an das
   aktuell fokussierte Fenster "geroutet" - siehe Scope-Hinweis zur noch
   fehlenden echten Zustellung). NICHT direkt aufrufen. */
void input_thread_body(void);

/* Registriert input_thread_body als echten Kernel-Thread
   (thread_create_dynamic) - wie compositor_thread_start ERST NACHDEM
   thread_manager initialisiert wurde. 0 = Erfolg, -1 wenn kein freier
   Scheduler-Slot mehr verfuegbar ist. */
int input_thread_start(void);

#endif

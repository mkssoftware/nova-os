/* NovaOS C-Kernel - Nova Desktop Shell (Port aus docs/NPSPEC/sysarchitekt
   ure/049-GUI/007-Desktop_Shell/NPSPEC-SHELL-ARCH-0001..0004 (Lifecycle),
   -DESKTOP-0001..0005 (Desktop/Icons), -TASKBAR-0001..0005 (Taskleiste/
   Fensterliste/Systemablage), -START-0001..0004 (Startmenue), -NOTIFY-
   0001..0004 (Benachrichtigungen).

   WICHTIG (Scope dieser Sitzung): Die vollen NPSPECs beschreiben eine
   vollstaendige Desktop-Umgebung als EIGENER Shell-Prozess mit eigenem
   Recovery-Supervisor (ARCH-0002/0004, braucht echten Multitasking-
   Userspace+IPC - BLOCKED), Capability-/Erweiterungs-Modell fuer
   Drittanbieter-Widgets (ARCH-0003, BLOCKED - Capability-Framework nicht
   portiert, siehe window.h/cap.h), einem vollen Nova-UI-Toolkit mit
   Controls/Data-Binding/Styles (006-Native_UI, existiert im Kernel nicht -
   hier wird direkt mit render2d.h-Primitiven + font.c-Text gezeichnet),
   sowie echter Maus-/Tastatur-Interaktion (Klick auf Start-Button/
   Taskbar-Icons/Kontextmenues/Drag&Drop - BLOCKED, siehe interrupt.c: IRQ1
   quittiert nur den Scancode, kein Input-Router/keine Maus-IRQ
   verdrahtet). Diese Portierung bildet nur das nach, was ohne diese drei
   Subsysteme sinnvoll moeglich ist: eine STATISCHE/zeitgesteuerte
   Darstellung ueber echte window.c/surface.c-Fenster (wie vom Compositor
   bereits automatisch komponiert), mit Daten, die zur Boot-Zeit registriert
   werden (Startmenue-Eintraege, Desktop-Icons, Systemablage-Platzhalter).

   Alle programmatischen Operationen (start_menu_toggle, notify_push) sind
   bewusst als API angelegt, die ein kuenftiger Input-Handler direkt
   aufrufen koennte - exakt das gleiche Muster wie window_focus/window_raise
   in window.h, die ebenfalls schon vor jedem Input-Subsystem bestehen.

   Taskleisten-Uhr zeigt nur die Laufzeit in Timer-Ticks (keine RTC-
   Anbindung in diesem Modul - kein cmos.c/rtc.c im Kernel vorhanden,
   time.c's CMOS-Lesefunktion ist nicht an diese Anzeige angebunden).
   Systemablage-Icons (Akku/Lautstaerke/Netzwerk) sind reine
   Text-Platzhalter ("N/A") - apower.h/network.h bieten keine fuer einen
   Endbenutzer sinnvolle Akkustand-/Lautstaerke-/Signalstaerke-Abfrage. */
#ifndef NOVA_DESKTOP_SHELL_H
#define NOVA_DESKTOP_SHELL_H

#include <stdint.h>

/* NPSPEC-SHELL-ARCH-0001, stark reduziert (wie compositor.h/window.h: nur
   die Zustaende, die ohne Recovery-Supervisor/eigenen Prozess sinnvoll
   unterscheidbar sind). READY = Taskbar/Icons/Startmenue angelegt,
   ACTIVE = zusaetzlich der periodische Shell-Thread (Uhr/Fensterliste/
   Notification-Timeout) laeuft. */
#define SHELL_STATE_UNINITIALIZED 0u
#define SHELL_STATE_READY         1u
#define SHELL_STATE_ACTIVE        2u

#define TASKBAR_HEIGHT 28u

#define START_MENU_CAPACITY 8u
#define START_MENU_WIDTH    160u
#define START_MENU_ITEM_H   20u

#define NOTIFY_QUEUE_CAPACITY 4u
#define NOTIFY_TOAST_WIDTH    220u
#define NOTIFY_TOAST_HEIGHT   48u

/* NPSPEC-SHELL-NOTIFY-0002 (Toast-Typen), reduziert auf die reine
   Einfaerbung - keine unterschiedlichen Icons (kein Icon-Decoder, siehe
   render2d.h). */
#define NOTIFY_TYPE_INFO    0u
#define NOTIFY_TYPE_SUCCESS 1u
#define NOTIFY_TYPE_WARNING 2u
#define NOTIFY_TYPE_ERROR   3u

#define DESKTOP_ICON_CAPACITY 12u
#define SYSTRAY_ICON_CAPACITY 4u

void desktop_shell_initialize(void);
int desktop_shell_self_test(void); /* 0 = ok */
uint32_t desktop_shell_state(void);

/* Legt Taskbar- und Icon-Layer-Fenster an (echte window.c/surface.c-
   Fenster, siehe compositor.c's compositor_draw_surfaces_clip - werden
   dadurch automatisch mitgezeichnet) und zeichnet sie einmalig. Erfordert
   ein initialisiertes display.c (display_width/height > 0). 0 = Erfolg,
   -1 wenn kein Display oder Fenster-/Surface-Tabelle voll. Zustand ->
   READY. Idempotent bei Mehrfachaufruf nur, solange desktop_shell_initialize
   zwischendurch nicht erneut lief (sonst wuerden doppelte Fenster
   entstehen - wie bei compositor_initialize/window_manager_initialize,
   siehe dortige Selbsttests). */
int desktop_shell_show(void);

/* Zeichnet die Taskbar neu: Start-Button, Uhr (Timer-Ticks seit Boot),
   und eine Liste der aktuell sichtbaren, NICHT zur Shell selbst
   gehoerenden Fenster (NPSPEC-SHELL-TASKBAR-0002, Running Tasks - rein
   lesend ueber window_handle_at_slot/window_is_visible, kein Klick-
   Handler). 0 = Erfolg, -1 wenn die Taskbar noch nicht angelegt wurde. */
int taskbar_redraw(void);

/* NPSPEC-SHELL-START-0004 (Pinned/statische Eintraege), zur Boot-Zeit
   hartcodiert registriert (kein Anwendungsregistrierungs-/Launcher-System
   vorhanden - START-0002 BLOCKED). 0 = Erfolg (*out_index optional), -1
   wenn Tabelle voll oder label == 0. */
int start_menu_register_static(const char *label, uint32_t *out_index);
uint32_t start_menu_entry_count(void);

/* Zeigt/verbirgt das bereits bei desktop_shell_show() angelegte,
   anfangs verborgene Startmenue-Fenster (surface_commit/surface_hide ueber
   dasselbe Handle - window_create legt es nur einmal an). 0 = Erfolg, -1
   wenn noch nicht angelegt. */
int start_menu_show(void);
int start_menu_hide(void);
int start_menu_toggle(void);
int start_menu_is_visible(void); /* 1/0, 0 auch wenn unbekannt */

/* NPSPEC-SHELL-DESKTOP-0002 (Icon Grid), statische Registrierung vor dem
   naechsten desktop_shell_show()-Aufruf - das Icon-Layer-Fenster wird dort
   einmalig mit allen bis dahin registrierten Icons gezeichnet (kein
   Drag&Drop/keine Neuanordnung, siehe window.h - kein Input). grid_x/
   grid_y sind Zellkoordinaten (0-basiert), keine Pixel. 0 = Erfolg, -1
   wenn Tabelle voll oder label == 0. */
int desktop_icon_register_static(uint32_t grid_x, uint32_t grid_y, const char *label);
uint32_t desktop_icon_count(void);

/* NPSPEC-SHELL-SYSTEM-0002 (System Tray Icons), reine Platzhalter-Eintraege
   (siehe Scope-Hinweis oben: kein echtes Battery/Volume/Network-Backend an
   dieser Stelle verdrahtet) - werden rechts in der Taskbar aufgereiht. */
int systray_register_static(const char *label);
uint32_t systray_entry_count(void);

/* NPSPEC-SHELL-NOTIFY-0001/0002 (Queue + Toast-Lifecycle): erzeugt ein
   eigenes Toast-Fenster oben rechts, das nach NOTIFY_TIMEOUT_TICKS
   automatisch wieder entfernt wird (notify_tick, kein Benutzer-Dismiss
   moeglich - kein Input). 0 = Erfolg (*out_slot optional), -1 wenn die
   Queue voll ist oder text == 0. */
int notify_push(uint32_t type, const char *text, uint32_t *out_slot);
/* Entfernt alle abgelaufenen Toasts (timer_get_ticks()-Vergleich). Vom
   periodischen Shell-Thread aufgerufen, kann aber auch direkt (z.B. in
   einem Selbsttest) aufgerufen werden. */
void notify_tick(void);
uint32_t notify_active_count(void);

/* Einziger Einstiegspunkt eines dauerhaft laufenden Kernel-Threads (siehe
   desktop_shell_thread_start), analog zu compositor_thread_body: ruft in
   festem Taktabstand taskbar_redraw() (Uhr/Fensterliste) und notify_tick()
   auf. NICHT direkt aufrufen. */
void desktop_shell_thread_body(void);

/* Registriert desktop_shell_thread_body als echten Kernel-Thread
   (thread_create_dynamic) - wie compositor_thread_start ERST NACHDEM
   thread_manager initialisiert wurde. 0 = Erfolg (Zustand -> ACTIVE), -1
   wenn kein freier Scheduler-Slot mehr verfuegbar ist. */
int desktop_shell_thread_start(void);

#endif

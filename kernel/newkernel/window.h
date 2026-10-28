/* NovaOS C-Kernel - Nova Window Manager (Port aus docs/NPSPEC/
   sysarchitecture/049-GUI/003-Window_Manager/NPSPEC-WINDOW-ARCH-0002.md
   (Lifecycle), -ARCH-0003.md (State Model, Datenmodell-Teil),
   NPSPEC-WINDOW-MANAGEMENT-0001.md (Creation/Destruction), -0005.md
   (Z-Order), -0008.md (Parent/Child).

   WICHTIG (Scope dieser Sitzung): Ein "Window" ist hier bewusst NICHT eine
   eigenstaendige Abstraktion mit eigenem Capability-/IPC-Modell (das
   braeuchte das unportierte Capability-Framework, NPSPEC-WINDOW-ARCH-0004,
   BLOCKED), sondern eine DUENNE Metadaten-Schicht ueber genau einer
   surface.c-Surface: "Window = Surface + WM-Metadaten (Parent, Z-Order,
   Fokus, Dekoriert-Flag)". Das Window-Handle IST das zugrundeliegende
   Surface-Handle (keine eigene Handle-Vergabe) - vereinfacht die API,
   ohne Information zu verlieren, da jedes Window exakt eine Surface
   besitzt. Lifecycle/Sichtbarkeit wird NICHT dupliziert, sondern ueber
   surface_state() abgeleitet (window_is_visible). Interaktive Teile
   (Maus-Drag/Resize/Fokus-Wechsel per Klick) sind BLOCKED, da kein
   Input-Subsystem existiert (IRQ1/Tastatur wird in interrupt.c nur
   quittiert, keine Maus-IRQ verdrahtet) - diese API bietet nur die
   PROGRAMMATISCHEN Operationen (window_raise/window_focus/...), die ein
   kuenftiger Input-Handler oder ein Client direkt aufrufen koennte. */
#ifndef NOVA_WINDOW_H
#define NOVA_WINDOW_H

#include <stdint.h>

#define WINDOW_CAPACITY 8u

/* NPSPEC-WINDOW-MANAGEMENT-0004 (Min/Maximize) + -0009 (Fullscreen),
   Darstellungsmodus getrennt vom Surface-Lifecycle (NPSPEC-WINDOW-ARCH-
   0003: "State Model getrennt von Lifecycle"). Reiner Datenmodell-/
   Geometrie-Mechanismus - interaktives Ausloesen per Maus-Doppelklick auf
   die Titelleiste ist BLOCKED (kein Input-Subsystem, siehe window.c). */
#define WINDOW_MODE_NORMAL     0u
#define WINDOW_MODE_MINIMIZED  1u
#define WINDOW_MODE_MAXIMIZED  2u
#define WINDOW_MODE_FULLSCREEN 3u

void window_manager_initialize(void);
int window_self_test(void); /* 0 = ok */

/* Erzeugt Surface+Window in einem Schritt (surface_create + surface_configure).
   0 = Erfolg (*out_handle gesetzt), -1 wenn Surface- oder Window-Tabelle
   voll. decorated != 0 markiert das Fenster fuer Titelleiste/Rahmen durch
   einen Folgemodul-Compositor-Pass (hier nur als Flag gespeichert, noch
   nicht gezeichnet). */
int window_create(uint32_t owner_pid, uint32_t x, uint32_t y, uint32_t w,
                   uint32_t h, int decorated, uint32_t *out_handle);

/* Zerstoert sowohl die Window-Metadaten als auch die zugrundeliegende
   Surface (surface_destroy). Ein Fokus auf diesem Handle wird aufgehoben,
   Kinder verlieren ihre Parent-Referenz (nicht rekursiv zerstoert - ein
   verwaistes Kind bleibt ein eigenstaendiges Top-Level-Fenster, analog zu
   X11/Win32-Verhalten bei Parent-Zerstoerung). 0 = Erfolg, -1 wenn
   unbekannt. */
int window_destroy(uint32_t handle);

/* Sichtbarkeit leitet sich 1:1 aus surface_state()==SURFACE_STATE_ACTIVE
   ab - keine eigene Window-Sichtbarkeits-Flag-Duplizierung. */
int window_is_visible(uint32_t handle); /* 1 = sichtbar, 0 sonst/unbekannt */

int window_is_decorated(uint32_t handle); /* 1/0, 0 auch wenn unbekannt */

/* 0 = Erfolg, -1 wenn handle oder parent_handle unbekannt, oder
   parent_handle == handle (keine Selbstreferenz). parent_handle == 0
   entfernt eine bestehende Parent-Beziehung. */
int window_set_parent(uint32_t handle, uint32_t parent_handle);
uint32_t window_parent(uint32_t handle); /* 0 wenn keiner/unbekannt */

/* Z-Order (NPSPEC-WINDOW-MANAGEMENT-0005): hoeherer Wert = weiter oben.
   window_raise setzt auf neuen Maximalwert+1, window_lower auf neuen
   Minimalwert-1 - reine Ordinalzahl, kein eigenstaendiger Compositor-
   Renderpass in dieser Sitzung (siehe Compositor-Draw-Reihenfolge separat
   in einem Folgemodul). 0 = Erfolg, -1 wenn unbekannt. */
int window_raise(uint32_t handle);
int window_lower(uint32_t handle);
uint32_t window_z_order(uint32_t handle); /* 0 wenn unbekannt */

/* Nur EIN Fenster kann fokussiert sein. window_focus(0) hebt jeden
   bestehenden Fokus auf. 0 = Erfolg, -1 wenn handle != 0 und unbekannt. */
int window_focus(uint32_t handle);
uint32_t window_focused_handle(void); /* 0 wenn keines fokussiert */

/* WINDOW_MODE_NORMAL wird auch fuer ein unbekanntes Handle zurueckgegeben
   (vereinfachte Fehlerbehandlung - Aufrufer, denen das unterscheiden
   wichtig ist, pruefen zusaetzlich window_find in surface_state()). */
uint32_t window_mode(uint32_t handle);

/* Minimiert ein sichtbares Fenster (surface_hide darunter) und merkt sich
   erstmalig dessen aktuelles Rechteck fuer window_restore(). 0 = Erfolg,
   -1 wenn Handle unbekannt oder die Surface nicht ACTIVE war. Idempotent
   (bereits minimiert -> weiterhin 0). */
int window_minimize(uint32_t handle);

/* Maximiert auf die volle Displaygroesse (display_width()/height()) via
   surface_resize() - funktioniert auch auf einer bereits ACTIVE Surface.
   Merkt sich (beim ersten Aufruf aus NORMAL) das aktuelle Rechteck fuer
   window_restore(). 0 = Erfolg, -1 wenn Handle unbekannt, kein Display
   verfuegbar, oder das Fenster aktuell minimiert ist (zuerst
   window_restore aufrufen). Idempotent. */
int window_maximize(uint32_t handle);

/* Wie window_maximize, aber zusaetzlich markiert als FULLSCREEN - der
   Compositor unterdrueckt in diesem Modus die Titelleiste auch bei
   decorated!=0 (NPSPEC-WINDOW-MANAGEMENT-0009: Vollbild ohne Chrome). */
int window_set_fullscreen(uint32_t handle);

/* Stellt NORMAL wieder her: aus MINIMIZED per surface_commit (kein
   Geometrie-Wechsel), aus MAXIMIZED/FULLSCREEN per surface_resize auf das
   beim ersten Maximize/Fullscreen gemerkte Rechteck. 0 = Erfolg, -1 wenn
   Handle unbekannt oder der zugrundeliegende surface_commit/resize
   fehlschlaegt. Idempotent (bereits NORMAL -> weiterhin 0). */
int window_restore(uint32_t handle);

uint32_t window_count_in_use(void);

/* Iteration ueber alle Slots (fuer den Compositor, analog zu
   surface_capacity()/surface_handle_at_slot()). */
uint32_t window_capacity(void);
uint32_t window_handle_at_slot(uint32_t slot_index); /* 0 wenn frei/ausserhalb */

#endif

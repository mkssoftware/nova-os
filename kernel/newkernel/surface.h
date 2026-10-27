/* NovaOS C-Kernel - Nova Surface Manager (Port aus docs/NPSPEC/sysarchitecture/
   049-GUI/002-Compositor/NPSPEC-COMPOSITOR-SURFACE-0001.md (Surface Management)
   und NPSPEC-COMPOSITOR-SURFACE-0002.md (Surface Lifecycle).

   WICHTIG (Scope dieser Sitzung): Der volle NPSPEC beschreibt client-eigene
   Surfaces mit Parent/Child-Hierarchie (SURFACE-0003), Capability-Gating
   (ARCH-0005, haengt am unportierten Capability-Framework) und Cross-Client-
   Sharing (SURFACE-0006). Dieses Modul bildet nur den Kern nach: eine flache,
   feste Tabelle von Surface-Handles mit echtem Lifecycle-Zustandsautomaten
   (Created->Configured->Attached->Committed/Active->Hidden->Destroying->
   Destroyed), ohne Hierarchie und ohne Capability-Pruefung. Buffer-Attach
   (echte Pixel-Daten) und Damage-Tracking folgen in separaten Folgemodulen -
   dieses Modul verwaltet nur Existenz/Geometrie/Zustand pro Surface. */
#ifndef NOVA_SURFACE_H
#define NOVA_SURFACE_H

#include <stdint.h>

#define SURFACE_CAPACITY 8u

#define SURFACE_STATE_FREE        0u /* Slot frei, kein Handle vergeben */
#define SURFACE_STATE_CREATED     1u
#define SURFACE_STATE_CONFIGURED  2u
#define SURFACE_STATE_ATTACHED    3u
#define SURFACE_STATE_ACTIVE      4u /* committed und sichtbar */
#define SURFACE_STATE_HIDDEN      5u
#define SURFACE_STATE_DESTROYING  6u

void surface_manager_initialize(void);
int surface_self_test(void); /* 0 = ok */

/* 0 = Erfolg (*out_handle gesetzt, Zustand -> CREATED), -1 wenn Tabelle voll. */
int surface_create(uint32_t owner_pid, uint32_t x, uint32_t y, uint32_t w,
                    uint32_t h, uint32_t *out_handle);

/* Erlaubt aus CREATED oder CONFIGURED (Re-Konfiguration vor dem ersten
   Commit). 0 = Erfolg (Zustand -> CONFIGURED), -1 wenn Handle unbekannt oder
   Zustand ungueltig. */
int surface_configure(uint32_t handle, uint32_t x, uint32_t y, uint32_t w,
                      uint32_t h);

/* Haengt einen Pixel-Buffer an (Zustand -> ATTACHED, NPSPEC-COMPOSITOR-
   BUFFER-0001). buffer_addr zeigt auf w*h aufeinanderfolgende uint32_t-
   Pixel (0xRRGGBB, gleiches Format wie framebuffer_set_pixel, Stride=w*4,
   KEIN eigener Pitch-Wert - Vereinfachung gegenueber dem vollen NPSPEC, das
   GPU-Speicher-Layouts mit eigenem Pitch/Padding erlaubt). Die eigentliche
   Seiten-Zuteilung/Mapping in den Userspace eines Clients erfolgt in einem
   Folgemodul (GUI-Syscall-Service) - hier wird nur die Adresse gespeichert
   und der Zustandsuebergang nachvollzogen. Erlaubt aus CONFIGURED oder
   ACTIVE (Re-Attach eines laufenden Clients, z.B. nach Resize). 0 = Erfolg,
   -1 wenn Handle unbekannt, Zustand ungueltig, oder buffer_addr == 0. */
int surface_attach_buffer(uint32_t handle, uint32_t buffer_addr);

/* Rohe Pufferadresse der zuletzt per surface_attach_buffer() angehaengten
   Daten. 0 wenn Handle unbekannt oder noch kein Buffer angehaengt wurde. */
uint32_t surface_buffer_addr(uint32_t handle);

/* NPSPEC-COMPOSITOR-BUFFER-0004 / SURFACE-0004 (Buffer-/Commit-Synchroni-
   sation): echte GPU-Fences sind ohne GPU bedeutungslos - stattdessen ein
   einfacher, monoton steigender Sequenzzaehler pro Surface, der bei jedem
   erfolgreichen surface_commit() erhoeht wird. Der Compositor vergleicht
   diesen gegen seinen zuletzt konsumierten Stand, um zu erkennen, ob seit
   dem letzten Redraw ein neuer Frame vorliegt (surface_consume_if_dirty). */
uint32_t surface_commit_seq(uint32_t handle); /* 0 wenn Handle unbekannt */

/* 0 = es lag ein neuer, noch nicht konsumierter Commit vor (Zustand wird
   sofort als konsumiert markiert), -1 = kein neuer Commit seit dem letzten
   Aufruf (oder Handle unbekannt) - vom Compositor einmal pro Surface und
   Composite-Durchlauf aufzurufen. */
int surface_consume_if_dirty(uint32_t handle);

/* Gesamtkapazitaet der Tabelle (fuer Iteration durch alle Slots von
   aussen, z.B. durch den Compositor). */
uint32_t surface_capacity(void);
/* Handle am Tabellenindex slot_index (0-basiert), 0 wenn frei oder
   slot_index >= surface_capacity(). Erlaubt dem Compositor, ueber alle
   belegten Surfaces zu iterieren, ohne die interne Tabelle offenzulegen. */
uint32_t surface_handle_at_slot(uint32_t slot_index);

/* Erlaubt aus ATTACHED oder HIDDEN. 0 = Erfolg (Zustand -> ACTIVE), -1 wenn
   Handle unbekannt oder Zustand ungueltig (z.B. noch kein Buffer angehaengt). */
int surface_commit(uint32_t handle);

/* Erlaubt aus ACTIVE. 0 = Erfolg (Zustand -> HIDDEN). */
int surface_hide(uint32_t handle);

/* Leitet die Zerstoerung ein (Zustand -> DESTROYING) und gibt den Slot
   sofort frei (kein asynchrones Teardown noetig, da keine GPU-Ressourcen
   im Spiel sind). 0 = Erfolg, -1 wenn Handle unbekannt. */
int surface_destroy(uint32_t handle);

uint32_t surface_state(uint32_t handle); /* SURFACE_STATE_FREE wenn unbekannt */
int surface_rect(uint32_t handle, uint32_t *out_x, uint32_t *out_y,
                  uint32_t *out_w, uint32_t *out_h); /* 0 = Erfolg */
uint32_t surface_owner_pid(uint32_t handle); /* 0 wenn unbekannt */

/* Damage-Tracking (NPSPEC-COMPOSITOR-SURFACE-0005). Jede Geometrie-Aenderung
   (configure) und jeder Commit vergroessert/erzeugt das aufsummierte
   Damage-Rechteck dieser Surface - echte Teilbereichs-Verfolgung innerhalb
   einer Surface (einzelne geaenderte Pixel) ist ohne Buffer-Attach (Folge-
   modul) nicht sinnvoll moeglich, daher wird bei jedem Commit konservativ
   das volle aktuelle Surface-Rechteck als beschaedigt markiert. 0 = Erfolg
   (*out_* gesetzt), -1 wenn Handle unbekannt oder kein Damage vorhanden. */
int surface_damage_rect(uint32_t handle, uint32_t *out_x, uint32_t *out_y,
                         uint32_t *out_w, uint32_t *out_h);
/* Setzt das Damage-Rechteck dieser Surface zurueck (vom Compositor nach
   erfolgtem Redraw aufzurufen). 0 = Erfolg, -1 wenn Handle unbekannt. */
int surface_clear_damage(uint32_t handle);

/* Anzahl Slots, die nicht FREE sind (fuer Diagnose/Selbsttest). */
uint32_t surface_count_in_use(void);

#endif

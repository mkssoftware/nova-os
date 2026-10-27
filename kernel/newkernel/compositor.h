/* NovaOS C-Kernel - Nova Compositor (reduzierter In-Kernel-Port der
   Layer-/Alpha-Komposition aus docs/NPSPEC/sysarchitecture/049-GUI/
   002-Compositor/NPSPEC-COMPOSITOR-RENDER-0002.md (Layer Composition) und
   NPSPEC-COMPOSITOR-EFFECTS-0001.md (Transparency and Alpha Composition).

   WICHTIG (Nutzer-Entscheidung 2026-10-26): Die vollen NPSPECs beschreiben
   einen eigenstaendigen GPU-beschleunigten Compositor mit Zero-Copy-Buffern,
   Fences, Occlusion Culling, Damage Tracking, Blur/Acrylic usw. Dieses
   Modul bildet nur den Kern nach, der ohne GPU-Treiber und Taskwechsel
   sinnvoll moeglich ist: eine feste, kleine Layer-Liste (Hintergrund +
   rechteckige "Fenster" + Cursor), Software-Alpha-Blending nach dem
   Porter-Duff-Verfahren Source-Over aus EFFECTS-0001 (Abschnitt 6), sowie
   ein einfacher, nicht weichgezeichneter Schatten-Platzhalter (statt
   echtem Blur aus EFFECTS-0002/0003). Kein Double/Triple-Buffering, kein
   Damage-Tracking, keine Hardware-Overlays - jeder compositor_composite()
   Aufruf zeichnet den kompletten Frame neu direkt in den Framebuffer. */
#ifndef NOVA_COMPOSITOR_H
#define NOVA_COMPOSITOR_H

#include <stdint.h>

#define COMPOSITOR_WINDOW_CAPACITY 4u

/* NPSPEC-COMPOSITOR-ARCH-0004 (Lifecycle), stark reduziert: nur die zwei
   Zustaende, die ohne eigenstaendigen Fault-Recovery-Supervisor (ARCH-0005,
   BLOCKED) sinnvoll unterscheidbar sind. READY wird von jedem
   compositor_initialize()-Aufruf gesetzt (auch von Selbsttests); RUNNING
   erst durch den echten, dauerhaft laufenden Compositor-Thread (siehe
   compositor_thread_start) - erst dann "uebernimmt der Display-Server die
   Ausgabe" im eigentlichen Sinn (kontinuierliches Redraw statt einmaligem
   compositor_composite()-Aufruf beim Boot). */
#define COMPOSITOR_STATE_UNINITIALIZED 0u
#define COMPOSITOR_STATE_READY         1u
#define COMPOSITOR_STATE_RUNNING       2u

void compositor_initialize(void);
int compositor_self_test(void); /* 0 = ok */
uint32_t compositor_state(void);

void compositor_set_background(uint32_t rgb);

/* 0 = Erfolg (*out_window_id gesetzt), -1 wenn Tabelle voll.
   alpha: 0 = vollstaendig transparent, 255 = vollstaendig deckend. */
int compositor_window_create(uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                              uint32_t rgb, uint32_t alpha, uint32_t *out_window_id);
/* 0 = Erfolg, -1 wenn window_id unbekannt. */
int compositor_window_set_shadow(uint32_t window_id, int enabled);

void compositor_set_cursor(uint32_t x, uint32_t y, uint32_t rgb);

/* Zeichnet Hintergrund, Schatten, Fenster (in Erstellungsreihenfolge) und
   Cursor vollstaendig neu in den Framebuffer. Fenster, die vollstaendig von
   einem spaeteren, voll deckenden Fenster ueberdeckt sind, werden dabei
   per Occlusion Culling (NPSPEC-COMPOSITOR-RENDER-0003) uebersprungen - das
   Endergebnis im Framebuffer ist identisch zu ungekuerztem Zeichnen. */
void compositor_composite(void);

/* Anzahl der beim letzten compositor_composite()-Aufruf per Occlusion
   Culling uebersprungenen Fenster-Zeichenoperationen (Diagnose/Selbsttest). */
uint32_t compositor_last_occluded_count(void);

/* Einziger Einstiegspunkt eines dauerhaft laufenden Kernel-Threads (siehe
   compositor_thread_start) - matcht thread_create_dynamic's Signatur
   (parameterlos, nicht zurueckkehrend). Ruft compositor_composite() in
   einem festen Zeitabstand (echte Timer-Ticks, siehe interrupt.h
   timer_get_ticks - NICHT jeden Scheduler-Durchlauf, das waere reine
   Verschwendung ohne VSync zum Takten gegen). NICHT direkt aufrufen. */
void compositor_thread_body(void);

/* Registriert compositor_thread_body als echten, dauerhaft laufenden
   Kernel-Thread (thread_create_dynamic) - ERST NACHDEM thread_manager
   initialisiert wurde, aber VOR scheduler_enable() (das erst viel spaeter,
   nach dem Ring-3-Rundweg, in syscall.c aufgerufen wird - der neue Thread
   sitzt bis dahin einfach inaktiv in seinem Scheduler-Slot, exakt wie
   scheduler_thread1/2). 0 = Erfolg (Zustand -> RUNNING), -1 wenn kein
   freier Scheduler-Slot mehr verfuegbar ist. */
int compositor_thread_start(void);

#endif

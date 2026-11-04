/* NovaOS C-Kernel - Nova Frame Scheduler (049-GUI/011-Graphic_Engine/
   008-Frame_Scheduling, Phase 8 - LETZTE Phase des Graphics-Engine-
   Vorhabens). Zehn NPSPECs dieses Ordners: ARCH/EVENT/DAMAGE/VSYNC/PACING/
   IDLE/THROTTLE/LATENCY/POWER/ADAPTIVE-0001.

   Dies ist das KAPSTUECK, das die gesamte seit Phase 1 aufgebaute Kette
   tatsaechlich benutzt, um die zentrale, in praktisch jedem einzelnen
   NPSPEC dieses gesamten Vorhabens wiederholte Forderung ECHT
   durchzusetzen: **"Ein unveraenderter Bildschirm benoetigt keine neuen
   Rendering-Frames"** (VSYNC-0001 §14). `fsched_frame_request()` prueft
   VOR jeder Planung echten ausstehenden Damage via `damage_pending_count()`
   (Phase 5) - ohne Damage wird GARANTIERT kein Frame erzeugt, kein
   Platzhalter, keine Attrappe.

   ECHTER AUFBAU AUF Phase 5 (Damage Tracking) UND den Zeitstempel-Mustern
   aus Phase 7 (`timer_get_ticks()`, bereits in `dispproto.c` fuer
   Praesentations-Zeitstempel verwendet).

   Da dieser Kernel keine echte Hardware-VSync-Quelle, kein Power-
   Management mit mehreren echten Energiezustaenden und keine Lastmessung
   (CPU/GPU-Auslastung) besitzt, bleibt der Scope auf das ohne diese
   Grundlagen ehrlich Implementierbare begrenzt: Idle/Active-Zustands-
   maschine pro Ausgabe (echt, von echtem Damage getrieben), Software-
   VSync-Zaehler (reiner Zeit-/Zaehlerfortschritt, erzwingt NIEMALS einen
   Frame), Deadline-Arithmetik (`Render Deadline = Target - Estimated -
   Margin`, reine deterministische Berechnung), Frame Rate Throttling
   (Mindestintervall seit letztem Frame, Damage bleibt bei Drosselung
   erhalten) und ein einfaches Power-Mode-Enum ohne echte Hardware-
   Kopplung.

   Bewusst NICHT Teil dieser Phase: echte Lastmessung (CPU Load/GPU Load -
   keine Performance-Counter-Infrastruktur angebunden), echte VRR-Hardware-
   Ansteuerung, echte Energiesparzustands-Uebergaenge (verbleibt beim noch
   nicht angebundenen Power-Management-Subsystem), Multi-Frame-Queue-
   Tiefensteuerung (ein Frame pro Request reicht fuer den hier
   nachgewiesenen Bedarfsnachweis). */
#ifndef NOVA_FRAMESCHED_H
#define NOVA_FRAMESCHED_H

#include <stdint.h>

#define FSCHED_OUTPUT_CAPACITY 4u
#define FSCHED_FRAME_CAPACITY  16u

/* NPSPEC-FRAME-ARCH-0001 §5 Frame Lifecycle (Deferred/Superseded/
   Cancelled/Failed entfallen bewusst - dieser Kernel modelliert keine
   echte Nebenlaeufigkeit, in der sie beobachtbar waeren). */
#define FRAME_SCHEDULED  1u
#define FRAME_RENDERING  2u
#define FRAME_READY      3u
#define FRAME_PRESENTED  4u

/* NPSPEC-FRAME-POWER-0001 §4 Power Modes (Critical Power entfaellt -
   reines Enum ohne echte Hardware-Kopplung, siehe Dateikopf). */
#define POWER_PERFORMANCE 1u
#define POWER_BALANCED    2u
#define POWER_SAVING      3u

/* NPSPEC-FRAME-IDLE-0001 §3 Idle State Model (vereinfacht auf die beiden
   tatsaechlich unterscheidbaren Zustaende dieses Kernels). */
#define OUTPUT_ACTIVE 1u
#define OUTPUT_IDLE   2u

#define FSCHED_ERR_NOT_FOUND     (-1)
#define FSCHED_ERR_CAPACITY      (-2)
#define FSCHED_ERR_INVALID_STATE (-3)

typedef struct {
    uint32_t output_id;
    uint32_t refresh_interval_ticks;
    uint32_t last_vsync_time;
    uint32_t vsync_counter;
    uint32_t idle_state;
    uint32_t power_mode;
    uint32_t min_interval_ticks; /* Throttle, 0 = kein Limit */
    uint32_t last_frame_time;
} output_sched_info_t;

typedef struct {
    uint32_t frame_id;
    uint32_t output_id;
    uint32_t target_id;
    uint32_t state;
    uint32_t event_time;
    uint32_t render_start;
    uint32_t render_complete;
    uint32_t presentation_time;
    uint32_t render_deadline;
} frame_info_t;

void framesched_initialize(void);
int framesched_self_test(void); /* 0 = ok */

int fsched_output_register(uint32_t output_id, uint32_t refresh_interval_ticks);
int fsched_output_set_power_mode(uint32_t output_id, uint32_t power_mode);
/* min_interval_ticks: Mindestabstand zwischen zwei Frames dieser Ausgabe
   (#5 THROTTLE-0001 "Minimum Interval = 1 / Maximum Frame Rate"). */
int fsched_output_set_throttle(uint32_t output_id, uint32_t min_interval_ticks);
int fsched_output_info(uint32_t output_id, output_sched_info_t *out_info);

/* Reiner Zeit-/Zaehlerfortschritt - erzwingt NIEMALS einen Frame (#5
   VSYNC-0001 "Ein VSync-Ereignis DARF allein keinen neuen Rendering-
   Auftrag erzwingen"). Aktualisiert last_vsync_time per timer_get_ticks()
   und zaehlt vsync_counter hoch. */
int fsched_vsync_tick(uint32_t output_id);

/* Herzstueck (siehe Dateikopf): prueft ECHTEN ausstehenden Damage fuer
   target_id via `damage_pending_count()` (Phase 5) UND das Throttle-
   Intervall der Ausgabe, BEVOR irgendetwas eingeplant wird. *out_scheduled
   == 0 bedeutet: kein Frame wurde erzeugt (weder Damage vorhanden noch
   Drosselung ausgeloest je nach Fall) - der ausstehende Damage bleibt in
   jedem Fall unberuehrt in damagetrack.c erhalten (#6 THROTTLE-0001/#2
   EVENT-0001). estimated_processing_ticks fliesst in die Deadline-
   Berechnung ein (naechster VSync dieser Ausgabe als Praesentationsziel).
   0 = Erfolg (*out_frame_id nur gueltig, wenn *out_scheduled!=0). */
int fsched_frame_request(uint32_t output_id, uint32_t target_id,
                          uint32_t estimated_processing_ticks,
                          uint32_t *out_frame_id, int *out_scheduled);

int fsched_frame_render_start(uint32_t frame_id);
int fsched_frame_render_complete(uint32_t frame_id);
int fsched_frame_mark_presented(uint32_t frame_id);
int fsched_frame_info(uint32_t frame_id, frame_info_t *out_info);

/* Reine deterministische Arithmetik (#6 VSYNC-0001/PACING-0001):
   Render Deadline = target_presentation_time - estimated_processing_ticks
   - safety_margin_ticks, nach unten auf 0 begrenzt (keine negative/
   unterlaufende Deadline). */
uint32_t fsched_compute_deadline(uint32_t target_presentation_time,
                                  uint32_t estimated_processing_ticks,
                                  uint32_t safety_margin_ticks);

/* NPSPEC-FRAME-IDLE-0001: echte Pruefung (kein Platzhalter) - idle ==1
   genau dann, wenn weder ausstehender Damage fuer target_id (Phase 5)
   noch ein noch nicht praesentierter Frame dieser Ausgabe existiert. */
int fsched_output_is_idle(uint32_t output_id, uint32_t target_id, int *out_idle);

#endif

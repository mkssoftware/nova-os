/* NovaOS C-Kernel - Nova Graphics Performance Architecture (049-GUI/
   011-Graphic_Engine/016-Graphics_Performance, Phase 12 - LETZTE Phase
   dieser Erweiterung des Graphics-Engine-Vorhabens). Acht NPSPECs dieses
   Ordners: ADAPTIVE/ARCH/CPU/GPU/LATENCY/MEMORY/METRICS/PROFILING-0001.

   Wie bei Phase 11 ausdruecklich vorgeschrieben (ADAPTIVE-0001 §4
   "Eine redundante Rendering- oder Ressourcenverwaltung DARF nicht
   entstehen", PROFILING-0001 §3 "Eine redundante Messinfrastruktur DARF
   nicht entstehen"): dieses Modul ist eine SCHLANKE, SUBSYSTEM-
   UEBERGREIFENDE Mess-/Aggregations-/Empfehlungsschicht, kein eigenes
   Rendering- oder Speicherverwaltungssystem.

   - **ARCH/METRICS-0001**: `gfxperf_begin()`/`gfxperf_end()` liefern
     die geforderte "einheitliche" Zeitmessung (Subsystem-Tag + Frame-ID
     zur Korrelation, #6/#7) auf Basis von `timer_get_ticks()`
     (Interrupt-Subsystem, bereits in Phase 7/8 verwendetes Muster) -
     EINE Zeitbasis fuer CPU-/Wall-Time, da dieser Kernel keine
     getrennte Hardware-Zykluszaehler-Infrastruktur fuer Wait Time
     besitzt (CPU-0001 §3 reduziert auf das, was ohne Thread-genaue
     CPU-Zeitbuchfuehrung ehrlich messbar ist - Wait Time explizit
     deferred, siehe unten).
   - **GPU-0001**: `gfxperf_gpu_metric_available()` liefert IMMER 0 -
     analog zu `matperf_gpu_available()` (bestehende Material Engine)
     und `gfxoptimize_vrs_available()` (Phase 11) - der laut Spec §15
     geforderte Normalfall ("Nicht verfuegbare Messwerte MUESSEN
     eindeutig gekennzeichnet werden"), kein Platzhalterwert.
   - **LATENCY-0001**: `gfxperf_frame_latency()` ist ein ECHTER Wrapper
     um `fsched_frame_info()` (Phase 8) - dessen `event_time`/
     `render_start`/`render_complete`/`presentation_time`-Felder liefern
     bereits exakt die von LATENCY-0001 §4/§8 geforderte Aufschluesselung
     (Warten bis Rendern/Render-Dauer/Rendern bis Praesentation) - keine
     neue Zeitstempel-Infrastruktur.
   - **MEMORY-0001**: ohne dynamische Heap-/GPU-Allokation (alle Tabellen
     dieses Kernels sind fest dimensioniert, siehe Phase 6/8's Memory-
     Debugging-Lektionen) reduziert sich diese Spec ehrlich auf einen
     reinen Architektur-Anschlusspunkt: `gfxperf_memory_report()`
     berechnet die Auslastung EINER vom Aufrufer gemeldeten festen
     Tabelle (used/capacity) - keine Rueckwirkung in fruehere Phasen
     (deren Tabellen bleiben unveraendert), nur ein bereitgestellter
     Berechnungsdienst.
   - **ADAPTIVE-0001**: `gfxperf_recommend_profile()` bildet eine
     gemessene mittlere Frame-Dauer MIT ECHTER HYSTERESE (§12, N
     aufeinanderfolgende abweichende Messungen noetig - gleiches Muster
     wie `matperf_quality_report_frame_ticks()`) auf eines der
     `GFXOPT_QUALITY_*`-Profile aus Phase 11 (`gfxoptimize.h`) ab - gibt
     nur eine EMPFEHLUNG zurueck (#13 "Wirkung MUSS ueberpruefbar sein"),
     wendet sie NICHT selbst an (das bleibt `gfxoptimize_set_quality_
     profile()`, Aufgabe des Aufrufers).
   - **PROFILING-0001**: `gfxperf_set_enabled()` deaktiviert Messung
     global (#17) - bei Deaktivierung liefert `gfxperf_begin()` den
     Sentinel-Span 0, `gfxperf_end(0, ...)` ist dann ein reiner No-Op
     (Dauer 0, keine Statistik-Aktualisierung).

   BEWUSST NICHT TEIL DIESER PHASE: echte GPU-Hardware-Zaehler (GPU-0001
   §3/§14 - keine GPU, siehe oben), Perzentile fuer Latenzverteilungen
   (METRICS-0001 §12, reine SOLL-Anforderung, keine Pflicht - diese Phase
   speichert nur Anzahl/Summe/Min/Max statt einer vollen Werteverteilung),
   Export von Profiling-Ergebnissen (PROFILING-0001 §13 SOLL, kein
   Dateisystem-/Serialisierungsziel definiert). */
#ifndef NOVA_GFXPERF_H
#define NOVA_GFXPERF_H

#include <stdint.h>

#define GFXPERF_MAX_SPANS 16u
#define GFXPERF_MAX_SUBSYSTEM_STATS 16u
#define GFXPERF_HYSTERESIS_COUNT 3u

#define GFXPERF_ERR_NOT_FOUND (-1)
#define GFXPERF_ERR_CAPACITY  (-2)

/* Subsystem-Tags fuer Korrelation (ARCH-0001 §6/METRICS-0001 §6). */
#define GFXPERF_SUBSYS_GENERIC    0u
#define GFXPERF_SUBSYS_RENDER     1u
#define GFXPERF_SUBSYS_SCENE      2u
#define GFXPERF_SUBSYS_COMPOSITOR 3u
#define GFXPERF_SUBSYS_DISPLAY    4u
#define GFXPERF_SUBSYS_MATERIAL   5u

void gfxperf_initialize(void);
int gfxperf_self_test(void); /* 0 = ok */

/* PROFILING-0001 §17: global deaktivierbar. */
int gfxperf_set_enabled(int enabled);
int gfxperf_enabled(void);

/* Startet eine Zeitmessung (timer_get_ticks(), Interrupt-Subsystem).
   *out_span_id == 0, wenn Messung deaktiviert ist (Sentinel, siehe
   Dateikopf) - IMMER 0 = Erfolg, GFXPERF_ERR_CAPACITY wenn die
   Span-Tabelle voll ist. */
int gfxperf_begin(uint32_t subsystem_tag, uint32_t frame_id, uint32_t *out_span_id);
/* span_id == 0 ist ein No-Op (*out_duration_ticks = 0). Aktualisiert bei
   einer echten Span die Subsystem-Statistik (Anzahl/Summe/Min/Max). 0 =
   Erfolg, GFXPERF_ERR_NOT_FOUND bei unbekannter span_id != 0. */
int gfxperf_end(uint32_t span_id, uint32_t *out_duration_ticks);

/* 0 = Erfolg (Werte gesetzt), GFXPERF_ERR_NOT_FOUND wenn fuer
   subsystem_tag noch keine abgeschlossene Messung existiert. */
int gfxperf_subsystem_stats(uint32_t subsystem_tag, uint32_t *out_count,
                             uint32_t *out_avg_ticks, uint32_t *out_min_ticks,
                             uint32_t *out_max_ticks);

/* GPU-0001: immer 0 in diesem Kernel (keine GPU-Hardware). */
int gfxperf_gpu_metric_available(void);

/* LATENCY-0001: echter Wrapper um fsched_frame_info() (Phase 8). Jede
   Differenz kann 0 sein, wenn der jeweilige Zeitstempel noch nicht
   erreicht wurde (z.B. vor render_complete) - das ist kein Fehler.
   0 = Erfolg, GFXPERF_ERR_NOT_FOUND wenn frame_id unbekannt ist. */
int gfxperf_frame_latency(uint32_t frame_id, uint32_t *out_wait_ticks,
                           uint32_t *out_render_ticks, uint32_t *out_present_ticks);

/* MEMORY-0001: reine Berechnung, keine Speicherung. *out_percent = used*100/capacity
   (0 wenn capacity == 0). 0 = immer Erfolg. */
int gfxperf_memory_report(uint32_t subsystem_tag, uint32_t used, uint32_t capacity,
                           uint32_t *out_percent);

/* ADAPTIVE-0001: liefert eine Qualitaetsprofil-EMPFEHLUNG (GFXOPT_QUALITY_*
   aus gfxoptimize.h, Phase 11) mit echter Hysterese (GFXPERF_HYSTERESIS_
   COUNT aufeinanderfolgende abweichende Berichte noetig) - wendet sie
   NICHT selbst an. avg_frame_ticks == 0 bedeutet "keine Messdaten" ->
   Empfehlung bleibt unveraendert. 0 = immer Erfolg. */
int gfxperf_recommend_profile(uint32_t avg_frame_ticks, uint32_t *out_profile);

#endif

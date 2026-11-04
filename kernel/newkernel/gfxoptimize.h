/* NovaOS C-Kernel - Nova Graphics Optimization Architecture (049-GUI/
   011-Graphic_Engine/015-Rendering_Optimization, Phase 11 des Graphics-
   Engine-Vorhabens). Zehn NPSPECs dieses Ordners: ARCH/BANDWIDTH/
   BATCHING/CULLING/OCCLUSION/POWER/PREDICTIVE/QUALITY/REUSE/VRS-0001.

   WIE BEI PHASE 10 DURCHGAENGIG VORGESCHRIEBEN (ARCH-0001 §3 "Redundante
   Rendering- und Ressourceninfrastrukturen DÜRFEN nicht entstehen",
   REUSE-0001 §2/§3 "Bestehende Cache-Systeme MÜSSEN verwendet werden...
   KEINE redundante zentrale Ergebnisspeicherung"): dieses Modul ist eine
   DUENNE ORCHESTRIERUNGSSCHICHT ueber bereits bestehende Subsysteme,
   keine neue Speicher-/Cache-Infrastruktur.

   - **CULLING/OCCLUSION-0001**: `gfxoptimize_should_render()` kombiniert
     ECHT drei bestehende Subsysteme: Occlusion-Zustand aus
     `layer_info()` (Phase 6), ausstehenden Damage aus
     `damage_pending_count()` (Phase 5) und einen vorhandenen gueltigen
     Render-Cache-Treffer aus `render_cache_find()`/`render_cache_get()`
     (Phase 3) - kein eigener Sichtbarkeits- oder Versionszustand.
     Verdeckte Damage-Informationen werden NICHT angetastet (OCCLUSION-
     0001 §8) - genau wie bereits in Phase 6/7 (`compositor_compose()`
     resolved Damage okkludierter Layer bewusst nie).
   - **REUSE-0001**: `gfxoptimize_try_reuse()` ist ein duenner,
     introspektiver Wrapper um `render_cache_find()`/`_get()` (Phase 3) -
     die eigentliche Wiederverwendungsentscheidung trifft bereits
     `render_execute_retained()` selbst.
   - **POWER-0001 §3 "Unnoetige Frame-Erzeugung MUSS vermieden werden"**
     ist bereits VOLLSTAENDIG durch `fsched_output_is_idle()` (Phase 8)
     erfuellt - `gfxoptimize_power_should_render_frame()` legt diese
     bestehende Garantie nur unter einem fuer diese Phase passenden Namen
     frei, implementiert sie nicht neu.
   - **QUALITY-0001**: `gfxoptimize_quality_profile_t` (sieben Profile)
     ist eine REINE Abbildung auf die bestehenden, bereits vorhandenen
     Qualitaets-/Energiestufen (`matperf.h`'s drei Qualitaetsstufen,
     `framesched.h`'s drei Power-Modi) - keine neue Qualitaetslogik,
     `gfxoptimize_apply_profile_to_output()` ruft echt
     `fsched_output_set_power_mode()` (Phase 8) auf. matperf.c's EIGENE
     automatische Hysterese-Anpassung (`matperf_quality_report_frame_
     ticks()`) bleibt UNVERAENDERT und unabhaengig bestehen (SOLL-Anfor-
     derung #5, keine MUSS-Pflicht zur Uebersteuerung) - diese Phase
     fuegt keinen Setter in matperf.c hinzu, um das bestehende, bereits
     boot-verifizierte Hysterese-Verhalten nicht zu veraendern.

   BEWUSST NICHT TEIL DIESER PHASE (alle vier haben als Pflicht-
   Abhaengigkeit mindestens eines der BLOCKED GPU-Module, 009-012, und
   damit keine ehrliche Software-Entsprechung - eine Attrappe wuerde
   gegen genau das in ARCH-0001 §3 ausgesprochene Verbot redundanter
   Infrastruktur verstossen):
   - **BATCHING-0001**: Draw Call Batching/Instanced Rendering/Multi-Draw
     sind reine GPU-Kommando-Konzepte (braucht Nova GPU Command
     Optimization, BLOCKED 009).
   - **BANDWIDTH-0001**: Texturkompression/asynchrone GPU-Transfers
     setzen GPU-Speicher voraus (Nova GPU Resource Management, BLOCKED
     011).
   - **PREDICTIVE-0001**: Shader-/Pipeline-Vorbereitung setzt Nova Shader
     Cache voraus (BLOCKED 012).
   - **VRS-0001**: Variable Rate Shading ist reine GPU-Hardware-Funktion.
     `gfxoptimize_vrs_available()` liefert deshalb IMMER 0 - analog zu
     `matperf_gpu_available()` (Phase 9/bestehende Material Engine), das
     ist der laut Spec §18 geforderte Normalfall ("Ohne Hardware-VRS MUSS
     normales Rendering moeglich bleiben"), kein Platzhalter. */
#ifndef NOVA_GFXOPTIMIZE_H
#define NOVA_GFXOPTIMIZE_H

#include <stdint.h>
#include "renderengine.h"

#define GFXOPTIMIZE_ERR_NOT_FOUND (-1)
#define GFXOPTIMIZE_ERR_INVALID   (-2)

/* NPSPEC-OPTIMIZE-QUALITY-0001 §3: alle sieben geforderten Profile. */
#define GFXOPT_QUALITY_ULTRA        0u
#define GFXOPT_QUALITY_HIGH         1u
#define GFXOPT_QUALITY_BALANCED     2u
#define GFXOPT_QUALITY_PERFORMANCE  3u
#define GFXOPT_QUALITY_POWER_SAVING 4u
#define GFXOPT_QUALITY_MINIMAL      5u
#define GFXOPT_QUALITY_CUSTOM       6u
#define GFXOPT_QUALITY_COUNT        7u

void gfxoptimize_initialize(void);
int gfxoptimize_self_test(void); /* 0 = ok */

/* CULLING-0001/OCCLUSION-0001: 0 = Node kann sicher uebersprungen werden
   (vollstaendig okkludiert UND kein ausstehender Damage UND gueltiger
   Render-Cache-Treffer vorhanden), 1 = Rendern (weiterhin) erforderlich.
   layer_id == 0 bedeutet "keine Layer-Zuordnung" (Occlusion-Pruefung
   entfaellt dann, konservativ wird IMMER 1 geliefert - #19 "unsichere
   Culling-Entscheidungen MÜSSEN konservativ behandelt werden"). 0 =
   Erfolg der Pruefung selbst (nicht zu verwechseln mit dem Cull-
   Ergebnis in *out_should_render), GFXOPTIMIZE_ERR_NOT_FOUND wenn
   node_id nicht existiert. */
int gfxoptimize_should_render(uint32_t node_id, uint32_t layer_id, uint32_t region_id,
                               int *out_should_render);

/* REUSE-0001: duenner Wrapper um render_cache_find()/_get() (Phase 3).
   0 = gueltiger Treffer (*out_value gesetzt), GFXOPTIMIZE_ERR_NOT_FOUND
   wenn kein (noch gueltiger) Eintrag existiert - sichere Neuberechnung
   bleibt dann Aufgabe des Aufrufers (render_execute_retained(), #19). */
int gfxoptimize_try_reuse(uint32_t node_id, uint32_t region_id, uint32_t *out_value);

/* QUALITY-0001: setzt/liest das aktuell gewaehlte Profil (rein lokaler
   Zustand dieser Phase, siehe Dateikopf - keine Rueckwirkung auf
   matperf.c's eigene automatische Hysterese). */
int gfxoptimize_set_quality_profile(uint32_t profile);
uint32_t gfxoptimize_quality_profile(void);

/* QUALITY-0001/POWER-0001: wendet den zum aktuellen Profil gehoerenden
   Power-Modus echt auf eine Frame-Scheduler-Ausgabe an
   (fsched_output_set_power_mode(), Phase 8). 0 = Erfolg,
   GFXOPTIMIZE_ERR_NOT_FOUND wenn output_id beim Frame Scheduler
   unbekannt ist. */
int gfxoptimize_apply_profile_to_output(uint32_t output_id);

/* POWER-0001 §3: duenner, benannter Wrapper um fsched_output_is_idle()
   (Phase 8) - siehe Dateikopf, keine neue Logik. 0 = Erfolg
   (*out_should_render gesetzt), GFXOPTIMIZE_ERR_NOT_FOUND wenn
   output_id unbekannt ist. */
int gfxoptimize_power_should_render_frame(uint32_t output_id, uint32_t target_id,
                                           int *out_should_render);

/* VRS-0001 §2/§18: immer 0 in diesem Kernel (keine GPU-Hardware). */
int gfxoptimize_vrs_available(void);

#endif

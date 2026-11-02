/* NovaOS C-Kernel - Nova Material Engine: Performance. Port aus
   docs/NPSPEC/sysarchitecture/049-GUI/010-Material_Engine/006-Performance/
   NPSPEC-MATERIAL-{CACHE,DAMAGE,FALLBACK,GPU,PERF,QUALITY}-0001.md.

   BEWUSSTE SCOPE-REDUKTION:
   - CACHE-0001 nennt sieben Cache-Typen (Definition/Effect/Backdrop/Mask/
     Layer/Composition/Pipeline Cache). Ohne Textur-/Pixelpuffer-
     Ressourcenverwaltung (kein GPU, keine persistenten Offscreen-Puffer
     in diesem Kernel) ist nur der "Definition Cache" (aufgeloeste
     Materialeigenschaften) sinnvoll umsetzbar - realisiert ueber
     material.h's `material_instance_generation()`: ein gueltiger
     Cache-Eintrag wird genau dann invalidiert, wenn sich die Generation
     der Instanz geaendert hat (CACHE-0001 Abschnitt "Aenderungen
     relevanter Eingaben muessen betroffene Eintraege invalidieren").
     Die anderen sechs Cache-Typen wuerden Pixeldaten cachen, was einen
     Offscreen-Speicherpool voraussetzt, der hier nicht existiert.
   - DAMAGE-0001 verlangt Integration mit "dem bestehenden Damage
     Tracking des Compositors" - compositor.h exponiert aber keine API,
     um von aussen Damage-Regionen einzuspeisen (sein Damage-Tracking ist
     rein intern). Dieses Modul fuehrt deshalb eine EIGENE, kleine
     Damage-Region-Liste fuer die Material Engine (gleiches Muster wie
     compositor.c: fester, kleiner Speicher, ein Merge-Versuch pro
     add()), die ein spaeterer Compositor-Integrationsschritt abfragen
     koennte - eine echte Verdrahtung in compositor.c selbst ist bewusst
     NICHT Teil dieser Aenderung (siehe matintegrate.h).
   - GPU-0001 ist komplett scope-reduziert: dieser Kernel hat keinen
     GPU-Treiber. `matperf_gpu_available()` liefert deshalb IMMER 0 -
     das ist der laut Spec geforderte Fallback-Fall, kein Platzhalter.
   - FALLBACK-0001/QUALITY-0001 sind echt umgesetzt (reine
     Parameter-Reduktion auf vorhandenen matfx.c-Eigenschaften). */
#ifndef NOVA_MATPERF_H
#define NOVA_MATPERF_H

#include <stdint.h>
#include "material.h"

void matperf_initialize(void);
int matperf_self_test(void); /* 0 = ok */

/* --- Cache (CACHE-0001) --- */
#define MATPERF_MAX_CACHE_ENTRIES 64u

/* Liefert die resolvten Eigenschaften einer Instanz - entweder aus dem
   Cache (wenn die Generation seit dem letzten Aufruf unveraendert ist)
   oder frisch berechnet (material_instance_resolve()) samt Aktualisierung
   des Cache-Eintrags. *out_was_cached wird auf 1/0 gesetzt (Diagnose/
   Selbsttest), darf NULL sein. 0 = Erfolg. */
int matperf_cache_resolve(uint32_t instance_id, material_properties_t *out,
                            int *out_was_cached);
void matperf_cache_invalidate(uint32_t instance_id);
void matperf_cache_clear(void);

/* --- Damage Tracking (DAMAGE-0001) --- */
#define MATPERF_MAX_DAMAGE_REGIONS 16u

typedef struct {
    uint32_t x, y, w, h;
} matperf_rect_t;

/* Leere Regionen (w==0 || h==0) werden ignoriert (DAMAGE-0001 "leere
   Regionen duerfen keine Rendering-Operationen ausloesen"). Versucht
   einen Merge mit einer bestehenden, ueberlappenden/angrenzenden Region
   (ein Versuch pro Aufruf); bei Tabellenueberlauf wird konservativ auf
   eine einzige Vollbild-Ersatzregion eskaliert. */
void matperf_damage_mark(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
uint32_t matperf_damage_region_count(void);
int matperf_damage_get(uint32_t index, matperf_rect_t *out);
void matperf_damage_clear(void);

/* --- Fallback-Stufen (FALLBACK-0001) --- */
#define MATPERF_FALLBACK_NATIVE      0u
#define MATPERF_FALLBACK_SIMPLIFIED  1u
#define MATPERF_FALLBACK_ALTERNATIVE 2u
#define MATPERF_FALLBACK_SOLID       3u

/* Wendet die geforderte Ersatzstufe auf bereits resolvte Eigenschaften an
   (FALLBACK-0001 Abschnitt 5 Tabelle: Acrylic->reduzierter Blur->Solid,
   Liquid->ohne Brechung/Verformung->Solid, Frosted->vereinfachter
   Blur->Solid, Crystal->reduzierte Reflexion/Beleuchtung->Solid,
   Solid->vereinfachte Farbe). type ist der MATERIAL_TYPE_* der
   zugrunde liegenden Definition. */
void matperf_fallback_apply(uint32_t level, uint32_t type, material_properties_t *props);

/* --- GPU-Beschleunigung (GPU-0001) --- */
typedef struct {
    uint32_t shader_support;      /* 0 = keine Shader-Verarbeitung verfuegbar */
    uint32_t compute_support;     /* 0 = keine Compute-Funktionen */
    uint32_t max_texture_size;    /* 0 = keine GPU-Texturen verfuegbar */
    uint32_t gpu_memory_bytes;    /* 0 = kein dedizierter GPU-Speicher */
} matperf_gpu_caps_t;

int matperf_gpu_available(void); /* immer 0 in diesem Kernel - siehe Scope-Note oben */
void matperf_gpu_query_caps(matperf_gpu_caps_t *out);

/* --- Rendering-Optimierung (PERF-0001) --- */
/* 0 = Material ist vollstaendig unsichtbar (Opacity 0) und darf
   uebersprungen werden (PERF-0001/ARCH-0001 "unsichtbare Materialien
   nicht rendern"), 1 = muss verarbeitet werden. */
int matperf_should_render(const material_properties_t *props);

/* --- Adaptive Qualitaet (QUALITY-0001) --- */
#define MATPERF_QUALITY_LOW    0u
#define MATPERF_QUALITY_MEDIUM 1u
#define MATPERF_QUALITY_HIGH   2u

void matperf_quality_initialize(void);
uint32_t matperf_quality_level(void);
/* Meldet die fuer einen Frame benoetigte Zeit (Timer-Ticks, siehe
   interrupt.h timer_get_ticks) - passt die globale Qualitaetsstufe mit
   Hysterese an (QUALITY-0001 "kurzzeitige Lastspitzen duerfen keine
   unnoetigen Wechsel ausloesen": erst nach MATPERF_QUALITY_HYSTERESIS
   aufeinanderfolgenden Berichten ueber/unter dem jeweiligen Schwellwert
   wird tatsaechlich gewechselt). */
void matperf_quality_report_frame_ticks(uint32_t ticks);
/* Reduziert Blur/Reflection/Highlight/Distortion gemaess der aktuellen
   globalen Qualitaetsstufe (QUALITY-0001 Abschnitt 4, reduziert auf die
   in matfx.c tatsaechlich vorhandenen Stellschrauben); HIGH veraendert
   nichts. */
void matperf_quality_apply(material_properties_t *props);

/* QUALITY-0001 Abschnitt 6: nicht alle Instanzen haben dieselbe
   Prioritaet - eine als hochprioritaer markierte Instanz rendert immer
   in HIGH, unabhaengig von der globalen Stufe. */
int matperf_quality_set_instance_priority(uint32_t instance_id, int high_priority);
uint32_t matperf_quality_level_for_instance(uint32_t instance_id);

#endif

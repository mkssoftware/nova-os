/* NovaOS C-Kernel - Nova Incremental Liquid Glass Rendering (049-GUI/
   011-Graphic_Engine/014-Incremental_Liquid_Glass, Phase 10 des Graphics-
   Engine-Vorhabens). Neun NPSPECs dieses Ordners: ADAPTIVE/BACKDROP/
   BLURCACHE/DEPENDENCY/INCREMENTAL/INVALIDATION/OPTIMIZE/
   REFLECTIONCACHE/REFRACTIONCACHE-0001.

   WICHTIGSTE, IN JEDER EINZELNEN SPEC WIEDERHOLTE ARCHITEKTUR-VORGABE:
   DEPENDENCY-0001 §2 "Eine redundante Dependency-Graph-Infrastruktur DARF
   nicht entstehen", OPTIMIZE-0001 §3 "Redundante Rendering-Infrastrukturen
   DÜRFEN nicht entstehen" - dieser Ordner ist durchgaengig als DUENNE
   LIQUID-GLASS-SPEZIALISIERUNG der bereits bestehenden Infrastruktur
   konzipiert, NICHT als eigenstaendiges Subsystem:

   - **DEPENDENCY-0001**: `liquidglass_set_backdrop()` ruft
     `materender_add_dependency()` (Phase 9, letztlich `gfx_dependency_add`
     Phase 1) auf - KEIN zweiter Graph.
   - **BLURCACHE/REFLECTIONCACHE/REFRACTIONCACHE-0001**: alle drei
     verlangen explizit "Nova Material Effect Caching MUSS als zentrale
     Cache-Infrastruktur verwendet werden" - hier umgesetzt durch
     `scene_cache_put()`/`scene_cache_get()` (Phase 2, lazy
     Versions-Abgleich), EIN Eintrag PRO (Node, Effektstufe)-Paar statt
     drei paralleler Tabellen. "Mapping Cache" vs. "Result Cache"
     (REFLECTIONCACHE §3/REFRACTIONCACHE §3) sind hier nicht getrennt
     benoetigt: diese Phase hat keine echten GPU-Texturen, nur einen
     einzelnen deterministischen Platzhalterwert pro Stufe.
   - **INCREMENTAL/OPTIMIZE-0001**: `liquidglass_evaluate_all()` iteriert
     NUR die fuer LIQUID tatsaechlich geltenden Pipeline-Stufen
     (`materender_pipeline_steps()`, Phase 9) und nutzt fuer jede einen
     Cache-Treffer, wenn moeglich - das IST bereits die geforderte
     "nur erforderliche Passes"-Elimination, kein separater Optimizer-
     Durchlauf noetig.
   - **BACKDROP-0001**: Blur- und Refraction-Stufen werden ZUSAETZLICH
     gegen die `state_version` des gebundenen Backdrop-Nodes geprueft
     (Phase 2) - Reflection/Lighting haengen nur vom eigenen Node ab. Bei
     fehlendem/ungueltigem Backdrop wird konservativ invalidiert
     (DEPENDENCY-0001 §15).
   - **ADAPTIVE-0001**: `matperf_quality_level()` (bestehende Material
     Engine) fliesst direkt in den Platzhalterwert ein - echte, aber
     minimale Integration, keine neue Qualitaetsverwaltung.

   BEWUSST NICHT TEIL DIESER PHASE: echte GPU-Textur-Caches fuer Blur-/
   Reflection-/Refraction-Maps (BLOCKED, 012-Shader_System/011-GPU_
   Resource_Management - setzen GPU-Speicher voraus), Zero-Copy Backdrop
   Sharing (BACKDROP-0001 §12 - derselbe Grund). */
#ifndef NOVA_LIQUIDGLASS_H
#define NOVA_LIQUIDGLASS_H

#include <stdint.h>
#include "renderengine.h"

#define LIQUIDGLASS_MAX_SLOTS 32u

#define LIQUIDGLASS_ERR_NOT_FOUND (-1)
#define LIQUIDGLASS_ERR_CAPACITY  (-2)
#define LIQUIDGLASS_ERR_INVALID   (-3)

/* Dieselben Stufen-Konstanten wie materender.h (MATERENDER_STAGE_*) -
   keine eigene, parallele Enum. */
#define LIQUIDGLASS_STAGE_BASE       0u
#define LIQUIDGLASS_STAGE_BLUR       1u
#define LIQUIDGLASS_STAGE_REFRACTION 2u
#define LIQUIDGLASS_STAGE_REFLECTION 3u
#define LIQUIDGLASS_STAGE_LIGHTING   4u
#define LIQUIDGLASS_STAGE_COUNT      5u

void liquidglass_initialize(void);
int liquidglass_self_test(void); /* 0 = ok */

/* Setzt/ersetzt den Backdrop-Node fuer node_id (BACKDROP-0001) UND fuegt
   gleichzeitig die Materialabhaengigkeit hinzu (DEPENDENCY-0001, ueber
   Phase 9/materender - keine eigene Kante). node_id MUSS an eine LIQUID-
   Materialinstanz gebunden sein (materender_bind(), Phase 9). 0 = Erfolg,
   LIQUIDGLASS_ERR_INVALID wenn node_id nicht an LIQUID gebunden ist. */
int liquidglass_set_backdrop(uint32_t node_id, uint32_t backdrop_node_id);

/* Wertet genau eine Effektstufe aus. *out_cache_hit == 1 bedeutet: sowohl
   der eigene Node (scene_cache_get(), Phase 2) als auch - fuer BLUR/
   REFRACTION - der gebundene Backdrop-Node sind seit dem letzten Aufruf
   unveraendert. 0 = Erfolg, LIQUIDGLASS_ERR_NOT_FOUND wenn node_id nicht
   existiert oder nicht an LIQUID gebunden ist, LIQUIDGLASS_ERR_INVALID
   wenn stage keine gueltige LIQUID-Pipeline-Stufe ist. */
int liquidglass_evaluate_stage(uint32_t node_id, uint32_t stage, render_rect_t region,
                                uint32_t *out_value, int *out_cache_hit);

/* Wertet ALLE fuer LIQUID geltenden Pipeline-Stufen (materender_
   pipeline_steps(), Phase 9) in Reihenfolge aus - die Umsetzung von
   INCREMENTAL-0001/OPTIMIZE-0001 "nur erforderliche Passes". Schreibt
   bis zu LIQUIDGLASS_STAGE_COUNT Ergebnisse/Cache-Treffer in die
   Ausgabe-Arrays, *out_count = Anzahl tatsaechlich ausgewerteter Stufen.
   0 = Erfolg. */
int liquidglass_evaluate_all(uint32_t node_id, render_rect_t region,
                              uint32_t *out_values, int *out_cache_hits,
                              uint32_t *out_count);

#endif

/* NovaOS C-Kernel - Nova Material-Aware Rendering (049-GUI/011-Graphic_Engine/
   013-Material-Aware_Rendering, Phase 9 des Graphics-Engine-Vorhabens).
   Acht NPSPECs dieses Ordners: RENDER/DEPENDENCY/CACHE/INCREMENTAL/PIPELINE/
   QUALITY/FALLBACK/RESOURCE-0001.

   WICHTIGSTE ARCHITEKTUR-ENTSCHEIDUNG: dies ist KEINE Neuimplementierung
   von Caching/Fallback/Qualitaet - diese Funktionen existieren bereits
   vollstaendig in der AELTEREN, unabhaengigen Nova Material Engine
   (material.c/matperf.c/matext.c, Port aus docs/.../010-Material_Engine/),
   die lange vor diesem Graphics-Engine-Vorhaben entstand. `materender.c`
   ist die ECHTE INTEGRATIONSSCHICHT, die diese bestehende Material Engine
   mit der seit Phase 1 aufgebauten Graphics-Engine-Kette (Scene Graph/
   Graphics State Dependency Graph/Render Engine/Damage Tracking) verbindet:

   - CACHE-0001: wird durch `matperf_cache_resolve()` (bestehend) ERFUELLT,
     kombiniert hier zusaetzlich mit `damage_pending_count()` (Phase 5) -
     ein Node gilt nur dann als Cache Hit, wenn BEIDE Seiten (Material-
     Generation UND Render-Damage) unveraendert sind. Keine dritte,
     parallele Cache-Tabelle.
   - FALLBACK-0001/QUALITY-0001: werden durch `matperf_fallback_apply()`/
     `matperf_quality_level_for_instance()`/`matperf_quality_apply()`
     (bestehend) ERFUELLT - diese Phase ruft sie nur auf.
   - DEPENDENCY-0001: wird durch `scene_node_add_dependency()` (Phase 2,
     letztlich `gfx_dependency_add()` aus Phase 1) mit dep_type
     `GFX_DEP_MATERIAL` (bereits in gfxstate.h definiert) ERFUELLT - kein
     neuer, paralleler Abhaengigkeitsgraph.
   - INCREMENTAL-0001: wird durch `damage_pending_count()`/
     `damage_resolve_target()` (Phase 5) ERFUELLT.
   - RESOURCE-0001: physische GPU-Speicherverwaltung ist laut Spec
     Abschnitt 2 explizit Aufgabe von "Nova GPU Resource Management"
     (BLOCKED, 011-GPU_Resource_Management) - die nicht-GPU-Ressourcen-
     Lebenszeit (an die Node-Bindung gekoppelt) wird durch die Bindetabelle
     dieser Phase abgedeckt, keine weitere Ressourcenverwaltung noetig.

   GENUINE NEUE BAUSTEINE dieser Phase: die Node<->Instanz-Bindetabelle
   (`materender_bind()`), und die deterministische Pipeline-Stufenauswahl
   (PIPELINE-0001) pro Materialtyp - eine feste, materialtypabhaengige
   Stufenliste (keine generische GPU-Pipeline-Graph-Konstruktion, das
   waere GPU Command Processing/Shader System, beide BLOCKED), sowie
   `materender_evaluate()`, das ALLE obigen Bausteine tatsaechlich
   end-to-end verkettet (inkl. eines echten Aufrufs von
   `render_execute_retained()`, Phase 3).

   BEWUSST NICHT TEIL DIESER PHASE: echte GPU-Shader-Ausfuehrung pro
   Pipeline-Stufe (BLOCKED, 012-Shader_System - dreht sich komplett um
   GPU-Backend-Kompilierung, keine Software-Entsprechung ohne GPU-Treiber),
   Zero-Copy-Ressourcen-Sharing (RESOURCE-0001 Abschnitt 7 - setzt
   gemeinsamen GPU-Speicher voraus). */
#ifndef NOVA_MATERENDER_H
#define NOVA_MATERENDER_H

#include <stdint.h>
#include "renderengine.h"

#define MATERENDER_MAX_BINDINGS 32u

#define MATERENDER_ERR_NOT_FOUND (-1)
#define MATERENDER_ERR_CAPACITY  (-2)
#define MATERENDER_ERR_INVALID   (-3)

#define MATERENDER_UNBOUND 0xFFFFFFFFu

/* NPSPEC-MATERIAL-PIPELINE-0001 §2: deklarative, kombinierbare
   Verarbeitungsschritte. Feste, deterministische Reihenfolge - kein
   generischer Graph (das waere GPU-Pipeline-Konstruktion). */
#define MATERENDER_STAGE_BASE       0u
#define MATERENDER_STAGE_BLUR       1u
#define MATERENDER_STAGE_REFRACTION 2u
#define MATERENDER_STAGE_REFLECTION 3u
#define MATERENDER_STAGE_LIGHTING   4u
#define MATERENDER_STAGE_COUNT      5u

void materender_initialize(void);
int materender_self_test(void); /* 0 = ok */

/* Bindet einen Scene Node (Phase 2) an eine Materialinstanz (bestehende
   Material Engine) - Grundlage fuer alle weiteren Funktionen dieser
   Phase. 0 = Erfolg, MATERENDER_ERR_CAPACITY wenn die Bindetabelle voll
   ist, MATERENDER_ERR_INVALID wenn die Instanz nicht existiert. Erneutes
   Binden desselben Nodes ersetzt die vorherige Bindung. */
int materender_bind(uint32_t node_id, uint32_t instance_id);
int materender_unbind(uint32_t node_id);
/* MATERENDER_UNBOUND wenn der Node nicht gebunden ist. */
uint32_t materender_bound_instance(uint32_t node_id);

/* Duenner Wrapper um scene_node_add_dependency() mit dep_type
   GFX_DEP_MATERIAL (DEPENDENCY-0001: z.B. "Backdrop-Inhalt hinter einer
   Liquid-Glass-Flaeche aendert sich" -> Glas-Node haengt vom Inhalts-Node
   ab). 0 = Erfolg, Fehlercodes von scene_node_add_dependency() direkt
   durchgereicht. */
int materender_add_dependency(uint32_t node_id, uint32_t depends_on_node_id,
                               uint32_t *out_edge_id);

/* Liefert die feste, materialtypabhaengige Stufenliste (PIPELINE-0001
   §5 "nur tatsaechlich erforderliche Materialeffekte"). 0 = Erfolg,
   MATERENDER_ERR_INVALID bei unbekanntem Typ (liefert dann eine
   einelementige BASE-Stufe als sicheren Fallback, analog
   material_fallback_id()). */
int materender_pipeline_steps(uint32_t material_type, uint32_t *out_steps,
                               uint32_t max_steps, uint32_t *out_count);

/* Herzstueck: verkettet Material Engine (Cache/Qualitaet/Fallback,
   bestehend) mit der Graphics-Engine-Kette (Damage Tracking Phase 5,
   Render Engine Phase 3) fuer einen gebundenen Node. *out_cache_hit == 1
   bedeutet: weder Material-Generation noch Render-Damage haben sich seit
   der letzten Auswertung geaendert. *out_fallback_level ist einer der
   MATPERF_FALLBACK_*-Werte aus matperf.h. 0 = Erfolg,
   MATERENDER_ERR_NOT_FOUND wenn der Node nicht gebunden ist oder die
   Instanz nicht mehr existiert. Loest nach erfolgreicher Verarbeitung
   ausstehenden Damage fuer node_id auf (damage_resolve_target(), Phase
   5) - Material-Unsichtbarkeit (matperf_should_render()==0) loest Damage
   ebenfalls auf (kein Bedarf, es erneut zu versuchen), liefert aber
   *out_result_value==0. */
int materender_evaluate(uint32_t node_id, render_rect_t region,
                         uint32_t *out_result_value, int *out_cache_hit,
                         uint32_t *out_fallback_level);

#endif

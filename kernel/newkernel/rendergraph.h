/* NovaOS C-Kernel - Nova Render Graph (049-GUI/011-Graphic_Engine/
   004-Render_Graph, Phase 4 des Graphics-Engine-Vorhabens). Zehn NPSPECs
   dieses Ordners: ARCH/PASS/DEPENDENCY/RESOURCE/SCHEDULE/BARRIER/MERGE/
   ALIAS/OPTIMIZE/PARALLEL-0001.

   WICHTIGE ARCHITEKTUR-ENTSCHEIDUNG: Nova Render Pass Dependencies
   (DEPENDENCY-0001) ist laut Spec ein EIGENSTAENDIGES Abhaengigkeitssystem
   (Pass-zu-Pass, aus RAW/WAR/WAR-Ressourcenkonflikten abgeleitet) - bewusst
   NICHT derselbe Graph wie Nova Graphics/Scene Dependency Tracking aus
   Phase 1/2 (die modellieren Element-/Node-Abhaengigkeiten, nicht Pass-
   Ausfuehrungsreihenfolgen). Dieses Modul fuehrt daher eine EIGENE kleine
   Pass-Dependency-Kantentabelle, nicht `gfx_dependency_add()`.

   Echte Wiederverwendung findet an einer anderen, konkreteren Stelle statt:
   `rg_execute()` ruft fuer jeden Pass mit zugeordnetem Scene Node
   `render_execute_retained()` aus Phase 3 auf - der Render Graph PLANT die
   Ausfuehrung (Reihenfolge, Culling), die tatsaechliche "Rendering"-Arbeit
   (Platzhalter-Berechnung + Cache) bleibt vollstaendig bei Phase 3.

   Da dieser Kernel keinen GPU-Treiber hat, bleibt der Scope auf das
   backend-unabhaengig sinnvoll Implementierbare begrenzt: Pass-Deklaration,
   Ressourcenmodell, automatische RAW/WAR/WAW-Abhaengigkeitsableitung,
   Dead-Pass-Elimination (Culling) und topologische Ausfuehrungsplanung
   (echtes Kahn-Algorithmus, kein blosses ID-Sortieren).

   Bewusst NICHT Teil dieser Phase (reine GPU-Backend-/Multi-Queue-Konzepte
   ohne sinnvolle Software-Entsprechung): Pass Merging (MERGE-0001 - fasst
   native GPU-Subpasses zusammen), Transient Resource Aliasing (ALIAS-0001 -
   gemeinsame physische GPU-Speichernutzung, dieser Kernel hat keinen GPU-
   Speicher zu aliasen), Resource Barriers (BARRIER-0001 - konkrete GPU-
   Synchronisationsprimitive), Parallel Execution (PARALLEL-0001 - Multi-
   Core/Multi-Queue-Ausfuehrung, dieser Kernel fuehrt die Pipeline ohnehin
   seriell aus). Diese vier gehoeren inhaltlich eng zu den BLOCKED GPU-
   Ordnern 009-011 (Nutzerentscheidung 2026-11-04). */
#ifndef NOVA_RENDERGRAPH_H
#define NOVA_RENDERGRAPH_H

#include <stdint.h>

#define RG_PASS_CAPACITY         16u
#define RG_RESOURCE_CAPACITY     16u
#define RG_PASS_MAX_RESOURCES    4u
#define RG_EDGE_CAPACITY         32u

#define RG_PASS_TYPE_GRAPHICS    1u
#define RG_PASS_TYPE_COMPUTE     2u
#define RG_PASS_TYPE_COPY        3u
#define RG_PASS_TYPE_RESOLVE     4u
#define RG_PASS_TYPE_CLEAR       5u
#define RG_PASS_TYPE_EFFECT      6u
#define RG_PASS_TYPE_COMPOSITION 7u

/* NPSPEC-RENDERGRAPH-PASS-0001 §5 Lebenszyklus. */
#define RG_PASS_DECLARED  1u
#define RG_PASS_SCHEDULED 2u
#define RG_PASS_COMPLETED 3u
#define RG_PASS_CULLED    4u

#define RG_RESOURCE_TRANSIENT  1u
#define RG_RESOURCE_PERSISTENT 2u
#define RG_RESOURCE_IMPORTED   3u

/* NPSPEC-RENDERGRAPH-DEPENDENCY-0001 §4 Hazard-Typen. */
#define RG_HAZARD_RAW 1u
#define RG_HAZARD_WAR 2u
#define RG_HAZARD_WAW 3u

#define RG_ERR_NOT_FOUND (-1)
#define RG_ERR_CAPACITY  (-2)
#define RG_ERR_CYCLE     (-3)
#define RG_ERR_INVALID   (-4)

typedef struct {
    uint32_t pass_id;
    uint32_t pass_type;
    uint32_t lifecycle_state;
    uint32_t node_id;
    uint32_t input_count;
    uint32_t output_count;
    uint32_t result_value;
    int required;
} rg_pass_info_t;

void rendergraph_initialize(void);
int rendergraph_self_test(void); /* 0 = ok */

int rg_resource_declare(uint32_t resource_class, uint32_t *out_resource_id);

/* node_id: 0 = reiner Graph-Pass ohne Scene-Node-Bindung (z.B. Effect/
   Composition-Passes, die nur Ressourcen verarbeiten), sonst Retained-
   Rendering-Ziel fuer rg_execute(). region_id wird 1:1 an
   render_execute_retained() durchgereicht. */
int rg_pass_declare(uint32_t pass_type, uint32_t node_id, uint32_t region_id,
                    uint32_t *out_pass_id);

int rg_pass_add_input(uint32_t pass_id, uint32_t resource_id);
int rg_pass_add_output(uint32_t pass_id, uint32_t resource_id);

/* Markiert das Ergebnis/den Seiteneffekt dieses Passes als von aussen
   benoetigt (#6 Dead Pass Elimination "externe Seiteneffekte muessen
   beruecksichtigt werden") - typischerweise der/die finalen Composition-
   Passes, deren Ausgabe an den (noch nicht existierenden) Compositor
   uebergeben wuerde. */
int rg_pass_mark_required(uint32_t pass_id);

/* Leitet RAW/WAR/WAW-Abhaengigkeiten aus den deklarierten Ressourcen-
   zugriffen ab (eigene Pass-Dependency-Kantentabelle, siehe Dateikopf),
   entfernt nicht benoetigte Passes (Culling, rueckwaerts ab den als
   required markierten Passes), und berechnet per echtem Kahn-Algorithmus
   eine gueltige topologische Ausfuehrungsreihenfolge der verbleibenden
   Passes. 0 = Erfolg (*out_order gefuellt, *out_count = Anzahl verbleibender
   Passes), RG_ERR_CYCLE bei (hier strukturell ausgeschlossenem, aber
   sicherheitshalber erkanntem) Zyklus. */
int rg_build(uint32_t *out_order, uint32_t max_out, uint32_t *out_count);

/* Fuehrt den zuletzt von rg_build() berechneten Plan aus: fuer jeden Pass
   mit node_id!=0 wird render_execute_retained() (Phase 3) aufgerufen -
   echte Wiederverwendung, keine eigene Rendering-Logik. Passes ohne
   node_id werden als reine Graph-Knoten ohne Seiteneffekt abgeschlossen.
   0 = Erfolg, RG_ERR_INVALID wenn rg_build() noch nie erfolgreich lief. */
int rg_execute(uint32_t damage_area, uint32_t total_area);

int rg_pass_info(uint32_t pass_id, rg_pass_info_t *out_info);

/* Liefert den Hazard-Typ der (falls vorhandenen) Kante pass_a->pass_b auf
   resource_id, wie von rg_build() zuletzt ermittelt. RG_ERR_NOT_FOUND wenn
   keine solche Kante besteht. */
int rg_hazard_between(uint32_t pass_a, uint32_t pass_b, uint32_t resource_id,
                      uint32_t *out_hazard_type);

#endif

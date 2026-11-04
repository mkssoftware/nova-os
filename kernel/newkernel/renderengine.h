/* NovaOS C-Kernel - Nova Render Engine (049-GUI/011-Graphic_Engine/
   003-Rendering_Engine, Phase 3 des Graphics-Engine-Vorhabens). ECHTER
   AUFBAU AUF Phase 2 (scenegraph.h) UND Phase 1 (gfxstate.h): Retained
   Rendering liest den veroeffentlichten Node-Zustand direkt per
   scene_node_info() und nutzt gfx_adaptive_select_strategy() (Phase 1,
   NICHT neu erfunden) fuer die Partial/Full-Entscheidung.

   Zwoelf NPSPECs dieses Ordners: ARCH/PIPELINE/RETAINED/IMMEDIATE/PARTIAL/
   INCREMENTAL/REGION/SCISSOR/CACHE/REUSE/COST/ADAPTIVE-0001.

   **Render Result Cache ist eine EIGENE Tabelle, getrennt von Scene Cache
   (Phase 2)** - RENDER-CACHE-0001 §15 "Abgrenzung" verlangt das explizit
   ("Scene Cache: abgeleitete logische Szeneninformationen" vs. "Render
   Result Cache: berechnete grafische Darstellungsergebnisse"). Beide
   nutzen dasselbe lazy Versions-Abgleich-Muster gegen den Owner-Node (wie
   Phase 2), aber als zwei unabhaengige Tabellen mit unabhaengigem
   Lebenszyklus (ein Render-Cache-Verlust darf den Scene-Zustand nicht
   beruehren, #Scene-unabhaengige Freigabe).

   Da dieser Kernel keinen GPU-Treiber hat (Nutzerentscheidung 2026-11-04,
   siehe [[project_gui_graphics_engine]]), ist "result_value" ein
   generischer Platzhalterwert (eine deterministische Pruefsumme aus
   Node/Version/Region/Strategie) statt eines echten Pixelbuffers - echte
   Rasterisierung ist Aufgabe des bereits bestehenden, unabhaengigen
   `render2d.c` und bleibt hier bewusst unangetastet. Diese Schicht bildet
   die ARCHITEKTUR (Cache-Gueltigkeit, Strategieauswahl, Kostenschaetzung,
   Scissor-Clamping), nicht die tatsaechliche Pixelerzeugung.

   Bewusst NICHT Teil dieser Phase: Render Command Reuse (REUSE-0001 - reine
   GPU-Command-Buffer-Konzepte, gehoeren zum BLOCKED 009-GPU_Command_
   Processing), echte Damage-Region-Ermittlung (eigene, spaetere Phase
   005-Damage_Tracking - damage_area/total_area werden hier bewusst als
   Parameter vom Aufrufer entgegengenommen statt selbst berechnet),
   Render-Graph-Organisation (eigene, spaetere Phase 004-Render_Graph),
   echte GPU-Beschleunigung/Parallelitaet (kein GPU-Treiber vorhanden). */
#ifndef NOVA_RENDERENGINE_H
#define NOVA_RENDERENGINE_H

#include <stdint.h>

#define RENDER_CACHE_CAPACITY 32u

#define RENDER_ERR_NOT_FOUND (-1)
#define RENDER_ERR_CAPACITY  (-2)
#define RENDER_ERR_INVALID   (-3)

typedef struct {
    uint32_t cache_id;
    uint32_t owner_node_id;
    uint32_t region_id;
    uint32_t state_version;
    uint32_t result_value;
    uint32_t strategy_used; /* GFX_STRATEGY_PARTIAL/FULL aus gfxstate.h */
} render_cache_info_t;

typedef struct {
    uint32_t cpu_cost;
    uint32_t memory_cost;
    uint32_t latency_cost;
    uint32_t confidence; /* 0-100, niedriger bei fehlenden Eingaben (#4 Einflussfaktoren) */
} render_cost_t;

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
} render_rect_t;

void renderengine_initialize(void);
int renderengine_self_test(void); /* 0 = ok */

/* Retained Rendering: liest den aktuellen Zustand von node_id (ECHTER
   Scene-Graph-Zugriff, Phase 2) und region_id (0 = gesamte Oberflaeche,
   sonst frei vom Aufrufer vergeben - echte Region-Aufteilung ist Aufgabe
   einer spaeteren Phase). Bei Cache Hit (gleicher Owner-Node, gleiche
   state_version, gleiche Region) wird das gespeicherte Ergebnis OHNE
   erneute "Berechnung" zurueckgegeben (*out_cache_hit=1). Bei Cache Miss
   waehlt render_execute_retained() per gfx_adaptive_select_strategy()
   (Phase 1) PARTIAL oder FULL, "rendert" (Platzhalter) und speichert das
   Ergebnis fuer kuenftige Treffer. 0 = Erfolg, RENDER_ERR_NOT_FOUND wenn
   node_id nicht existiert. */
int render_execute_retained(uint32_t node_id, uint32_t region_id,
                             uint32_t damage_area, uint32_t total_area,
                             uint32_t *out_result_value, int *out_cache_hit,
                             uint32_t *out_strategy);

int render_cache_find(uint32_t node_id, uint32_t region_id, uint32_t *out_cache_id);
int render_cache_get(uint32_t cache_id, render_cache_info_t *out_info);

/* Immediate Rendering: VOELLIG zustandslos (#2 "Anwendungen muessen ihre
   grafischen Inhalte nicht dauerhaft im Scene Graph verwalten") - kein
   Scene Node, kein Cache-Eintrag, keine gespeicherte Historie. Liefert ein
   rein aus den Eingaben abgeleitetes, deterministisches Ergebnis. */
int render_execute_immediate(uint32_t target_id, uint32_t payload, uint32_t *out_result_value);

/* Deterministische Kostenschaetzung (#14 Deterministic Mode - hier der
   einzige Modus, da dieser Kernel keine Laufzeitmessungen hat, von denen
   kalibriert werden koennte). confidence sinkt auf 50, wenn total_area
   unbekannt (0) ist - #4 "nicht verfuegbare Informationen muessen durch
   konservative Annahmen behandelt werden". */
void render_cost_estimate(uint32_t change_count, uint32_t damage_area,
                           uint32_t total_area, render_cost_t *out_cost);

/* Begrenzt ein Scissor Rectangle auf gueltige Render-Target-Grenzen (#3/#14
   SCISSOR-0001). Ein Rect, das vollstaendig ausserhalb des Ziels liegt,
   wird zu einem leeren Rect (0,0,0,0) - "darf keine Pixel veraendern".
   0 = Erfolg, RENDER_ERR_INVALID wenn target_width/target_height==0. */
int render_scissor_clamp(uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                          uint32_t target_width, uint32_t target_height,
                          render_rect_t *out_rect);

#endif

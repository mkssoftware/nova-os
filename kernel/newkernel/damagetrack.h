/* NovaOS C-Kernel - Nova Damage Tracking (049-GUI/011-Graphic_Engine/
   005-Damage_Tracking, Phase 5 des Graphics-Engine-Vorhabens). Neun
   NPSPECs dieses Ordners: ARCH/REGION/PROPAGATION/DEPENDENCY/HIERARCHY/
   MERGE/OCCLUSION/MATERIAL/PRESENT-0001.

   ECHTER AUFBAU AUF mehreren frueheren Phasen gleichzeitig:
   - `render_rect_t` wird direkt aus Phase 3 (renderengine.h) wiederverwendet
     statt eines eigenen, parallelen Rechteck-Typs - Damage Regions UND
     Scissor Rectangles sind geometrisch dasselbe Konzept.
   - `damage_from_node_change()` liest den echten Scene-Node-Zustand (Phase
     2) und propagiert per `gfx_dependency_invalidate_from()` (Phase 1,
     dieselbe Funktion, nicht neu erfunden) an alle transitiv abhaengigen
     Elemente - genau die in DEPENDENCY-0001 §8 geforderte Wiederverwendung
     ("Nova Graphics Dependency Tracking... SOLL vorhandene Abhaengigkeits-
     informationen wiederverwenden"). Anders als bei Phase 4 (Render Pass
     Dependencies, bewusst EIGENSTAENDIG) ist die Wiederverwendung hier
     ausdruecklich korrekt, weil Damage-Abhaengigkeiten UND Graphics/Scene-
     Abhaengigkeiten (Phase 1/2) tatsaechlich dieselbe Semantik meinen:
     "wird dieses Element durch die Aenderung eines anderen beeinflusst".

   Da dieser Kernel keine echte Geometrie-/Transformations-Pipeline, kein
   Window-Manager-Z-Order und keine Material Engine-Anbindung an den Scene
   Graph besitzt, bleibt der Scope auf das ohne diese Grundlagen ehrlich
   Implementierbare begrenzt: rechteckbasierte Regionen (konservativ, wie
   von der Spec explizit erlaubt), Merge/Fragmentierungs-Fallback auf Full
   Damage, Material-Aware Expansion (generischer Rand-Zuwachs, z.B. fuer
   einen Blur-Radius) und ein einfaches, vom Aufrufer gesteuertes
   Occlusion-Deferral (kein eigenes Verdeckungs-Geometrie-Modell).

   Bewusst NICHT Teil dieser Phase: Partial Frame Presentation (PRESENT-
   0001 - Framebuffer-/Display-Praesentation gehoert zur spaeteren Phase
   007-Display_Server-Surface_Protocol), echte Occlusion-Berechnung aus
   Z-Order/Opacity (braucht einen noch nicht angebundenen Window Manager/
   Compositor - Phase 006), Material-Effektketten (braucht eine Anbindung
   der bereits bestehenden, aber unabhaengigen Material Engine an den
   Scene Graph). */
#ifndef NOVA_DAMAGETRACK_H
#define NOVA_DAMAGETRACK_H

#include <stdint.h>
#include "renderengine.h" /* render_rect_t */

#define DAMAGE_REGION_CAPACITY 32u
#define DAMAGE_MAX_PER_TARGET_BEFORE_FULL 4u
#define DAMAGE_PROPAGATE_MAX 8u

#define DAMAGE_TYPE_PARTIAL 1u
#define DAMAGE_TYPE_FULL    2u

#define DAMAGE_ERR_NOT_FOUND (-1)
#define DAMAGE_ERR_CAPACITY  (-2)

typedef struct {
    uint32_t region_id;
    uint32_t target_id;
    render_rect_t bounds;
    uint32_t state_version;
    uint32_t region_type;
    int deferred;
} damage_region_info_t;

void damagetrack_initialize(void);
int damagetrack_self_test(void); /* 0 = ok */

/* NPSPEC-DAMAGE-REGION-0001: reine Rechteck-Operationen, konservativ
   korrekt (#15 "Eine konservative rechteckige Begrenzung MUSS moeglich
   sein"), unabhaengig von der Region-Verwaltungstabelle. Ein Rect mit
   width==0 oder height==0 gilt als leer. */
render_rect_t damage_rect_union(render_rect_t a, render_rect_t b);
int damage_rect_overlaps_or_adjacent(render_rect_t a, render_rect_t b, uint32_t margin);
render_rect_t damage_rect_expand(render_rect_t r, uint32_t margin);

/* Fuegt eine Damage Region fuer target_id hinzu (z.B. ein Scene Node, eine
   Surface - generische ID). 0 = Erfolg (*out_region_id gesetzt),
   DAMAGE_ERR_CAPACITY wenn die Tabelle voll ist. */
int damage_add(uint32_t target_id, render_rect_t rect, uint32_t state_version,
                uint32_t *out_region_id);

/* NPSPEC-DAMAGE-HIERARCHY-0001/-PROPAGATION-0001/-DEPENDENCY-0001: erzeugt
   eine Damage Region fuer node_id (liest dessen aktuelle state_version
   echt aus scenegraph.c, Phase 2) UND fuer jedes transitiv abhaengige
   Element, das `gfx_dependency_invalidate_from()` (Phase 1) findet -
   konservativ mit denselben bounds, da keine echte Koordinaten-Abbildung
   zwischen Elementen existiert (#8 DEPENDENCY-0001 "Bei unvollstaendigen
   Abhaengigkeitsinformationen MUSS eine konservative Aktualisierung
   erfolgen"). 0 = Erfolg (*out_count = Anzahl erzeugter Regionen, inkl.
   node_id selbst). */
int damage_from_node_change(uint32_t node_id, render_rect_t local_bounds, uint32_t *out_count);

/* NPSPEC-DAMAGE-OCCLUSION-0001: vom Aufrufer gesteuertes Zurueckstellen -
   eine deferred Region bleibt in der Tabelle erhalten (kein Datenverlust,
   #5 "Zurueckgestellter Damage MUSS erhalten bleiben"), wird aber von
   damage_merge_target()/damage_pending_count() ausgeschlossen, bis sie per
   damage_mark_visible() wieder aktiviert wird. */
int damage_mark_occluded(uint32_t region_id);
int damage_mark_visible(uint32_t region_id);

/* NPSPEC-DAMAGE-ARCH-0001 §14 Damage Lifecycle "Resolved": entfernt eine
   erfolgreich verarbeitete Region endgueltig. 0 = Erfolg,
   DAMAGE_ERR_NOT_FOUND sonst. */
int damage_resolve(uint32_t region_id);

int damage_region_info(uint32_t region_id, damage_region_info_t *out_info);

/* Anzahl ausstehender (nicht deferred) Regionen fuer target_id. */
int damage_pending_count(uint32_t target_id, uint32_t *out_count);

/* NPSPEC-DAMAGE-MERGE-0001: fasst alle ausstehenden (nicht deferred)
   Regionen von target_id zu einer moeglichst kleinen, aber vollstaendig
   deckenden Menge zusammen (einfacher, konservativer Single-Pass-Greedy-
   Merge: jede Region wird in den ersten ueberlappenden/benachbarten
   bestehenden Eintrag der Ergebnismenge vereinigt, sonst als neuer Eintrag
   aufgenommen - verliert nie Flaeche, #2 "Saemtliche ursprueglichen
   Damage-Bereiche MUESSEN erhalten bleiben"). Bei zu hoher Fragmentierung
   (> DAMAGE_MAX_PER_TARGET_BEFORE_FULL ausstehende Regionen) wird
   stattdessen eine einzige Vereinigung ALLER Regionen mit
   DAMAGE_TYPE_FULL zurueckgegeben (#7 Region Fragmentation Fallback).
   0 = Erfolg (*out_count gesetzt, *out_merged gefuellt). */
int damage_merge_target(uint32_t target_id, render_rect_t *out_merged, uint32_t max_out,
                        uint32_t *out_count, uint32_t *out_region_type);

/* Loest alle ausstehenden (nicht deferred) Regionen fuer target_id auf
   einmal auf - Hilfsfunktion fuer Verbraucher (z.B. den Compositor, Phase
   6), die nach erfolgreicher Verarbeitung ALLER Regionen eines Ziels
   dieses als abgeschlossen markieren wollen, ohne jede region_id einzeln
   zu kennen. *out_count = Anzahl aufgeloester Regionen (kann 0 sein). */
int damage_resolve_target(uint32_t target_id, uint32_t *out_count);

#endif

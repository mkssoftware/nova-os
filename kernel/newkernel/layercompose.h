/* NovaOS C-Kernel - Nova Compositor / Composition Layer Management
   (049-GUI/011-Graphic_Engine/006-Compositor, Phase 6 des Graphics-Engine-
   Vorhabens). Zehn NPSPECs dieses Ordners: ARCH/LAYER/SURFACE/OCCLUSION/
   OVERLAY/INCREMENTAL/CACHE/SCHEDULE/FALLBACK/ZERO-COPY-0001.

   Modulname bewusst NICHT "compositor.c/.h" - dieser Kernel hat bereits
   einen eigenstaendigen, laengst funktionierenden Framebuffer-Compositor
   (`compositor.c`, echte Pixel-Komposition fuer den Desktop). Dieses Modul
   implementiert die NPSPEC-ARCHITEKTUR-Schicht (Composition Tree/Layer-
   Hierarchie, Occlusion Culling, Damage-getriebene Komposition, Cache) OHNE
   den bestehenden Pixel-Compositor zu beruehren - "Der Composition Tree
   ist vom logischen Nova Scene Graph UND von einer bestehenden Pixel-
   Implementierung zu unterscheiden" (ARCH-0001 §4, analog angewandt).

   ECHTER AUFBAU AUF mehreren frueheren Phasen:
   - `render_rect_t` (Phase 3) fuer Layer-Bounds.
   - `render_execute_retained()` (Phase 3) fuer Layer, die an einen echten
     Scene Node gebunden sind (node_id!=0).
   - `damage_add()`/`damage_pending_count()`/`damage_resolve_target()`
     (Phase 5) fuer die Damage-getriebene Kompositions-Entscheidung -
     Layer-Aenderungen (Bounds/Opacity/Z-Order) erzeugen ECHTE Damage
     Regions, `compositor_compose()` loest sie nach erfolgreicher
     Verarbeitung ECHT auf (nicht nur symbolisch).

   Occlusion Culling (OCCLUSION-0001) ist hier bewusst EINFACH und
   KONSERVATIV implementiert (volle Enthaltung statt Teil-Regionen): eine
   Layer gilt nur dann als vollstaendig verdeckt, wenn eine opake
   (`opaque && opacity==100`), weiter vorne liegende (hoherer z_order) Layer
   ihre Bounds VOLLSTAENDIG enthaelt - alles andere bleibt sichtbar (#13
   OCCLUSION-ARCH "Unsichere Bereiche werden nicht ausgeschlossen"). Layer
   mit `needs_backdrop` (Material-Aware Occlusion, z.B. Liquid Glass) werden
   NIEMALS als verdeckt markiert, unabhaengig von geometrischer Abdeckung.

   Bewusst NICHT Teil dieser Phase (reine GPU-/Display-Hardware-Konzepte
   ohne Software-Entsprechung): Hardware Overlay Integration (OVERLAY-0001),
   Zero-Copy Composition (ZERO-COPY-0001 - setzt gemeinsame GPU-Buffer
   voraus), Composition Fallback zwischen GPU-Qualitaetsstufen (FALLBACK-
   0001 - dieser Kernel hat nur EINE Stufe: Software), Multi-Display/
   Composition Scheduling mit Deadlines (SCHEDULE-0001 - gehoert inhaltlich
   zur spaeteren Phase 008-Frame_Scheduling). */
#ifndef NOVA_LAYERCOMPOSE_H
#define NOVA_LAYERCOMPOSE_H

#include <stdint.h>
#include "renderengine.h" /* render_rect_t */

#define LAYER_CAPACITY 16u
#define COMPOSITION_CACHE_CAPACITY 16u

#define LAYER_CATEGORY_DESKTOP     1u
#define LAYER_CATEGORY_APPLICATION 2u
#define LAYER_CATEGORY_SYSTEM      3u
#define LAYER_CATEGORY_POPUP       4u
#define LAYER_CATEGORY_OVERLAY     5u
#define LAYER_CATEGORY_CURSOR      6u

/* NPSPEC-COMPOSITOR-OCCLUSION-0001 §5 Visibility Classification (Partially
   Visible/Unknown entfallen bewusst - dieses Modell kennt nur vollstaendige
   Enthaltung, siehe Dateikopf). */
#define LAYER_VISIBLE          1u
#define LAYER_OCCLUDED         2u
#define LAYER_EFFECT_DEPENDENT 3u /* needs_backdrop, nie okkludiert */

#define LAYER_ERR_NOT_FOUND     (-1)
#define LAYER_ERR_CAPACITY      (-2)
#define LAYER_ERR_CYCLE         (-3)

typedef struct {
    uint32_t layer_id;
    uint32_t parent_id;
    uint32_t category;
    uint32_t surface_id;
    uint32_t node_id;
    render_rect_t bounds;
    int32_t z_order;
    uint32_t opacity;
    int opaque;
    int needs_backdrop;
    uint32_t visibility_state;
    uint32_t state_version;
} layer_info_t;

void layercompose_initialize(void);
int layercompose_self_test(void); /* 0 = ok */

/* surface_id/node_id: 0 = keine Bindung (reine Gruppen-/Transform-Layer
   ohne Inhalt, #3 LAYER-0001 "Layer DUERFEN ohne eigene Surface
   existieren"). parent_id: 0 = kein Parent (Wurzel-Ebene), sonst MUSS
   bereits existieren. Da diese Phase keine Reparenting-Operation anbietet,
   kann durch layer_create() selbst strukturell kein Zyklus entstehen (ein
   neuer Layer kann nicht Vorfahre eines bereits existierenden sein) -
   LAYER_ERR_CYCLE bleibt fuer eine kuenftige Reparenting-Funktion
   reserviert. 0 = Erfolg (*out_layer_id gesetzt), LAYER_ERR_NOT_FOUND wenn
   parent_id nicht existiert, LAYER_ERR_CAPACITY wenn die Tabelle voll ist. */
int layer_create(uint32_t parent_id, uint32_t category, uint32_t surface_id, uint32_t node_id,
                  render_rect_t bounds, int32_t z_order, uint32_t *out_layer_id);

/* Jede dieser drei Aenderungen bumpt die state_version der Layer und
   erzeugt per `damage_add()` (Phase 5) eine ECHTE Damage Region fuer
   layer_id. */
int layer_set_bounds(uint32_t layer_id, render_rect_t bounds);
int layer_set_opacity(uint32_t layer_id, uint32_t opacity);
int layer_set_z_order(uint32_t layer_id, int32_t z_order);

int layer_set_opaque(uint32_t layer_id, int opaque);
int layer_set_needs_backdrop(uint32_t layer_id, int needs_backdrop);

int layer_remove(uint32_t layer_id);
int layer_info(uint32_t layer_id, layer_info_t *out_info);

/* NPSPEC-COMPOSITOR-OCCLUSION-0001: bewertet ALLE Layer neu (siehe
   Dateikopf fuer das vereinfachte, konservative Modell). Layer mit
   needs_backdrop werden immer LAYER_EFFECT_DEPENDENT (niemals okkludiert).
   0 = immer Erfolg. */
int compositor_occlusion_pass(void);

/* NPSPEC-COMPOSITOR-INCREMENTAL-0001/-CACHE-0001: verarbeitet alle NICHT
   okkludierten Layer in aufsteigender Z-Order (hinten->vorne, Painter's
   Algorithm). Fuer jede Layer mit ausstehendem Damage (`damage_pending_
   count()`, Phase 5) wird neu "komponiert" (bei node_id!=0 echter Aufruf
   von `render_execute_retained()`, Phase 3; sonst ein Platzhalterwert) und
   per `damage_resolve_target()` (Phase 5) als erledigt markiert. Layer ohne
   ausstehenden Damage liefern ihr zwischengespeichertes Ergebnis ohne
   erneute Berechnung. *out_composited_value ist eine Platzhalter-
   Akkumulation (XOR) aller verarbeiteten/zwischengespeicherten Werte -
   kein echter Pixelbuffer, siehe Dateikopf von renderengine.h fuer dieselbe
   Designentscheidung in Phase 3. *out_processed_count zaehlt NUR echte
   Neuberechnungen (keine Cache-Treffer). 0 = immer Erfolg. */
int compositor_compose(uint32_t damage_area, uint32_t total_area,
                        uint32_t *out_composited_value, uint32_t *out_processed_count);

#endif

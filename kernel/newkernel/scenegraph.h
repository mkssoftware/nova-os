/* NovaOS C-Kernel - Nova Scene Graph (049-GUI/011-Graphic_Engine/
   002-Scene_Graph, Phase 2 des Graphics-Engine-Vorhabens). ECHTER AUFBAU AUF
   Phase 1 (gfxstate.h): eine Scene-Transaktion IST eine Graphics State
   Transaction (1:1-Wrapper um deren tx_id, kein Parallel-Zustand), eine
   Scene-Node-Aenderung wird als Graphics Change Record IN DERSELBEN
   Transaktion erfasst (gfx_transaction_record_change()), und Scene-Node-
   Abhaengigkeiten sind Eintraege im SELBEN globalen Dependency Graph aus
   Phase 1 (gfx_dependency_add()) - die transitive Invalidierungs-
   Propagation bei jedem Commit kommt dadurch VOLLSTAENDIG KOSTENLOS aus
   Phase 1, ohne ein einziges Byte Duplikation.

   Acht zusammengehoerige NPSPECs dieses Ordners: ARCH/NODE/PROPERTY/STATE/
   CHANGE/DEPENDENCY/CACHE/UPDATE-0001.

   Transaktionsmodell (erfuellt #6 aus SCENE-ARCH-0001 "Nicht abgeschlossene
   Aenderungen DUERFEN fuer Rendering und Komposition nicht sichtbar sein"
   als echtes MUSS, nicht nur Konvention):
   - Neue Nodes werden SOFORT angelegt (Zustand CREATED), aber erst bei
     Commit in die children[]-Liste ihres Parents eingehaengt (ATTACHED) -
     bis dahin sind sie fuer niemanden ausser dem eigenen Transaktionspfad
     sichtbar, Eigenschaftsaenderungen an ihnen duerfen daher sofort in die
     lebende Tabelle geschrieben werden (kein Rollback-Bedarf: ein Abort
     verwirft den kompletten, nie veroeffentlichten Node-Slot einfach).
   - Aenderungen an einem BEREITS veroeffentlichten (ATTACHED) Node -
     Property-Writes, Remove, Reparent - werden als Pending Ops in der
     Transaktion aufgezeichnet und ERST beim Commit tatsaechlich auf die
     lebende Tabelle angewandt. Ein Abort verwirft die Pending Ops einfach,
     ohne dass irgendetwas zurueckgerollt werden muss, weil nichts vorher
     angewendet wurde.

   Scene Cache (CACHE-0001) prueft Gueltigkeit LAZY beim Zugriff (gleiches
   Muster wie die lazy BROKEN-/Lazy-Subscriber-Tod-Checks in channel.c/
   pubsub.c aus dem IPC-Neuentwurf): ein Cache-Eintrag vergleicht seine
   gespeicherte state_version mit der AKTUELLEN state_version seines
   Owner-Nodes - weichen sie ab, ist er sofort INVALID, ohne dass irgendwo
   eine Invalidierungs-Sweep-Liste gefuehrt werden muss.

   Bewusst NICHT Teil dieser Phase: Property-Vererbung/-Bindings (PROPERTY-
   0001 §6/§7 - braucht Nova UI, existiert noch nicht), transitive Cache-
   Invalidierung UEBER Abhaengigkeitskanten hinweg (nur die direkte Owner-
   Node-Version wird geprueft - die cross-node Variante ist explizit
   Aufgabe von Damage Tracking/Render Graph, siehe CACHE-0001 §11), echte
   Geometrie-/Transformations-Berechnung (kein Renderer existiert noch),
   Parallelitaet/mehrere gleichzeitig offene Transaktionen auf DERSELBEN
   Szene (dieser Kernel hat keine Multi-Thread-Contention auf Grafikzustand
   zu modellieren). */
#ifndef NOVA_SCENEGRAPH_H
#define NOVA_SCENEGRAPH_H

#include <stdint.h>

#define SCENE_CAPACITY              16u
#define SCENE_NODE_CAPACITY         64u
#define SCENE_NODE_MAX_CHILDREN     8u
#define SCENE_PROPERTY_CAPACITY     128u
#define SCENE_PROPERTY_DEF_CAPACITY 32u
#define SCENE_CACHE_CAPACITY        32u
#define SCENE_TX_CAPACITY           8u
#define SCENE_TX_MAX_OPS            16u

/* NPSPEC-SCENE-NODE-0001 §10 Lebenszyklus. */
#define SCENE_NODE_CREATED  1u
#define SCENE_NODE_ATTACHED 2u
#define SCENE_NODE_DETACHED 3u
#define SCENE_NODE_RELEASED 4u

/* NPSPEC-SCENE-NODE-0001 §4 Node-Typen. */
#define SCENE_NODE_TYPE_GROUP     1u
#define SCENE_NODE_TYPE_TRANSFORM 2u
#define SCENE_NODE_TYPE_GEOMETRY  3u
#define SCENE_NODE_TYPE_TEXT      4u
#define SCENE_NODE_TYPE_IMAGE     5u
#define SCENE_NODE_TYPE_SURFACE   6u
#define SCENE_NODE_TYPE_CLIP      7u
#define SCENE_NODE_TYPE_MATERIAL  8u
#define SCENE_NODE_TYPE_EFFECT    9u
#define SCENE_NODE_TYPE_CUSTOM    10u

/* NPSPEC-SCENE-CHANGE-0001 §4 Aenderungstypen - als opaker change_type an
   gfx_transaction_record_change() durchgereicht (Phase 1 kennt deren
   Bedeutung nicht, braucht sie auch nicht). */
#define SCENE_CHANGE_NODE_CREATED       0u
#define SCENE_CHANGE_NODE_REMOVED       1u
#define SCENE_CHANGE_NODE_REPARENTED    2u
#define SCENE_CHANGE_CHILDREN_REORDERED 3u
#define SCENE_CHANGE_PROPERTY_CHANGED   4u
#define SCENE_CHANGE_GEOMETRY_CHANGED   5u
#define SCENE_CHANGE_TRANSFORM_CHANGED  6u
#define SCENE_CHANGE_VISIBILITY_CHANGED 7u
#define SCENE_CHANGE_CONTENT_CHANGED    8u
#define SCENE_CHANGE_RESOURCE_CHANGED   9u

/* NPSPEC-SCENE-PROPERTY-0001 §4 Property-Typen. */
#define SCENE_PROP_TYPE_BOOL         0u
#define SCENE_PROP_TYPE_INT          1u
#define SCENE_PROP_TYPE_FLOAT        2u
#define SCENE_PROP_TYPE_COLOR        3u
#define SCENE_PROP_TYPE_VECTOR       4u
#define SCENE_PROP_TYPE_MATRIX       5u
#define SCENE_PROP_TYPE_RECT         6u
#define SCENE_PROP_TYPE_STRING       7u
#define SCENE_PROP_TYPE_ENUM         8u
#define SCENE_PROP_TYPE_RESOURCE_REF 9u
#define SCENE_PROP_TYPE_MATERIAL_REF 10u
#define SCENE_PROP_TYPE_STRUCTURED   11u

/* NPSPEC-SCENE-PROPERTY-0001 §10 Property-Metadaten-Flags. */
#define SCENE_PROP_FLAG_AFFECTS_LAYOUT       0x01u
#define SCENE_PROP_FLAG_AFFECTS_GEOMETRY     0x02u
#define SCENE_PROP_FLAG_AFFECTS_RENDER       0x04u
#define SCENE_PROP_FLAG_AFFECTS_COMPOSITION  0x08u
#define SCENE_PROP_FLAG_AFFECTS_DEPENDENCIES 0x10u
#define SCENE_PROP_FLAG_INHERITABLE          0x20u
#define SCENE_PROP_FLAG_ANIMATABLE           0x40u

/* NPSPEC-SCENE-CACHE-0001 §4 Kategorien (Auswahl). */
#define SCENE_CACHE_PROPERTY   0u
#define SCENE_CACHE_TRANSFORM  1u
#define SCENE_CACHE_GEOMETRY   2u
#define SCENE_CACHE_BOUNDS     3u
#define SCENE_CACHE_LAYOUT     4u
#define SCENE_CACHE_VISIBILITY 5u
#define SCENE_CACHE_HIERARCHY  6u
#define SCENE_CACHE_MATERIAL   7u

/* NPSPEC-SCENE-CACHE-0001 §8 Lebenszyklus (COMPUTING entfaellt bewusst -
   dieser Kernel hat keine echte Parallelverarbeitung von Cache-Eintraegen
   zu modellieren, analog zur CREATED/CONNECTING-Kollabierung bei Channels). */
#define SCENE_CACHE_MISSING   0u
#define SCENE_CACHE_VALID     1u
#define SCENE_CACHE_INVALID   2u
#define SCENE_CACHE_EVICTABLE 3u

#define SCENE_ERR_NOT_FOUND     (-1)
#define SCENE_ERR_CAPACITY      (-2)
#define SCENE_ERR_INVALID_STATE (-3)
#define SCENE_ERR_CYCLE         (-4)
#define SCENE_ERR_DENIED        (-5)

typedef struct {
    uint32_t node_id;
    uint32_t scene_id;
    uint32_t node_type;
    uint32_t parent_id;
    uint32_t child_count;
    uint32_t state_version;
    uint32_t lifecycle_state;
} scene_node_info_t;

typedef struct {
    uint32_t cache_id;
    uint32_t owner_node_id;
    uint32_t result_type;
    uint32_t state_version;
    uint32_t cached_value;
    uint32_t validity_state; /* bereits lazy gegen den Owner-Node geprueft */
} scene_cache_info_t;

void scenegraph_initialize(void);
int scenegraph_self_test(void); /* 0 = ok */

/* Legt eine neue Szene MIT einem initialen, bereits ATTACHED Root-GROUP-Node
   an (Bootstrap-Schritt, bewusst OHNE Transaktion - analog zu topic_create()
   im IPC-Neuentwurf, der auch kein Transaktionsrahmen braucht). 0 = Erfolg,
   SCENE_ERR_CAPACITY wenn Szenen- oder Node-Tabelle voll ist. */
int scene_create(uint32_t *out_scene_id, uint32_t *out_root_node_id);

/* Registriert eine Property-Definition (Typ/Standardwert/Flags), unabhaengig
   von einzelnen Node-Instanzen (#3 PROPERTY-0001). 0 = Erfolg,
   SCENE_ERR_CAPACITY wenn die Definitionstabelle voll ist. Erneutes
   Definieren derselben property_id ueberschreibt die bestehende Definition. */
int scene_property_define(uint32_t property_id, uint32_t property_type,
                           uint32_t default_value, uint32_t flags);

/* 1:1-Wrapper um gfx_transaction_begin()/_commit()/_abort() aus Phase 1 -
   siehe Dateikopf. */
int scene_transaction_begin(uint32_t scene_id, uint32_t *out_tx_id);
int scene_transaction_commit(uint32_t tx_id, uint32_t *out_state_version);
int scene_transaction_abort(uint32_t tx_id);

/* Legt sofort einen neuen Node im Zustand CREATED an (siehe Dateikopf fuer
   das Sichtbarkeitsmodell). parent_id MUSS ein bereits ATTACHED Node
   derselben Szene sein (oder ein in DERSELBEN Transaktion zuvor erzeugter,
   noch CREATED Node). 0 = Erfolg (*out_node_id gesetzt). */
int scene_node_create(uint32_t tx_id, uint32_t parent_id, uint32_t node_type,
                       uint32_t *out_node_id);

/* node_id MUSS aktuell ATTACHED sein. Wirkung (Entfernen aus den Children
   des Parents, Uebergang zu DETACHED) wird erst bei Commit angewandt. */
int scene_node_remove(uint32_t tx_id, uint32_t node_id);

/* node_id MUSS aktuell ATTACHED sein, new_parent_id MUSS existieren.
   Wirkung erst bei Commit. */
int scene_node_reparent(uint32_t tx_id, uint32_t node_id, uint32_t new_parent_id);

/* Bei einem CREATED Node (noch nicht veroeffentlicht) wirkt sofort; bei
   einem ATTACHED Node wird die Aenderung erst bei Commit sichtbar (#6
   Zustandsverwaltung - vorher liefert scene_node_get_property() weiterhin
   den alten Wert). 0 = Erfolg. */
int scene_node_set_property(uint32_t tx_id, uint32_t node_id, uint32_t property_id,
                             uint32_t value);

/* Liefert den zuletzt VEROEFFENTLICHTEN Wert (oder den Definitions-
   Standardwert, falls fuer diesen Node nie gesetzt). Funktioniert fuer
   CREATED/ATTACHED/DETACHED Nodes. 0 = Erfolg. */
int scene_node_get_property(uint32_t node_id, uint32_t property_id, uint32_t *out_value);

int scene_node_info(uint32_t node_id, scene_node_info_t *out_info);

/* Entfernt endgueltig einen DETACHED Node (RELEASED, Slot wird frei). 0 =
   Erfolg, SCENE_ERR_INVALID_STATE wenn nicht DETACHED. */
int scene_node_release(uint32_t node_id);

/* Duenner, validierender Wrapper um gfx_dependency_add() aus Phase 1 - dep_type
   ist einer der GFX_DEP_*-Werte aus gfxstate.h (Scene Graph definiert keinen
   eigenen, parallelen Satz). SCENE_ERR_NOT_FOUND wenn einer der Nodes nicht
   existiert, sonst Fehler/Erfolg von gfx_dependency_add() direkt
   durchgereicht (SCENE_ERR_CYCLE/_CAPACITY sind numerisch identisch zu
   GFX_ERR_CYCLE/_CAPACITY). */
int scene_node_add_dependency(uint32_t source_node_id, uint32_t target_node_id,
                               uint32_t dep_type, uint32_t *out_edge_id);

/* Speichert value als Cache-Ergebnis, gebunden an die AKTUELLE state_version
   des Owner-Nodes. 0 = Erfolg, SCENE_ERR_NOT_FOUND wenn der Node nicht
   existiert, SCENE_ERR_CAPACITY wenn die Cache-Tabelle voll ist. */
int scene_cache_put(uint32_t owner_node_id, uint32_t result_type, uint32_t value,
                     uint32_t *out_cache_id);

/* Liest einen Cache-Eintrag; prueft dabei LAZY, ob die gespeicherte
   state_version noch zur aktuellen state_version des Owner-Nodes passt -
   bei Abweichung (oder wenn der Node inzwischen RELEASED wurde) liefert
   out_info->validity_state == SCENE_CACHE_INVALID, unabhaengig vom zuletzt
   gespeicherten Wert. 0 = Erfolg (Eintrag existiert, validity_state zeigt
   den aktuellen Zustand), SCENE_ERR_NOT_FOUND wenn cache_id unbekannt. */
int scene_cache_get(uint32_t cache_id, scene_cache_info_t *out_info);

#endif

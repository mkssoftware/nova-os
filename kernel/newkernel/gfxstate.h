/* NovaOS C-Kernel - Nova Graphics Architecture, Phase 1: die uebergreifende
   Zustands-/Aenderungs-/Abhaengigkeits-/Semantik-/Adaptivitaets-Schicht aus
   049-GUI/011-Graphic_Engine/001-Architecture (acht zusammengehoerige
   NPSPECs: ARCH/STATE/DECLARATIVE/CHANGE/DEPENDENCY/SEMANTIC/ADAPTIVE/
   TRANSACTION-0001). Bewusst UNABHAENGIG von einem Scene Graph (eigene,
   spaetere Phase 002-Scene_Graph) - die Spec verlangt das explizit
   ("Der Dependency Graph...DARF dessen hierarchische Struktur nicht
   voraussetzen"). Element-Identitaeten sind hier einfache, vom Aufrufer
   vergebene uint32_t-IDs; ein spaeterer Scene-Graph-Knoten wird genau
   solche IDs an diese Schicht uebergeben.

   Nicht Teil dieser Phase (eigene spaetere Schichten): Scene Graph (002),
   Render Engine (003), Render Graph (004), Damage Tracking (005),
   Compositor (006), Display-Protokoll (007), Frame Scheduling (008). Die
   GPU-Hardware-Ordner 009-011 sind BLOCKED (kein GPU-Treiber vorhanden,
   Nutzerentscheidung 2026-11-04). */
#ifndef NOVA_GFXSTATE_H
#define NOVA_GFXSTATE_H

#include <stdint.h>

#define GFX_TX_CAPACITY           8u
#define GFX_TX_MAX_CHANGES        16u
#define GFX_DEPENDENCY_CAPACITY   64u
#define GFX_SEMANTIC_CAPACITY     32u

#define GFX_TX_STATE_OPEN       1u
#define GFX_TX_STATE_COMMITTED  2u
#define GFX_TX_STATE_ABORTED    3u

/* NPSPEC-GRAPHICS-CHANGE-0001 §4 Aenderungstypen. */
#define GFX_CHANGE_PROPERTY   0u
#define GFX_CHANGE_CONTENT    1u
#define GFX_CHANGE_GEOMETRY   2u
#define GFX_CHANGE_LAYOUT     3u
#define GFX_CHANGE_TRANSFORM  4u
#define GFX_CHANGE_VISIBILITY 5u
#define GFX_CHANGE_MATERIAL   6u
#define GFX_CHANGE_HIERARCHY  7u
#define GFX_CHANGE_RESOURCE   8u
#define GFX_CHANGE_SURFACE    9u

/* NPSPEC-GRAPHICS-DEPENDENCY-0001 §4 Abhaengigkeitstypen. */
#define GFX_DEP_PROPERTY  0u
#define GFX_DEP_LAYOUT    1u
#define GFX_DEP_TRANSFORM 2u
#define GFX_DEP_VISIBILITY 3u
#define GFX_DEP_MATERIAL  4u
#define GFX_DEP_EFFECT    5u
#define GFX_DEP_SURFACE   6u
#define GFX_DEP_RESOURCE  7u

/* NPSPEC-GRAPHICS-SEMANTIC-0001 §4 Klassifizierung - Bitmaske, ein Element
   DARF mehreren Kategorien angehoeren. */
#define GFX_SEM_STRUCTURAL  0x01u
#define GFX_SEM_CONTENT     0x02u
#define GFX_SEM_INTERACTIVE 0x04u
#define GFX_SEM_DECORATIVE  0x08u
#define GFX_SEM_MATERIAL    0x10u
#define GFX_SEM_DYNAMIC     0x20u
#define GFX_SEM_SURFACE     0x40u

/* NPSPEC-GRAPHICS-ADAPTIVE-0001 §3 Verarbeitungsstrategien (reduziert auf
   die beiden Grundstrategien - Hardware-/GPU-abhaengige Varianten gehoeren
   zu den zurueckgestellten GPU-Ordnern 009-011). */
#define GFX_STRATEGY_PARTIAL 1u
#define GFX_STRATEGY_FULL    2u

#define GFX_ERR_NOT_FOUND     (-1)
#define GFX_ERR_CAPACITY      (-2)
#define GFX_ERR_INVALID_STATE (-3)
#define GFX_ERR_CYCLE         (-4)
#define GFX_ERR_INVALID       (-5)

typedef struct {
    uint32_t tx_id;
    uint32_t state;        /* GFX_TX_STATE_* */
    uint32_t state_version; /* 0 solange nicht COMMITTED */
    uint32_t change_count;
} gfx_transaction_info_t;

typedef struct {
    uint32_t edge_id;
    uint32_t source_id;
    uint32_t target_id;
    uint32_t dep_type;
    uint32_t invalidation_total;
} gfx_dependency_info_t;

void gfxstate_initialize(void);
int gfxstate_self_test(void); /* 0 = ok */

/* --- NPSPEC-GRAPHICS-TRANSACTION-0001 / -CHANGE-0001 ------------------- */

/* Oeffnet eine neue Transaktion (Zustand OPEN). Belegt bevorzugt einen noch
   nie benutzten Slot, sonst einen bereits abgeschlossenen (COMMITTED/
   ABORTED, kein striktes LRU) (#6 "mehrere Transaktionen duerfen
   gleichzeitig vorbereitet werden" - begrenzte History statt unbegrenztem
   Wachstum). 0 = Erfolg (*out_tx_id gesetzt), GFX_ERR_CAPACITY wenn alle
   Slots noch OFFEN sind. */
int gfx_transaction_begin(uint32_t *out_tx_id);

/* Sammelt eine Aenderung innerhalb einer OFFENEN Transaktion (#3 "zusammen-
   gehoerige Aenderungen werden gesammelt"). property_id ist eine vom
   Aufrufer definierte, generische Kennung der betroffenen Eigenschaft/
   Ressource. 0 = Erfolg, GFX_ERR_INVALID_STATE wenn die Transaktion nicht
   OPEN ist, GFX_ERR_CAPACITY wenn GFX_TX_MAX_CHANGES erreicht ist. */
int gfx_transaction_record_change(uint32_t tx_id, uint32_t element_id, uint32_t change_type,
                                   uint32_t property_id, uint32_t from_version, uint32_t to_version);

/* Uebernimmt alle gesammelten Aenderungen atomar (#4 Transaktionsverarbei-
   tung): vergibt eine neue, global monoton steigende Zustandsversion
   (Versionen werden AUSSCHLIESSLICH beim Commit vergeben, niemals bei
   begin() - ein Abort verbraucht dadurch nie eine Version, #9
   Fehlerbehandlung), markiert die Transaktion COMMITTED, und propagiert
   die Aenderungen an alle transitiv abhaengigen Elemente (Graphics
   Dependency Tracking, zaehlt deren invalidation_total hoch). 0 = Erfolg
   (*out_state_version gesetzt), GFX_ERR_INVALID_STATE wenn nicht OPEN. */
int gfx_transaction_commit(uint32_t tx_id, uint32_t *out_state_version);

/* Verwirft eine OFFENE Transaktion vollstaendig (#5 Zustandskonsistenz -
   kein teilweise uebernommener Zustand). 0 = Erfolg, GFX_ERR_INVALID_STATE
   wenn nicht OPEN. */
int gfx_transaction_abort(uint32_t tx_id);

int gfx_transaction_info(uint32_t tx_id, gfx_transaction_info_t *out_info);

/* --- NPSPEC-GRAPHICS-DEPENDENCY-0001 ------------------------------------ */

/* Fuegt eine gerichtete Abhaengigkeit source_id -> target_id hinzu. Wird
   abgewiesen, wenn dadurch ein Zyklus entstuende (#4 "Zyklen erkennen und
   kontrolliert behandeln") oder source_id==target_id. 0 = Erfolg
   (*out_edge_id gesetzt), GFX_ERR_CYCLE, GFX_ERR_CAPACITY. */
int gfx_dependency_add(uint32_t source_id, uint32_t target_id, uint32_t dep_type,
                        uint32_t *out_edge_id);

int gfx_dependency_remove(uint32_t edge_id);

/* Transitive Aenderungspropagation (#6 Aenderungspropagation): sammelt
   alle Elemente, die (direkt oder transitiv) von changed_id abhaengen, in
   out_affected (Kapazitaet max_out - bei Erreichen wird abgeschnitten,
   *out_count gibt die tatsaechlich gesammelte Anzahl), und zaehlt die
   invalidation_total jeder durchlaufenen Kante hoch. 0 = immer Erfolg
   (auch bei 0 Abhaengigkeiten). */
int gfx_dependency_invalidate_from(uint32_t changed_id, uint32_t *out_affected,
                                    uint32_t max_out, uint32_t *out_count);

int gfx_dependency_info(uint32_t edge_id, gfx_dependency_info_t *out_info);

/* --- NPSPEC-GRAPHICS-SEMANTIC-0001 -------------------------------------- */

/* Setzt/ueberschreibt die semantische Klassifizierung (Bitmaske aus
   GFX_SEM_*) eines Elements. Semantik ist optional (#7 "Fehlende Semantik
   darf die korrekte Darstellung nicht verhindern") - gfx_semantic_get()
   liefert 0 fuer unbekannte Elemente, kein Fehler. 0 = Erfolg,
   GFX_ERR_CAPACITY wenn die Tabelle voll ist und element_id neu ist. */
int gfx_semantic_set(uint32_t element_id, uint32_t semantic_mask);

/* Liefert die Bitmaske, oder 0 falls element_id keine Semantik besitzt. */
uint32_t gfx_semantic_get(uint32_t element_id);

/* --- NPSPEC-GRAPHICS-ADAPTIVE-0001 -------------------------------------- */

/* Regelbasierte, deterministische Strategieauswahl (#10 "ein deterministi-
   scher Betriebsmodus MUSS unterstuetzt werden" - hier der einzige Modus,
   da dieser Kernel ohne GPU-Telemetrie sowieso nur regelbasiert entscheiden
   kann). total_area==0 gilt als unbekannt/konservativ -> FULL. Sonst FULL
   bei >=50% veraenderter Flaeche ODER mehr als GFX_ADAPTIVE_CHANGE_LIMIT
   Einzelaenderungen, sonst PARTIAL. */
uint32_t gfx_adaptive_select_strategy(uint32_t change_count, uint32_t damage_area,
                                       uint32_t total_area);

#endif

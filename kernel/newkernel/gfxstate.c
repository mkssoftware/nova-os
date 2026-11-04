#include "gfxstate.h"
#include <stddef.h>

typedef struct {
    uint32_t element_id;
    uint32_t change_type;
    uint32_t property_id;
    uint32_t from_version;
    uint32_t to_version;
} gfx_change_record_t;

typedef struct {
    uint32_t tx_id;
    uint32_t state;
    uint32_t state_version;
    gfx_change_record_t changes[GFX_TX_MAX_CHANGES];
    uint32_t change_count;
    int ever_used;
} gfx_transaction_t;

typedef struct {
    uint32_t edge_id;
    uint32_t source_id;
    uint32_t target_id;
    uint32_t dep_type;
    uint32_t invalidation_total;
    int in_use;
} gfx_dependency_t;

typedef struct {
    uint32_t element_id;
    uint32_t semantic_mask;
    int in_use;
} gfx_semantic_entry_t;

#define GFX_ADAPTIVE_CHANGE_LIMIT 32u

static gfx_transaction_t transactions[GFX_TX_CAPACITY];
static gfx_dependency_t dependencies[GFX_DEPENDENCY_CAPACITY];
static gfx_semantic_entry_t semantics[GFX_SEMANTIC_CAPACITY];
static uint32_t gfx_next_tx_id = 1;
static uint32_t gfx_next_edge_id = 1;
static uint32_t gfx_next_state_version = 1;
static int gfxstate_ready = 0;

void gfxstate_initialize(void) {
    for (uint32_t i = 0; i < GFX_TX_CAPACITY; ++i) {
        transactions[i] = (gfx_transaction_t){0};
    }
    for (uint32_t i = 0; i < GFX_DEPENDENCY_CAPACITY; ++i) {
        dependencies[i] = (gfx_dependency_t){0};
    }
    for (uint32_t i = 0; i < GFX_SEMANTIC_CAPACITY; ++i) {
        semantics[i] = (gfx_semantic_entry_t){0};
    }
    gfx_next_tx_id = 1;
    gfx_next_edge_id = 1;
    gfx_next_state_version = 1;
    gfxstate_ready = 1;
}

static gfx_transaction_t *tx_find(uint32_t tx_id) {
    if (tx_id == 0) {
        return NULL;
    }
    for (uint32_t i = 0; i < GFX_TX_CAPACITY; ++i) {
        if (transactions[i].ever_used && transactions[i].tx_id == tx_id) {
            return &transactions[i];
        }
    }
    return NULL;
}

static gfx_dependency_t *dep_find(uint32_t edge_id) {
    if (edge_id == 0) {
        return NULL;
    }
    for (uint32_t i = 0; i < GFX_DEPENDENCY_CAPACITY; ++i) {
        if (dependencies[i].in_use && dependencies[i].edge_id == edge_id) {
            return &dependencies[i];
        }
    }
    return NULL;
}

/* BFS: kann man von `from` aus (ueber bestehende Kanten source->target)
   den Knoten `to` erreichen? Dient der Zyklus-Erkennung in
   gfx_dependency_add(): source_id->target_id wuerde einen Zyklus
   schliessen, wenn target_id transitiv bereits source_id erreicht. */
static int can_reach(uint32_t from, uint32_t to) {
    if (from == to) {
        return 1;
    }
    uint32_t queue[GFX_DEPENDENCY_CAPACITY];
    uint32_t visited[GFX_DEPENDENCY_CAPACITY];
    uint32_t queue_count = 0;
    uint32_t visited_count = 0;

    queue[queue_count++] = from;
    visited[visited_count++] = from;

    while (queue_count > 0) {
        uint32_t current = queue[0];
        for (uint32_t i = 1; i < queue_count; ++i) {
            queue[i - 1] = queue[i];
        }
        queue_count--;

        for (uint32_t i = 0; i < GFX_DEPENDENCY_CAPACITY; ++i) {
            if (!dependencies[i].in_use || dependencies[i].source_id != current) {
                continue;
            }
            uint32_t next = dependencies[i].target_id;
            if (next == to) {
                return 1;
            }
            int already_visited = 0;
            for (uint32_t v = 0; v < visited_count; ++v) {
                if (visited[v] == next) {
                    already_visited = 1;
                    break;
                }
            }
            if (!already_visited && visited_count < GFX_DEPENDENCY_CAPACITY && queue_count < GFX_DEPENDENCY_CAPACITY) {
                visited[visited_count++] = next;
                queue[queue_count++] = next;
            }
        }
    }
    return 0;
}

int gfx_dependency_add(uint32_t source_id, uint32_t target_id, uint32_t dep_type,
                        uint32_t *out_edge_id) {
    if (source_id == target_id) {
        return GFX_ERR_CYCLE;
    }
    if (can_reach(target_id, source_id)) {
        return GFX_ERR_CYCLE;
    }

    uint32_t slot;
    for (slot = 0; slot < GFX_DEPENDENCY_CAPACITY; ++slot) {
        if (!dependencies[slot].in_use) {
            break;
        }
    }
    if (slot >= GFX_DEPENDENCY_CAPACITY) {
        return GFX_ERR_CAPACITY;
    }

    dependencies[slot].edge_id = gfx_next_edge_id++;
    dependencies[slot].source_id = source_id;
    dependencies[slot].target_id = target_id;
    dependencies[slot].dep_type = dep_type;
    dependencies[slot].invalidation_total = 0;
    dependencies[slot].in_use = 1;

    *out_edge_id = dependencies[slot].edge_id;
    return 0;
}

int gfx_dependency_remove(uint32_t edge_id) {
    gfx_dependency_t *e = dep_find(edge_id);
    if (!e) {
        return GFX_ERR_NOT_FOUND;
    }
    *e = (gfx_dependency_t){0};
    return 0;
}

int gfx_dependency_invalidate_from(uint32_t changed_id, uint32_t *out_affected,
                                    uint32_t max_out, uint32_t *out_count) {
    uint32_t queue[GFX_DEPENDENCY_CAPACITY];
    uint32_t visited[GFX_DEPENDENCY_CAPACITY];
    uint32_t queue_count = 0;
    uint32_t visited_count = 0;
    uint32_t affected_count = 0;

    queue[queue_count++] = changed_id;
    visited[visited_count++] = changed_id;

    while (queue_count > 0) {
        uint32_t current = queue[0];
        for (uint32_t i = 1; i < queue_count; ++i) {
            queue[i - 1] = queue[i];
        }
        queue_count--;

        for (uint32_t i = 0; i < GFX_DEPENDENCY_CAPACITY; ++i) {
            if (!dependencies[i].in_use || dependencies[i].source_id != current) {
                continue;
            }
            dependencies[i].invalidation_total++;
            uint32_t next = dependencies[i].target_id;

            int already_visited = 0;
            for (uint32_t v = 0; v < visited_count; ++v) {
                if (visited[v] == next) {
                    already_visited = 1;
                    break;
                }
            }
            if (!already_visited) {
                if (visited_count < GFX_DEPENDENCY_CAPACITY && queue_count < GFX_DEPENDENCY_CAPACITY) {
                    visited[visited_count++] = next;
                    queue[queue_count++] = next;
                }
                if (affected_count < max_out) {
                    out_affected[affected_count++] = next;
                }
            }
        }
    }

    *out_count = affected_count;
    return 0;
}

int gfx_dependency_info(uint32_t edge_id, gfx_dependency_info_t *out_info) {
    gfx_dependency_t *e = dep_find(edge_id);
    if (!e) {
        return GFX_ERR_NOT_FOUND;
    }
    out_info->edge_id = e->edge_id;
    out_info->source_id = e->source_id;
    out_info->target_id = e->target_id;
    out_info->dep_type = e->dep_type;
    out_info->invalidation_total = e->invalidation_total;
    return 0;
}

int gfx_semantic_set(uint32_t element_id, uint32_t semantic_mask) {
    for (uint32_t i = 0; i < GFX_SEMANTIC_CAPACITY; ++i) {
        if (semantics[i].in_use && semantics[i].element_id == element_id) {
            semantics[i].semantic_mask = semantic_mask;
            return 0;
        }
    }
    for (uint32_t i = 0; i < GFX_SEMANTIC_CAPACITY; ++i) {
        if (!semantics[i].in_use) {
            semantics[i].element_id = element_id;
            semantics[i].semantic_mask = semantic_mask;
            semantics[i].in_use = 1;
            return 0;
        }
    }
    return GFX_ERR_CAPACITY;
}

uint32_t gfx_semantic_get(uint32_t element_id) {
    for (uint32_t i = 0; i < GFX_SEMANTIC_CAPACITY; ++i) {
        if (semantics[i].in_use && semantics[i].element_id == element_id) {
            return semantics[i].semantic_mask;
        }
    }
    return 0; /* keine Semantik bekannt - optional, kein Fehler (#7) */
}

uint32_t gfx_adaptive_select_strategy(uint32_t change_count, uint32_t damage_area,
                                       uint32_t total_area) {
    if (total_area == 0) {
        return GFX_STRATEGY_FULL; /* unbekannte Flaeche - konservativ */
    }
    if (damage_area * 2u >= total_area) {
        return GFX_STRATEGY_FULL; /* >=50% der Flaeche veraendert */
    }
    if (change_count > GFX_ADAPTIVE_CHANGE_LIMIT) {
        return GFX_STRATEGY_FULL;
    }
    return GFX_STRATEGY_PARTIAL;
}

static void propagate_commit(gfx_transaction_t *tx) {
    uint32_t affected[GFX_DEPENDENCY_CAPACITY];
    uint32_t affected_count;
    for (uint32_t i = 0; i < tx->change_count; ++i) {
        gfx_dependency_invalidate_from(tx->changes[i].element_id, affected,
                                        GFX_DEPENDENCY_CAPACITY, &affected_count);
    }
}

int gfx_transaction_begin(uint32_t *out_tx_id) {
    uint32_t slot;
    for (slot = 0; slot < GFX_TX_CAPACITY; ++slot) {
        if (!transactions[slot].ever_used) {
            break;
        }
    }
    if (slot >= GFX_TX_CAPACITY) {
        /* Alle Slots schon einmal benutzt: aeltesten abgeschlossenen
           (nicht mehr OPEN) wiederverwenden, statt unbegrenzt zu wachsen. */
        for (slot = 0; slot < GFX_TX_CAPACITY; ++slot) {
            if (transactions[slot].state != GFX_TX_STATE_OPEN) {
                break;
            }
        }
        if (slot >= GFX_TX_CAPACITY) {
            return GFX_ERR_CAPACITY; /* alle Slots aktuell OFFEN */
        }
    }

    transactions[slot] = (gfx_transaction_t){0};
    transactions[slot].tx_id = gfx_next_tx_id++;
    transactions[slot].state = GFX_TX_STATE_OPEN;
    transactions[slot].ever_used = 1;

    *out_tx_id = transactions[slot].tx_id;
    return 0;
}

int gfx_transaction_record_change(uint32_t tx_id, uint32_t element_id, uint32_t change_type,
                                   uint32_t property_id, uint32_t from_version, uint32_t to_version) {
    gfx_transaction_t *tx = tx_find(tx_id);
    if (!tx) {
        return GFX_ERR_NOT_FOUND;
    }
    if (tx->state != GFX_TX_STATE_OPEN) {
        return GFX_ERR_INVALID_STATE;
    }
    if (tx->change_count >= GFX_TX_MAX_CHANGES) {
        return GFX_ERR_CAPACITY;
    }

    gfx_change_record_t *rec = &tx->changes[tx->change_count++];
    rec->element_id = element_id;
    rec->change_type = change_type;
    rec->property_id = property_id;
    rec->from_version = from_version;
    rec->to_version = to_version;
    return 0;
}

int gfx_transaction_commit(uint32_t tx_id, uint32_t *out_state_version) {
    gfx_transaction_t *tx = tx_find(tx_id);
    if (!tx) {
        return GFX_ERR_NOT_FOUND;
    }
    if (tx->state != GFX_TX_STATE_OPEN) {
        return GFX_ERR_INVALID_STATE;
    }

    tx->state_version = gfx_next_state_version++;
    tx->state = GFX_TX_STATE_COMMITTED;
    propagate_commit(tx);

    *out_state_version = tx->state_version;
    return 0;
}

int gfx_transaction_abort(uint32_t tx_id) {
    gfx_transaction_t *tx = tx_find(tx_id);
    if (!tx) {
        return GFX_ERR_NOT_FOUND;
    }
    if (tx->state != GFX_TX_STATE_OPEN) {
        return GFX_ERR_INVALID_STATE;
    }
    tx->state = GFX_TX_STATE_ABORTED;
    /* state_version bleibt 0 - eine abgebrochene Transaktion verbraucht
       NIEMALS eine Zustandsversion (#9 Fehlerbehandlung). */
    return 0;
}

int gfx_transaction_info(uint32_t tx_id, gfx_transaction_info_t *out_info) {
    gfx_transaction_t *tx = tx_find(tx_id);
    if (!tx) {
        return GFX_ERR_NOT_FOUND;
    }
    out_info->tx_id = tx->tx_id;
    out_info->state = tx->state;
    out_info->state_version = tx->state_version;
    out_info->change_count = tx->change_count;
    return 0;
}

/* ----------------------------------------------------------------------- */
/* Selbsttest                                                              */
/* ----------------------------------------------------------------------- */

int gfxstate_self_test(void) {
    if (!gfxstate_ready) {
        return -1;
    }

    const uint32_t ELEM_A = 100u, ELEM_B = 200u, ELEM_C = 300u;

    /* Transaktion: sammeln -> committen -> Version/Change-Count pruefen. */
    uint32_t tx1;
    if (gfx_transaction_begin(&tx1) != 0) return -1;
    if (gfx_transaction_record_change(tx1, ELEM_A, GFX_CHANGE_GEOMETRY, 1, 0, 1) != 0) return -1;
    if (gfx_transaction_record_change(tx1, ELEM_B, GFX_CHANGE_MATERIAL, 2, 0, 1) != 0) return -1;

    uint32_t v1;
    if (gfx_transaction_commit(tx1, &v1) != 0) return -1;
    if (v1 == 0) return -1;

    gfx_transaction_info_t txinfo;
    if (gfx_transaction_info(tx1, &txinfo) != 0) return -1;
    if (txinfo.state != GFX_TX_STATE_COMMITTED || txinfo.change_count != 2 || txinfo.state_version != v1) return -1;

    /* Doppeltes Commit/Aendern einer abgeschlossenen Transaktion -> Fehler. */
    uint32_t v_dummy;
    if (gfx_transaction_commit(tx1, &v_dummy) != GFX_ERR_INVALID_STATE) return -1;
    if (gfx_transaction_record_change(tx1, ELEM_A, GFX_CHANGE_PROPERTY, 0, 0, 0) != GFX_ERR_INVALID_STATE) return -1;

    /* Abort: verbraucht NIE eine Zustandsversion. */
    uint32_t tx2;
    if (gfx_transaction_begin(&tx2) != 0) return -1;
    if (gfx_transaction_record_change(tx2, ELEM_A, GFX_CHANGE_VISIBILITY, 3, 0, 1) != 0) return -1;
    if (gfx_transaction_abort(tx2) != 0) return -1;
    if (gfx_transaction_info(tx2, &txinfo) != 0) return -1;
    if (txinfo.state != GFX_TX_STATE_ABORTED || txinfo.state_version != 0) return -1;

    uint32_t tx3;
    if (gfx_transaction_begin(&tx3) != 0) return -1;
    uint32_t v3;
    if (gfx_transaction_commit(tx3, &v3) != 0) return -1;
    if (v3 != v1 + 1) return -1; /* Abort hat keine Versionsnummer verbraucht */

    /* Dependency Graph: A->B->C, Zyklus C->A muss abgewiesen werden. */
    uint32_t edge_ab, edge_bc, edge_dummy;
    if (gfx_dependency_add(ELEM_A, ELEM_B, GFX_DEP_MATERIAL, &edge_ab) != 0) return -1;
    if (gfx_dependency_add(ELEM_B, ELEM_C, GFX_DEP_LAYOUT, &edge_bc) != 0) return -1;
    if (gfx_dependency_add(ELEM_C, ELEM_A, GFX_DEP_PROPERTY, &edge_dummy) != GFX_ERR_CYCLE) return -1;
    if (gfx_dependency_add(ELEM_A, ELEM_A, GFX_DEP_PROPERTY, &edge_dummy) != GFX_ERR_CYCLE) return -1;

    /* Transitive Propagation: Aenderung an A muss B UND C erfassen. */
    uint32_t affected[8];
    uint32_t affected_count;
    if (gfx_dependency_invalidate_from(ELEM_A, affected, 8, &affected_count) != 0) return -1;
    if (affected_count != 2) return -1;
    int saw_b = 0, saw_c = 0;
    for (uint32_t i = 0; i < affected_count; ++i) {
        if (affected[i] == ELEM_B) saw_b = 1;
        if (affected[i] == ELEM_C) saw_c = 1;
    }
    if (!saw_b || !saw_c) return -1;

    gfx_dependency_info_t depinfo;
    if (gfx_dependency_info(edge_ab, &depinfo) != 0) return -1;
    if (depinfo.invalidation_total != 1) return -1; /* genau ein invalidate_from(A)-Durchlauf bisher */

    /* Kante entfernen: danach darf C ueber A nicht mehr erreicht werden. */
    if (gfx_dependency_remove(edge_bc) != 0) return -1;
    if (gfx_dependency_invalidate_from(ELEM_A, affected, 8, &affected_count) != 0) return -1;
    if (affected_count != 1 || affected[0] != ELEM_B) return -1;
    if (gfx_dependency_info(edge_bc, &depinfo) != GFX_ERR_NOT_FOUND) return -1;

    /* Commit propagiert automatisch: Aenderung an B in tx1 oben haette
       bereits (B hat keine ausgehenden Kanten mehr nach dem remove) keine
       Auswirkung - stattdessen hier gezielt pruefen, dass ein Commit mit
       Aenderung an A erneut auf B propagiert (edge_ab zaehlt weiter hoch). */
    uint32_t tx4;
    if (gfx_transaction_begin(&tx4) != 0) return -1;
    if (gfx_transaction_record_change(tx4, ELEM_A, GFX_CHANGE_TRANSFORM, 4, 1, 2) != 0) return -1;
    uint32_t v4;
    if (gfx_transaction_commit(tx4, &v4) != 0) return -1;
    if (gfx_dependency_info(edge_ab, &depinfo) != 0) return -1;
    if (depinfo.invalidation_total != 3) return -1; /* 1 (initial) + 1 (remove-Test) + 1 (dieser Commit) */

    /* Semantik: optional, Bitmaske, unbekanntes Element liefert 0. */
    if (gfx_semantic_get(ELEM_A) != 0) return -1;
    if (gfx_semantic_set(ELEM_A, GFX_SEM_STRUCTURAL | GFX_SEM_CONTENT) != 0) return -1;
    if (gfx_semantic_get(ELEM_A) != (GFX_SEM_STRUCTURAL | GFX_SEM_CONTENT)) return -1;
    if (gfx_semantic_set(ELEM_A, GFX_SEM_DECORATIVE) != 0) return -1; /* ueberschreiben */
    if (gfx_semantic_get(ELEM_A) != GFX_SEM_DECORATIVE) return -1;

    /* Adaptive Strategie: deterministische Regeln. */
    if (gfx_adaptive_select_strategy(1, 10, 0) != GFX_STRATEGY_FULL) return -1;       /* total_area unbekannt */
    if (gfx_adaptive_select_strategy(1, 10, 1000) != GFX_STRATEGY_PARTIAL) return -1;  /* 1% Flaeche */
    if (gfx_adaptive_select_strategy(1, 600, 1000) != GFX_STRATEGY_FULL) return -1;    /* 60% Flaeche */
    if (gfx_adaptive_select_strategy(100, 1, 1000) != GFX_STRATEGY_FULL) return -1;    /* viele Einzelaenderungen */

    /* Unbekannte IDs. */
    if (gfx_transaction_info(0xFFFFu, &txinfo) != GFX_ERR_NOT_FOUND) return -1;
    if (gfx_dependency_info(0xFFFFu, &depinfo) != GFX_ERR_NOT_FOUND) return -1;

    return 0;
}

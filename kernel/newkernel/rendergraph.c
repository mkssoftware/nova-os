/* Dynamisch wachsende Resource-/Pass-/Edge-Tabellen (2026-11-05, Antwort
   auf "viele feste Grenzen nicht dynamisch loesbar" - siehe
   [[project_general_allocator]], siebtes umgestelltes Modul).
   RG_RESOURCE_CAPACITY/RG_PASS_CAPACITY/RG_EDGE_CAPACITY (rendergraph.h)
   sind jetzt nur noch Startkapazitaeten. RG_PASS_MAX_RESOURCES (In-
   Struct-Array pro Pass) bleibt bewusst UNVERAENDERT - eine echte
   Praxisgrenze (Ein-/Ausgaben EINES Passes), keine wachsende Sammlung.
   `current_plan[]` (bisher statisch RG_PASS_CAPACITY) UND die lokalen
   Scratch-Arrays in rg_build() (`needed[]`/`in_degree[]`/`queue[]`,
   bisher ebenfalls fest RG_PASS_CAPACITY) muessen mit passes_capacity
   mitwachsen - rg_build() alloziert sie jetzt dynamisch auf die
   AKTUELLE Kapazitaet und gibt sie ueber einen gemeinsamen
   Cleanup-Pfad frei (die Funktion hat mehrere Rueckgabestellen). */
#include "rendergraph.h"
#include "renderengine.h"
#include "kalloc.h"
#include <stddef.h>

typedef struct {
    uint32_t resource_id;
    uint32_t resource_class;
    int in_use;
} rg_resource_t;

typedef struct {
    uint32_t pass_id;
    uint32_t pass_type;
    uint32_t lifecycle_state;
    uint32_t node_id;
    uint32_t region_id;
    uint32_t inputs[RG_PASS_MAX_RESOURCES];
    uint32_t input_count;
    uint32_t outputs[RG_PASS_MAX_RESOURCES];
    uint32_t output_count;
    uint32_t result_value;
    int required;
    int in_use;
} rg_pass_t;

typedef struct {
    uint32_t pass_a; /* muss vor pass_b laufen */
    uint32_t pass_b;
    uint32_t resource_id;
    uint32_t hazard_type;
    int in_use;
} rg_edge_t;

static rg_resource_t *resources = NULL;
static uint32_t resources_capacity = 0;
static rg_pass_t *passes = NULL;
static uint32_t passes_capacity = 0;
static rg_edge_t *edges = NULL;
static uint32_t edges_capacity = 0;
static uint32_t rg_next_resource_id = 1;
static uint32_t rg_next_pass_id = 1;
static uint32_t edge_count = 0;

static uint32_t *current_plan = NULL;
static uint32_t current_plan_capacity = 0;
static uint32_t current_plan_count = 0;
static int plan_valid = 0;

static int rendergraph_ready = 0;

void rendergraph_initialize(void) {
    if (resources) kfree(resources);
    resources = (rg_resource_t *)kcalloc(RG_RESOURCE_CAPACITY, sizeof(rg_resource_t));
    resources_capacity = resources ? RG_RESOURCE_CAPACITY : 0u;

    if (passes) kfree(passes);
    passes = (rg_pass_t *)kcalloc(RG_PASS_CAPACITY, sizeof(rg_pass_t));
    passes_capacity = passes ? RG_PASS_CAPACITY : 0u;

    if (edges) kfree(edges);
    edges = (rg_edge_t *)kcalloc(RG_EDGE_CAPACITY, sizeof(rg_edge_t));
    edges_capacity = edges ? RG_EDGE_CAPACITY : 0u;

    if (current_plan) kfree(current_plan);
    current_plan = NULL;
    current_plan_capacity = 0;

    rg_next_resource_id = 1;
    rg_next_pass_id = 1;
    edge_count = 0;
    current_plan_count = 0;
    plan_valid = 0;
    rendergraph_ready = 1;
}

/* Verdoppelt resources[]/passes[]/edges[] (identisches Muster wie die
   bereits umgestellten Module). 0 = Erfolg (*out_slot = erster freier
   Slot), -1 bei Erschoepfung (Tabelle/Kapazitaet bleiben unveraendert
   gueltig). */
static int resources_grow(uint32_t *out_slot) {
    uint32_t new_capacity = (resources_capacity == 0u) ? RG_RESOURCE_CAPACITY : resources_capacity * 2u;
    rg_resource_t *grown = (rg_resource_t *)krealloc(resources, new_capacity * (uint32_t)sizeof(rg_resource_t));
    if (!grown) return -1;
    for (uint32_t i = resources_capacity; i < new_capacity; ++i) grown[i] = (rg_resource_t){0};
    *out_slot = resources_capacity;
    resources = grown;
    resources_capacity = new_capacity;
    return 0;
}

static int passes_grow(uint32_t *out_slot) {
    uint32_t new_capacity = (passes_capacity == 0u) ? RG_PASS_CAPACITY : passes_capacity * 2u;
    rg_pass_t *grown = (rg_pass_t *)krealloc(passes, new_capacity * (uint32_t)sizeof(rg_pass_t));
    if (!grown) return -1;
    for (uint32_t i = passes_capacity; i < new_capacity; ++i) grown[i] = (rg_pass_t){0};
    *out_slot = passes_capacity;
    passes = grown;
    passes_capacity = new_capacity;
    return 0;
}

static int edges_grow(void) {
    uint32_t new_capacity = (edges_capacity == 0u) ? RG_EDGE_CAPACITY : edges_capacity * 2u;
    rg_edge_t *grown = (rg_edge_t *)krealloc(edges, new_capacity * (uint32_t)sizeof(rg_edge_t));
    if (!grown) return -1;
    for (uint32_t i = edges_capacity; i < new_capacity; ++i) grown[i] = (rg_edge_t){0};
    edges = grown;
    edges_capacity = new_capacity;
    return 0;
}

/* Stellt sicher, dass current_plan[] mindestens needed_capacity Eintraege
   fassen kann (current_plan wird bei jedem rg_build() ohnehin vollstaendig
   neu befuellt - ein einfaches krealloc() auf Bedarf reicht, kein
   Zero-Fill des Zuwachses noetig). 0 = Erfolg, -1 bei Erschoepfung. */
static int ensure_current_plan_capacity(uint32_t needed_capacity) {
    if (current_plan_capacity >= needed_capacity) return 0;
    uint32_t *grown = (uint32_t *)krealloc(current_plan, needed_capacity * (uint32_t)sizeof(uint32_t));
    if (!grown) return -1;
    current_plan = grown;
    current_plan_capacity = needed_capacity;
    return 0;
}

static rg_pass_t *pass_find(uint32_t pass_id) {
    if (pass_id == 0) return NULL;
    for (uint32_t i = 0; i < passes_capacity; ++i) {
        if (passes[i].in_use && passes[i].pass_id == pass_id) return &passes[i];
    }
    return NULL;
}

static int resource_exists(uint32_t resource_id) {
    for (uint32_t i = 0; i < resources_capacity; ++i) {
        if (resources[i].in_use && resources[i].resource_id == resource_id) return 1;
    }
    return 0;
}

static int has_resource(const uint32_t *list, uint32_t count, uint32_t resource_id) {
    for (uint32_t i = 0; i < count; ++i) {
        if (list[i] == resource_id) return 1;
    }
    return 0;
}

int rg_resource_declare(uint32_t resource_class, uint32_t *out_resource_id) {
    uint32_t slot;
    for (slot = 0; slot < resources_capacity; ++slot) {
        if (!resources[slot].in_use) break;
    }
    if (slot >= resources_capacity) {
        if (resources_grow(&slot) != 0) return RG_ERR_CAPACITY;
    }
    resources[slot].resource_id = rg_next_resource_id++;
    resources[slot].resource_class = resource_class;
    resources[slot].in_use = 1;
    *out_resource_id = resources[slot].resource_id;
    return 0;
}

int rg_pass_declare(uint32_t pass_type, uint32_t node_id, uint32_t region_id,
                    uint32_t *out_pass_id) {
    uint32_t slot;
    for (slot = 0; slot < passes_capacity; ++slot) {
        if (!passes[slot].in_use) break;
    }
    if (slot >= passes_capacity) {
        if (passes_grow(&slot) != 0) return RG_ERR_CAPACITY;
    }

    passes[slot] = (rg_pass_t){0};
    passes[slot].pass_id = rg_next_pass_id++;
    passes[slot].pass_type = pass_type;
    passes[slot].lifecycle_state = RG_PASS_DECLARED;
    passes[slot].node_id = node_id;
    passes[slot].region_id = region_id;
    passes[slot].in_use = 1;

    *out_pass_id = passes[slot].pass_id;
    plan_valid = 0;
    return 0;
}

int rg_pass_add_input(uint32_t pass_id, uint32_t resource_id) {
    rg_pass_t *p = pass_find(pass_id);
    if (!p) return RG_ERR_NOT_FOUND;
    if (!resource_exists(resource_id)) return RG_ERR_NOT_FOUND;
    if (p->input_count >= RG_PASS_MAX_RESOURCES) return RG_ERR_CAPACITY;
    p->inputs[p->input_count++] = resource_id;
    plan_valid = 0;
    return 0;
}

int rg_pass_add_output(uint32_t pass_id, uint32_t resource_id) {
    rg_pass_t *p = pass_find(pass_id);
    if (!p) return RG_ERR_NOT_FOUND;
    if (!resource_exists(resource_id)) return RG_ERR_NOT_FOUND;
    if (p->output_count >= RG_PASS_MAX_RESOURCES) return RG_ERR_CAPACITY;
    p->outputs[p->output_count++] = resource_id;
    plan_valid = 0;
    return 0;
}

int rg_pass_mark_required(uint32_t pass_id) {
    rg_pass_t *p = pass_find(pass_id);
    if (!p) return RG_ERR_NOT_FOUND;
    p->required = 1;
    plan_valid = 0;
    return 0;
}

static int add_edge(uint32_t pass_a, uint32_t pass_b, uint32_t resource_id, uint32_t hazard_type) {
    if (edge_count >= edges_capacity) {
        if (edges_grow() != 0) return RG_ERR_CAPACITY;
    }
    edges[edge_count].pass_a = pass_a;
    edges[edge_count].pass_b = pass_b;
    edges[edge_count].resource_id = resource_id;
    edges[edge_count].hazard_type = hazard_type;
    edge_count++;
    return 0;
}

int rg_build(uint32_t *out_order, uint32_t max_out, uint32_t *out_count) {
    edge_count = 0;
    plan_valid = 0;

    /* 1. RAW/WAR/WAW-Kanten aus Ressourcenzugriffen ableiten. Kanten laufen
       IMMER vom frueher deklarierten zum spaeter deklarierten Pass - dadurch
       ist der resultierende Graph strukturell azyklisch (#6 Zyklus-
       Erkennung ist dadurch trivial erfuellt, nicht umgangen). */
    for (uint32_t i = 0; i < passes_capacity; ++i) {
        if (!passes[i].in_use) continue;
        for (uint32_t j = 0; j < passes_capacity; ++j) {
            if (!passes[j].in_use || passes[j].pass_id <= passes[i].pass_id) continue;
            rg_pass_t *a = &passes[i];
            rg_pass_t *b = &passes[j];

            for (uint32_t r = 0; r < a->output_count; ++r) {
                uint32_t res = a->outputs[r];
                if (has_resource(b->inputs, b->input_count, res)) {
                    int rc = add_edge(a->pass_id, b->pass_id, res, RG_HAZARD_RAW);
                    if (rc != 0) return rc;
                }
                if (has_resource(b->outputs, b->output_count, res)) {
                    int rc = add_edge(a->pass_id, b->pass_id, res, RG_HAZARD_WAW);
                    if (rc != 0) return rc;
                }
            }
            for (uint32_t r = 0; r < a->input_count; ++r) {
                uint32_t res = a->inputs[r];
                if (has_resource(b->outputs, b->output_count, res)) {
                    int rc = add_edge(a->pass_id, b->pass_id, res, RG_HAZARD_WAR);
                    if (rc != 0) return rc;
                }
            }
        }
    }

    /* needed[]/in_degree[]/queue[] auf die AKTUELLE passes_capacity
       dynamisch alloziert (bisher fest RG_PASS_CAPACITY grosse lokale
       Arrays) - ueber einen gemeinsamen Cleanup-Pfad freigegeben, da
       diese Funktion mehrere Rueckgabestellen hat. */
    int *needed = (int *)kcalloc(passes_capacity, sizeof(int));
    uint32_t *in_degree = (uint32_t *)kcalloc(passes_capacity, sizeof(uint32_t));
    uint32_t *queue = (uint32_t *)kcalloc(passes_capacity, sizeof(uint32_t));
    int result = 0;
    uint32_t produced = 0;

    if (!needed || !in_degree || !queue) {
        result = RG_ERR_CAPACITY;
        goto cleanup;
    }
    if (ensure_current_plan_capacity(passes_capacity) != 0) {
        result = RG_ERR_CAPACITY;
        goto cleanup;
    }

    /* 2. Dead Pass Elimination: rueckwaerts ab den required-Passes entlang
       RAW-Kanten (nur RAW beschreibt eine echte Produzenten-Abhaengigkeit -
       WAR/WAW sind reine Ausfuehrungsordnung, kein Bedarf). */
    for (uint32_t i = 0; i < passes_capacity; ++i) {
        needed[i] = passes[i].in_use && passes[i].required;
    }
    int changed = 1;
    while (changed) {
        changed = 0;
        for (uint32_t e = 0; e < edge_count; ++e) {
            if (edges[e].hazard_type != RG_HAZARD_RAW) continue;
            rg_pass_t *target = pass_find(edges[e].pass_b);
            rg_pass_t *source = pass_find(edges[e].pass_a);
            if (!target || !source) continue;
            uint32_t ti = (uint32_t)(target - passes);
            uint32_t si = (uint32_t)(source - passes);
            if (needed[ti] && !needed[si]) {
                needed[si] = 1;
                changed = 1;
            }
        }
    }

    for (uint32_t i = 0; i < passes_capacity; ++i) {
        if (!passes[i].in_use) continue;
        passes[i].lifecycle_state = needed[i] ? RG_PASS_SCHEDULED : RG_PASS_CULLED;
    }

    /* 3. Topologische Sortierung (Kahn) ausschliesslich ueber benoetigte
       Passes und Kanten, deren beide Enden benoetigt werden. */
    for (uint32_t e = 0; e < edge_count; ++e) {
        rg_pass_t *source = pass_find(edges[e].pass_a);
        rg_pass_t *target = pass_find(edges[e].pass_b);
        if (!source || !target) continue;
        uint32_t si = (uint32_t)(source - passes);
        uint32_t ti = (uint32_t)(target - passes);
        if (needed[si] && needed[ti]) in_degree[ti]++;
    }

    uint32_t queue_count = 0;
    for (uint32_t i = 0; i < passes_capacity; ++i) {
        if (passes[i].in_use && needed[i] && in_degree[i] == 0) {
            queue[queue_count++] = i;
        }
    }

    uint32_t total_needed = 0;
    for (uint32_t i = 0; i < passes_capacity; ++i) {
        if (passes[i].in_use && needed[i]) total_needed++;
    }

    uint32_t qi = 0;
    while (qi < queue_count) {
        uint32_t idx = queue[qi++];
        if (produced < max_out) {
            current_plan[produced] = passes[idx].pass_id;
        }
        produced++;

        for (uint32_t e = 0; e < edge_count; ++e) {
            rg_pass_t *source = pass_find(edges[e].pass_a);
            rg_pass_t *target = pass_find(edges[e].pass_b);
            if (!source || !target) continue;
            uint32_t si = (uint32_t)(source - passes);
            uint32_t ti = (uint32_t)(target - passes);
            if (si != idx || !needed[si] || !needed[ti]) continue;
            if (--in_degree[ti] == 0) {
                queue[queue_count++] = ti;
            }
        }
    }

    if (produced != total_needed) {
        result = RG_ERR_CYCLE; /* sicherheitshalber erkannt, strukturell hier ausgeschlossen */
        goto cleanup;
    }

    current_plan_count = (produced < max_out) ? produced : max_out;
    plan_valid = 1;

    for (uint32_t i = 0; i < current_plan_count && i < max_out; ++i) {
        out_order[i] = current_plan[i];
    }
    *out_count = produced;

cleanup:
    kfree(needed);
    kfree(in_degree);
    kfree(queue);
    return result;
}

int rg_execute(uint32_t damage_area, uint32_t total_area) {
    if (!plan_valid) return RG_ERR_INVALID;

    for (uint32_t i = 0; i < current_plan_count; ++i) {
        rg_pass_t *p = pass_find(current_plan[i]);
        if (!p) continue;

        if (p->node_id != 0) {
            uint32_t result_value, strategy;
            int cache_hit;
            render_execute_retained(p->node_id, p->region_id, damage_area, total_area,
                                     &result_value, &cache_hit, &strategy);
            p->result_value = result_value;
        } else {
            p->result_value = 0;
        }
        p->lifecycle_state = RG_PASS_COMPLETED;
    }
    return 0;
}

int rg_pass_info(uint32_t pass_id, rg_pass_info_t *out_info) {
    rg_pass_t *p = pass_find(pass_id);
    if (!p) return RG_ERR_NOT_FOUND;
    out_info->pass_id = p->pass_id;
    out_info->pass_type = p->pass_type;
    out_info->lifecycle_state = p->lifecycle_state;
    out_info->node_id = p->node_id;
    out_info->input_count = p->input_count;
    out_info->output_count = p->output_count;
    out_info->result_value = p->result_value;
    out_info->required = p->required;
    return 0;
}

int rg_hazard_between(uint32_t pass_a, uint32_t pass_b, uint32_t resource_id,
                      uint32_t *out_hazard_type) {
    for (uint32_t e = 0; e < edge_count; ++e) {
        if (edges[e].pass_a == pass_a && edges[e].pass_b == pass_b &&
            edges[e].resource_id == resource_id) {
            *out_hazard_type = edges[e].hazard_type;
            return 0;
        }
    }
    return RG_ERR_NOT_FOUND;
}

/* ----------------------------------------------------------------------- */
/* Selbsttest                                                              */
/* ----------------------------------------------------------------------- */

#include "scenegraph.h"

int rendergraph_self_test(void) {
    if (!rendergraph_ready) return -1;

    uint32_t scene1, root1;
    if (scene_create(&scene1, &root1) != 0) return -1;
    uint32_t tx1, nodeA;
    if (scene_transaction_begin(scene1, &tx1) != 0) return -1;
    if (scene_node_create(tx1, root1, SCENE_NODE_TYPE_GEOMETRY, &nodeA) != 0) return -1;
    uint32_t v1;
    if (scene_transaction_commit(tx1, &v1) != 0) return -1;

    uint32_t resA, resB, resC;
    if (rg_resource_declare(RG_RESOURCE_TRANSIENT, &resA) != 0) return -1;
    if (rg_resource_declare(RG_RESOURCE_TRANSIENT, &resB) != 0) return -1;
    if (rg_resource_declare(RG_RESOURCE_TRANSIENT, &resC) != 0) return -1;

    uint32_t passA, passB, passC, passD;
    if (rg_pass_declare(RG_PASS_TYPE_GRAPHICS, nodeA, 0, &passA) != 0) return -1;
    if (rg_pass_add_output(passA, resA) != 0) return -1;

    if (rg_pass_declare(RG_PASS_TYPE_EFFECT, 0, 0, &passB) != 0) return -1;
    if (rg_pass_add_input(passB, resA) != 0) return -1;
    if (rg_pass_add_output(passB, resB) != 0) return -1;

    if (rg_pass_declare(RG_PASS_TYPE_COMPOSITION, 0, 0, &passC) != 0) return -1;
    if (rg_pass_add_input(passC, resB) != 0) return -1;
    if (rg_pass_mark_required(passC) != 0) return -1;

    /* Unbenoetigter Pass: schreibt resC, das niemand liest, nicht required
       -> muss per Dead Pass Elimination entfallen. */
    if (rg_pass_declare(RG_PASS_TYPE_GRAPHICS, 0, 0, &passD) != 0) return -1;
    if (rg_pass_add_output(passD, resC) != 0) return -1;

    uint32_t order[RG_PASS_CAPACITY];
    uint32_t count;
    if (rg_build(order, RG_PASS_CAPACITY, &count) != 0) return -1;
    if (count != 3) return -1; /* passD wurde gecullt */

    /* Topologische Reihenfolge: passA vor passB vor passC. */
    int idx_a = -1, idx_b = -1, idx_c = -1;
    for (uint32_t i = 0; i < count; ++i) {
        if (order[i] == passA) idx_a = (int)i;
        if (order[i] == passB) idx_b = (int)i;
        if (order[i] == passC) idx_c = (int)i;
    }
    if (idx_a < 0 || idx_b < 0 || idx_c < 0) return -1;
    if (!(idx_a < idx_b && idx_b < idx_c)) return -1;

    rg_pass_info_t pinfo;
    if (rg_pass_info(passD, &pinfo) != 0) return -1;
    if (pinfo.lifecycle_state != RG_PASS_CULLED) return -1;
    if (rg_pass_info(passA, &pinfo) != 0) return -1;
    if (pinfo.lifecycle_state != RG_PASS_SCHEDULED) return -1;

    uint32_t hazard;
    if (rg_hazard_between(passA, passB, resA, &hazard) != 0) return -1;
    if (hazard != RG_HAZARD_RAW) return -1;
    if (rg_hazard_between(passB, passC, resB, &hazard) != 0) return -1;
    if (hazard != RG_HAZARD_RAW) return -1;

    /* Ausfuehrung: passA ist an nodeA gebunden -> echter Aufruf von
       render_execute_retained() (Phase 3). Ein direkter Vergleichsaufruf
       mit denselben Parametern muss (Cache Hit) denselben Wert liefern. */
    if (rg_execute(10, 1000) != 0) return -1;
    if (rg_pass_info(passA, &pinfo) != 0) return -1;
    if (pinfo.lifecycle_state != RG_PASS_COMPLETED) return -1;

    uint32_t direct_result, direct_strategy;
    int direct_hit;
    if (render_execute_retained(nodeA, 0, 10, 1000, &direct_result, &direct_hit, &direct_strategy) != 0) return -1;
    if (direct_hit != 1) return -1; /* durch rg_execute() bereits berechnet */
    if (pinfo.result_value != direct_result) return -1;

    if (rg_pass_info(passB, &pinfo) != 0) return -1;
    if (pinfo.lifecycle_state != RG_PASS_COMPLETED || pinfo.result_value != 0) return -1;

    /* Unbekannte IDs. */
    if (rg_pass_add_input(0xFFFFu, resA) != RG_ERR_NOT_FOUND) return -1;
    if (rg_pass_add_output(passA, 0xFFFFu) != RG_ERR_NOT_FOUND) return -1;
    if (rg_pass_info(0xFFFFu, &pinfo) != RG_ERR_NOT_FOUND) return -1;
    if (rg_hazard_between(passA, passC, resA, &hazard) != RG_ERR_NOT_FOUND) return -1;

    /* Beweist die eigentliche Aenderung dieser Session: eine lange Kette
       aus GROWTH_N Passes (jeder liest die Ausgabe seines Vorgaengers und
       schreibt eine neue, eigene Ressource) erzwingt mindestens ein
       passes_grow() (GROWTH_N > RG_PASS_CAPACITY), resources_grow()
       (GROWTH_N Ressourcen > RG_RESOURCE_CAPACITY) UND edges_grow()
       (GROWTH_N-1 RAW-Kanten > RG_EDGE_CAPACITY) - mit der urspruenglichen
       festen Kapazitaet waere spaetestens rg_resource_declare() oder
       rg_pass_declare() hier fehlgeschlagen. */
    const uint32_t GROWTH_N = 40u;
    /* Auf dem kalloc-Heap statt auf dem Stack (dieses freestanding Target
       hat kein Laufzeit-chkstk/keine Stack-Probe-Unterstuetzung - drei
       Arrays dieser Groesse zusaetzlich zu den bereits vorhandenen
       lokalen Variablen dieser Funktion hoben das Stack-Frame ueber die
       Schwelle, ab der der Compiler einen __chkstk_ms-Aufruf einfuegt,
       der hier nicht linkbar ist). */
    uint32_t *growth_res = (uint32_t *)kalloc(GROWTH_N * (uint32_t)sizeof(uint32_t));
    uint32_t *growth_pass = (uint32_t *)kalloc(GROWTH_N * (uint32_t)sizeof(uint32_t));
    uint32_t *growth_order = (uint32_t *)kalloc(GROWTH_N * (uint32_t)sizeof(uint32_t));
    if (!growth_res || !growth_pass || !growth_order) return -1;

    for (uint32_t i = 0; i < GROWTH_N; ++i) {
        if (rg_resource_declare(RG_RESOURCE_TRANSIENT, &growth_res[i]) != 0) return -1;
    }
    for (uint32_t i = 0; i < GROWTH_N; ++i) {
        if (rg_pass_declare(RG_PASS_TYPE_GRAPHICS, 0, 0, &growth_pass[i]) != 0) return -1;
        if (i > 0) {
            if (rg_pass_add_input(growth_pass[i], growth_res[i - 1]) != 0) return -1;
        }
        if (rg_pass_add_output(growth_pass[i], growth_res[i]) != 0) return -1;
    }
    if (rg_pass_mark_required(growth_pass[GROWTH_N - 1u]) != 0) return -1;

    /* rg_build() wertet IMMER alle noch in_use-Passes aus, nicht nur die
       dieser Kette - die frueher im Test als required markierten passA/
       passB/passC bleiben als eigene, unabhaengige Komponente im Graphen
       bestehen und werden hier mit ausgewertet. Der erste Aufruf dient nur
       dazu, die tatsaechliche Gesamtzahl (growth_count, hier typischerweise
       GROWTH_N + 3) zu ermitteln, bevor ein ausreichend grosser Puffer fuer
       die vollstaendige Reihenfolge alloziert wird - absolute Positionen/
       Anzahlen waeren hier fragil, eine Positions-/Reihenfolgepruefung
       innerhalb des tatsaechlichen Ergebnisses ist robust. */
    uint32_t growth_count;
    if (rg_build(growth_order, GROWTH_N, &growth_count) != 0) return -1;
    if (growth_count < GROWTH_N) return -1; /* mindestens die gesamte Kette muss benoetigt werden */

    kfree(growth_order);
    growth_order = (uint32_t *)kalloc(growth_count * (uint32_t)sizeof(uint32_t));
    if (!growth_order) return -1;
    uint32_t growth_count2;
    if (rg_build(growth_order, growth_count, &growth_count2) != 0) return -1;
    if (growth_count2 != growth_count) return -1;

    int growth_idx_first = -1, growth_idx_last = -1;
    for (uint32_t i = 0; i < growth_count; ++i) {
        if (growth_order[i] == growth_pass[0]) growth_idx_first = (int)i;
        if (growth_order[i] == growth_pass[GROWTH_N - 1u]) growth_idx_last = (int)i;
    }
    if (growth_idx_first < 0 || growth_idx_last < 0) return -1;
    if (!(growth_idx_first < growth_idx_last)) return -1; /* erster Pass der Kette laeuft vor dem required Endpunkt */

    kfree(growth_res);
    kfree(growth_pass);
    kfree(growth_order);

    return 0;
}

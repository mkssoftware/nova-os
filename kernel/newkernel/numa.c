/* NovaOS C-Kernel - Nova NUMA Topology (siehe numa.h fuer Scope). */
#include "numa.h"
#include "pmm.h"

/* Ohne ACPI/SRAT taggt pmm.c JEDEN Frame mit PMM_NUMA_UNKNOWN, niemals
   mit einer konkreten Node-Nummer (siehe numa.h-Scope-Hinweis). Der
   einzige registrierte logische Node (0) muss deshalb auf genau diesen
   physischen Tag abgebildet werden, sonst findet pmm_alloc_page_strict()
   nie einen passenden Frame. */
static uint32_t numa_node_to_pmm_tag(uint32_t node) {
    if (node == 0u) return PMM_NUMA_UNKNOWN;
    return node; /* fuer spaetere echte Mehrknoten-Tags (ACPI/SRAT) unveraendert */
}

static uint32_t g_node_exists[NUMA_NODE_CAPACITY];
static uint32_t g_node_count = 0;
static uint32_t g_interleave_cursor = 0;
static uint32_t g_strict_denial_count = 0;

void numa_initialize(void) {
    for (uint32_t i = 0; i < NUMA_NODE_CAPACITY; ++i) g_node_exists[i] = 0;
    /* Siehe numa.h-Scope-Hinweis: ohne ACPI/SRAT gibt es nur Node 0. */
    g_node_exists[0] = 1;
    g_node_count = 1;
    g_interleave_cursor = 0;
    g_strict_denial_count = 0;
}

uint32_t numa_node_count(void) { return g_node_count; }

int numa_node_exists(uint32_t node) {
    if (node >= NUMA_NODE_CAPACITY) return 0;
    return (int)g_node_exists[node];
}

static uint32_t next_interleave_node(void) {
    for (uint32_t tries = 0; tries < NUMA_NODE_CAPACITY; ++tries) {
        uint32_t candidate = g_interleave_cursor;
        g_interleave_cursor = (g_interleave_cursor + 1u) % NUMA_NODE_CAPACITY;
        if (g_node_exists[candidate]) {
            return candidate;
        }
    }
    return 0; /* unerreichbar, solange numa_initialize() mind. Node 0 registriert hat */
}

int numa_alloc_page(uint32_t preferred_node, uint32_t policy, uint32_t *out_page, uint32_t *out_numa_node) {
    if (policy == NUMA_POLICY_STRICT) {
        if (!numa_node_exists(preferred_node)) {
            ++g_strict_denial_count;
            return -1;
        }
        uint32_t numa_got;
        uint32_t page = pmm_alloc_page_strict(numa_node_to_pmm_tag(preferred_node), &numa_got);
        if (page == 0) {
            ++g_strict_denial_count;
            return -1;
        }
        *out_page = page;
        *out_numa_node = preferred_node;
        return 0;
    }

    if (policy == NUMA_POLICY_INTERLEAVED) {
        uint32_t node = next_interleave_node();
        uint32_t numa_got;
        uint32_t page = pmm_alloc_page_preferred(numa_node_to_pmm_tag(node), &numa_got);
        if (page == 0) return -1;
        *out_page = page;
        *out_numa_node = node;
        return 0;
    }

    /* NUMA_POLICY_PREFERRED (Default-Fallback fuer unbekannte Policy-Werte,
       grosszuegigste Option). */
    uint32_t numa_got;
    uint32_t page = pmm_alloc_page_preferred(numa_node_to_pmm_tag(preferred_node), &numa_got);
    if (page == 0) return -1;
    *out_page = page;
    *out_numa_node = numa_node_exists(preferred_node) ? preferred_node : 0u;
    return 0;
}

uint32_t numa_interleave_cursor(void) { return g_interleave_cursor; }
uint32_t numa_strict_denial_count(void) { return g_strict_denial_count; }

int numa_self_test(void) {
    numa_initialize();

    if (numa_node_count() != 1u) return -1;
    if (!numa_node_exists(0u)) return -1;
    if (numa_node_exists(1u)) return -1; /* Anforderung #1/#2 - nicht erkannte Nodes existieren nicht */

    /* STRICT auf existierenden Node 0 muss gelingen. */
    uint32_t page, numa_node;
    if (numa_alloc_page(0u, NUMA_POLICY_STRICT, &page, &numa_node) != 0) return -1;
    if (pmm_free_page(page) != 0) return -1;

    /* STRICT auf einen nicht existierenden Node DARF NICHT automatisch
       auf Node 0 ausweichen (Anforderung #4). */
    uint32_t denials_before = numa_strict_denial_count();
    if (numa_alloc_page(5u, NUMA_POLICY_STRICT, &page, &numa_node) == 0) {
        pmm_free_page(page);
        return -1;
    }
    if (numa_strict_denial_count() != denials_before + 1u) return -1;

    /* PREFERRED auf einen nicht existierenden Node DARF ausweichen
       (Anforderung #5) - pmm_alloc_page_preferred faellt intern zurueck. */
    if (numa_alloc_page(5u, NUMA_POLICY_PREFERRED, &page, &numa_node) != 0) return -1;
    if (pmm_free_page(page) != 0) return -1;

    /* INTERLEAVED muss den Cursor fortschreiben. */
    uint32_t cursor_before = numa_interleave_cursor();
    if (numa_alloc_page(0u, NUMA_POLICY_INTERLEAVED, &page, &numa_node) != 0) return -1;
    if (pmm_free_page(page) != 0) return -1;
    if (numa_interleave_cursor() == cursor_before && NUMA_NODE_CAPACITY > 1u) {
        /* Cursor muss sich bei mehr als einem moeglichen Slot bewegen. */
        return -1;
    }

    return 0;
}

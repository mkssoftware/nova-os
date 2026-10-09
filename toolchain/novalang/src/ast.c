#include "ast.h"
#include <stdlib.h>
#include <string.h>

#define ARENA_BLOCK_SIZE (64 * 1024)

void nl_arena_init(NlArena *a) {
    memset(a, 0, sizeof(*a));
    a->block_size = ARENA_BLOCK_SIZE;
}

void *nl_arena_alloc(NlArena *a, uint32_t size) {
    size = (size + 7u) & ~7u; /* 8-byte align */
    if (a->block_count == 0 || a->used + size > a->block_size) {
        if (a->block_count >= a->block_cap) {
            a->block_cap = a->block_cap ? a->block_cap * 2 : 8;
            a->blocks = realloc(a->blocks, a->block_cap * sizeof(char *));
        }
        uint32_t bsz = size > a->block_size ? size : a->block_size;
        a->blocks[a->block_count++] = malloc(bsz);
        a->used = 0;
        a->block_size = bsz;
    }
    void *p = a->blocks[a->block_count - 1] + a->used;
    a->used += size;
    return p;
}

void nl_arena_free(NlArena *a) {
    for (uint32_t i = 0; i < a->block_count; i++) free(a->blocks[i]);
    free(a->blocks);
    memset(a, 0, sizeof(*a));
}

NlNode *nl_node_new(NlArena *a, NlNodeKind kind, NlRange range) {
    NlNode *n = nl_arena_alloc(a, sizeof(NlNode));
    memset(n, 0, sizeof(NlNode));
    n->kind  = kind;
    n->range = range;
    return n;
}

int nl_node_push(NlArena *a, NlNodeList *list, NlNode *child) {
    if (list->count >= list->cap) {
        uint32_t ncap = list->cap ? list->cap * 2 : 4;
        NlNode **nb = nl_arena_alloc(a, ncap * sizeof(NlNode *));
        if (list->items)
            memcpy(nb, list->items, list->count * sizeof(NlNode *));
        list->items = nb;
        list->cap   = ncap;
    }
    list->items[list->count++] = child;
    return 0;
}

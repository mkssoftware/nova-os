#include "diagnostic.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

void nl_diag_init(NlDiagList *dl) {
    memset(dl, 0, sizeof(*dl));
}

void nl_diag_free(NlDiagList *dl) {
    for (uint32_t i = 0; i < dl->count; i++) free(dl->items[i].message);
    free(dl->items);
    memset(dl, 0, sizeof(*dl));
}

void nl_diag_emit(NlDiagList *dl, NlDiagLevel level, NlRange range, const char *fmt, ...) {
    if (dl->count >= dl->cap) {
        dl->cap = dl->cap ? dl->cap * 2 : 16;
        dl->items = realloc(dl->items, dl->cap * sizeof(NlDiag));
    }
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    NlDiag *d = &dl->items[dl->count++];
    d->level   = level;
    d->range   = range;
    d->message = strdup(buf);

    if (level >= DIAG_ERROR) dl->error_count++;
    else if (level == DIAG_WARNING) dl->warning_count++;
}

void nl_diag_print(const NlDiagList *dl, const char *src, const char *filename) {
    static const char *level_name[] = { "note", "warning", "error", "fatal" };
    (void)src;
    for (uint32_t i = 0; i < dl->count; i++) {
        const NlDiag *d = &dl->items[i];
        fprintf(stderr, "%s:%u:%u: %s: %s\n",
            filename ? filename : "<input>",
            d->range.start.line,
            d->range.start.col,
            level_name[d->level],
            d->message);
    }
}

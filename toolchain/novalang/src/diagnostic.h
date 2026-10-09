#ifndef NL_DIAGNOSTIC_H
#define NL_DIAGNOSTIC_H

#include "lexer.h"
#include <stdint.h>

typedef enum NlDiagLevel {
    DIAG_NOTE    = 0,
    DIAG_WARNING = 1,
    DIAG_ERROR   = 2,
    DIAG_FATAL   = 3
} NlDiagLevel;

typedef struct NlDiag {
    NlDiagLevel  level;
    NlRange      range;
    char        *message;
} NlDiag;

typedef struct NlDiagList {
    NlDiag  *items;
    uint32_t count;
    uint32_t cap;
    uint32_t error_count;
    uint32_t warning_count;
} NlDiagList;

void nl_diag_init(NlDiagList *dl);
void nl_diag_free(NlDiagList *dl);
void nl_diag_emit(NlDiagList *dl, NlDiagLevel level, NlRange range, const char *fmt, ...);
void nl_diag_print(const NlDiagList *dl, const char *src, const char *filename);

#endif /* NL_DIAGNOSTIC_H */

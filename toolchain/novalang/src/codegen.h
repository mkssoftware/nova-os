#ifndef NL_CODEGEN_H
#define NL_CODEGEN_H

#include "ast.h"
#include "lexer.h"
#include "typechecker.h"

typedef struct NlCgOptions {
    const char *input_path;
    const char *output_c;    /* temp .c file to write */
    const char *output_exe;  /* final .exe path */
} NlCgOptions;

/* Generate C source and invoke GCC to produce output_exe.
 * Returns 0 on success, non-zero on failure. */
int nl_codegen(NlNode *unit, NlLexer *lex, NlTypeChecker *tc,
               const NlCgOptions *opts);

#endif /* NL_CODEGEN_H */

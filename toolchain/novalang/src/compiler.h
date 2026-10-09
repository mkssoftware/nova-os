#ifndef NL_COMPILER_H
#define NL_COMPILER_H

#include "lexer.h"
#include "ast.h"
#include "diagnostic.h"
#include "typechecker.h"
#include "ir.h"

typedef enum NlOutputKind {
    NL_OUT_BYTECODE,    /* .nlb file */
    NL_OUT_IR_DUMP,     /* text IR dump */
    NL_OUT_AST_DUMP,    /* text AST dump */
    NL_OUT_CHECK_ONLY   /* type-check only, no output */
} NlOutputKind;

typedef struct NlCompilerOptions {
    const char  *input_path;
    const char  *output_path;
    NlOutputKind output_kind;
    int          verbose;
} NlCompilerOptions;

typedef struct NlCompileResult {
    int       success;
    uint32_t  error_count;
    uint32_t  warning_count;
} NlCompileResult;

NlCompileResult nl_compile(const NlCompilerOptions *opts);

#endif /* NL_COMPILER_H */

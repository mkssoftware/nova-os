/*
 * compiler.c – NovaLang Compiler Pipeline
 *
 * Bindet Lexer → Parser → Typechecker → IR → Bytecode zusammen.
 */

#include "compiler.h"
#include "lexer.h"
#include "ast.h"
#include "parser.h"
#include "typechecker.h"
#include "ir.h"
#include "bytecode.h"
#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Forward declaration – defined in bytecode.c but not exported in the header */
int nl_bytecode_write(const NlIrModule *m, const char *path);

/* -------------------------------------------------------------------------
 * Helper: read entire file into malloc'd buffer (NUL-terminated)
 * ---------------------------------------------------------------------- */
static char *read_file(const char *path, uint32_t *out_len)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "Fehler: Datei nicht gefunden: %s\n", path);
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);
    char *buf = (char *)malloc((size_t)sz + 1);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    buf[rd] = '\0';
    fclose(f);
    *out_len = (uint32_t)rd;
    return buf;
}

/* -------------------------------------------------------------------------
 * nl_compile – main pipeline entry point
 * ---------------------------------------------------------------------- */
NlCompileResult nl_compile(const NlCompilerOptions *opts)
{
    NlCompileResult res = {0, 0, 0};

    /* ---- 1. Read source ---- */
    uint32_t src_len = 0;
    char    *src     = read_file(opts->input_path, &src_len);
    if (!src) {
        res.error_count = 1;
        return res;
    }

    /* ---- 2. Lex ---- */
    NlLexer lex;
    if (nl_lexer_init(&lex, src, src_len) != 0) {
        free(src);
        res.error_count = 1;
        return res;
    }
    if (nl_lexer_run(&lex) != 0 || lex.error_count > 0) {
        fprintf(stderr, "%s\n", lex.error_buf);
        nl_lexer_free(&lex);
        free(src);
        res.error_count = lex.error_count ? lex.error_count : 1;
        return res;
    }

    /* ---- 3. Parse ---- */
    NlArena    arena;
    NlDiagList diags;
    nl_arena_init(&arena);
    nl_diag_init(&diags);

    NlParser parser;
    nl_parser_init(&parser, &lex, &arena, &diags);
    NlNode *unit = nl_parse(&parser);

    if (diags.error_count > 0) {
        nl_diag_print(&diags, src, opts->input_path);
        res.error_count   = diags.error_count;
        res.warning_count = diags.warning_count;
        goto cleanup;
    }

    if (opts->output_kind == NL_OUT_AST_DUMP) {
        /* Simple AST dump: just print the diagnostic-free token count */
        printf("AST: %u Tokens geparst, kein Fehler.\n", lex.token_count);
        res.success = 1;
        goto cleanup;
    }

    /* ---- 4. Type-check ---- */
    NlTypeChecker tc;
    nl_tc_init(&tc, &arena, &diags, lex.strings, lex.string_count);
    nl_tc_check(&tc, unit);

    if (diags.error_count > 0) {
        nl_diag_print(&diags, src, opts->input_path);
        res.error_count   = diags.error_count;
        res.warning_count = diags.warning_count;
        goto cleanup;
    }

    /* Print warnings even on success */
    if (diags.warning_count > 0)
        nl_diag_print(&diags, src, opts->input_path);
    res.warning_count = diags.warning_count;

    if (opts->output_kind == NL_OUT_CHECK_ONLY) {
        res.success = 1;
        goto cleanup;
    }

    /* ---- 5. IR generation (stub, kept for --ir dump) ---- */
    NlIrModule ir;
    nl_ir_module_init(&ir, &arena);

    if (opts->output_kind == NL_OUT_IR_DUMP) {
        nl_ir_print(&ir);
        res.success = 1;
        goto cleanup;
    }

    /* ---- 6. Code generation: AST → C → GCC → .exe ---- */
    if (opts->output_path) {
        /* Derive a temp .c path alongside the output */
        char tmp_c[4096];
        strncpy(tmp_c, opts->output_path, sizeof(tmp_c) - 5);
        tmp_c[sizeof(tmp_c) - 5] = '\0';
        /* strip trailing extension if any */
        char *dot = strrchr(tmp_c, '.');
        if (dot && (dot > strrchr(tmp_c, '/') && dot > strrchr(tmp_c, '\\')))
            *dot = '\0';
        strcat(tmp_c, "_cg_tmp.c");

        NlCgOptions cg;
        cg.input_path  = opts->input_path;
        cg.output_c    = tmp_c;
        cg.output_exe  = opts->output_path;

        if (nl_codegen(unit, &lex, &tc, &cg) != 0) {
            fprintf(stderr, "Fehler: Codegenerierung fehlgeschlagen: %s\n",
                    opts->output_path);
            res.error_count = 1;
            goto cleanup;
        }
    }

    res.success = 1;

cleanup:
    nl_diag_free(&diags);
    nl_arena_free(&arena);
    nl_lexer_free(&lex);
    free(src);
    return res;
}

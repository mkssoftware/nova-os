#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Symboltabelle fuer "Dim x As String = ..." (v0.2, siehe
   NOVALANG_SPEC_v0.2.md) - String-Variablen werden VOLLSTAENDIG zur
   Compile-Zeit aufgeloest. Bewusst ein einfaches, festes Array statt
   einer Hashtabelle - v0.1-v0.3-Programme sind winzig. Integer-
   Variablen (v0.3, echte Laufzeit-Variablen) leben NICHT hier, sondern
   direkt in NcModule (siehe ast.h) - Codegen braucht sie dort fuer die
   DATA-Section. */
#define NC_MAX_SYMBOLS 64u
typedef struct NcSymbol {
    char    *name;
    char    *value;
    uint32_t len;
} NcSymbol;

typedef struct NcParser {
    NcLexer  *lx;
    NcToken   cur;
    char      error[256];
    int       failed;
    NcSymbol  symbols[NC_MAX_SYMBOLS];
    uint32_t  symbol_count;
    NcModule *mod; /* fuer Integer-Variablen-Tabelle, siehe find_int_var() */
} NcParser;

static NcSymbol *find_symbol(NcParser *p, const char *name, uint32_t len) {
    for (uint32_t i = 0; i < p->symbol_count; ++i) {
        if (strlen(p->symbols[i].name) == len && strncmp(p->symbols[i].name, name, len) == 0) {
            return &p->symbols[i];
        }
    }
    return NULL;
}

static int find_int_var(NcParser *p, const char *name, uint32_t len) {
    for (uint32_t i = 0; i < p->mod->int_var_count; ++i) {
        if (strlen(p->mod->int_var_names[i]) == len &&
            strncmp(p->mod->int_var_names[i], name, len) == 0) {
            return (int)i;
        }
    }
    return -1;
}

static void fail(NcParser *p, const char *msg) {
    if (!p->failed) {
        snprintf(p->error, sizeof(p->error), "Zeile %u: %s", p->cur.line, msg);
        p->failed = 1;
    }
}

static void advance(NcParser *p) {
    if (p->failed) return;
    p->cur = nc_lexer_next(p->lx);
    if (p->lx->has_error) {
        snprintf(p->error, sizeof(p->error), "%s", p->lx->error);
        p->failed = 1;
    }
}

static int is_ident(NcParser *p, const char *word) {
    return p->cur.kind == TOK_IDENT && p->cur.len == strlen(word) &&
           strncmp(p->cur.text, word, p->cur.len) == 0;
}

static void expect_kind(NcParser *p, NcTokenKind kind, const char *what) {
    if (p->failed) return;
    if (p->cur.kind != kind) {
        char msg[128];
        snprintf(msg, sizeof(msg), "%s erwartet", what);
        fail(p, msg);
        return;
    }
    advance(p);
}

static void skip_newlines(NcParser *p) {
    while (!p->failed && p->cur.kind == TOK_NEWLINE) advance(p);
}

static char *dup_ident(NcParser *p) {
    char *s = (char *)malloc(p->cur.len + 1);
    memcpy(s, p->cur.text, p->cur.len);
    s[p->cur.len] = '\0';
    return s;
}

/* StringExpr := StringTerm ("&" StringTerm)*
   StringTerm := StringLiteral | Ident (muss eine bereits per Dim
   definierte String-Variable sein)
   Vollstaendig zur Compile-Zeit ausgewertet - liefert ein frisches
   malloc'tes Ergebnis, *out_len gesetzt. */
static char *parse_string_expr(NcParser *p, uint32_t *out_len) {
    char *result = NULL;
    uint32_t result_len = 0;

    for (;;) {
        char *term; uint32_t term_len;
        if (p->cur.kind == TOK_STRING) {
            term = p->cur.str_val;
            term_len = p->cur.str_len;
            advance(p);
        } else if (p->cur.kind == TOK_IDENT) {
            NcSymbol *sym = find_symbol(p, p->cur.text, p->cur.len);
            if (!sym) { fail(p, "unbekannte String-Variable (nicht per 'Dim' definiert)"); free(result); return NULL; }
            term_len = sym->len;
            term = (char *)malloc(term_len + 1);
            memcpy(term, sym->value, term_len);
            term[term_len] = '\0';
            advance(p);
        } else {
            fail(p, "Zeichenkettenliteral oder Variable erwartet");
            free(result);
            return NULL;
        }

        if (!result) {
            result = term;
            result_len = term_len;
        } else {
            char *joined = (char *)malloc(result_len + term_len + 1);
            memcpy(joined, result, result_len);
            memcpy(joined + result_len, term, term_len);
            joined[result_len + term_len] = '\0';
            free(result);
            free(term);
            result = joined;
            result_len += term_len;
        }

        if (p->failed) { free(result); return NULL; }
        if (p->cur.kind != TOK_AMP) break;
        advance(p); /* '&' */
    }

    *out_len = result_len;
    return result;
}

/* IntExpr := IntTerm (("+" | "-") IntTerm)*
   IntTerm := IntLiteral | Ident (muss eine bereits per Dim definierte
   Integer-Variable sein). Anders als StringExpr NICHT compile-time
   ausgewertet - liefert eine Kette von NcIntTerm fuer den Codegenerator
   (echte Laufzeit-Arithmetik, siehe NOVALANG_SPEC_v0.3.md). */
static NcIntTerm *parse_int_expr(NcParser *p) {
    NcIntTerm *head = NULL, *tail = NULL;
    char op = '+';
    for (;;) {
        NcIntTerm *term = (NcIntTerm *)calloc(1, sizeof(NcIntTerm));
        term->op = op;
        if (p->cur.kind == TOK_INT) {
            term->is_var = 0;
            term->literal = p->cur.int_val;
            advance(p);
        } else if (p->cur.kind == TOK_IDENT) {
            int idx = find_int_var(p, p->cur.text, p->cur.len);
            if (idx < 0) { fail(p, "unbekannte Integer-Variable"); free(term); return NULL; }
            term->is_var = 1;
            term->var_index = (uint32_t)idx;
            advance(p);
        } else {
            fail(p, "Ganzzahlliteral oder Variable erwartet");
            free(term);
            return NULL;
        }
        if (tail) tail->next = term; else head = term;
        tail = term;
        if (p->failed) return NULL;
        if (p->cur.kind == TOK_PLUS) { op = '+'; advance(p); continue; }
        if (p->cur.kind == TOK_MINUS) { op = '-'; advance(p); continue; }
        break;
    }
    return head;
}

static int is_cmp_token(NcTokenKind k) {
    return k == TOK_EQUALS || k == TOK_NE || k == TOK_LT ||
           k == TOK_GT || k == TOK_LE || k == TOK_GE;
}

static NcCmpOp token_to_cmp(NcTokenKind k) {
    switch (k) {
        case TOK_EQUALS: return CMP_EQ;
        case TOK_NE: return CMP_NE;
        case TOK_LT: return CMP_LT;
        case TOK_GT: return CMP_GT;
        case TOK_LE: return CMP_LE;
        default: return CMP_GE;
    }
}

static NcStmt *parse_stmt_list(NcParser *p, int allow_else_stop);

/* ConsoleWriteLineStmt := "Console" "." "WriteLine" "(" StringExpr ")" NEWLINE */
static NcStmt *parse_writeline(NcParser *p) {
    advance(p); /* "Console" */
    expect_kind(p, TOK_DOT, "'.'");
    if (p->failed) return NULL;
    if (!is_ident(p, "WriteLine")) { fail(p, "'WriteLine' erwartet"); return NULL; }
    advance(p);
    expect_kind(p, TOK_LPAREN, "'('");
    if (p->failed) return NULL;
    uint32_t slen = 0;
    char *str = parse_string_expr(p, &slen);
    if (p->failed || !str) return NULL;
    expect_kind(p, TOK_RPAREN, "')'");
    if (p->failed) { free(str); return NULL; }
    expect_kind(p, TOK_NEWLINE, "Zeilenende");
    if (p->failed) { free(str); return NULL; }

    NcStmt *st = (NcStmt *)calloc(1, sizeof(NcStmt));
    st->kind = STMT_WRITELINE;
    st->string_arg = str;
    st->string_len = slen;
    return st;
}

/* ReturnStmt := "Return" IntLiteral NEWLINE */
static NcStmt *parse_return(NcParser *p) {
    advance(p); /* "Return" */
    if (p->cur.kind != TOK_INT) { fail(p, "Ganzzahlliteral erwartet (Return kennt keine Ausdruecke)"); return NULL; }
    int64_t val = p->cur.int_val;
    advance(p);
    expect_kind(p, TOK_NEWLINE, "Zeilenende");
    if (p->failed) return NULL;

    NcStmt *st = (NcStmt *)calloc(1, sizeof(NcStmt));
    st->kind = STMT_RETURN;
    st->int_arg = val;
    return st;
}

/* Ein Operand von SysCall(...) - bewusst nur Literal/Variable, kein
   verschachtelter Ausdruck (siehe NOVALANG_SPEC_v0.4.md). */
static int parse_sys_arg(NcParser *p, NcSysArg *out) {
    if (p->cur.kind == TOK_INT) {
        out->is_var = 0;
        out->literal = p->cur.int_val;
        advance(p);
        return 0;
    }
    if (p->cur.kind == TOK_IDENT) {
        int idx = find_int_var(p, p->cur.text, p->cur.len);
        if (idx < 0) { fail(p, "unbekannte Integer-Variable in SysCall-Argument"); return -1; }
        out->is_var = 1;
        out->var_index = (uint32_t)idx;
        advance(p);
        return 0;
    }
    fail(p, "Ganzzahlliteral oder Variable erwartet (SysCall-Argument)");
    return -1;
}

/* SysCallAssignStmt := Ident "=" "SysCall" "(" SysArg "," SysArg ","
   SysArg "," SysArg ")" NEWLINE - direkter Zugriff auf den NovaOS-
   Syscall-Mechanismus (service/operation/arg1/arg2, siehe syscall.c
   syscall_dispatch() - EAX/EBX/ECX/EDX/ESI, int 0x80, Ergebnis in EAX),
   siehe NOVALANG_SPEC_v0.4.md. target_idx ist der Index der bereits per
   find_int_var() aufgeloesten Ziel-Variable. */
static NcStmt *parse_assign_syscall(NcParser *p, int target_idx) {
    advance(p); /* "SysCall" */
    expect_kind(p, TOK_LPAREN, "'('");
    if (p->failed) return NULL;
    NcSysArg args[4];
    for (int i = 0; i < 4; ++i) {
        if (parse_sys_arg(p, &args[i]) != 0) return NULL;
        if (i < 3) expect_kind(p, TOK_COMMA, "','");
        if (p->failed) return NULL;
    }
    expect_kind(p, TOK_RPAREN, "')'");
    if (p->failed) return NULL;
    expect_kind(p, TOK_NEWLINE, "Zeilenende");
    if (p->failed) return NULL;

    NcStmt *st = (NcStmt *)calloc(1, sizeof(NcStmt));
    st->kind = STMT_ASSIGN_SYSCALL;
    st->assign_var_index = (uint32_t)target_idx;
    for (int i = 0; i < 4; ++i) st->syscall_args[i] = args[i];
    return st;
}

/* AssignStmt := Ident "=" (IntExpr | SysCallExpr) NEWLINE (Ident muss
   bereits per "Dim ... As Integer" deklariert sein). */
static NcStmt *parse_assign(NcParser *p) {
    int idx = find_int_var(p, p->cur.text, p->cur.len);
    advance(p); /* Ident */
    expect_kind(p, TOK_EQUALS, "'='");
    if (p->failed) return NULL;

    if (is_ident(p, "SysCall")) return parse_assign_syscall(p, idx);

    NcIntTerm *expr = parse_int_expr(p);
    if (p->failed || !expr) return NULL;
    expect_kind(p, TOK_NEWLINE, "Zeilenende");
    if (p->failed) return NULL;

    NcStmt *st = (NcStmt *)calloc(1, sizeof(NcStmt));
    st->kind = STMT_ASSIGN_INT;
    st->assign_var_index = (uint32_t)idx;
    st->assign_expr = expr;
    return st;
}

/* IfStmt := "If" Ident CmpOp (IntLiteral | Ident) "Then" NEWLINE
              Statement*
            ("Else" NEWLINE Statement*)?
            "End" "If" NEWLINE
   Die linke Seite MUSS eine bereits deklarierte Integer-Variable sein
   (kein beliebiger Ausdruck, siehe NOVALANG_SPEC_v0.3.md). */
static NcStmt *parse_if(NcParser *p) {
    advance(p); /* "If" */
    if (p->cur.kind != TOK_IDENT) { fail(p, "Integer-Variable erwartet (If-Bedingung)"); return NULL; }
    int lhs_idx = find_int_var(p, p->cur.text, p->cur.len);
    if (lhs_idx < 0) { fail(p, "unbekannte Integer-Variable in If-Bedingung"); return NULL; }
    advance(p);
    if (!is_cmp_token(p->cur.kind)) { fail(p, "Vergleichsoperator erwartet (=, <>, <, >, <=, >=)"); return NULL; }
    NcCmpOp cmp = token_to_cmp(p->cur.kind);
    advance(p);

    int rhs_is_var = 0;
    int64_t rhs_lit = 0;
    uint32_t rhs_idx = 0;
    if (p->cur.kind == TOK_INT) {
        rhs_lit = p->cur.int_val;
        advance(p);
    } else if (p->cur.kind == TOK_IDENT) {
        int idx = find_int_var(p, p->cur.text, p->cur.len);
        if (idx < 0) { fail(p, "unbekannte Integer-Variable in If-Bedingung"); return NULL; }
        rhs_is_var = 1;
        rhs_idx = (uint32_t)idx;
        advance(p);
    } else {
        fail(p, "Ganzzahlliteral oder Variable erwartet");
        return NULL;
    }

    expect_kind(p, TOK_KW_THEN, "'Then'");
    expect_kind(p, TOK_NEWLINE, "Zeilenende");
    if (p->failed) return NULL;
    skip_newlines(p);

    NcStmt *then_body = parse_stmt_list(p, 1 /* Else darf hier abbrechen */);
    if (p->failed) return NULL;

    NcStmt *else_body = NULL;
    if (p->cur.kind == TOK_KW_ELSE) {
        advance(p);
        expect_kind(p, TOK_NEWLINE, "Zeilenende");
        if (p->failed) return NULL;
        skip_newlines(p);
        else_body = parse_stmt_list(p, 0);
        if (p->failed) return NULL;
    }

    expect_kind(p, TOK_KW_END, "'End'");
    expect_kind(p, TOK_KW_IF, "'If'");
    expect_kind(p, TOK_NEWLINE, "Zeilenende");
    if (p->failed) return NULL;

    NcStmt *st = (NcStmt *)calloc(1, sizeof(NcStmt));
    st->kind = STMT_IF;
    st->if_lhs_var_index = (uint32_t)lhs_idx;
    st->if_cmp_op = cmp;
    st->if_rhs_is_var = rhs_is_var;
    st->if_rhs_literal = rhs_lit;
    st->if_rhs_var_index = rhs_idx;
    st->if_then_body = then_body;
    st->if_else_body = else_body;
    return st;
}

/* DimStmt := "Dim" Ident "As" ("String" "=" StringExpr | "Integer" "=" IntLiteral) NEWLINE
   Erzeugt KEINEN Statement-Knoten: String-Dim ist rein compile-time
   (siehe NcSymbol-Kommentar oben), Integer-Dim traegt sich direkt in
   mod->int_var_* ein (Codegen baut daraus die DATA-Section). NULL-
   Rueckgabewert gibt es hier nicht - der Aufrufer muss p->failed statt
   eines Rueckgabewerts pruefen. */
static void parse_dim(NcParser *p) {
    advance(p); /* "Dim" */
    if (p->cur.kind != TOK_IDENT) { fail(p, "Variablenname erwartet"); return; }
    if (find_symbol(p, p->cur.text, p->cur.len) || find_int_var(p, p->cur.text, p->cur.len) >= 0) {
        fail(p, "Variable bereits definiert (keine Neudeklaration/-zuweisung per Dim)");
        return;
    }
    char *name = dup_ident(p);
    advance(p);
    expect_kind(p, TOK_KW_AS, "'As'");
    if (p->failed) { free(name); return; }

    if (is_ident(p, "String")) {
        advance(p);
        expect_kind(p, TOK_EQUALS, "'='");
        if (p->failed) { free(name); return; }
        uint32_t vlen = 0;
        char *value = parse_string_expr(p, &vlen);
        if (p->failed || !value) { free(name); return; }
        expect_kind(p, TOK_NEWLINE, "Zeilenende");
        if (p->failed) { free(name); free(value); return; }
        if (p->symbol_count >= NC_MAX_SYMBOLS) { fail(p, "zu viele String-Variablen (Limit)"); free(name); free(value); return; }
        p->symbols[p->symbol_count].name = name;
        p->symbols[p->symbol_count].value = value;
        p->symbols[p->symbol_count].len = vlen;
        p->symbol_count++;
    } else if (is_ident(p, "Integer")) {
        advance(p);
        expect_kind(p, TOK_EQUALS, "'='");
        if (p->failed) { free(name); return; }
        if (p->cur.kind != TOK_INT) { fail(p, "Ganzzahlliteral erwartet (Dim-Integer-Initialwert, kein Ausdruck)"); free(name); return; }
        int64_t initial = p->cur.int_val;
        advance(p);
        expect_kind(p, TOK_NEWLINE, "Zeilenende");
        if (p->failed) { free(name); return; }
        if (p->mod->int_var_count >= 64u) { fail(p, "zu viele Integer-Variablen (v0.3-Limit 64)"); free(name); return; }
        p->mod->int_var_names[p->mod->int_var_count] = name;
        p->mod->int_var_initial[p->mod->int_var_count] = initial;
        p->mod->int_var_count++;
    } else {
        fail(p, "'String' oder 'Integer' erwartet");
        free(name);
    }
}

/* Parst eine Statement-Folge bis TOK_KW_END (immer) oder zusaetzlich bis
   TOK_KW_ELSE, wenn allow_else_stop gesetzt ist (fuer den Then-Zweig
   eines If). Wird fuer Function-Body UND If-Then/Else-Koerper
   gleichermassen verwendet - rekursiv fuer verschachtelte If. */
static NcStmt *parse_stmt_list(NcParser *p, int allow_else_stop) {
    NcStmt *head = NULL, *tail = NULL;
    while (!p->failed && p->cur.kind != TOK_KW_END &&
           !(allow_else_stop && p->cur.kind == TOK_KW_ELSE)) {
        if (p->cur.kind == TOK_KW_DIM) {
            parse_dim(p);
            if (p->failed) return head;
            skip_newlines(p);
            continue;
        }
        NcStmt *st;
        if (is_ident(p, "Console")) {
            st = parse_writeline(p);
        } else if (p->cur.kind == TOK_KW_RETURN) {
            st = parse_return(p);
        } else if (p->cur.kind == TOK_KW_IF) {
            st = parse_if(p);
        } else if (p->cur.kind == TOK_IDENT && find_int_var(p, p->cur.text, p->cur.len) >= 0) {
            st = parse_assign(p);
        } else {
            fail(p, "Anweisung erwartet (Dim/Console.WriteLine/Return/If/Zuweisung)");
            return head;
        }
        if (p->failed) return head;
        if (tail) tail->next = st; else head = st;
        tail = st;
        skip_newlines(p);
    }
    return head;
}

static NcFunction *parse_function(NcParser *p) {
    advance(p); /* "Function" */
    if (p->cur.kind != TOK_IDENT) { fail(p, "Funktionsname erwartet"); return NULL; }
    if (p->cur.len != 4 || strncmp(p->cur.text, "Main", 4) != 0) {
        fail(p, "diese Version kennt nur 'Function Main()'");
        return NULL;
    }
    char *name = dup_ident(p);
    advance(p);
    expect_kind(p, TOK_LPAREN, "'('");
    expect_kind(p, TOK_RPAREN, "')'");
    expect_kind(p, TOK_KW_AS, "'As'");
    if (p->failed) { free(name); return NULL; }
    if (!is_ident(p, "Int32")) { fail(p, "'Int32' erwartet (kein anderer Rueckgabetyp unterstuetzt)"); free(name); return NULL; }
    advance(p);
    expect_kind(p, TOK_NEWLINE, "Zeilenende");
    if (p->failed) { free(name); return NULL; }
    skip_newlines(p);

    NcFunction *fn = (NcFunction *)calloc(1, sizeof(NcFunction));
    fn->name = name;
    fn->body = parse_stmt_list(p, 0);
    if (p->failed) return fn;

    expect_kind(p, TOK_KW_END, "'End'");
    expect_kind(p, TOK_KW_FUNCTION, "'Function'");
    expect_kind(p, TOK_NEWLINE, "Zeilenende");
    skip_newlines(p);
    return fn;
}

NcModule *nc_parse_module(NcLexer *lx, char *out_error, uint32_t out_error_cap) {
    NcParser p;
    memset(&p, 0, sizeof(p));
    p.lx = lx;
    advance(&p);
    skip_newlines(&p);

    expect_kind(&p, TOK_KW_MODULE, "'Module'");
    if (p.failed) goto error;
    if (p.cur.kind != TOK_IDENT) { fail(&p, "Modulname erwartet"); goto error; }
    char *mod_name = dup_ident(&p);
    advance(&p);
    expect_kind(&p, TOK_NEWLINE, "Zeilenende");
    if (p.failed) { free(mod_name); goto error; }
    skip_newlines(&p);

    NcModule *mod = (NcModule *)calloc(1, sizeof(NcModule));
    mod->name = mod_name;
    p.mod = mod;

    while (!p.failed && p.cur.kind == TOK_KW_FUNCTION) {
        NcFunction *fn = parse_function(&p);
        if (p.failed) goto error;
        if (mod->main_func != NULL) { fail(&p, "diese Version kennt nur genau eine Function pro Module"); goto error; }
        mod->main_func = fn;
        skip_newlines(&p);
    }

    expect_kind(&p, TOK_KW_END, "'End'");
    expect_kind(&p, TOK_KW_MODULE, "'Module'");
    if (!p.failed && p.cur.kind != TOK_EOF) {
        expect_kind(&p, TOK_NEWLINE, "Zeilenende");
    }
    skip_newlines(&p);
    if (!p.failed && p.cur.kind != TOK_EOF) { fail(&p, "unerwarteter Inhalt nach 'End Module' (nur ein Module pro Datei)"); }
    if (p.failed) goto error;
    if (mod->main_func == NULL) { fail(&p, "'Function Main' fehlt"); goto error; }

    return mod;

error:
    snprintf(out_error, out_error_cap, "%s", p.error);
    return NULL;
}

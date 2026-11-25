#include "parser.h"
#include "codegen_x86.h"
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
    /* v0.9 (siehe NOVALANG_SPEC_v0.9.md): current_func_name ist waehrend
       des Parsens EINES Funktionskoerpers der (unmangled) Name dieser
       Function, genutzt fuer das Parameter-Namensmangling (siehe
       lookup_var()). in_main steuert, ob "Return" als Mains spezielle
       YIELD-Endlosschleife (STMT_RETURN, nur Literal) oder als echter
       Funktionsrueckgabewert (STMT_FUNC_RETURN, beliebiger IntExpr, real
       per "ret") interpretiert wird - siehe parse_return(). */
    const char *current_func_name;
    int       in_main;
    /* v0.9: Namen ALLER Functions im Modul, per Vorab-Scan (siehe
       prescan_function_names() in nc_parse_module()) VOR dem eigentlichen
       Parsen gesammelt - notwendig, damit Main() (textuell meist VOR
       ihren Helfer-Functions) diese per Vorwaertsreferenz aufrufen kann
       (find_function_any()). find_function() (ohne "_any") bleibt der
       Check gegen BEREITS VOLLSTAENDIG geparste Functions (mod->functions),
       fuer die "Function bereits definiert"-Duplikatpruefung. */
    char     *all_func_names[16];
    uint32_t  all_func_count;
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

/* v0.9: Parameter werden als Integer-Variablen mit gemangeltem Namen
   ("<FuncName>.<ParamName>") registriert (siehe parse_function()), um
   Namenskollisionen zwischen Functions zu vermeiden. lookup_var() ist
   der EINZIGE Ort, an dem eine Variablenreferenz IM FUNKTIONSKOERPER
   (Zuweisung, Bedingung, SysCall-/Call-Argument) aufgeloest wird: erst
   gemangelt (innerhalb der aktuell geparsten Function) versuchen, dann
   auf den Klarnamen zurueckfallen (Modul-globale "Dim"-Variablen, oder
   v0.1-v0.8-Programme ohne Parameter). Dim-Deklarationen selbst (siehe
   parse_dim()) bleiben bewusst UNGEMANGELT und nutzen weiterhin
   find_int_var() direkt. */
static int lookup_var(NcParser *p, const char *name, uint32_t len) {
    if (p->current_func_name) {
        char mangled[160];
        int n = snprintf(mangled, sizeof(mangled), "%s.%.*s", p->current_func_name, (int)len, name);
        if (n > 0 && (size_t)n < sizeof(mangled)) {
            int idx = find_int_var(p, mangled, (uint32_t)n);
            if (idx >= 0) return idx;
        }
    }
    return find_int_var(p, name, len);
}

/* Mehrere Functions pro Module (v0.9, siehe NOVALANG_SPEC_v0.9.md).
   find_function() sieht NUR bereits VOLLSTAENDIG geparste Functions
   (mod->functions) - fuer die Duplikatpruefung in parse_function() (eine
   function, die gerade geparst wird, darf sich dort noch nicht selbst
   finden). find_function_any() sieht ALLE Functions des Moduls (per
   Vorab-Scan, siehe prescan_function_names()) - fuer die Aufloesung
   eines Aufrufziels ("x = Foo(...)"), da Main ihre Helfer-Functions fast
   immer per Vorwaertsreferenz aufruft (Main steht textuell zuerst). Die
   zurueckgegebenen Indizes sind in BEIDEN Faellen identisch zum
   spaeteren mod->functions[]-Index, weil Parse- und Scan-Reihenfolge
   gleich der Quelltextreihenfolge sind. */
static int find_function(NcParser *p, const char *name, uint32_t len) {
    for (uint32_t i = 0; i < p->mod->function_count; ++i) {
        const char *fn_name = p->mod->functions[i]->name;
        if (strlen(fn_name) == len && strncmp(fn_name, name, len) == 0) return (int)i;
    }
    return -1;
}

static int find_function_any(NcParser *p, const char *name, uint32_t len) {
    for (uint32_t i = 0; i < p->all_func_count; ++i) {
        if (strlen(p->all_func_names[i]) == len && strncmp(p->all_func_names[i], name, len) == 0) return (int)i;
    }
    return -1;
}

/* Byte-Puffer fuer Syscall-Argument-Structs (v0.5, siehe
   NOVALANG_SPEC_v0.5.md) - "Dim buf As Buffer(N)". */
static int find_buffer(NcParser *p, const char *name, uint32_t len) {
    for (uint32_t i = 0; i < p->mod->buf_count; ++i) {
        if (strlen(p->mod->buf_names[i]) == len &&
            strncmp(p->mod->buf_names[i], name, len) == 0) {
            return (int)i;
        }
    }
    return -1;
}
static uint32_t buffer_addr(NcParser *p, int idx) {
    return NC_BUFFER_VADDR + p->mod->buf_byte_offset[idx];
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

/* Erkennt "AddressOf(buf)" (loest sofort zu einem compile-time-
   konstanten Literal auf - Pufferadressen stehen fest, es gibt keine
   Laufzeit-Zeigerarithmetik) oder "PeekInt(buf, byteOffset)"/
   "PeekByte(buf, byteOffset)" (liest 4 bzw. 1 Byte, v0.5/v0.7) - siehe
   NOVALANG_SPEC_v0.5.md/v0.7.md. Der Byteoffset ist seit v0.15 (siehe
   NOVALANG_SPEC_v0.15.md, symmetrisch zu PokeInt/PokeByte aus v0.12)
   ENTWEDER ein Ganzzahlliteral (unveraendert, inkl. Bereichspruefung,
   out_peek_addr = fertige Adresse) ODER eine Integer-Variable (NEU -
   keine Bereichspruefung moeglich, out_peek_addr = nur die Puffer-
   BASISADRESSE, out_offset_is_var/out_offset_var_index gesetzt).
   Rueckgabe: 1 = erkannt+verarbeitet, 0 = keiner der Spezialfaelle
   (Aufrufer parst normal weiter), -1 = Fehler INNERHALB eines
   erkannten Spezialfalls. */
static int try_parse_special_term(NcParser *p, int *out_is_peek, uint32_t *out_peek_addr,
                                   int *out_peek_width, int64_t *out_literal,
                                   int *out_offset_is_var, uint32_t *out_offset_var_index) {
    if (is_ident(p, "AddressOf")) {
        advance(p);
        expect_kind(p, TOK_LPAREN, "'('");
        if (p->failed) return -1;
        if (p->cur.kind != TOK_IDENT) { fail(p, "Puffername erwartet (AddressOf)"); return -1; }
        int idx = find_buffer(p, p->cur.text, p->cur.len);
        if (idx < 0) { fail(p, "unbekannter Puffer (nicht per 'Dim ... As Buffer' definiert)"); return -1; }
        advance(p);
        expect_kind(p, TOK_RPAREN, "')'");
        if (p->failed) return -1;
        *out_is_peek = 0;
        *out_literal = (int64_t)buffer_addr(p, idx);
        return 1;
    }
    int width = 0;
    if (is_ident(p, "PeekInt")) width = 4;
    else if (is_ident(p, "PeekByte")) width = 1;
    if (width != 0) {
        advance(p);
        expect_kind(p, TOK_LPAREN, "'('");
        if (p->failed) return -1;
        if (p->cur.kind != TOK_IDENT) { fail(p, "Puffername erwartet (Peek)"); return -1; }
        int idx = find_buffer(p, p->cur.text, p->cur.len);
        if (idx < 0) { fail(p, "unbekannter Puffer (nicht per 'Dim ... As Buffer' definiert)"); return -1; }
        advance(p);
        expect_kind(p, TOK_COMMA, "','");
        if (p->failed) return -1;

        *out_offset_is_var = 0;
        *out_offset_var_index = 0;
        if (p->cur.kind == TOK_INT) {
            int64_t offset = p->cur.int_val;
            if (offset < 0 || offset + width > (int64_t)p->mod->buf_size[idx]) { fail(p, "Peek-Offset ausserhalb des Puffers"); return -1; }
            *out_peek_addr = buffer_addr(p, idx) + (uint32_t)offset;
            advance(p);
        } else if (p->cur.kind == TOK_IDENT) {
            int vidx = lookup_var(p, p->cur.text, p->cur.len);
            if (vidx < 0) { fail(p, "unbekannte Integer-Variable als Peek-Offset"); return -1; }
            *out_offset_is_var = 1;
            *out_offset_var_index = (uint32_t)vidx;
            *out_peek_addr = buffer_addr(p, idx);
            advance(p);
        } else {
            fail(p, "Ganzzahl-Byteoffset oder Integer-Variable erwartet (Peek)");
            return -1;
        }

        expect_kind(p, TOK_RPAREN, "')'");
        if (p->failed) return -1;
        *out_is_peek = 1;
        *out_peek_width = width;
        return 1;
    }
    return 0;
}

/* IntExpr := IntTerm (("+" | "-") IntTerm)*
   IntTerm := IntLiteral | Ident | "AddressOf(...)" | "PeekInt(...)".
   Ident muss eine bereits per Dim definierte Integer-Variable sein.
   Anders als StringExpr NICHT compile-time ausgewertet - liefert eine
   Kette von NcIntTerm fuer den Codegenerator (echte Laufzeit-
   Arithmetik, siehe NOVALANG_SPEC_v0.3.md). */
static NcIntTerm *parse_int_expr(NcParser *p) {
    NcIntTerm *head = NULL, *tail = NULL;
    char op = '+';
    for (;;) {
        NcIntTerm *term = (NcIntTerm *)calloc(1, sizeof(NcIntTerm));
        term->op = op;
        int is_peek = 0; uint32_t peek_addr = 0; int peek_width = 0; int64_t lit = 0;
        int offset_is_var = 0; uint32_t offset_var_index = 0;
        int special = try_parse_special_term(p, &is_peek, &peek_addr, &peek_width, &lit,
                                              &offset_is_var, &offset_var_index);
        if (special < 0) { free(term); return NULL; }
        if (special == 1) {
            if (is_peek) {
                /* v0.15-Grenze (siehe NOVALANG_SPEC_v0.15.md): ein
                   Peek mit Laufzeit-Offset-Variable ist NUR als EINZIGER
                   Term erlaubt (Codegen laedt dafuer den Offset nach
                   ECX und das Ergebnis direkt nach EAX) - als
                   NICHT-erster Term einer +/--Kette wuerde das mit dem
                   ECX-Akkumulator-Scratch fuer PeekByte-Fortsetzungen
                   kollidieren (siehe emit_int_expr()). Bewusst
                   abgelehnt statt stillschweigend falschen Code zu
                   erzeugen. */
                if (offset_is_var && tail != NULL) {
                    fail(p, "Peek mit Laufzeit-Offset-Variable nur als einziger Term erlaubt (nicht in einer +/--Kette)");
                    free(term);
                    return NULL;
                }
                term->is_peek = 1; term->peek_addr = peek_addr; term->peek_width = peek_width;
                term->peek_offset_is_var = offset_is_var; term->peek_offset_var_index = offset_var_index;
            }
            else { term->is_var = 0; term->literal = lit; }
        } else if (p->cur.kind == TOK_INT) {
            term->is_var = 0;
            term->literal = p->cur.int_val;
            advance(p);
        } else if (p->cur.kind == TOK_IDENT) {
            int idx = lookup_var(p, p->cur.text, p->cur.len);
            if (idx < 0) { fail(p, "unbekannte Integer-Variable"); free(term); return NULL; }
            term->is_var = 1;
            term->var_index = (uint32_t)idx;
            advance(p);
        } else {
            fail(p, "Ganzzahlliteral, Variable, AddressOf(...) oder PeekInt(...) erwartet");
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
static int parse_sys_arg(NcParser *p, NcSysArg *out);

/* ConsoleStmt := "Console" "." ("WriteLine" "(" StringExpr ")" |
                                  "WriteBuffer" "(" Ident "," SysArg ")") NEWLINE
   WriteBuffer (v0.7, siehe NOVALANG_SPEC_v0.7.md): gibt `laenge` Byte
   aus `puffer` 1:1 aus - KEIN automatisches "\r\n" (anders als
   WriteLine) und die Laenge ist haeufig eine LAUFZEIT-Groesse (Variable),
   nicht wie bei WriteLine aus einem Compile-Zeit-Literal abgeleitet. */
static NcStmt *parse_writeline(NcParser *p) {
    advance(p); /* "Console" */
    expect_kind(p, TOK_DOT, "'.'");
    if (p->failed) return NULL;

    if (is_ident(p, "WriteBuffer")) {
        advance(p);
        expect_kind(p, TOK_LPAREN, "'('");
        if (p->failed) return NULL;
        if (p->cur.kind != TOK_IDENT) { fail(p, "Puffername erwartet (WriteBuffer)"); return NULL; }
        int idx = find_buffer(p, p->cur.text, p->cur.len);
        if (idx < 0) { fail(p, "unbekannter Puffer (nicht per 'Dim ... As Buffer' definiert)"); return NULL; }
        uint32_t buf_addr = buffer_addr(p, idx);
        advance(p);
        expect_kind(p, TOK_COMMA, "','");
        if (p->failed) return NULL;
        NcSysArg len_arg;
        memset(&len_arg, 0, sizeof(len_arg));
        if (parse_sys_arg(p, &len_arg) != 0) return NULL;
        expect_kind(p, TOK_RPAREN, "')'");
        if (p->failed) return NULL;
        expect_kind(p, TOK_NEWLINE, "Zeilenende");
        if (p->failed) return NULL;

        NcStmt *st = (NcStmt *)calloc(1, sizeof(NcStmt));
        st->kind = STMT_WRITEBUFFER;
        st->writebuf_addr = buf_addr;
        st->writebuf_len = len_arg;
        return st;
    }

    if (!is_ident(p, "WriteLine")) { fail(p, "'WriteLine' oder 'WriteBuffer' erwartet"); return NULL; }
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

/* ReturnStmt (v0.9, siehe NOVALANG_SPEC_v0.9.md): in Main unveraendert
   "Return" IntLiteral NEWLINE (STMT_RETURN - geht in die dauerhafte
   YIELD-Schleife, siehe codegen_x86.c, Rueckgabewert wird NICHT
   kodiert). In jeder ANDEREN Function: "Return" IntExpr NEWLINE
   (STMT_FUNC_RETURN - echter Funktionsrueckgabewert in EAX, echtes
   "ret"). */
static NcStmt *parse_return(NcParser *p) {
    advance(p); /* "Return" */
    if (p->in_main) {
        if (p->cur.kind != TOK_INT) { fail(p, "Ganzzahlliteral erwartet (Return in Main kennt keine Ausdruecke)"); return NULL; }
        int64_t val = p->cur.int_val;
        advance(p);
        expect_kind(p, TOK_NEWLINE, "Zeilenende");
        if (p->failed) return NULL;

        NcStmt *st = (NcStmt *)calloc(1, sizeof(NcStmt));
        st->kind = STMT_RETURN;
        st->int_arg = val;
        return st;
    }

    NcIntTerm *expr = parse_int_expr(p);
    if (p->failed || !expr) return NULL;
    expect_kind(p, TOK_NEWLINE, "Zeilenende");
    if (p->failed) return NULL;

    NcStmt *st = (NcStmt *)calloc(1, sizeof(NcStmt));
    st->kind = STMT_FUNC_RETURN;
    st->return_expr = expr;
    return st;
}

/* Ein Operand von SysCall(...)/PokeInt(...) - Literal/Variable/
   "AddressOf(...)"/"PeekInt(...)", bewusst KEIN verschachtelter
   Ausdruck (siehe NOVALANG_SPEC_v0.4.md Abschnitt 3). */
static int parse_sys_arg(NcParser *p, NcSysArg *out) {
    int is_peek = 0; uint32_t peek_addr = 0; int peek_width = 0; int64_t lit = 0;
    int offset_is_var = 0; uint32_t offset_var_index = 0;
    int special = try_parse_special_term(p, &is_peek, &peek_addr, &peek_width, &lit,
                                          &offset_is_var, &offset_var_index);
    if (special < 0) return -1;
    if (special == 1) {
        if (is_peek) {
            out->is_peek = 1; out->peek_addr = peek_addr; out->peek_width = peek_width;
            out->peek_offset_is_var = offset_is_var; out->peek_offset_var_index = offset_var_index;
        }
        else { out->is_var = 0; out->literal = lit; }
        return 0;
    }
    if (p->cur.kind == TOK_INT) {
        out->is_var = 0;
        out->literal = p->cur.int_val;
        advance(p);
        return 0;
    }
    if (p->cur.kind == TOK_IDENT) {
        int idx = lookup_var(p, p->cur.text, p->cur.len);
        if (idx < 0) { fail(p, "unbekannte Integer-Variable in SysCall-Argument"); return -1; }
        out->is_var = 1;
        out->var_index = (uint32_t)idx;
        advance(p);
        return 0;
    }
    fail(p, "Ganzzahlliteral, Variable, AddressOf(...) oder PeekInt(...) erwartet (SysCall-Argument)");
    return -1;
}

/* PokeStmt := "PokeInt" "(" Ident "," (IntLiteral | Ident) "," SysArg ")"
   NEWLINE - schreibt 4 Byte (PokeInt) bzw. 1 Byte (PokeByte, v0.7 - nur
   das niedrigste Byte des Werts) in einen Puffer, siehe
   NOVALANG_SPEC_v0.5.md/v0.7.md. width_name ist "PokeInt" oder
   "PokeByte" (bereits per is_ident() erkannt, siehe Aufrufer).
   Der Byteoffset ist seit v0.12 (siehe NOVALANG_SPEC_v0.12.md) ENTWEDER
   ein Ganzzahlliteral (unveraendert - Adresse komplett zur Compile-Zeit
   aufgeloest, inkl. Bereichspruefung) ODER eine bereits deklarierte
   Integer-Variable (NEU - z.B. eine mitgefuehrte Laufzeit-Zeilenlaenge;
   KEINE Bereichspruefung moeglich, der Offset ist erst zur Laufzeit
   bekannt). */
static NcStmt *parse_poke(NcParser *p, int width) {
    advance(p); /* "PokeInt" oder "PokeByte" */
    expect_kind(p, TOK_LPAREN, "'('");
    if (p->failed) return NULL;
    if (p->cur.kind != TOK_IDENT) { fail(p, "Puffername erwartet (Poke)"); return NULL; }
    int idx = find_buffer(p, p->cur.text, p->cur.len);
    if (idx < 0) { fail(p, "unbekannter Puffer (nicht per 'Dim ... As Buffer' definiert)"); return NULL; }
    advance(p);
    expect_kind(p, TOK_COMMA, "','");
    if (p->failed) return NULL;

    int offset_is_var = 0;
    uint32_t offset_var_index = 0;
    uint32_t resolved_addr = 0;
    if (p->cur.kind == TOK_INT) {
        int64_t offset = p->cur.int_val;
        if (offset < 0 || offset + width > (int64_t)p->mod->buf_size[idx]) { fail(p, "Poke-Offset ausserhalb des Puffers"); return NULL; }
        resolved_addr = buffer_addr(p, idx) + (uint32_t)offset;
        advance(p);
    } else if (p->cur.kind == TOK_IDENT) {
        int vidx = lookup_var(p, p->cur.text, p->cur.len);
        if (vidx < 0) { fail(p, "unbekannte Integer-Variable als Poke-Offset"); return NULL; }
        offset_is_var = 1;
        offset_var_index = (uint32_t)vidx;
        resolved_addr = buffer_addr(p, idx); /* nur die Basisadresse, siehe ast.h */
        advance(p);
    } else {
        fail(p, "Ganzzahl-Byteoffset oder Integer-Variable erwartet (Poke)");
        return NULL;
    }

    expect_kind(p, TOK_COMMA, "','");
    if (p->failed) return NULL;
    NcSysArg value;
    memset(&value, 0, sizeof(value));
    if (parse_sys_arg(p, &value) != 0) return NULL;
    expect_kind(p, TOK_RPAREN, "')'");
    if (p->failed) return NULL;
    expect_kind(p, TOK_NEWLINE, "Zeilenende");
    if (p->failed) return NULL;

    NcStmt *st = (NcStmt *)calloc(1, sizeof(NcStmt));
    st->kind = STMT_POKE;
    st->poke_addr = resolved_addr;
    st->poke_width = width;
    st->poke_offset_is_var = offset_is_var;
    st->poke_offset_var_index = offset_var_index;
    st->poke_value = value;
    return st;
}

/* "PokeString(buf, byteOffset, StringExpr)" - schreibt compile-time-
   bekannte String-Bytes in einen Puffer (siehe NOVALANG_SPEC_v0.5.md).
   Erzeugt KEINEN Statement-Knoten (reine Daten-Initialisierung, wie
   String-"Dim") - traegt sich direkt in mod->patch_* ein. */
static void parse_poke_string(NcParser *p) {
    advance(p); /* "PokeString" */
    expect_kind(p, TOK_LPAREN, "'('");
    if (p->failed) return;
    if (p->cur.kind != TOK_IDENT) { fail(p, "Puffername erwartet (PokeString)"); return; }
    int idx = find_buffer(p, p->cur.text, p->cur.len);
    if (idx < 0) { fail(p, "unbekannter Puffer (nicht per 'Dim ... As Buffer' definiert)"); return; }
    advance(p);
    expect_kind(p, TOK_COMMA, "','");
    if (p->failed) return;
    if (p->cur.kind != TOK_INT) { fail(p, "Ganzzahl-Byteoffset erwartet (PokeString)"); return; }
    int64_t offset = p->cur.int_val;
    advance(p);
    expect_kind(p, TOK_COMMA, "','");
    if (p->failed) return;
    uint32_t slen = 0;
    char *str = parse_string_expr(p, &slen);
    if (p->failed || !str) return;
    if (offset < 0 || offset + (int64_t)slen > (int64_t)p->mod->buf_size[idx]) {
        fail(p, "PokeString ueberschreitet den Puffer");
        free(str);
        return;
    }
    expect_kind(p, TOK_RPAREN, "')'");
    if (p->failed) { free(str); return; }
    expect_kind(p, TOK_NEWLINE, "Zeilenende");
    if (p->failed) { free(str); return; }

    if (p->mod->patch_count >= 16u) { fail(p, "zu viele PokeString-Aufrufe (v0.5-Limit 16)"); free(str); return; }
    p->mod->patch_bytes[p->mod->patch_count] = str;
    p->mod->patch_addr[p->mod->patch_count] = buffer_addr(p, idx) + (uint32_t)offset;
    p->mod->patch_len[p->mod->patch_count] = slen;
    p->mod->patch_count++;
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
    memset(args, 0, sizeof(args));
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

/* CallAssignStmt (v0.9, siehe NOVALANG_SPEC_v0.9.md): "x = FuncName(arg1,
   ...)" - Aufruf einer anderen, bereits bekannten Function (echtes
   "call rel32"/"ret", siehe codegen_x86.c). Arguments nutzen dieselbe
   eingeschraenkte SysArg-Grammatik wie SysCall(...) (Literal/Variable/
   AddressOf/Peek, kein verschachtelter Ausdruck). */
static NcStmt *parse_call(NcParser *p, int target_idx) {
    int func_idx = find_function_any(p, p->cur.text, p->cur.len);
    advance(p); /* Funktionsname */
    expect_kind(p, TOK_LPAREN, "'('");
    if (p->failed) return NULL;

    NcSysArg args[8];
    memset(args, 0, sizeof(args));
    uint32_t arg_count = 0;
    if (p->cur.kind != TOK_RPAREN) {
        for (;;) {
            if (arg_count >= 8u) { fail(p, "zu viele Argumente (v0.9-Limit 8)"); return NULL; }
            if (parse_sys_arg(p, &args[arg_count]) != 0) return NULL;
            arg_count++;
            if (p->cur.kind == TOK_COMMA) { advance(p); continue; }
            break;
        }
    }
    expect_kind(p, TOK_RPAREN, "')'");
    if (p->failed) return NULL;
    expect_kind(p, TOK_NEWLINE, "Zeilenende");
    if (p->failed) return NULL;

    NcStmt *st = (NcStmt *)calloc(1, sizeof(NcStmt));
    st->kind = STMT_ASSIGN_CALL;
    st->assign_var_index = (uint32_t)target_idx;
    st->call_func_index = (uint32_t)func_idx;
    for (uint32_t i = 0; i < arg_count; ++i) st->call_args[i] = args[i];
    st->call_arg_count = arg_count;
    return st;
}

/* AssignStmt := Ident "=" (IntExpr | SysCallExpr | CallExpr) NEWLINE
   (Ident muss bereits per "Dim ... As Integer" deklariert sein, oder
   ein per Namensmangling aufgeloester Parameter, siehe lookup_var()). */
static NcStmt *parse_assign(NcParser *p) {
    int idx = lookup_var(p, p->cur.text, p->cur.len);
    advance(p); /* Ident */
    expect_kind(p, TOK_EQUALS, "'='");
    if (p->failed) return NULL;

    if (is_ident(p, "SysCall")) return parse_assign_syscall(p, idx);
    if (p->cur.kind == TOK_IDENT && find_function_any(p, p->cur.text, p->cur.len) >= 0) {
        return parse_call(p, idx);
    }

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

/* Cond := Ident CmpOp (IntLiteral | Ident) - gemeinsame Bedingungs-
   Grammatik von If UND While (siehe NOVALANG_SPEC_v0.3.md/v0.6.md). Die
   linke Seite MUSS eine bereits deklarierte Integer-Variable sein (kein
   beliebiger Ausdruck). 0 = Erfolg, -1 = Fehler (p->failed gesetzt). */
static int parse_cond(NcParser *p, uint32_t *lhs_idx, NcCmpOp *cmp,
                       int *rhs_is_var, int64_t *rhs_lit, uint32_t *rhs_idx) {
    if (p->cur.kind != TOK_IDENT) { fail(p, "Integer-Variable erwartet (Bedingung)"); return -1; }
    int lidx = lookup_var(p, p->cur.text, p->cur.len);
    if (lidx < 0) { fail(p, "unbekannte Integer-Variable in Bedingung"); return -1; }
    *lhs_idx = (uint32_t)lidx;
    advance(p);
    if (!is_cmp_token(p->cur.kind)) { fail(p, "Vergleichsoperator erwartet (=, <>, <, >, <=, >=)"); return -1; }
    *cmp = token_to_cmp(p->cur.kind);
    advance(p);

    *rhs_is_var = 0;
    *rhs_lit = 0;
    *rhs_idx = 0;
    if (p->cur.kind == TOK_INT) {
        *rhs_lit = p->cur.int_val;
        advance(p);
    } else if (p->cur.kind == TOK_IDENT) {
        int idx = lookup_var(p, p->cur.text, p->cur.len);
        if (idx < 0) { fail(p, "unbekannte Integer-Variable in Bedingung"); return -1; }
        *rhs_is_var = 1;
        *rhs_idx = (uint32_t)idx;
        advance(p);
    } else {
        fail(p, "Ganzzahlliteral oder Variable erwartet");
        return -1;
    }
    return 0;
}

/* IfStmt := "If" Cond "Then" NEWLINE
              Statement*
            ("Else" NEWLINE Statement*)?
            "End" "If" NEWLINE */
static NcStmt *parse_if(NcParser *p) {
    advance(p); /* "If" */
    uint32_t lhs_idx; NcCmpOp cmp; int rhs_is_var; int64_t rhs_lit; uint32_t rhs_idx;
    if (parse_cond(p, &lhs_idx, &cmp, &rhs_is_var, &rhs_lit, &rhs_idx) != 0) return NULL;

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
    st->if_lhs_var_index = lhs_idx;
    st->if_cmp_op = cmp;
    st->if_rhs_is_var = rhs_is_var;
    st->if_rhs_literal = rhs_lit;
    st->if_rhs_var_index = rhs_idx;
    st->if_then_body = then_body;
    st->if_else_body = else_body;
    return st;
}

/* WhileStmt := "While" Cond NEWLINE Statement* "End" "While" NEWLINE
   (siehe NOVALANG_SPEC_v0.6.md) - kein "Exit While" in dieser Version. */
static NcStmt *parse_while(NcParser *p) {
    advance(p); /* "While" */
    uint32_t lhs_idx; NcCmpOp cmp; int rhs_is_var; int64_t rhs_lit; uint32_t rhs_idx;
    if (parse_cond(p, &lhs_idx, &cmp, &rhs_is_var, &rhs_lit, &rhs_idx) != 0) return NULL;

    expect_kind(p, TOK_NEWLINE, "Zeilenende");
    if (p->failed) return NULL;
    skip_newlines(p);

    NcStmt *body = parse_stmt_list(p, 0);
    if (p->failed) return NULL;

    expect_kind(p, TOK_KW_END, "'End'");
    expect_kind(p, TOK_KW_WHILE, "'While'");
    expect_kind(p, TOK_NEWLINE, "Zeilenende");
    if (p->failed) return NULL;

    NcStmt *st = (NcStmt *)calloc(1, sizeof(NcStmt));
    st->kind = STMT_WHILE;
    st->if_lhs_var_index = lhs_idx;
    st->if_cmp_op = cmp;
    st->if_rhs_is_var = rhs_is_var;
    st->if_rhs_literal = rhs_lit;
    st->if_rhs_var_index = rhs_idx;
    st->if_then_body = body;
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
    if (find_symbol(p, p->cur.text, p->cur.len) || find_int_var(p, p->cur.text, p->cur.len) >= 0 ||
        find_buffer(p, p->cur.text, p->cur.len) >= 0) {
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
    } else if (is_ident(p, "Buffer")) {
        /* "Dim buf As Buffer(N)" - N zero-initialisierte Byte in der
           Puffer-Unterregion (siehe NOVALANG_SPEC_v0.5.md), fuer
           Syscall-Argument-Structs. KEIN "=" (anders als String/
           Integer) - ein Puffer hat keinen sinnvollen Literalwert. */
        advance(p);
        expect_kind(p, TOK_LPAREN, "'('");
        if (p->failed) { free(name); return; }
        if (p->cur.kind != TOK_INT || p->cur.int_val <= 0) { fail(p, "positive Ganzzahl-Puffergroesse erwartet"); free(name); return; }
        uint32_t size = (uint32_t)p->cur.int_val;
        advance(p);
        expect_kind(p, TOK_RPAREN, "')'");
        expect_kind(p, TOK_NEWLINE, "Zeilenende");
        if (p->failed) { free(name); return; }
        if (p->mod->buf_count >= 16u) { fail(p, "zu viele Puffer (v0.5-Limit 16)"); free(name); return; }
        uint32_t used = 0;
        for (uint32_t i = 0; i < p->mod->buf_count; ++i) used += p->mod->buf_size[i];
        if (used + size > NC_BUFFER_REGION_SIZE) { fail(p, "Puffer-Region voll (v0.5-Limit 4 KiB gesamt)"); free(name); return; }
        p->mod->buf_names[p->mod->buf_count] = name;
        p->mod->buf_byte_offset[p->mod->buf_count] = used;
        p->mod->buf_size[p->mod->buf_count] = size;
        p->mod->buf_count++;
    } else {
        fail(p, "'String', 'Integer' oder 'Buffer' erwartet");
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
        if (is_ident(p, "PokeString")) {
            parse_poke_string(p);
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
        } else if (p->cur.kind == TOK_KW_WHILE) {
            st = parse_while(p);
        } else if (is_ident(p, "PokeInt")) {
            st = parse_poke(p, 4);
        } else if (is_ident(p, "PokeByte")) {
            st = parse_poke(p, 1);
        } else if (p->cur.kind == TOK_IDENT && lookup_var(p, p->cur.text, p->cur.len) >= 0) {
            st = parse_assign(p);
        } else {
            fail(p, "Anweisung erwartet (Dim/Console.WriteLine/Return/If/While/Zuweisung/PokeInt/PokeByte/PokeString)");
            return head;
        }
        if (p->failed) return head;
        if (tail) tail->next = st; else head = st;
        tail = st;
        skip_newlines(p);
    }
    return head;
}

/* FunctionDecl (v0.9, siehe NOVALANG_SPEC_v0.9.md): "Function" Ident
   "(" (Ident "As" "Integer" ("," Ident "As" "Integer")*)? ")" "As"
   "Int32" NEWLINE Statement* "End" "Function" NEWLINE.
   is_first_in_module == 1 erzwingt, dass dies "Main" ist (Main MUSS die
   erste Function im Module sein - haelt entry_offset=0 trivial, siehe
   codegen_x86.c). Jeder Parameter wird wie eine "Dim ... As Integer"-
   Variable registriert, nur mit gemangeltem Namen ("<FuncName>.<Param>",
   siehe lookup_var()) - dadurch braucht die restliche Parser-/Codegen-
   Maschinerie (Zuweisung, Bedingung, SysCall-/Call-Argument) KEINE
   Sonderbehandlung fuer Parameter. */
static NcFunction *parse_function(NcParser *p, int is_first_in_module) {
    advance(p); /* "Function" */
    if (p->cur.kind != TOK_IDENT) { fail(p, "Funktionsname erwartet"); return NULL; }
    int is_main = (p->cur.len == 4 && strncmp(p->cur.text, "Main", 4) == 0);
    if (is_first_in_module && !is_main) { fail(p, "die erste Function im Module muss 'Main' heissen"); return NULL; }
    if (!is_first_in_module && is_main) { fail(p, "'Main' darf nur einmal, als erste Function, deklariert werden"); return NULL; }
    if (!is_first_in_module && find_function(p, p->cur.text, p->cur.len) >= 0) { fail(p, "Function bereits definiert"); return NULL; }
    char *name = dup_ident(p);
    advance(p);
    expect_kind(p, TOK_LPAREN, "'('");
    if (p->failed) { free(name); return NULL; }

    NcFunction *fn = (NcFunction *)calloc(1, sizeof(NcFunction));
    fn->name = name;
    fn->is_main = is_main;
    p->current_func_name = name;

    if (p->cur.kind != TOK_RPAREN) {
        for (;;) {
            if (p->cur.kind != TOK_IDENT) { fail(p, "Parametername erwartet"); p->current_func_name = NULL; return fn; }
            char pname[160];
            int n = snprintf(pname, sizeof(pname), "%s.%.*s", name, (int)p->cur.len, p->cur.text);
            advance(p);
            expect_kind(p, TOK_KW_AS, "'As'");
            if (p->failed) { p->current_func_name = NULL; return fn; }
            if (!is_ident(p, "Integer")) { fail(p, "'Integer' erwartet (einzig unterstuetzter Parametertyp)"); p->current_func_name = NULL; return fn; }
            advance(p);
            if (n <= 0 || (size_t)n >= sizeof(pname)) { fail(p, "Parametername zu lang"); p->current_func_name = NULL; return fn; }
            if (p->mod->int_var_count >= 64u) { fail(p, "zu viele Integer-Variablen (v0.3-Limit 64)"); p->current_func_name = NULL; return fn; }
            if (fn->param_count >= 8u) { fail(p, "zu viele Parameter (v0.9-Limit 8)"); p->current_func_name = NULL; return fn; }
            char *pname_dup = (char *)malloc((size_t)n + 1);
            memcpy(pname_dup, pname, (size_t)n + 1);
            uint32_t var_idx = p->mod->int_var_count;
            p->mod->int_var_names[var_idx] = pname_dup;
            p->mod->int_var_initial[var_idx] = 0;
            p->mod->int_var_count++;
            fn->param_var_index[fn->param_count] = var_idx;
            fn->param_count++;
            if (p->cur.kind == TOK_COMMA) { advance(p); continue; }
            break;
        }
    }
    expect_kind(p, TOK_RPAREN, "')'");
    expect_kind(p, TOK_KW_AS, "'As'");
    if (p->failed) { p->current_func_name = NULL; return fn; }
    if (!is_ident(p, "Int32")) { fail(p, "'Int32' erwartet (kein anderer Rueckgabetyp unterstuetzt)"); p->current_func_name = NULL; return fn; }
    advance(p);
    expect_kind(p, TOK_NEWLINE, "Zeilenende");
    if (p->failed) { p->current_func_name = NULL; return fn; }
    skip_newlines(p);

    p->in_main = is_main;
    fn->body = parse_stmt_list(p, 0);
    p->current_func_name = NULL;
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

    /* v0.9-Vorab-Scan (siehe NcParser::all_func_names-Kommentar oben):
       sammelt die Namen ALLER "Function <Ident>(..." im Quelltext, BEVOR
       irgendein Funktionskoerper geparst wird - auf einem EIGENEN,
       unabhaengigen Lexer ueber denselben Quelltext (lx->src/lx->len),
       der eigentliche Parser-Lexer `lx` bleibt davon unberuehrt. Ohne
       das koennte Main() keine spaeter im Modul deklarierte Helfer-
       Function per Vorwaertsreferenz aufrufen. */
    {
        NcLexer prelex;
        nc_lexer_init(&prelex, lx->src, lx->len);
        for (;;) {
            NcToken t = nc_lexer_next(&prelex);
            if (prelex.has_error || t.kind == TOK_EOF) break;
            if (t.kind != TOK_KW_FUNCTION) continue;
            NcToken name_tok = nc_lexer_next(&prelex);
            if (prelex.has_error) break;
            if (name_tok.kind == TOK_IDENT && p.all_func_count < 16u) {
                char *nm = (char *)malloc((size_t)name_tok.len + 1u);
                memcpy(nm, name_tok.text, name_tok.len);
                nm[name_tok.len] = '\0';
                p.all_func_names[p.all_func_count++] = nm;
            }
        }
    }

    while (!p.failed && p.cur.kind == TOK_KW_FUNCTION) {
        int is_first = (mod->function_count == 0);
        NcFunction *fn = parse_function(&p, is_first);
        if (p.failed) goto error;
        if (mod->function_count >= 16u) { fail(&p, "zu viele Functions (v0.9-Limit 16)"); goto error; }
        mod->functions[mod->function_count] = fn;
        mod->function_count++;
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
    if (mod->function_count == 0) { fail(&p, "'Function Main' fehlt"); goto error; }

    return mod;

error:
    snprintf(out_error, out_error_cap, "%s", p.error);
    return NULL;
}

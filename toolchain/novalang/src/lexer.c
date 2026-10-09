#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* -------------------------------------------------------------------------
 * Keyword table (lower-case canonical forms)
 * ---------------------------------------------------------------------- */

typedef struct { const char *word; NlTokenKind kind; } KwEntry;

static const KwEntry kw_table[] = {
    {"addhandler", KW_ADDHANDLER}, {"and", KW_AND}, {"andalso", KW_ANDALSO},
    {"as", KW_AS}, {"async", KW_ASYNC}, {"await", KW_AWAIT},
    {"boolean", KW_BOOLEAN}, {"byref", KW_BYREF}, {"byte", KW_BYTE},
    {"byval", KW_BYVAL}, {"call", KW_CALL}, {"case", KW_CASE},
    {"catch", KW_CATCH}, {"char", KW_CHAR}, {"class", KW_CLASS},
    {"const", KW_CONST}, {"continue", KW_CONTINUE}, {"decimal", KW_DECIMAL},
    {"delegate", KW_DELEGATE}, {"dim", KW_DIM}, {"do", KW_DO},
    {"double", KW_DOUBLE}, {"each", KW_EACH}, {"else", KW_ELSE},
    {"elseif", KW_ELSEIF}, {"end", KW_END}, {"enum", KW_ENUM},
    {"event", KW_EVENT}, {"exit", KW_EXIT}, {"false", KW_FALSE},
    {"finally", KW_FINALLY}, {"for", KW_FOR}, {"friend", KW_FRIEND},
    {"function", KW_FUNCTION}, {"get", KW_GET}, {"handles", KW_HANDLES},
    {"if", KW_IF}, {"implements", KW_IMPLEMENTS}, {"imports", KW_IMPORTS},
    {"in", KW_IN}, {"inherits", KW_INHERITS}, {"integer", KW_INTEGER},
    {"interface", KW_INTERFACE}, {"is", KW_IS}, {"isnot", KW_ISNOT},
    {"long", KW_LONG}, {"loop", KW_LOOP}, {"me", KW_ME}, {"mod", KW_MOD},
    {"module", KW_MODULE}, {"mustinherit", KW_MUSTINHERIT},
    {"mustoverride", KW_MUSTOVERRIDE}, {"mybase", KW_MYBASE},
    {"myclass", KW_MYCLASS}, {"namespace", KW_NAMESPACE}, {"new", KW_NEW},
    {"next", KW_NEXT}, {"not", KW_NOT}, {"nothing", KW_NOTHING},
    {"of", KW_OF}, {"or", KW_OR}, {"orelse", KW_ORELSE},
    {"optional", KW_OPTIONAL}, {"overridable", KW_OVERRIDABLE},
    {"overrides", KW_OVERRIDES}, {"partial", KW_PARTIAL},
    {"private", KW_PRIVATE}, {"property", KW_PROPERTY},
    {"protected", KW_PROTECTED}, {"public", KW_PUBLIC},
    {"raiseevent", KW_RAISEEVENT}, {"readonly", KW_READONLY},
    {"return", KW_RETURN}, {"sbyte", KW_SBYTE}, {"select", KW_SELECT},
    {"set", KW_SET}, {"shared", KW_SHARED}, {"short", KW_SHORT},
    {"single", KW_SINGLE}, {"static", KW_STATIC}, {"step", KW_STEP},
    {"string", KW_STRING}, {"structure", KW_STRUCTURE}, {"sub", KW_SUB},
    {"then", KW_THEN}, {"throw", KW_THROW}, {"to", KW_TO},
    {"true", KW_TRUE}, {"try", KW_TRY}, {"uinteger", KW_UINTEGER},
    {"ulong", KW_ULONG}, {"ushort", KW_USHORT}, {"using", KW_USING},
    {"while", KW_WHILE}, {"with", KW_WITH}, {"writeonly", KW_WRITEONLY},
    {"xor", KW_XOR},
    {NULL, TK_INVALID}
};

static NlTokenKind keyword_lookup(const char *lower, uint32_t len) {
    for (int i = 0; kw_table[i].word; i++) {
        if (strlen(kw_table[i].word) == len &&
            memcmp(kw_table[i].word, lower, len) == 0) {
            return kw_table[i].kind;
        }
    }
    return TK_IDENT;
}

/* -------------------------------------------------------------------------
 * Helpers
 * ---------------------------------------------------------------------- */

static int is_ident_start(unsigned char c) {
    return isalpha(c) || c == '_' || c >= 0x80;
}

static int is_ident_cont(unsigned char c) {
    return isalnum(c) || c == '_' || c >= 0x80;
}

static char tolower_ascii(char c) {
    return (c >= 'A' && c <= 'Z') ? c + 32 : c;
}

static int add_token(NlLexer *lex, NlToken t) {
    if (lex->token_count >= lex->token_cap) {
        lex->token_cap = lex->token_cap ? lex->token_cap * 2 : 256;
        NlToken *nb = realloc(lex->tokens, lex->token_cap * sizeof(NlToken));
        if (!nb) return -1;
        lex->tokens = nb;
    }
    lex->tokens[lex->token_count++] = t;
    return 0;
}

static char peek(NlLexer *lex) {
    if (lex->pos >= lex->src_len) return 0;
    return lex->src[lex->pos];
}

static char peek2(NlLexer *lex) {
    if (lex->pos + 1 >= lex->src_len) return 0;
    return lex->src[lex->pos + 1];
}

static char advance(NlLexer *lex) {
    char c = lex->src[lex->pos++];
    if (c == '\n') { lex->line++; lex->col = 1; }
    else { lex->col++; }
    return c;
}

/* -------------------------------------------------------------------------
 * String interning
 * ---------------------------------------------------------------------- */

static uint32_t intern_string(NlLexer *lex, const char *s, uint32_t len) {
    for (uint32_t i = 0; i < lex->string_count; i++) {
        if (strcmp(lex->strings[i], s) == 0) return i;
    }
    if (lex->string_count >= lex->string_cap) {
        lex->string_cap = lex->string_cap ? lex->string_cap * 2 : 128;
        char **nb = realloc(lex->strings, lex->string_cap * sizeof(char *));
        if (!nb) return 0;
        lex->strings = nb;
    }
    char *copy = malloc(len + 1);
    if (!copy) return 0;
    memcpy(copy, s, len);
    copy[len] = '\0';
    lex->strings[lex->string_count] = copy;
    return lex->string_count++;
}

/* -------------------------------------------------------------------------
 * Token scanners
 * ---------------------------------------------------------------------- */

static NlToken scan_ident(NlLexer *lex) {
    NlToken t;
    memset(&t, 0, sizeof(t));
    t.range.start = (NlPos){ lex->pos, lex->line, lex->col };
    uint32_t start = lex->pos;

    while (lex->pos < lex->src_len && is_ident_cont((unsigned char)peek(lex)))
        advance(lex);

    uint32_t len = lex->pos - start;
    t.lexeme = lex->src + start;
    t.lexeme_len = len;
    t.range.end = (NlPos){ lex->pos, lex->line, lex->col };

    /* lower-case for keyword lookup */
    char lower[256];
    if (len < sizeof(lower)) {
        for (uint32_t i = 0; i < len; i++) lower[i] = tolower_ascii(t.lexeme[i]);
        lower[len] = '\0';
        t.kind = keyword_lookup(lower, len);
        if (t.kind == KW_TRUE) { t.kind = TK_BOOL_LIT; t.val.int_val = 1; }
        else if (t.kind == KW_FALSE) { t.kind = TK_BOOL_LIT; t.val.int_val = 0; }
        else if (t.kind == KW_NOTHING) { t.kind = TK_NOTHING; }
        if (t.kind == TK_IDENT)
            t.val.str_id = intern_string(lex, lower, len);
    } else {
        t.kind = TK_IDENT;
    }
    return t;
}

static NlToken scan_number(NlLexer *lex) {
    NlToken t;
    memset(&t, 0, sizeof(t));
    t.range.start = (NlPos){ lex->pos, lex->line, lex->col };
    uint32_t start = lex->pos;
    int is_float = 0;
    int is_hex = 0;

    if (peek(lex) == '&' && (peek2(lex) == 'H' || peek2(lex) == 'h')) {
        /* hex literal */
        advance(lex); advance(lex);
        is_hex = 1;
        while (lex->pos < lex->src_len && isxdigit((unsigned char)peek(lex)))
            advance(lex);
    } else {
        while (lex->pos < lex->src_len && isdigit((unsigned char)peek(lex)))
            advance(lex);
        if (peek(lex) == '.' && isdigit((unsigned char)peek2(lex))) {
            is_float = 1;
            advance(lex);
            while (lex->pos < lex->src_len && isdigit((unsigned char)peek(lex)))
                advance(lex);
        }
        if (peek(lex) == 'E' || peek(lex) == 'e') {
            is_float = 1;
            advance(lex);
            if (peek(lex) == '+' || peek(lex) == '-') advance(lex);
            while (lex->pos < lex->src_len && isdigit((unsigned char)peek(lex)))
                advance(lex);
        }
        /* type suffix: D = Decimal, F/S/L/I/US/UL/UI */
        char suf = peek(lex);
        if (suf == 'D' || suf == 'd') { advance(lex); t.kind = TK_DECIMAL_LIT; }
        else if (suf == 'F' || suf == 'f') { advance(lex); is_float = 1; }
        else if (!is_float && (suf == 'L' || suf == 'l')) advance(lex);
    }

    uint32_t len = lex->pos - start;
    t.lexeme = lex->src + start;
    t.lexeme_len = len;
    t.range.end = (NlPos){ lex->pos, lex->line, lex->col };

    if (t.kind == TK_DECIMAL_LIT) {
        /* leave val as 0 for now — full decimal needs big-int */
    } else if (is_float) {
        t.kind = TK_FLOAT_LIT;
        char buf[64];
        if (len < 63) { memcpy(buf, t.lexeme, len); buf[len] = '\0'; }
        t.val.float_val = atof(buf);
    } else {
        t.kind = TK_INT_LIT;
        if (is_hex)
            t.val.int_val = (int64_t)strtoll(t.lexeme + 2, NULL, 16);
        else {
            char buf[64];
            if (len < 63) { memcpy(buf, t.lexeme, len); buf[len] = '\0'; }
            t.val.int_val = (int64_t)strtoll(buf, NULL, 10);
        }
    }
    return t;
}

static NlToken scan_string(NlLexer *lex) {
    NlToken t;
    memset(&t, 0, sizeof(t));
    t.kind = TK_STRING_LIT;
    t.range.start = (NlPos){ lex->pos, lex->line, lex->col };
    advance(lex); /* consume opening " */
    uint32_t start = lex->pos;

    while (lex->pos < lex->src_len) {
        char c = peek(lex);
        if (c == '"') {
            if (peek2(lex) == '"') { advance(lex); advance(lex); continue; } /* escaped "" */
            break;
        }
        if (c == '\n' || c == '\r') break; /* unterminated */
        advance(lex);
    }

    uint32_t len = lex->pos - start;
    t.val.str_id = intern_string(lex, lex->src + start, len);
    if (peek(lex) == '"') advance(lex); /* consume closing " */
    t.lexeme = lex->src + start - 1;
    t.lexeme_len = len + 2;
    t.range.end = (NlPos){ lex->pos, lex->line, lex->col };
    return t;
}

/* -------------------------------------------------------------------------
 * Main lexer loop
 * ---------------------------------------------------------------------- */

int nl_lexer_init(NlLexer *lex, const char *src, uint32_t len) {
    memset(lex, 0, sizeof(*lex));
    lex->src = src;
    lex->src_len = len;
    lex->line = 1;
    lex->col = 1;
    return 0;
}

int nl_lexer_run(NlLexer *lex) {
    int pending_newline = 0; /* suppress initial newlines */

    while (lex->pos < lex->src_len) {
        char c = peek(lex);

        /* Skip CR */
        if (c == '\r') { advance(lex); continue; }

        /* Newline */
        if (c == '\n') {
            if (!lex->in_continuation) {
                NlToken t = { TK_NEWLINE,
                    {{lex->pos,lex->line,lex->col},{lex->pos+1,lex->line,lex->col+1}},
                    lex->src + lex->pos, 1, {0} };
                if (lex->token_count > 0)
                    add_token(lex, t);
            }
            advance(lex);
            lex->in_continuation = 0;
            continue;
        }

        /* Whitespace */
        if (c == ' ' || c == '\t') { advance(lex); continue; }

        /* Comment */
        if (c == '\'') {
            while (lex->pos < lex->src_len && peek(lex) != '\n') advance(lex);
            continue;
        }

        /* Line continuation _ */
        if (c == '_') {
            uint32_t save = lex->pos;
            advance(lex);
            /* Check that rest of line is whitespace/comment */
            int ok = 1;
            while (lex->pos < lex->src_len && peek(lex) != '\n') {
                char cc = peek(lex);
                if (cc == '\'') { while (lex->pos < lex->src_len && peek(lex) != '\n') advance(lex); break; }
                if (cc != ' ' && cc != '\t' && cc != '\r') { ok = 0; break; }
                advance(lex);
            }
            if (ok) { lex->in_continuation = 1; continue; }
            /* Not a continuation — treat as identifier start? Shouldn't happen in valid code */
            lex->pos = save;
        }

        NlToken t;
        memset(&t, 0, sizeof(t));
        NlPos sp = { lex->pos, lex->line, lex->col };

        /* Identifiers and keywords */
        if (is_ident_start((unsigned char)c)) {
            t = scan_ident(lex);
            add_token(lex, t);
            continue;
        }

        /* Escaped identifier [name] */
        if (c == '[') {
            advance(lex);
            uint32_t start = lex->pos;
            while (lex->pos < lex->src_len && peek(lex) != ']' && peek(lex) != '\n')
                advance(lex);
            uint32_t len = lex->pos - start;
            char lower[256];
            if (len < sizeof(lower)) {
                for (uint32_t i = 0; i < len; i++) lower[i] = tolower_ascii(lex->src[start+i]);
                lower[len] = '\0';
                t.val.str_id = intern_string(lex, lower, len);
            }
            if (peek(lex) == ']') advance(lex);
            t.kind = TK_IDENT_ESCAPED;
            t.lexeme = lex->src + start;
            t.lexeme_len = len;
            t.range.start = sp;
            t.range.end = (NlPos){lex->pos, lex->line, lex->col};
            add_token(lex, t);
            continue;
        }

        /* Numbers */
        if (isdigit((unsigned char)c)) {
            t = scan_number(lex);
            add_token(lex, t);
            continue;
        }
        /* Hex: &H... */
        if (c == '&' && (peek2(lex) == 'H' || peek2(lex) == 'h')) {
            t = scan_number(lex);
            add_token(lex, t);
            continue;
        }

        /* Strings */
        if (c == '"') {
            t = scan_string(lex);
            add_token(lex, t);
            continue;
        }

        /* Character literal: "x"c */
        /* (handled as string then checked by parser — simplified) */

        advance(lex);
        t.lexeme = lex->src + lex->pos - 1;
        t.lexeme_len = 1;
        t.range.start = sp;
        t.range.end = (NlPos){lex->pos, lex->line, lex->col};

        switch (c) {
            case '+':
                t.kind = (peek(lex) == '=') ? (advance(lex), OP_PLUS_EQ) : OP_PLUS; break;
            case '-':
                t.kind = (peek(lex) == '=') ? (advance(lex), OP_MINUS_EQ) : OP_MINUS; break;
            case '*':
                t.kind = (peek(lex) == '=') ? (advance(lex), OP_MUL_EQ) : OP_MUL; break;
            case '/':
                t.kind = (peek(lex) == '=') ? (advance(lex), OP_DIV_EQ) : OP_DIV; break;
            case '\\':
                t.kind = OP_IDIV; break;
            case '^':
                t.kind = OP_POW; break;
            case '&':
                t.kind = (peek(lex) == '=') ? (advance(lex), OP_AMP_EQ) : OP_AMP; break;
            case '=':
                t.kind = OP_EQ; break;
            case '<':
                if (peek(lex) == '=') { advance(lex); t.kind = OP_LE; }
                else if (peek(lex) == '>') { advance(lex); t.kind = OP_NEQ; }
                else t.kind = OP_LT;
                break;
            case '>':
                t.kind = (peek(lex) == '=') ? (advance(lex), OP_GE) : OP_GT; break;
            case '(':
                t.kind = PUNCT_LPAREN; break;
            case ')':
                t.kind = PUNCT_RPAREN; break;
            case ',':
                t.kind = PUNCT_COMMA; break;
            case '.':
                t.kind = PUNCT_DOT; break;
            case ':':
                /* colon can serve as statement separator (like newline) */
                t.kind = TK_NEWLINE; break;
            case '!':
                t.kind = PUNCT_BANG; break;
            case '#':
                t.kind = PUNCT_HASH; break;
            case '@':
                t.kind = PUNCT_AT; break;
            case '?':
                t.kind = PUNCT_QUESTION; break;
            default:
                t.kind = TK_INVALID;
                lex->error_count++;
                break;
        }
        add_token(lex, t);
    }

    /* EOF */
    NlToken eof;
    memset(&eof, 0, sizeof(eof));
    eof.kind = TK_EOF;
    eof.range.start = eof.range.end = (NlPos){lex->pos, lex->line, lex->col};
    add_token(lex, &eof);

    return lex->error_count ? -1 : 0;
}

void nl_lexer_free(NlLexer *lex) {
    free(lex->tokens);
    for (uint32_t i = 0; i < lex->string_count; i++) free(lex->strings[i]);
    free(lex->strings);
    memset(lex, 0, sizeof(*lex));
}

const char *nl_token_kind_name(NlTokenKind k) {
    switch (k) {
        case TK_INT_LIT:     return "IntLit";
        case TK_FLOAT_LIT:   return "FloatLit";
        case TK_STRING_LIT:  return "StringLit";
        case TK_BOOL_LIT:    return "BoolLit";
        case TK_NOTHING:     return "Nothing";
        case TK_IDENT:       return "Ident";
        case KW_DIM:         return "Dim";
        case KW_CONST:       return "Const";
        case KW_IF:          return "If";
        case KW_THEN:        return "Then";
        case KW_ELSE:        return "Else";
        case KW_ELSEIF:      return "ElseIf";
        case KW_END:         return "End";
        case KW_FOR:         return "For";
        case KW_NEXT:        return "Next";
        case KW_WHILE:       return "While";
        case KW_DO:          return "Do";
        case KW_LOOP:        return "Loop";
        case KW_FUNCTION:    return "Function";
        case KW_SUB:         return "Sub";
        case KW_RETURN:      return "Return";
        case KW_CLASS:       return "Class";
        case KW_MODULE:      return "Module";
        case KW_NAMESPACE:   return "Namespace";
        case KW_IMPORTS:     return "Imports";
        case KW_PUBLIC:      return "Public";
        case KW_PRIVATE:     return "Private";
        case OP_PLUS:        return "+";
        case OP_MINUS:       return "-";
        case OP_MUL:         return "*";
        case OP_DIV:         return "/";
        case OP_EQ:          return "=";
        case OP_NEQ:         return "<>";
        case OP_LT:          return "<";
        case OP_GT:          return ">";
        case OP_AMP:         return "&";
        case PUNCT_LPAREN:   return "(";
        case PUNCT_RPAREN:   return ")";
        case PUNCT_COMMA:    return ",";
        case PUNCT_DOT:      return ".";
        case TK_NEWLINE:     return "Newline";
        case TK_EOF:         return "EOF";
        default:             return "?";
    }
}

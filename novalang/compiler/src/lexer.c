#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

void nc_lexer_init(NcLexer *lx, const char *src, uint32_t len) {
    lx->src = src;
    lx->len = len;
    lx->pos = 0;
    lx->line = 1;
    lx->error[0] = '\0';
    lx->has_error = 0;
}

static int at_end(NcLexer *lx) { return lx->pos >= lx->len; }
static char peek(NcLexer *lx) { return at_end(lx) ? '\0' : lx->src[lx->pos]; }
static char peek2(NcLexer *lx) { return (lx->pos + 1 >= lx->len) ? '\0' : lx->src[lx->pos + 1]; }
static char advance(NcLexer *lx) { char c = lx->src[lx->pos++]; if (c == '\n') lx->line++; return c; }

static void set_error(NcLexer *lx, const char *msg) {
    if (!lx->has_error) {
        snprintf(lx->error, sizeof(lx->error), "Zeile %u: %s", lx->line, msg);
        lx->has_error = 1;
    }
}

static NcTokenKind keyword_kind(const char *text, uint32_t len) {
    struct { const char *word; NcTokenKind kind; } kws[] = {
        {"Module", TOK_KW_MODULE}, {"End", TOK_KW_END},
        {"Function", TOK_KW_FUNCTION}, {"As", TOK_KW_AS},
        {"Return", TOK_KW_RETURN}, {"Dim", TOK_KW_DIM},
        {"If", TOK_KW_IF}, {"Then", TOK_KW_THEN}, {"Else", TOK_KW_ELSE}
    };
    for (unsigned i = 0; i < sizeof(kws) / sizeof(kws[0]); ++i) {
        uint32_t wl = (uint32_t)strlen(kws[i].word);
        if (wl == len && strncmp(kws[i].word, text, len) == 0) return kws[i].kind;
    }
    return TOK_IDENT;
}

NcToken nc_lexer_next(NcLexer *lx) {
    NcToken tok;
    memset(&tok, 0, sizeof(tok));

    for (;;) {
        if (at_end(lx)) { tok.kind = TOK_EOF; tok.line = lx->line; return tok; }
        char c = peek(lx);
        if (c == ' ' || c == '\t' || c == '\r') { advance(lx); continue; }
        if (c == '\'') { /* Kommentar bis Zeilenende */
            while (!at_end(lx) && peek(lx) != '\n') advance(lx);
            continue;
        }
        break;
    }

    tok.line = lx->line;
    char c = peek(lx);

    if (c == '\n') {
        advance(lx);
        tok.kind = TOK_NEWLINE;
        return tok;
    }

    if (c == '.') { advance(lx); tok.kind = TOK_DOT; return tok; }
    if (c == '(') { advance(lx); tok.kind = TOK_LPAREN; return tok; }
    if (c == ')') { advance(lx); tok.kind = TOK_RPAREN; return tok; }
    if (c == ',') { advance(lx); tok.kind = TOK_COMMA; return tok; }
    if (c == '&') { advance(lx); tok.kind = TOK_AMP; return tok; }
    if (c == '=') { advance(lx); tok.kind = TOK_EQUALS; return tok; }
    if (c == '+') { advance(lx); tok.kind = TOK_PLUS; return tok; }
    if (c == '-') { advance(lx); tok.kind = TOK_MINUS; return tok; }
    if (c == '<') {
        advance(lx);
        if (peek(lx) == '>') { advance(lx); tok.kind = TOK_NE; return tok; }
        if (peek(lx) == '=') { advance(lx); tok.kind = TOK_LE; return tok; }
        tok.kind = TOK_LT;
        return tok;
    }
    if (c == '>') {
        advance(lx);
        if (peek(lx) == '=') { advance(lx); tok.kind = TOK_GE; return tok; }
        tok.kind = TOK_GT;
        return tok;
    }

    if (c == '"') {
        advance(lx);
        uint32_t start = lx->pos;
        char buf[1024];
        uint32_t blen = 0;
        while (!at_end(lx) && peek(lx) != '"') {
            if (peek(lx) == '\n') { set_error(lx, "Zeichenkette ohne schliessendes \""); break; }
            if (blen + 1 >= sizeof(buf)) { set_error(lx, "Zeichenkette zu lang (Limit 1023 Bytes, v0.1)"); break; }
            buf[blen++] = advance(lx);
        }
        if (lx->has_error) return tok;
        if (at_end(lx)) { set_error(lx, "Zeichenkette ohne schliessendes \""); return tok; }
        advance(lx); /* schliessendes " */
        (void)start;
        tok.kind = TOK_STRING;
        tok.str_val = (char *)malloc(blen + 1);
        memcpy(tok.str_val, buf, blen);
        tok.str_val[blen] = '\0';
        tok.str_len = blen;
        return tok;
    }

    if (isdigit((unsigned char)c)) {
        uint32_t start = lx->pos;
        while (!at_end(lx) && isdigit((unsigned char)peek(lx))) advance(lx);
        tok.kind = TOK_INT;
        tok.text = lx->src + start;
        tok.len = lx->pos - start;
        tok.int_val = strtoll(tok.text, NULL, 10);
        return tok;
    }

    if (isalpha((unsigned char)c) || c == '_') {
        uint32_t start = lx->pos;
        while (!at_end(lx) && (isalnum((unsigned char)peek(lx)) || peek(lx) == '_')) advance(lx);
        tok.text = lx->src + start;
        tok.len = lx->pos - start;
        tok.kind = keyword_kind(tok.text, tok.len);
        return tok;
    }

    (void)peek2;
    char msg[64];
    snprintf(msg, sizeof(msg), "unerwartetes Zeichen '%c'", c);
    set_error(lx, msg);
    return tok;
}

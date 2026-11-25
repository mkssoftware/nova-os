#ifndef NOVAC_LEXER_H
#define NOVAC_LEXER_H

#include <stdint.h>

/* NovaLang v0.1 Lexer - siehe specifications/NOVALANG_SPEC_v0.1.md Abschnitt 2
   fuer die vollstaendige, bewusst kleine Grammatik dieser Version. */

typedef enum NcTokenKind {
    TOK_EOF,
    TOK_NEWLINE,
    TOK_IDENT,
    TOK_STRING,
    TOK_INT,
    TOK_DOT,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_COMMA,
    TOK_AMP,    /* '&' - String-Verkettung, siehe NOVALANG_SPEC_v0.2.md */
    TOK_EQUALS, /* '=' - Zuweisung UND Vergleich "=" (v0.2/v0.3) */
    TOK_PLUS,   /* '+' - siehe NOVALANG_SPEC_v0.3.md */
    TOK_MINUS,  /* '-' */
    TOK_LT,     /* '<' */
    TOK_GT,     /* '>' */
    TOK_NE,     /* '<>' */
    TOK_LE,     /* '<=' */
    TOK_GE,     /* '>=' */

    /* Schluesselwoerter (als eigene Kinds, damit der Parser nicht staendig
       strcmp() gegen Ident-Text machen muss). */
    TOK_KW_MODULE,
    TOK_KW_END,
    TOK_KW_FUNCTION,
    TOK_KW_AS,
    TOK_KW_RETURN,
    TOK_KW_DIM,
    TOK_KW_IF,
    TOK_KW_THEN,
    TOK_KW_ELSE,
    TOK_KW_WHILE
} NcTokenKind;

typedef struct NcToken {
    NcTokenKind kind;
    const char *text;   /* Zeiger in den Quelltext (nicht NUL-terminiert fuer Ident/Keyword) */
    uint32_t    len;
    int64_t     int_val; /* nur TOK_INT */
    char       *str_val; /* nur TOK_STRING: entschachtelte, malloc'te Bytes (ohne Anfuehrungszeichen) */
    uint32_t    str_len;
    uint32_t    line;
} NcToken;

typedef struct NcLexer {
    const char *src;
    uint32_t    len;
    uint32_t    pos;
    uint32_t    line;
    char        error[256];
    int         has_error;
} NcLexer;

void    nc_lexer_init(NcLexer *lx, const char *src, uint32_t len);
/* Liefert das naechste Token. Bei Fehlern wird lx->has_error gesetzt und
   TOK_EOF zurueckgegeben - der Aufrufer MUSS has_error nach jedem Aufruf
   pruefen, bevor er das Token weiterverwendet. */
NcToken nc_lexer_next(NcLexer *lx);

#endif

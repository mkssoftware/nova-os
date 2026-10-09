#ifndef NL_LEXER_H
#define NL_LEXER_H

#include <stdint.h>
#include <stddef.h>

/* -------------------------------------------------------------------------
 * Source positions
 * ---------------------------------------------------------------------- */

typedef struct NlPos {
    uint32_t offset;   /* byte offset in source */
    uint32_t line;     /* 1-based */
    uint32_t col;      /* 1-based, UTF-8 character units */
} NlPos;

typedef struct NlRange {
    NlPos start;
    NlPos end;
} NlRange;

/* -------------------------------------------------------------------------
 * Token kinds  (case-insensitive keywords, VB.NET-oriented)
 * ---------------------------------------------------------------------- */

typedef enum NlTokenKind {

    /* Literals */
    TK_INT_LIT = 1,
    TK_FLOAT_LIT,
    TK_DECIMAL_LIT,
    TK_STRING_LIT,
    TK_CHAR_LIT,
    TK_BOOL_LIT,           /* True / False */
    TK_NOTHING,            /* Nothing */

    /* Identifier */
    TK_IDENT,
    TK_IDENT_ESCAPED,      /* [identifier] */

    /* --- Keywords ---------------------------------------------------- */
    KW_ADDHANDLER,
    KW_REMOVEHANDLER,
    KW_WHEN,
    KW_AND,
    KW_ANDALSO,
    KW_AS,
    KW_ASYNC,
    KW_AWAIT,
    KW_BOOLEAN,
    KW_BYREF,
    KW_BYTE,
    KW_BYVAL,
    KW_CALL,
    KW_CASE,
    KW_CATCH,
    KW_CHAR,
    KW_CLASS,
    KW_CONST,
    KW_CONTINUE,
    KW_DECIMAL,
    KW_DELEGATE,
    KW_DIM,
    KW_DO,
    KW_DOUBLE,
    KW_EACH,
    KW_ELSE,
    KW_ELSEIF,
    KW_END,
    KW_ENUM,
    KW_EVENT,
    KW_EXIT,
    KW_FALSE,
    KW_FINALLY,
    KW_FOR,
    KW_FRIEND,
    KW_FUNCTION,
    KW_GET,
    KW_HANDLES,
    KW_IF,
    KW_IMPLEMENTS,
    KW_IMPORTS,
    KW_IN,
    KW_INHERITS,
    KW_INTEGER,
    KW_INTERFACE,
    KW_IS,
    KW_ISNOT,
    KW_LONG,
    KW_LOOP,
    KW_ME,
    KW_MOD,
    KW_MODULE,
    KW_MUSTINHERIT,
    KW_MUSTOVERRIDE,
    KW_MYBASE,
    KW_MYCLASS,
    KW_NAMESPACE,
    KW_NEW,
    KW_NEXT,
    KW_NOT,
    KW_NOTHING,
    KW_OF,
    KW_OR,
    KW_ORELSE,
    KW_OPTIONAL,
    KW_OVERRIDABLE,
    KW_OVERRIDES,
    KW_PARTIAL,
    KW_PRIVATE,
    KW_PROPERTY,
    KW_PROTECTED,
    KW_PUBLIC,
    KW_RAISEEVENT,
    KW_READONLY,
    KW_RETURN,
    KW_SBYTE,
    KW_SELECT,
    KW_SET,
    KW_SHARED,
    KW_SHORT,
    KW_SINGLE,
    KW_STATIC,
    KW_STEP,
    KW_STRING,
    KW_STRUCTURE,
    KW_SUB,
    KW_THEN,
    KW_THROW,
    KW_TO,
    KW_TRUE,
    KW_TRY,
    KW_UINTEGER,
    KW_ULONG,
    KW_USHORT,
    KW_USING,
    KW_WHILE,
    KW_WITH,
    KW_WRITEONLY,
    KW_XOR,

    /* --- Operators ---------------------------------------------------- */
    OP_PLUS,       /* + */
    OP_MINUS,      /* - */
    OP_MUL,        /* * */
    OP_DIV,        /* / */
    OP_IDIV,       /* \ */
    OP_POW,        /* ^ */
    OP_AMP,        /* & (string concat) */
    OP_EQ,         /* = */
    OP_NEQ,        /* <> */
    OP_LT,         /* < */
    OP_LE,         /* <= */
    OP_GT,         /* > */
    OP_GE,         /* >= */
    OP_ASSIGN,     /* = (also OP_EQ, resolved by context) */

    /* Compound assignment */
    OP_PLUS_EQ,    /* += */
    OP_MINUS_EQ,   /* -= */
    OP_MUL_EQ,     /* *= */
    OP_DIV_EQ,     /* /= */
    OP_AMP_EQ,     /* &= */

    /* --- Punctuation -------------------------------------------------- */
    PUNCT_LPAREN,  /* ( */
    PUNCT_RPAREN,  /* ) */
    PUNCT_COMMA,   /* , */
    PUNCT_DOT,     /* . */
    PUNCT_COLON,   /* : */
    PUNCT_BANG,    /* ! */
    PUNCT_HASH,    /* # */
    PUNCT_AT,      /* @ */
    PUNCT_QUESTION, /* ? (nullable suffix) */

    /* --- Structure ---------------------------------------------------- */
    TK_NEWLINE,
    TK_LINE_CONT,  /* _ at end of line */
    TK_EOF,
    TK_INVALID,

    TK_KIND_COUNT

} NlTokenKind;

/* -------------------------------------------------------------------------
 * Token
 * ---------------------------------------------------------------------- */

typedef struct NlToken {
    NlTokenKind kind;
    NlRange     range;
    const char *lexeme;    /* points into source (NOT NUL-terminated) */
    uint32_t    lexeme_len;
    union {
        int64_t  int_val;
        double   float_val;
        uint32_t str_id;   /* interned string index */
    } val;
} NlToken;

/* -------------------------------------------------------------------------
 * Lexer
 * ---------------------------------------------------------------------- */

typedef struct NlLexer {
    const char *src;
    uint32_t    src_len;
    uint32_t    pos;       /* current byte offset */
    uint32_t    line;
    uint32_t    col;
    int         in_continuation; /* inside _ line continuation */

    /* token pool (dynamic) */
    NlToken    *tokens;
    uint32_t    token_count;
    uint32_t    token_cap;

    /* string intern pool */
    char      **strings;
    uint32_t    string_count;
    uint32_t    string_cap;

    /* errors */
    uint32_t    error_count;
    char        error_buf[1024];
} NlLexer;

/* -------------------------------------------------------------------------
 * API
 * ---------------------------------------------------------------------- */

int  nl_lexer_init(NlLexer *lex, const char *src, uint32_t len);
int  nl_lexer_run(NlLexer *lex);
void nl_lexer_free(NlLexer *lex);

const char *nl_token_kind_name(NlTokenKind k);

#endif /* NL_LEXER_H */

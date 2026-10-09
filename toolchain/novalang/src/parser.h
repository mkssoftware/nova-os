#ifndef NL_PARSER_H
#define NL_PARSER_H

#include "lexer.h"
#include "ast.h"
#include "diagnostic.h"

typedef struct NlParser {
    const NlToken *tokens;
    uint32_t       token_count;
    uint32_t       pos;          /* current token index */
    NlArena       *arena;
    NlDiagList    *diags;
    /* string intern pool (shared with lexer) */
    char         **strings;
    uint32_t       string_count;
} NlParser;

void    nl_parser_init(NlParser *p, const NlLexer *lex, NlArena *arena, NlDiagList *diags);
NlNode *nl_parse(NlParser *p);

#endif /* NL_PARSER_H */

#include "parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* -------------------------------------------------------------------------
 * Parser helpers
 * ---------------------------------------------------------------------- */

static const NlToken *cur(NlParser *p) {
    return &p->tokens[p->pos < p->token_count ? p->pos : p->token_count - 1];
}

static const NlToken *peek_tok(NlParser *p, int offset) {
    uint32_t idx = (uint32_t)((int)p->pos + offset);
    if (idx >= p->token_count) return &p->tokens[p->token_count - 1];
    return &p->tokens[idx];
}

static NlTokenKind cur_kind(NlParser *p) { return cur(p)->kind; }

static const NlToken *advance_tok(NlParser *p) {
    const NlToken *t = cur(p);
    if (p->pos < p->token_count - 1) p->pos++;
    return t;
}

static void skip_newlines(NlParser *p) {
    while (cur_kind(p) == TK_NEWLINE) advance_tok(p);
}

static int check(NlParser *p, NlTokenKind k) { return cur_kind(p) == k; }

static int match(NlParser *p, NlTokenKind k) {
    if (check(p, k)) { advance_tok(p); return 1; }
    return 0;
}

static const NlToken *expect(NlParser *p, NlTokenKind k) {
    if (check(p, k)) return advance_tok(p);
    NlRange r = cur(p)->range;
    nl_diag_emit(p->diags, DIAG_ERROR, r,
        "expected '%s' but got '%s'", nl_token_kind_name(k), nl_token_kind_name(cur_kind(p)));
    return cur(p);
}

static void expect_newline(NlParser *p) {
    if (check(p, TK_EOF)) return;
    if (check(p, TK_NEWLINE)) { advance_tok(p); return; }
    nl_diag_emit(p->diags, DIAG_ERROR, cur(p)->range, "expected newline");
}

static NlNode *err_node(NlParser *p) {
    NlNode *n = nl_node_new(p->arena, ND_INT_LIT, cur(p)->range);
    advance_tok(p);
    return n;
}

/* -------------------------------------------------------------------------
 * Forward declarations
 * ---------------------------------------------------------------------- */
static NlNode *parse_expr(NlParser *p);
static NlNode *parse_stmt(NlParser *p);
static NlNode *parse_block(NlParser *p);
static NlNode *parse_decl(NlParser *p);
static NlNode *parse_type_ref(NlParser *p);

/* -------------------------------------------------------------------------
 * Modifier flags
 * ---------------------------------------------------------------------- */
static uint32_t parse_modifiers(NlParser *p) {
    uint32_t flags = 0;
    for (;;) {
        switch (cur_kind(p)) {
            case KW_PUBLIC:      flags |= NL_ACC_PUBLIC;      advance_tok(p); break;
            case KW_PRIVATE:     flags |= NL_ACC_PRIVATE;     advance_tok(p); break;
            case KW_PROTECTED:   flags |= NL_ACC_PROTECTED;   advance_tok(p); break;
            case KW_FRIEND:      flags |= NL_ACC_FRIEND;      advance_tok(p); break;
            case KW_SHARED:      flags |= NL_ACC_SHARED;      advance_tok(p); break;
            case KW_READONLY:    flags |= NL_ACC_READONLY;    advance_tok(p); break;
            case KW_OVERRIDABLE: flags |= NL_ACC_OVERRIDABLE; advance_tok(p); break;
            case KW_MUSTOVERRIDE:flags |= NL_ACC_MUSTOVERRIDE;advance_tok(p); break;
            case KW_OVERRIDES:   flags |= NL_ACC_OVERRIDES;   advance_tok(p); break;
            case KW_ASYNC:       flags |= NL_ACC_ASYNC;       advance_tok(p); break;
            case KW_STATIC:      flags |= NL_ACC_STATIC;      advance_tok(p); break;
            default: return flags;
        }
    }
}

/* -------------------------------------------------------------------------
 * Type references
 * ---------------------------------------------------------------------- */
static NlNode *parse_type_ref(NlParser *p) {
    NlRange r = cur(p)->range;
    NlNode *n = nl_node_new(p->arena, ND_TYPE_NAME, r);
    if (!check(p, TK_IDENT) && cur_kind(p) < KW_BOOLEAN) {
        /* built-in type keyword */
        n->val.op = cur_kind(p);
        advance_tok(p);
    } else {
        n->val.str_id = cur(p)->val.str_id;
        advance_tok(p);
        /* dotted qualified name */
        while (check(p, PUNCT_DOT) && peek_tok(p,1)->kind == TK_IDENT) {
            advance_tok(p);
            NlNode *m = nl_node_new(p->arena, ND_TYPE_NAME, cur(p)->range);
            m->val.str_id = cur(p)->val.str_id;
            advance_tok(p);
            nl_node_push(p->arena, &n->children, m);
        }
    }
    /* Generic: T(Of ...) */
    if (check(p, PUNCT_LPAREN) && peek_tok(p,1)->kind == KW_OF) {
        advance_tok(p); advance_tok(p); /* ( Of */
        NlNode *gen = nl_node_new(p->arena, ND_GENERIC_TYPE, r);
        nl_node_push(p->arena, &gen->children, n);
        do {
            nl_node_push(p->arena, &gen->children, parse_type_ref(p));
        } while (match(p, PUNCT_COMMA));
        expect(p, PUNCT_RPAREN);
        n = gen;
    }
    /* Nullable: T? */
    if (check(p, PUNCT_QUESTION)) {
        advance_tok(p);
        NlNode *nul = nl_node_new(p->arena, ND_NULLABLE_TYPE, r);
        nl_node_push(p->arena, &nul->children, n);
        n = nul;
    }
    /* Array: T() */
    while (check(p, PUNCT_LPAREN) && peek_tok(p,1)->kind == PUNCT_RPAREN) {
        advance_tok(p); advance_tok(p);
        NlNode *arr = nl_node_new(p->arena, ND_ARRAY_TYPE, r);
        nl_node_push(p->arena, &arr->children, n);
        n = arr;
    }
    n->range.end = cur(p)->range.start;
    return n;
}

/* -------------------------------------------------------------------------
 * Parameter list
 * ---------------------------------------------------------------------- */
static NlNode *parse_param_list(NlParser *p) {
    NlRange r = cur(p)->range;
    NlNode *list = nl_node_new(p->arena, ND_PARAM_LIST, r);
    expect(p, PUNCT_LPAREN);
    while (!check(p, PUNCT_RPAREN) && !check(p, TK_EOF)) {
        NlRange pr = cur(p)->range;
        NlNode *param = nl_node_new(p->arena, ND_PARAM, pr);
        /* optional: ByVal / ByRef / Optional / ParamArray */
        if (match(p, KW_BYVAL)) {}
        else if (match(p, KW_BYREF)) param->flags |= NL_ACC_READONLY; /* reuse flag */
        else if (match(p, KW_OPTIONAL)) {}
        /* name */
        param->val.str_id = cur(p)->val.str_id;
        advance_tok(p);
        /* As type */
        if (match(p, KW_AS))
            nl_node_push(p->arena, &param->children, parse_type_ref(p));
        /* default value */
        if (match(p, OP_EQ))
            nl_node_push(p->arena, &param->children, parse_expr(p));
        nl_node_push(p->arena, &list->children, param);
        if (!match(p, PUNCT_COMMA)) break;
    }
    expect(p, PUNCT_RPAREN);
    return list;
}

/* -------------------------------------------------------------------------
 * Argument list
 * ---------------------------------------------------------------------- */
static NlNode *parse_arg_list(NlParser *p) {
    NlRange r = cur(p)->range;
    NlNode *list = nl_node_new(p->arena, ND_ARG_LIST, r);
    expect(p, PUNCT_LPAREN);
    while (!check(p, PUNCT_RPAREN) && !check(p, TK_EOF)) {
        NlRange ar = cur(p)->range;
        NlNode *arg = nl_node_new(p->arena, ND_ARG, ar);
        /* named arg: name := expr */
        if (check(p, TK_IDENT) && peek_tok(p,1)->kind == OP_EQ &&
            peek_tok(p,2) != NULL) {
            /* check if this is actually := — we approximate with := not existing,
               so named args use :=  which in our token stream would be two tokens.
               For simplicity treat plain = at arg position as assignment, not named. */
        }
        nl_node_push(p->arena, &arg->children, parse_expr(p));
        nl_node_push(p->arena, &list->children, arg);
        if (!match(p, PUNCT_COMMA)) break;
    }
    expect(p, PUNCT_RPAREN);
    return list;
}

/* -------------------------------------------------------------------------
 * Expressions
 * ---------------------------------------------------------------------- */

static NlNode *parse_primary(NlParser *p) {
    NlRange r = cur(p)->range;
    const NlToken *t = cur(p);

    switch (t->kind) {
        case TK_INT_LIT: {
            NlNode *n = nl_node_new(p->arena, ND_INT_LIT, r);
            n->val.int_val = t->val.int_val;
            advance_tok(p);
            return n;
        }
        case TK_FLOAT_LIT: case TK_DECIMAL_LIT: {
            NlNode *n = nl_node_new(p->arena, ND_FLOAT_LIT, r);
            n->val.float_val = t->val.float_val;
            advance_tok(p);
            return n;
        }
        case TK_STRING_LIT: {
            NlNode *n = nl_node_new(p->arena, ND_STRING_LIT, r);
            n->val.str_id = t->val.str_id;
            advance_tok(p);
            return n;
        }
        case TK_BOOL_LIT: {
            NlNode *n = nl_node_new(p->arena, ND_BOOL_LIT, r);
            n->val.int_val = t->val.int_val;
            advance_tok(p);
            return n;
        }
        case TK_NOTHING: {
            NlNode *n = nl_node_new(p->arena, ND_NOTHING, r);
            advance_tok(p);
            return n;
        }
        case KW_ME: case KW_MYBASE: case KW_MYCLASS: {
            NlNode *n = nl_node_new(p->arena, ND_IDENT, r);
            n->val.op = t->kind;
            advance_tok(p);
            return n;
        }
        case KW_NEW: {
            advance_tok(p);
            NlNode *n = nl_node_new(p->arena, ND_NEW_EXPR, r);
            nl_node_push(p->arena, &n->children, parse_type_ref(p));
            if (check(p, PUNCT_LPAREN))
                nl_node_push(p->arena, &n->children, parse_arg_list(p));
            return n;
        }
        case KW_NOT: {
            advance_tok(p);
            NlNode *n = nl_node_new(p->arena, ND_UNARY_EXPR, r);
            n->val.op = KW_NOT;
            nl_node_push(p->arena, &n->children, parse_primary(p));
            return n;
        }
        case OP_MINUS: {
            advance_tok(p);
            NlNode *n = nl_node_new(p->arena, ND_UNARY_EXPR, r);
            n->val.op = OP_MINUS;
            nl_node_push(p->arena, &n->children, parse_primary(p));
            return n;
        }
        case PUNCT_LPAREN: {
            advance_tok(p);
            NlNode *n = parse_expr(p);
            expect(p, PUNCT_RPAREN);
            return n;
        }
        case KW_IF: {
            /* If(cond, then, else) ternary */
            advance_tok(p);
            NlNode *n = nl_node_new(p->arena, ND_CONDITIONAL_EXPR, r);
            expect(p, PUNCT_LPAREN);
            nl_node_push(p->arena, &n->children, parse_expr(p));
            expect(p, PUNCT_COMMA);
            nl_node_push(p->arena, &n->children, parse_expr(p));
            expect(p, PUNCT_COMMA);
            nl_node_push(p->arena, &n->children, parse_expr(p));
            expect(p, PUNCT_RPAREN);
            return n;
        }
        case KW_AWAIT: {
            advance_tok(p);
            NlNode *n = nl_node_new(p->arena, ND_AWAIT_EXPR, r);
            nl_node_push(p->arena, &n->children, parse_expr(p));
            return n;
        }
        case TK_IDENT: case TK_IDENT_ESCAPED: {
            NlNode *n = nl_node_new(p->arena, ND_IDENT, r);
            n->val.str_id = t->val.str_id;
            advance_tok(p);
            return n;
        }
        default: {
            /* built-in type names used as identifiers (e.g. Integer.MaxValue) */
            if (t->kind >= KW_BOOLEAN && t->kind <= KW_XOR) {
                NlNode *n = nl_node_new(p->arena, ND_IDENT, r);
                n->val.op = t->kind;
                advance_tok(p);
                return n;
            }
            nl_diag_emit(p->diags, DIAG_ERROR, r, "unexpected token '%s' in expression",
                nl_token_kind_name(t->kind));
            return err_node(p);
        }
    }
}

static NlNode *parse_postfix(NlParser *p) {
    NlNode *n = parse_primary(p);
    for (;;) {
        NlRange r = cur(p)->range;
        if (check(p, PUNCT_DOT)) {
            advance_tok(p);
            NlNode *mem = nl_node_new(p->arena, ND_MEMBER_ACCESS, r);
            nl_node_push(p->arena, &mem->children, n);
            NlNode *name = nl_node_new(p->arena, ND_IDENT, cur(p)->range);
            name->val.str_id = cur(p)->val.str_id;
            advance_tok(p);
            nl_node_push(p->arena, &mem->children, name);
            n = mem;
        } else if (check(p, PUNCT_LPAREN)) {
            /* call or index */
            NlNode *call = nl_node_new(p->arena, ND_CALL_EXPR, r);
            nl_node_push(p->arena, &call->children, n);
            nl_node_push(p->arena, &call->children, parse_arg_list(p));
            n = call;
        } else if (check(p, PUNCT_BANG)) {
            advance_tok(p);
            NlNode *mem = nl_node_new(p->arena, ND_MEMBER_ACCESS, r);
            nl_node_push(p->arena, &mem->children, n);
            NlNode *name = nl_node_new(p->arena, ND_IDENT, cur(p)->range);
            name->val.str_id = cur(p)->val.str_id;
            advance_tok(p);
            nl_node_push(p->arena, &mem->children, name);
            n = mem;
        } else {
            break;
        }
    }
    return n;
}

static int get_binary_prec(NlTokenKind k) {
    switch (k) {
        case KW_OR: case KW_ORELSE: return 1;
        case KW_AND: case KW_ANDALSO: return 2;
        case KW_XOR: return 3;
        case OP_EQ: case OP_NEQ: case OP_LT: case OP_LE:
        case OP_GT: case OP_GE: case KW_IS: case KW_ISNOT: return 4;
        case OP_AMP: return 5;
        case OP_PLUS: case OP_MINUS: return 6;
        case KW_MOD: return 7;
        case OP_MUL: case OP_DIV: case OP_IDIV: return 8;
        case OP_POW: return 9;
        default: return -1;
    }
}

static NlNode *parse_binop(NlParser *p, int min_prec) {
    NlNode *left = parse_postfix(p);
    for (;;) {
        NlTokenKind op = cur_kind(p);
        int prec = get_binary_prec(op);
        if (prec < min_prec) break;
        NlRange r = cur(p)->range;
        advance_tok(p);
        NlNode *right = parse_binop(p, prec + 1);
        NlNode *bin = nl_node_new(p->arena, ND_BINARY_EXPR, r);
        bin->val.op = op;
        nl_node_push(p->arena, &bin->children, left);
        nl_node_push(p->arena, &bin->children, right);
        left = bin;
    }
    return left;
}

static NlNode *parse_expr(NlParser *p) {
    return parse_binop(p, 1);
}

/* -------------------------------------------------------------------------
 * Statements
 * ---------------------------------------------------------------------- */

static NlNode *parse_if_stmt(NlParser *p) {
    NlRange r = cur(p)->range;
    advance_tok(p); /* If */
    NlNode *n = nl_node_new(p->arena, ND_IF_STMT, r);
    nl_node_push(p->arena, &n->children, parse_expr(p));
    expect(p, KW_THEN);
    /* single-line If? */
    if (!check(p, TK_NEWLINE) && !check(p, TK_EOF)) {
        nl_node_push(p->arena, &n->children, parse_stmt(p));
        return n;
    }
    advance_tok(p); /* newline */
    NlNode *then_block = nl_node_new(p->arena, ND_BLOCK, cur(p)->range);
    while (!check(p, KW_END) && !check(p, KW_ELSE) && !check(p, KW_ELSEIF) && !check(p, TK_EOF)) {
        skip_newlines(p);
        if (check(p, KW_END) || check(p, KW_ELSE) || check(p, KW_ELSEIF) || check(p, TK_EOF)) break;
        nl_node_push(p->arena, &then_block->children, parse_stmt(p));
    }
    nl_node_push(p->arena, &n->children, then_block);
    while (check(p, KW_ELSEIF)) {
        NlRange er = cur(p)->range;
        advance_tok(p);
        NlNode *ei = nl_node_new(p->arena, ND_ELSEIF_CLAUSE, er);
        nl_node_push(p->arena, &ei->children, parse_expr(p));
        expect(p, KW_THEN);
        advance_tok(p); /* newline */
        NlNode *eib = nl_node_new(p->arena, ND_BLOCK, cur(p)->range);
        while (!check(p, KW_END) && !check(p, KW_ELSE) && !check(p, KW_ELSEIF) && !check(p, TK_EOF)) {
            skip_newlines(p);
            if (check(p, KW_END) || check(p, KW_ELSE) || check(p, KW_ELSEIF) || check(p, TK_EOF)) break;
            nl_node_push(p->arena, &eib->children, parse_stmt(p));
        }
        nl_node_push(p->arena, &ei->children, eib);
        nl_node_push(p->arena, &n->children, ei);
    }
    if (check(p, KW_ELSE)) {
        advance_tok(p);
        advance_tok(p); /* newline */
        NlNode *eb = nl_node_new(p->arena, ND_BLOCK, cur(p)->range);
        while (!check(p, KW_END) && !check(p, TK_EOF)) {
            skip_newlines(p);
            if (check(p, KW_END) || check(p, TK_EOF)) break;
            nl_node_push(p->arena, &eb->children, parse_stmt(p));
        }
        nl_node_push(p->arena, &n->children, eb);
    }
    expect(p, KW_END); expect(p, KW_IF);
    expect_newline(p);
    return n;
}

static NlNode *parse_for_stmt(NlParser *p) {
    NlRange r = cur(p)->range;
    advance_tok(p); /* For */
    NlNode *n;
    if (check(p, KW_EACH)) {
        advance_tok(p);
        n = nl_node_new(p->arena, ND_FOR_EACH_STMT, r);
        NlNode *var = nl_node_new(p->arena, ND_IDENT, cur(p)->range);
        var->val.str_id = cur(p)->val.str_id;
        advance_tok(p);
        if (match(p, KW_AS)) nl_node_push(p->arena, &var->children, parse_type_ref(p));
        nl_node_push(p->arena, &n->children, var);
        expect(p, KW_IN);
        nl_node_push(p->arena, &n->children, parse_expr(p));
    } else {
        n = nl_node_new(p->arena, ND_FOR_STMT, r);
        NlNode *var = nl_node_new(p->arena, ND_IDENT, cur(p)->range);
        var->val.str_id = cur(p)->val.str_id;
        advance_tok(p);
        if (match(p, KW_AS)) nl_node_push(p->arena, &var->children, parse_type_ref(p));
        nl_node_push(p->arena, &n->children, var);
        expect(p, OP_EQ);
        nl_node_push(p->arena, &n->children, parse_expr(p));
        expect(p, KW_TO);
        nl_node_push(p->arena, &n->children, parse_expr(p));
        if (match(p, KW_STEP))
            nl_node_push(p->arena, &n->children, parse_expr(p));
    }
    expect_newline(p);
    nl_node_push(p->arena, &n->children, parse_block(p));
    expect(p, KW_NEXT);
    if (check(p, TK_IDENT)) advance_tok(p); /* optional var name */
    expect_newline(p);
    return n;
}

static NlNode *parse_while_stmt(NlParser *p) {
    NlRange r = cur(p)->range;
    advance_tok(p);
    NlNode *n = nl_node_new(p->arena, ND_WHILE_STMT, r);
    nl_node_push(p->arena, &n->children, parse_expr(p));
    expect_newline(p);
    nl_node_push(p->arena, &n->children, parse_block(p));
    expect(p, KW_END); expect(p, KW_WHILE); expect_newline(p);
    return n;
}

static NlNode *parse_do_stmt(NlParser *p) {
    NlRange r = cur(p)->range;
    advance_tok(p);
    NlNode *n = nl_node_new(p->arena, ND_DO_STMT, r);
    /* Do While / Do Until */
    if (check(p, KW_WHILE) || check(p, TK_IDENT) /* Until */) {
        int is_until = (cur(p)->val.str_id != 0); /* simplified */
        advance_tok(p);
        nl_node_push(p->arena, &n->children, parse_expr(p));
        n->val.int_val = is_until;
    }
    expect_newline(p);
    nl_node_push(p->arena, &n->children, parse_block(p));
    expect(p, KW_LOOP);
    if (check(p, KW_WHILE)) {
        advance_tok(p);
        nl_node_push(p->arena, &n->children, parse_expr(p));
    }
    expect_newline(p);
    return n;
}

static NlNode *parse_try_stmt(NlParser *p) {
    NlRange r = cur(p)->range;
    advance_tok(p); /* Try */
    NlNode *n = nl_node_new(p->arena, ND_TRY_STMT, r);
    expect_newline(p);
    nl_node_push(p->arena, &n->children, parse_block(p));
    while (check(p, KW_CATCH)) {
        NlRange cr = cur(p)->range;
        advance_tok(p);
        NlNode *cc = nl_node_new(p->arena, ND_CATCH_CLAUSE, cr);
        if (check(p, TK_IDENT)) {
            NlNode *var = nl_node_new(p->arena, ND_IDENT, cur(p)->range);
            var->val.str_id = cur(p)->val.str_id;
            advance_tok(p);
            if (match(p, KW_AS)) nl_node_push(p->arena, &var->children, parse_type_ref(p));
            nl_node_push(p->arena, &cc->children, var);
        }
        if (check(p, KW_WHEN)) { advance_tok(p); nl_node_push(p->arena, &cc->children, parse_expr(p)); }
        expect_newline(p);
        nl_node_push(p->arena, &cc->children, parse_block(p));
        nl_node_push(p->arena, &n->children, cc);
    }
    if (check(p, KW_FINALLY)) {
        NlRange fr = cur(p)->range;
        advance_tok(p); expect_newline(p);
        NlNode *fc = nl_node_new(p->arena, ND_FINALLY_CLAUSE, fr);
        nl_node_push(p->arena, &fc->children, parse_block(p));
        nl_node_push(p->arena, &n->children, fc);
    }
    expect(p, KW_END); expect(p, KW_TRY); expect_newline(p);
    return n;
}

static NlNode *parse_var_decl(NlParser *p, int is_const) {
    NlRange r = cur(p)->range;
    advance_tok(p); /* Dim / Const */
    NlNode *n = nl_node_new(p->arena, is_const ? ND_CONST_DECL : ND_VAR_DECL, r);
    n->val.str_id = cur(p)->val.str_id;
    advance_tok(p);
    if (match(p, KW_AS)) nl_node_push(p->arena, &n->children, parse_type_ref(p));
    if (match(p, OP_EQ)) nl_node_push(p->arena, &n->children, parse_expr(p));
    expect_newline(p);
    return n;
}

static NlNode *parse_sub_decl(NlParser *p, uint32_t flags) {
    NlRange r = cur(p)->range;
    advance_tok(p); /* Sub */
    NlNode *n = nl_node_new(p->arena, ND_SUB_DECL, r);
    n->flags = flags;
    n->val.str_id = cur(p)->val.str_id;
    advance_tok(p);
    nl_node_push(p->arena, &n->children, parse_param_list(p));
    expect_newline(p);
    nl_node_push(p->arena, &n->children, parse_block(p));
    expect(p, KW_END); expect(p, KW_SUB); expect_newline(p);
    return n;
}

static NlNode *parse_function_decl(NlParser *p, uint32_t flags) {
    NlRange r = cur(p)->range;
    advance_tok(p); /* Function */
    NlNode *n = nl_node_new(p->arena, ND_FUNCTION_DECL, r);
    n->flags = flags;
    n->val.str_id = cur(p)->val.str_id;
    advance_tok(p);
    nl_node_push(p->arena, &n->children, parse_param_list(p));
    if (match(p, KW_AS)) nl_node_push(p->arena, &n->children, parse_type_ref(p));
    expect_newline(p);
    nl_node_push(p->arena, &n->children, parse_block(p));
    expect(p, KW_END); expect(p, KW_FUNCTION); expect_newline(p);
    return n;
}

static NlNode *parse_class_decl(NlParser *p, uint32_t flags) {
    NlRange r = cur(p)->range;
    advance_tok(p); /* Class / Structure / Module / Interface */
    NlNodeKind kind = (flags & 0x8000) ? ND_STRUCT_DECL :
                      (flags & 0x4000) ? ND_MODULE_DECL :
                      (flags & 0x2000) ? ND_INTERFACE_DECL : ND_CLASS_DECL;
    NlNode *n = nl_node_new(p->arena, kind, r);
    n->flags = flags & 0xFFF;
    n->val.str_id = cur(p)->val.str_id;
    advance_tok(p);
    if (match(p, KW_INHERITS)) nl_node_push(p->arena, &n->children, parse_type_ref(p));
    if (match(p, KW_IMPLEMENTS)) {
        do { nl_node_push(p->arena, &n->children, parse_type_ref(p)); } while (match(p, PUNCT_COMMA));
    }
    expect_newline(p);
    while (!check(p, KW_END) && !check(p, TK_EOF)) {
        skip_newlines(p);
        if (check(p, KW_END) || check(p, TK_EOF)) break;
        nl_node_push(p->arena, &n->children, parse_decl(p));
    }
    expect(p, KW_END);
    advance_tok(p); /* Class/Structure/Module/Interface keyword */
    expect_newline(p);
    return n;
}

static NlNode *parse_decl(NlParser *p) {
    skip_newlines(p);
    uint32_t flags = parse_modifiers(p);
    switch (cur_kind(p)) {
        case KW_SUB:       return parse_sub_decl(p, flags);
        case KW_FUNCTION:  return parse_function_decl(p, flags);
        case KW_DIM:       return parse_var_decl(p, 0);
        case KW_CONST:     return parse_var_decl(p, 1);
        case KW_CLASS:     return parse_class_decl(p, flags);
        case KW_STRUCTURE: return parse_class_decl(p, flags | 0x8000);
        case KW_MODULE:    return parse_class_decl(p, flags | 0x4000);
        case KW_INTERFACE: return parse_class_decl(p, flags | 0x2000);
        case KW_NAMESPACE: {
            NlRange r = cur(p)->range;
            advance_tok(p);
            NlNode *n = nl_node_new(p->arena, ND_NAMESPACE_DECL, r);
            n->val.str_id = cur(p)->val.str_id;
            advance_tok(p);
            expect_newline(p);
            while (!check(p, KW_END) && !check(p, TK_EOF)) {
                skip_newlines(p);
                if (check(p, KW_END) || check(p, TK_EOF)) break;
                nl_node_push(p->arena, &n->children, parse_decl(p));
            }
            expect(p, KW_END); expect(p, KW_NAMESPACE); expect_newline(p);
            return n;
        }
        case KW_ENUM: {
            NlRange r = cur(p)->range;
            advance_tok(p);
            NlNode *n = nl_node_new(p->arena, ND_ENUM_DECL, r);
            n->flags = flags;
            n->val.str_id = cur(p)->val.str_id;
            advance_tok(p);
            if (match(p, KW_AS)) nl_node_push(p->arena, &n->children, parse_type_ref(p));
            expect_newline(p);
            while (!check(p, KW_END) && !check(p, TK_EOF)) {
                skip_newlines(p);
                if (check(p, KW_END) || check(p, TK_EOF)) break;
                NlNode *m = nl_node_new(p->arena, ND_ENUM_MEMBER, cur(p)->range);
                m->val.str_id = cur(p)->val.str_id;
                advance_tok(p);
                if (match(p, OP_EQ)) nl_node_push(p->arena, &m->children, parse_expr(p));
                expect_newline(p);
                nl_node_push(p->arena, &n->children, m);
            }
            expect(p, KW_END); expect(p, KW_ENUM); expect_newline(p);
            return n;
        }
        case KW_PROPERTY: {
            NlRange r = cur(p)->range;
            advance_tok(p);
            NlNode *n = nl_node_new(p->arena, ND_PROPERTY_DECL, r);
            n->flags = flags;
            n->val.str_id = cur(p)->val.str_id;
            advance_tok(p);
            if (check(p, PUNCT_LPAREN)) nl_node_push(p->arena, &n->children, parse_param_list(p));
            if (match(p, KW_AS)) nl_node_push(p->arena, &n->children, parse_type_ref(p));
            expect_newline(p);
            while (!check(p, KW_END) && !check(p, TK_EOF)) {
                skip_newlines(p);
                if (check(p, KW_END) || check(p, TK_EOF)) break;
                nl_node_push(p->arena, &n->children, parse_decl(p));
            }
            expect(p, KW_END); expect(p, KW_PROPERTY); expect_newline(p);
            return n;
        }
        case KW_GET: case KW_SET: {
            NlTokenKind kw = cur_kind(p);
            NlRange r = cur(p)->range;
            advance_tok(p);
            NlNode *n = nl_node_new(p->arena, kw == KW_GET ? ND_FUNCTION_DECL : ND_SUB_DECL, r);
            n->val.op = kw;
            if (check(p, PUNCT_LPAREN)) nl_node_push(p->arena, &n->children, parse_param_list(p));
            expect_newline(p);
            nl_node_push(p->arena, &n->children, parse_block(p));
            expect(p, KW_END);
            advance_tok(p); /* Get/Set */
            expect_newline(p);
            return n;
        }
        default:
            return parse_stmt(p);
    }
}

static NlNode *parse_stmt(NlParser *p) {
    skip_newlines(p);
    NlRange r = cur(p)->range;
    switch (cur_kind(p)) {
        case KW_DIM:    return parse_var_decl(p, 0);
        case KW_CONST:  return parse_var_decl(p, 1);
        case KW_IF:     return parse_if_stmt(p);
        case KW_FOR:    return parse_for_stmt(p);
        case KW_WHILE:  return parse_while_stmt(p);
        case KW_DO:     return parse_do_stmt(p);
        case KW_TRY:    return parse_try_stmt(p);
        case KW_RETURN: {
            advance_tok(p);
            NlNode *n = nl_node_new(p->arena, ND_RETURN_STMT, r);
            if (!check(p, TK_NEWLINE) && !check(p, TK_EOF))
                nl_node_push(p->arena, &n->children, parse_expr(p));
            expect_newline(p);
            return n;
        }
        case KW_THROW: {
            advance_tok(p);
            NlNode *n = nl_node_new(p->arena, ND_THROW_STMT, r);
            if (!check(p, TK_NEWLINE) && !check(p, TK_EOF))
                nl_node_push(p->arena, &n->children, parse_expr(p));
            expect_newline(p);
            return n;
        }
        case KW_EXIT: {
            advance_tok(p);
            NlNode *n = nl_node_new(p->arena, ND_EXIT_STMT, r);
            n->val.op = cur_kind(p);
            advance_tok(p);
            expect_newline(p);
            return n;
        }
        case KW_CONTINUE: {
            advance_tok(p);
            NlNode *n = nl_node_new(p->arena, ND_CONTINUE_STMT, r);
            n->val.op = cur_kind(p);
            advance_tok(p);
            expect_newline(p);
            return n;
        }
        case KW_CALL: {
            advance_tok(p);
            NlNode *n = nl_node_new(p->arena, ND_CALL_STMT, r);
            nl_node_push(p->arena, &n->children, parse_expr(p));
            expect_newline(p);
            return n;
        }
        case KW_RAISEEVENT: {
            advance_tok(p);
            NlNode *n = nl_node_new(p->arena, ND_RAISEEVENT_STMT, r);
            n->val.str_id = cur(p)->val.str_id;
            advance_tok(p);
            if (check(p, PUNCT_LPAREN)) nl_node_push(p->arena, &n->children, parse_arg_list(p));
            expect_newline(p);
            return n;
        }
        case KW_ADDHANDLER: case KW_REMOVEHANDLER: {
            NlNodeKind kind = cur_kind(p) == KW_ADDHANDLER ? ND_ADDHANDLER_STMT : ND_REMOVEHANDLER_STMT;
            advance_tok(p);
            NlNode *n = nl_node_new(p->arena, kind, r);
            nl_node_push(p->arena, &n->children, parse_expr(p));
            expect(p, PUNCT_COMMA);
            nl_node_push(p->arena, &n->children, parse_expr(p));
            expect_newline(p);
            return n;
        }
        case KW_WITH: {
            advance_tok(p);
            NlNode *n = nl_node_new(p->arena, ND_WITH_STMT, r);
            nl_node_push(p->arena, &n->children, parse_expr(p));
            expect_newline(p);
            nl_node_push(p->arena, &n->children, parse_block(p));
            expect(p, KW_END); expect(p, KW_WITH); expect_newline(p);
            return n;
        }
        default: {
            /* expression-statement or assignment */
            NlNode *lhs = parse_expr(p);
            /* assignment operators */
            NlTokenKind ak = cur_kind(p);
            if (ak == OP_EQ || ak == OP_PLUS_EQ || ak == OP_MINUS_EQ ||
                ak == OP_MUL_EQ || ak == OP_DIV_EQ || ak == OP_AMP_EQ) {
                advance_tok(p);
                NlNode *rhs = parse_expr(p);
                NlNode *n = nl_node_new(p->arena, ak == OP_EQ ? ND_ASSIGN_STMT : ND_COMPOUND_ASSIGN, r);
                n->val.op = ak;
                nl_node_push(p->arena, &n->children, lhs);
                nl_node_push(p->arena, &n->children, rhs);
                expect_newline(p);
                return n;
            }
            NlNode *n = nl_node_new(p->arena, ND_CALL_STMT, r);
            nl_node_push(p->arena, &n->children, lhs);
            expect_newline(p);
            return n;
        }
    }
}

static NlNode *parse_block(NlParser *p) {
    NlRange r = cur(p)->range;
    NlNode *block = nl_node_new(p->arena, ND_BLOCK, r);
    static const NlTokenKind end_tokens[] = {
        KW_END, KW_ELSE, KW_ELSEIF, KW_CATCH, KW_FINALLY,
        KW_NEXT, KW_LOOP, TK_EOF, (NlTokenKind)0
    };
    for (;;) {
        skip_newlines(p);
        NlTokenKind k = cur_kind(p);
        int at_end = 0;
        for (int i = 0; end_tokens[i]; i++) if (k == end_tokens[i]) { at_end = 1; break; }
        if (at_end) break;
        nl_node_push(p->arena, &block->children, parse_decl(p));
    }
    return block;
}

/* -------------------------------------------------------------------------
 * Top-level
 * ---------------------------------------------------------------------- */
void nl_parser_init(NlParser *p, const NlLexer *lex, NlArena *arena, NlDiagList *diags) {
    memset(p, 0, sizeof(*p));
    p->tokens      = lex->tokens;
    p->token_count = lex->token_count;
    p->arena       = arena;
    p->diags       = diags;
    p->strings     = lex->strings;
    p->string_count= lex->string_count;
}

NlNode *nl_parse(NlParser *p) {
    NlRange r = { {0,1,1}, {0,1,1} };
    NlNode *unit = nl_node_new(p->arena, ND_COMPILATION_UNIT, r);
    skip_newlines(p);
    /* Imports */
    while (check(p, KW_IMPORTS)) {
        NlRange ir = cur(p)->range;
        advance_tok(p);
        NlNode *imp = nl_node_new(p->arena, ND_IMPORTS_STMT, ir);
        imp->val.str_id = cur(p)->val.str_id;
        advance_tok(p);
        while (check(p, PUNCT_DOT)) {
            advance_tok(p);
            NlNode *part = nl_node_new(p->arena, ND_IDENT, cur(p)->range);
            part->val.str_id = cur(p)->val.str_id;
            advance_tok(p);
            nl_node_push(p->arena, &imp->children, part);
        }
        expect_newline(p);
        nl_node_push(p->arena, &unit->children, imp);
        skip_newlines(p);
    }
    /* Declarations */
    while (!check(p, TK_EOF)) {
        skip_newlines(p);
        if (check(p, TK_EOF)) break;
        nl_node_push(p->arena, &unit->children, parse_decl(p));
    }
    return unit;
}

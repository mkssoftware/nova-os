#include "typechecker.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* -------------------------------------------------------------------------
 * Type table
 * ---------------------------------------------------------------------- */

NlTypeRef nl_type_register(NlTypeTable *t, NlArena *a, NlTypeKind kind,
                            uint32_t name_id, uint8_t is_value) {
    if (t->count >= t->cap) {
        t->cap = t->cap ? t->cap * 2 : 32;
        t->types = realloc(t->types, t->cap * sizeof(NlTypeDesc));
    }
    NlTypeRef id = t->count++;
    NlTypeDesc *d = &t->types[id];
    memset(d, 0, sizeof(*d));
    d->id            = id;
    d->kind          = kind;
    d->name_id       = name_id;
    d->is_value_type = is_value;
    (void)a;
    return id;
}

int nl_type_is_numeric(NlTypeRef t) {
    return t >= NL_TY_BYTE && t <= NL_TY_DECIMAL;
}

int nl_type_is_assignable(NlTypeRef dest, NlTypeRef src) {
    if (dest == src) return 1;
    if (dest == NL_TY_OBJECT) return 1;
    if (dest == NL_TY_DOUBLE && nl_type_is_numeric(src)) return 1;
    if (nl_type_is_numeric(dest) && nl_type_is_numeric(src)) return src <= dest;
    return 0;
}

/* -------------------------------------------------------------------------
 * Scope management
 * ---------------------------------------------------------------------- */

static NlScope *push_scope(NlTypeChecker *tc) {
    NlScope *s = nl_arena_alloc(tc->arena, sizeof(NlScope));
    s->symbols = NULL;
    s->parent  = tc->scope;
    tc->scope  = s;
    return s;
}

static void pop_scope(NlTypeChecker *tc) {
    if (tc->scope) tc->scope = tc->scope->parent;
}

static void declare(NlTypeChecker *tc, uint32_t name_id, NlTypeRef type, NlNode *decl) {
    NlSymbol *sym = nl_arena_alloc(tc->arena, sizeof(NlSymbol));
    sym->name_id  = name_id;
    sym->type     = type;
    sym->decl_node= decl;
    sym->next     = tc->scope->symbols;
    tc->scope->symbols = sym;
}

static NlSymbol *lookup(NlTypeChecker *tc, uint32_t name_id) {
    for (NlScope *s = tc->scope; s; s = s->parent) {
        for (NlSymbol *sym = s->symbols; sym; sym = sym->next)
            if (sym->name_id == name_id) return sym;
    }
    return NULL;
}

/* -------------------------------------------------------------------------
 * Initialise with built-in types
 * ---------------------------------------------------------------------- */

void nl_tc_init(NlTypeChecker *tc, NlArena *a, NlDiagList *diags,
                char **strings, uint32_t string_count) {
    memset(tc, 0, sizeof(*tc));
    tc->arena        = a;
    tc->diags        = diags;
    tc->strings      = strings;
    tc->string_count = string_count;

    tc->table = nl_arena_alloc(a, sizeof(NlTypeTable));
    memset(tc->table, 0, sizeof(NlTypeTable));

    /* Register built-ins in canonical order matching NL_TY_* constants */
    static const char *builtin_names[] = {
        "void","boolean","byte","sbyte","short","ushort",
        "integer","uinteger","long","ulong",
        "single","double","decimal","char","string","object"
    };
    for (int i = 0; i < NL_TY_BUILTIN_COUNT; i++)
        nl_type_register(tc->table, a, TY_PRIMITIVE, i /* placeholder name_id */, 1);
    (void)builtin_names;

    /* global scope */
    tc->scope = nl_arena_alloc(a, sizeof(NlScope));
    memset(tc->scope, 0, sizeof(NlScope));
}

/* -------------------------------------------------------------------------
 * Type node resolution
 * ---------------------------------------------------------------------- */

NlTypeRef nl_tc_resolve_type_node(NlTypeChecker *tc, NlNode *n) {
    if (!n) return NL_TY_VOID;
    if (n->kind == ND_NULLABLE_TYPE) {
        NlTypeRef inner = nl_tc_resolve_type_node(tc, n->children.items ? n->children.items[0] : NULL);
        /* create or find nullable wrapper */
        NlTypeRef nid = nl_type_register(tc->table, tc->arena, TY_NULLABLE, inner, 0);
        tc->table->types[nid].elem_type = inner;
        return nid;
    }
    if (n->kind == ND_ARRAY_TYPE) {
        NlTypeRef inner = nl_tc_resolve_type_node(tc, n->children.items ? n->children.items[0] : NULL);
        NlTypeRef aid = nl_type_register(tc->table, tc->arena, TY_ARRAY, inner, 0);
        tc->table->types[aid].elem_type = inner;
        return aid;
    }
    if (n->kind == ND_TYPE_NAME) {
        /* match built-in keywords */
        switch (n->val.op) {
            case KW_BOOLEAN: return NL_TY_BOOL;
            case KW_BYTE:    return NL_TY_BYTE;
            case KW_SBYTE:   return NL_TY_SBYTE;
            case KW_SHORT:   return NL_TY_SHORT;
            case KW_USHORT:  return NL_TY_USHORT;
            case KW_INTEGER: return NL_TY_INTEGER;
            case KW_UINTEGER:return NL_TY_UINTEGER;
            case KW_LONG:    return NL_TY_LONG;
            case KW_ULONG:   return NL_TY_ULONG;
            case KW_SINGLE:  return NL_TY_SINGLE;
            case KW_DOUBLE:  return NL_TY_DOUBLE;
            case KW_DECIMAL: return NL_TY_DECIMAL;
            case KW_CHAR:    return NL_TY_CHAR;
            case KW_STRING:  return NL_TY_STRING;
            default: break;
        }
        /* lookup user-defined */
        NlSymbol *sym = lookup(tc, n->val.str_id);
        if (sym) return sym->type;
        /* unknown type */
        nl_diag_emit(tc->diags, DIAG_ERROR, n->range, "unknown type");
        return NL_TY_OBJECT;
    }
    return NL_TY_OBJECT;
}

/* -------------------------------------------------------------------------
 * Expression type inference
 * ---------------------------------------------------------------------- */

NlTypeRef nl_tc_check_expr(NlTypeChecker *tc, NlNode *n) {
    if (!n) return NL_TY_VOID;
    NlTypeRef t = NL_TY_OBJECT;
    switch (n->kind) {
        case ND_INT_LIT:    t = NL_TY_LONG;    break;
        case ND_FLOAT_LIT:  t = NL_TY_DOUBLE;  break;
        case ND_STRING_LIT: t = NL_TY_STRING;  break;
        case ND_BOOL_LIT:   t = NL_TY_BOOL;    break;
        case ND_NOTHING:    t = NL_TY_OBJECT;  break;
        case ND_IDENT: {
            NlSymbol *sym = lookup(tc, n->val.str_id);
            if (sym) t = sym->type;
            else {
                nl_diag_emit(tc->diags, DIAG_ERROR, n->range,
                    "undefined identifier");
                t = NL_TY_OBJECT;
            }
            break;
        }
        case ND_BINARY_EXPR: {
            NlTypeRef lt = nl_tc_check_expr(tc, n->children.count > 0 ? n->children.items[0] : NULL);
            NlTypeRef rt = nl_tc_check_expr(tc, n->children.count > 1 ? n->children.items[1] : NULL);
            NlTokenKind op = n->val.op;
            if (op == OP_EQ || op == OP_NEQ || op == OP_LT || op == OP_LE ||
                op == OP_GT || op == OP_GE || op == KW_AND || op == KW_OR ||
                op == KW_ANDALSO || op == KW_ORELSE || op == KW_NOT || op == KW_XOR ||
                op == KW_IS || op == KW_ISNOT) {
                t = NL_TY_BOOL;
            } else if (op == OP_AMP) {
                t = NL_TY_STRING;
            } else {
                /* numeric: widening */
                t = lt > rt ? lt : rt;
            }
            (void)lt; (void)rt;
            break;
        }
        case ND_UNARY_EXPR: {
            NlTypeRef inner = nl_tc_check_expr(tc, n->children.count > 0 ? n->children.items[0] : NULL);
            t = n->val.op == KW_NOT ? NL_TY_BOOL : inner;
            break;
        }
        case ND_CALL_EXPR: {
            /* check callee and args; return type = object for now */
            nl_tc_check_expr(tc, n->children.count > 0 ? n->children.items[0] : NULL);
            if (n->children.count > 1) {
                NlNode *args = n->children.items[1];
                for (uint32_t i = 0; i < args->children.count; i++)
                    nl_tc_check_expr(tc, args->children.items[i]->children.count > 0 ?
                        args->children.items[i]->children.items[0] : NULL);
            }
            t = NL_TY_OBJECT;
            break;
        }
        case ND_MEMBER_ACCESS:
            nl_tc_check_expr(tc, n->children.count > 0 ? n->children.items[0] : NULL);
            t = NL_TY_OBJECT;
            break;
        case ND_NEW_EXPR:
            if (n->children.count > 0) t = nl_tc_resolve_type_node(tc, n->children.items[0]);
            break;
        case ND_CONDITIONAL_EXPR: {
            NlTypeRef cond = nl_tc_check_expr(tc, n->children.count > 0 ? n->children.items[0] : NULL);
            NlTypeRef tt   = nl_tc_check_expr(tc, n->children.count > 1 ? n->children.items[1] : NULL);
            NlTypeRef ft   = nl_tc_check_expr(tc, n->children.count > 2 ? n->children.items[2] : NULL);
            if (cond != NL_TY_BOOL)
                nl_diag_emit(tc->diags, DIAG_ERROR, n->range, "If() condition must be Boolean");
            t = tt > ft ? tt : ft;
            break;
        }
        case ND_AWAIT_EXPR:
            nl_tc_check_expr(tc, n->children.count > 0 ? n->children.items[0] : NULL);
            t = NL_TY_OBJECT;
            break;
        default:
            break;
    }
    n->type_ref = t;
    return t;
}

/* -------------------------------------------------------------------------
 * Statement and declaration checking
 * ---------------------------------------------------------------------- */

static void check_stmt(NlTypeChecker *tc, NlNode *n);
static void check_decl(NlTypeChecker *tc, NlNode *n);

static void check_block(NlTypeChecker *tc, NlNode *block) {
    if (!block) return;
    for (uint32_t i = 0; i < block->children.count; i++)
        check_decl(tc, block->children.items[i]);
}

static void check_stmt(NlTypeChecker *tc, NlNode *n) {
    if (!n) return;
    switch (n->kind) {
        case ND_ASSIGN_STMT: case ND_COMPOUND_ASSIGN: {
            NlTypeRef rt = nl_tc_check_expr(tc, n->children.count > 1 ? n->children.items[1] : NULL);
            NlTypeRef lt = nl_tc_check_expr(tc, n->children.count > 0 ? n->children.items[0] : NULL);
            if (!nl_type_is_assignable(lt, rt))
                nl_diag_emit(tc->diags, DIAG_WARNING, n->range, "type mismatch in assignment");
            break;
        }
        case ND_CALL_STMT:
            nl_tc_check_expr(tc, n->children.count > 0 ? n->children.items[0] : NULL);
            break;
        case ND_RETURN_STMT: {
            NlTypeRef t = n->children.count > 0 ?
                nl_tc_check_expr(tc, n->children.items[0]) : NL_TY_VOID;
            if (!nl_type_is_assignable(tc->current_return_type, t))
                nl_diag_emit(tc->diags, DIAG_WARNING, n->range, "return type mismatch");
            break;
        }
        case ND_IF_STMT: {
            NlTypeRef ct = nl_tc_check_expr(tc, n->children.count > 0 ? n->children.items[0] : NULL);
            if (ct != NL_TY_BOOL && ct != NL_TY_OBJECT)
                nl_diag_emit(tc->diags, DIAG_WARNING, n->range, "If condition should be Boolean");
            for (uint32_t i = 1; i < n->children.count; i++)
                check_block(tc, n->children.items[i]);
            break;
        }
        case ND_WHILE_STMT: {
            nl_tc_check_expr(tc, n->children.count > 0 ? n->children.items[0] : NULL);
            push_scope(tc);
            check_block(tc, n->children.count > 1 ? n->children.items[1] : NULL);
            pop_scope(tc);
            break;
        }
        case ND_FOR_STMT: {
            push_scope(tc);
            if (n->children.count > 0) {
                NlNode *var = n->children.items[0];
                NlTypeRef vt = var->children.count > 0 ?
                    nl_tc_resolve_type_node(tc, var->children.items[0]) : NL_TY_LONG;
                declare(tc, var->val.str_id, vt, var);
            }
            for (uint32_t i = 1; i < n->children.count; i++) {
                NlNode *child = n->children.items[i];
                if (child->kind == ND_BLOCK) check_block(tc, child);
                else nl_tc_check_expr(tc, child);
            }
            pop_scope(tc);
            break;
        }
        case ND_FOR_EACH_STMT: {
            push_scope(tc);
            if (n->children.count > 0) {
                NlNode *var = n->children.items[0];
                NlTypeRef vt = var->children.count > 0 ?
                    nl_tc_resolve_type_node(tc, var->children.items[0]) : NL_TY_OBJECT;
                declare(tc, var->val.str_id, vt, var);
            }
            if (n->children.count > 1) nl_tc_check_expr(tc, n->children.items[1]);
            if (n->children.count > 2) check_block(tc, n->children.items[2]);
            pop_scope(tc);
            break;
        }
        case ND_TRY_STMT: {
            push_scope(tc);
            check_block(tc, n->children.count > 0 ? n->children.items[0] : NULL);
            pop_scope(tc);
            for (uint32_t i = 1; i < n->children.count; i++)
                check_block(tc, n->children.items[i]);
            break;
        }
        case ND_THROW_STMT:
            if (n->children.count > 0) nl_tc_check_expr(tc, n->children.items[0]);
            break;
        case ND_VAR_DECL: case ND_CONST_DECL:
            check_decl(tc, n);
            break;
        case ND_BLOCK:
            check_block(tc, n);
            break;
        default:
            break;
    }
}

static void check_decl(NlTypeChecker *tc, NlNode *n) {
    if (!n) return;
    switch (n->kind) {
        case ND_VAR_DECL: case ND_CONST_DECL: {
            NlTypeRef t = NL_TY_OBJECT;
            /* first child might be type ref */
            int ci = 0;
            if (n->children.count > 0 && (n->children.items[0]->kind == ND_TYPE_NAME ||
                n->children.items[0]->kind == ND_NULLABLE_TYPE ||
                n->children.items[0]->kind == ND_ARRAY_TYPE)) {
                t = nl_tc_resolve_type_node(tc, n->children.items[0]);
                ci = 1;
            }
            if ((uint32_t)ci < n->children.count) {
                NlTypeRef init_t = nl_tc_check_expr(tc, n->children.items[ci]);
                if (t == NL_TY_OBJECT) t = init_t; /* infer */
            }
            declare(tc, n->val.str_id, t, n);
            n->type_ref = t;
            break;
        }
        case ND_FUNCTION_DECL: {
            NlTypeRef ret = NL_TY_VOID;
            /* find return type among children */
            for (uint32_t i = 0; i < n->children.count; i++) {
                NlNode *c = n->children.items[i];
                if (c->kind == ND_TYPE_NAME || c->kind == ND_NULLABLE_TYPE ||
                    c->kind == ND_ARRAY_TYPE) {
                    ret = nl_tc_resolve_type_node(tc, c); break;
                }
            }
            NlTypeRef saved = tc->current_return_type;
            tc->current_return_type = ret;
            push_scope(tc);
            /* declare params */
            for (uint32_t i = 0; i < n->children.count; i++) {
                NlNode *c = n->children.items[i];
                if (c->kind == ND_PARAM_LIST) {
                    for (uint32_t j = 0; j < c->children.count; j++) {
                        NlNode *par = c->children.items[j];
                        NlTypeRef pt = par->children.count > 0 ?
                            nl_tc_resolve_type_node(tc, par->children.items[0]) : NL_TY_OBJECT;
                        declare(tc, par->val.str_id, pt, par);
                    }
                }
            }
            /* check body */
            for (uint32_t i = 0; i < n->children.count; i++) {
                NlNode *c = n->children.items[i];
                if (c->kind == ND_BLOCK) check_block(tc, c);
            }
            pop_scope(tc);
            tc->current_return_type = saved;
            break;
        }
        case ND_SUB_DECL: {
            NlTypeRef saved = tc->current_return_type;
            tc->current_return_type = NL_TY_VOID;
            push_scope(tc);
            for (uint32_t i = 0; i < n->children.count; i++) {
                NlNode *c = n->children.items[i];
                if (c->kind == ND_PARAM_LIST) {
                    for (uint32_t j = 0; j < c->children.count; j++) {
                        NlNode *par = c->children.items[j];
                        NlTypeRef pt = par->children.count > 0 ?
                            nl_tc_resolve_type_node(tc, par->children.items[0]) : NL_TY_OBJECT;
                        declare(tc, par->val.str_id, pt, par);
                    }
                } else if (c->kind == ND_BLOCK) {
                    check_block(tc, c);
                }
            }
            pop_scope(tc);
            tc->current_return_type = saved;
            break;
        }
        case ND_CLASS_DECL: case ND_STRUCT_DECL: case ND_MODULE_DECL:
        case ND_INTERFACE_DECL: case ND_NAMESPACE_DECL: {
            push_scope(tc);
            for (uint32_t i = 0; i < n->children.count; i++)
                check_decl(tc, n->children.items[i]);
            pop_scope(tc);
            break;
        }
        case ND_COMPILATION_UNIT:
            for (uint32_t i = 0; i < n->children.count; i++)
                check_decl(tc, n->children.items[i]);
            break;
        default:
            check_stmt(tc, n);
            break;
    }
}

void nl_tc_check(NlTypeChecker *tc, NlNode *unit) {
    check_decl(tc, unit);
}

/*
 * codegen.c – NovaLang → C transpiler
 *
 * Walks the typed AST and emits a .c file, then calls GCC to produce .exe.
 */
#include "codegen.h"
#include "ast.h"
#include "lexer.h"
#include "typechecker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------------------------
 * Runtime preamble embedded in every generated C file
 * ---------------------------------------------------------------------- */
static const char *nl_runtime =
"#include <stdio.h>\n"
"#include <stdlib.h>\n"
"#include <string.h>\n"
"#include <math.h>\n"
"\n"
"static char _nl_sb[131072];\n"
"static int  _nl_sb_pos = 0;\n"
"static char* nl_fmt_i(long long v){"
" char*b=_nl_sb+_nl_sb_pos;_nl_sb_pos=(_nl_sb_pos+64)&0x1FFFF;"
" snprintf(b,64,\"%lld\",v);return b;}\n"
"static char* nl_fmt_d(double v){"
" char*b=_nl_sb+_nl_sb_pos;_nl_sb_pos=(_nl_sb_pos+64)&0x1FFFF;"
" snprintf(b,64,\"%g\",v);return b;}\n"
"static char* nl_fmt_b(int v){return v?\"True\":\"False\";}\n"
"static char* nl_cat(const char*a,const char*b){"
" size_t la=strlen(a),lb=strlen(b);"
" char*r=_nl_sb+_nl_sb_pos;"
" _nl_sb_pos=(_nl_sb_pos+(int)(la+lb+1))&0x1FFFF;"
" memcpy(r,a,la);memcpy(r+la,b,lb+1);return r;}\n"
"\n";

/* -------------------------------------------------------------------------
 * Codegen context
 * ---------------------------------------------------------------------- */
typedef struct {
    FILE          *f;
    NlLexer       *lex;
    NlTypeChecker *tc;
    int            indent;
} CgCtx;

static void cg_indent(CgCtx *cx) {
    for (int i = 0; i < cx->indent; i++) fputc(' ', cx->f), fputc(' ', cx->f),
                                         fputc(' ', cx->f), fputc(' ', cx->f);
}

/* Get interned string by id */
static const char *cg_str(CgCtx *cx, uint32_t id) {
    if (id < cx->lex->string_count && cx->lex->strings[id])
        return cx->lex->strings[id];
    return "_";
}

/* -------------------------------------------------------------------------
 * Type emission
 * ---------------------------------------------------------------------- */
static void emit_c_type_ref(CgCtx *cx, NlTypeRef t) {
    switch (t) {
        case NL_TY_VOID:     fputs("void",                  cx->f); return;
        case NL_TY_BOOL:     fputs("int",                   cx->f); return;
        case NL_TY_BYTE:     fputs("unsigned char",         cx->f); return;
        case NL_TY_SBYTE:    fputs("signed char",           cx->f); return;
        case NL_TY_SHORT:    fputs("short",                 cx->f); return;
        case NL_TY_USHORT:   fputs("unsigned short",        cx->f); return;
        case NL_TY_INTEGER:  fputs("long long",             cx->f); return;
        case NL_TY_UINTEGER: fputs("unsigned long long",    cx->f); return;
        case NL_TY_LONG:     fputs("long long",             cx->f); return;
        case NL_TY_ULONG:    fputs("unsigned long long",    cx->f); return;
        case NL_TY_SINGLE:   fputs("float",                 cx->f); return;
        case NL_TY_DOUBLE:   fputs("double",                cx->f); return;
        case NL_TY_DECIMAL:  fputs("double",                cx->f); return;
        case NL_TY_CHAR:     fputs("char",                  cx->f); return;
        case NL_TY_STRING:   fputs("char*",                 cx->f); return;
        case NL_TY_OBJECT:   fputs("void*",                 cx->f); return;
        default: {
            /* user-defined type */
            if (cx->tc && t < cx->tc->table->count)
                fputs(cg_str(cx, cx->tc->table->types[t].name_id), cx->f);
            else
                fputs("void*", cx->f);
        }
    }
}

static void emit_c_type_node(CgCtx *cx, NlNode *n) {
    if (!n) { fputs("void", cx->f); return; }
    if (n->kind != ND_TYPE_NAME) { fputs("void*", cx->f); return; }
    switch (n->val.op) {
        case KW_BOOLEAN:  fputs("int",              cx->f); return;
        case KW_BYTE:     fputs("unsigned char",    cx->f); return;
        case KW_SBYTE:    fputs("signed char",      cx->f); return;
        case KW_SHORT:    fputs("short",            cx->f); return;
        case KW_USHORT:   fputs("unsigned short",   cx->f); return;
        case KW_INTEGER:  fputs("long long",        cx->f); return;
        case KW_UINTEGER: fputs("unsigned long long",cx->f);return;
        case KW_LONG:     fputs("long long",        cx->f); return;
        case KW_ULONG:    fputs("unsigned long long",cx->f);return;
        case KW_SINGLE:   fputs("float",            cx->f); return;
        case KW_DOUBLE:   fputs("double",           cx->f); return;
        case KW_DECIMAL:  fputs("double",           cx->f); return;
        case KW_CHAR:     fputs("char",             cx->f); return;
        case KW_STRING:   fputs("char*",            cx->f); return;
        case 0:
            /* identifier-based type name */
            fputs(cg_str(cx, n->val.str_id), cx->f);
            return;
        default:
            fputs("void*", cx->f); return;
    }
}

/* default type for a variable when no explicit type given */
static void emit_default_c_type(CgCtx *cx, NlTypeRef t) {
    if (t == NL_TY_OBJECT || t == 0)
        fputs("long long", cx->f);
    else
        emit_c_type_ref(cx, t);
}

/* -------------------------------------------------------------------------
 * String literal emission (handle VB "" escaping)
 * ---------------------------------------------------------------------- */
static void emit_c_string(CgCtx *cx, const char *s) {
    fputc('"', cx->f);
    while (*s) {
        if (s[0] == '"' && s[1] == '"') { fputs("\\\"", cx->f); s += 2; continue; }
        switch (*s) {
            case '"':  fputs("\\\"", cx->f); break;
            case '\\': fputs("\\\\", cx->f); break;
            case '\n': fputs("\\n",  cx->f); break;
            case '\r': fputs("\\r",  cx->f); break;
            case '\t': fputs("\\t",  cx->f); break;
            default:   fputc(*s,     cx->f); break;
        }
        s++;
    }
    fputc('"', cx->f);
}

/* -------------------------------------------------------------------------
 * Helper: collect dotted identifier path as a flat string
 * e.g. Nova.Math.Sqrt → "Nova.Math.Sqrt"
 * ---------------------------------------------------------------------- */
static void collect_path(CgCtx *cx, NlNode *n, char *buf, size_t cap) {
    if (!n || cap == 0) return;
    if (n->kind == ND_IDENT) {
        strncat(buf, cg_str(cx, n->val.str_id), cap - strlen(buf) - 1);
    } else if (n->kind == ND_MEMBER_ACCESS && n->children.count >= 2) {
        collect_path(cx, n->children.items[0], buf, cap);
        strncat(buf, ".", cap - strlen(buf) - 1);
        collect_path(cx, n->children.items[1], buf, cap);
    }
}

/* -------------------------------------------------------------------------
 * Check if a type ref refers to an enum
 * ---------------------------------------------------------------------- */
static int is_enum(CgCtx *cx, NlTypeRef t) {
    return cx->tc && t < cx->tc->table->count &&
           cx->tc->table->types[t].kind == TY_ENUM;
}

/* -------------------------------------------------------------------------
 * Forward declaration
 * ---------------------------------------------------------------------- */
static void emit_expr(CgCtx *cx, NlNode *n);
static void emit_stmt(CgCtx *cx, NlNode *n);
static void emit_block_body(CgCtx *cx, NlNode *block);

/* -------------------------------------------------------------------------
 * Expression emission
 * ---------------------------------------------------------------------- */

/* Emit expr, auto-converting to string if needed for & concatenation */
static void emit_as_string(CgCtx *cx, NlNode *n) {
    NlTypeRef t = n ? n->type_ref : NL_TY_OBJECT;
    if (t == NL_TY_STRING) {
        emit_expr(cx, n);
    } else if (t == NL_TY_DOUBLE || t == NL_TY_SINGLE || t == NL_TY_DECIMAL) {
        fputs("nl_fmt_d(", cx->f); emit_expr(cx, n); fputc(')', cx->f);
    } else if (t == NL_TY_BOOL) {
        fputs("nl_fmt_b(", cx->f); emit_expr(cx, n); fputc(')', cx->f);
    } else if (t == NL_TY_OBJECT || t == 0) {
        /* unknown type: try to emit as-is (it might already be a string) */
        emit_expr(cx, n);
    } else {
        fputs("nl_fmt_i(", cx->f); emit_expr(cx, n); fputc(')', cx->f);
    }
}

/* Emit call arguments */
static void emit_args(CgCtx *cx, NlNode *arglist) {
    if (!arglist) return;
    for (uint32_t i = 0; i < arglist->children.count; i++) {
        if (i > 0) fputs(", ", cx->f);
        NlNode *arg = arglist->children.items[i];
        NlNode *val = arg->children.count > 0 ? arg->children.items[0] : NULL;
        emit_expr(cx, val);
    }
}

/* Emit CStr(arg) with type-aware format selection */
static void emit_cstr_call(CgCtx *cx, NlNode *arglist) {
    NlNode *arg = (arglist && arglist->children.count > 0) ?
                  arglist->children.items[0] : NULL;
    NlNode *val = (arg && arg->children.count > 0) ? arg->children.items[0] : NULL;
    NlTypeRef t = val ? val->type_ref : NL_TY_OBJECT;
    if (t == NL_TY_STRING) {
        emit_expr(cx, val); return;
    } else if (t == NL_TY_DOUBLE || t == NL_TY_SINGLE || t == NL_TY_DECIMAL) {
        fputs("nl_fmt_d(", cx->f); emit_expr(cx, val); fputc(')', cx->f);
    } else if (t == NL_TY_BOOL) {
        fputs("nl_fmt_b(", cx->f); emit_expr(cx, val); fputc(')', cx->f);
    } else {
        fputs("nl_fmt_i(", cx->f); emit_expr(cx, val); fputc(')', cx->f);
    }
}

static void emit_expr(CgCtx *cx, NlNode *n) {
    if (!n) { fputs("0", cx->f); return; }
    switch (n->kind) {
        case ND_INT_LIT:
            fprintf(cx->f, "%lldLL", (long long)n->val.int_val);
            break;
        case ND_FLOAT_LIT:
            fprintf(cx->f, "%.17g", n->val.float_val);
            break;
        case ND_STRING_LIT:
            emit_c_string(cx, cg_str(cx, n->val.str_id));
            break;
        case ND_BOOL_LIT:
            fputs(n->val.int_val ? "1" : "0", cx->f);
            break;
        case ND_NOTHING:
            fputs("NULL", cx->f);
            break;
        case ND_IDENT:
            fputs(cg_str(cx, n->val.str_id), cx->f);
            break;
        case ND_MEMBER_ACCESS: {
            NlNode *obj = n->children.count > 0 ? n->children.items[0] : NULL;
            NlNode *mem = n->children.count > 1 ? n->children.items[1] : NULL;
            /* Enum member access: Operation.Addition → just emit member name */
            if (obj && is_enum(cx, obj->type_ref)) {
                if (mem) fputs(cg_str(cx, mem->val.str_id), cx->f);
                break;
            }
            /* Otherwise: struct field or namespace path */
            emit_expr(cx, obj);
            fputc('.', cx->f);
            if (mem) fputs(cg_str(cx, mem->val.str_id), cx->f);
            break;
        }
        case ND_CALL_EXPR: {
            NlNode *callee  = n->children.count > 0 ? n->children.items[0] : NULL;
            NlNode *arglist = n->children.count > 1 ? n->children.items[1] : NULL;

            /* Collect dotted path of callee */
            char path[256] = "";
            collect_path(cx, callee, path, sizeof(path));

            /* Dispatch known built-in calls */
#define PATH_IC(s) (strcasecmp(path, s) == 0)
            if (PATH_IC("Console.WriteLine") || PATH_IC("Console.writeline")) {
                NlNode *a0 = (arglist && arglist->children.count > 0) ?
                             arglist->children.items[0] : NULL;
                NlNode *v0 = (a0 && a0->children.count > 0) ? a0->children.items[0] : NULL;
                fputs("puts(", cx->f); emit_as_string(cx, v0); fputc(')', cx->f);
            } else if (PATH_IC("Console.Write") || PATH_IC("Console.write")) {
                NlNode *a0 = (arglist && arglist->children.count > 0) ?
                             arglist->children.items[0] : NULL;
                NlNode *v0 = (a0 && a0->children.count > 0) ? a0->children.items[0] : NULL;
                fputs("fputs(", cx->f); emit_as_string(cx, v0);
                fputs(", stdout)", cx->f);
            } else if (PATH_IC("Nova.Math.Sqrt") || PATH_IC("math.sqrt")) {
                fputs("sqrt(", cx->f); emit_args(cx, arglist); fputc(')', cx->f);
            } else if (PATH_IC("CStr") || PATH_IC("cstr")) {
                emit_cstr_call(cx, arglist);
            } else if (PATH_IC("CDbl") || PATH_IC("cdbl")) {
                fputs("(double)(", cx->f); emit_args(cx, arglist); fputc(')', cx->f);
            } else if (PATH_IC("CInt") || PATH_IC("cint")) {
                fputs("(long long)(", cx->f); emit_args(cx, arglist); fputc(')', cx->f);
            } else if (PATH_IC("CLng") || PATH_IC("clng")) {
                fputs("(long long)(", cx->f); emit_args(cx, arglist); fputc(')', cx->f);
            } else if (PATH_IC("CSng") || PATH_IC("csng")) {
                fputs("(float)(", cx->f); emit_args(cx, arglist); fputc(')', cx->f);
            } else if (PATH_IC("CBool") || PATH_IC("cbool")) {
                fputs("(int)(", cx->f); emit_args(cx, arglist); fputc(')', cx->f);
            } else {
                /* Generic call: emit callee(args) */
                emit_expr(cx, callee);
                fputc('(', cx->f); emit_args(cx, arglist); fputc(')', cx->f);
            }
#undef PATH_IC
            break;
        }
        case ND_BINARY_EXPR: {
            NlNode *lhs = n->children.count > 0 ? n->children.items[0] : NULL;
            NlNode *rhs = n->children.count > 1 ? n->children.items[1] : NULL;
            NlTokenKind op = n->val.op;
            if (op == OP_AMP) {
                /* String concatenation */
                fputs("nl_cat(", cx->f);
                emit_as_string(cx, lhs);
                fputs(", ", cx->f);
                emit_as_string(cx, rhs);
                fputc(')', cx->f);
            } else {
                fputc('(', cx->f);
                emit_expr(cx, lhs);
                switch (op) {
                    case OP_PLUS:   fputs(" + ", cx->f); break;
                    case OP_MINUS:  fputs(" - ", cx->f); break;
                    case OP_MUL:    fputs(" * ", cx->f); break;
                    case OP_DIV:    fputs(" / ", cx->f); break;
                    case OP_IDIV:   fputs(" / ", cx->f); break;
                    case KW_MOD:    fputs(" % ", cx->f); break;
                    case OP_EQ:     fputs(" == ", cx->f); break;
                    case OP_NEQ:    fputs(" != ", cx->f); break;
                    case OP_LT:     fputs(" < ",  cx->f); break;
                    case OP_LE:     fputs(" <= ", cx->f); break;
                    case OP_GT:     fputs(" > ",  cx->f); break;
                    case OP_GE:     fputs(" >= ", cx->f); break;
                    case KW_AND: case KW_ANDALSO: fputs(" && ", cx->f); break;
                    case KW_OR:  case KW_ORELSE:  fputs(" || ", cx->f); break;
                    case KW_XOR:    fputs(" ^ ",  cx->f); break;
                    default:        fputs(" + ", cx->f); break;
                }
                emit_expr(cx, rhs);
                fputc(')', cx->f);
            }
            break;
        }
        case ND_UNARY_EXPR: {
            NlNode *operand = n->children.count > 0 ? n->children.items[0] : NULL;
            switch (n->val.op) {
                case KW_NOT:    fputs("!(", cx->f); emit_expr(cx, operand); fputc(')', cx->f); break;
                case OP_MINUS:  fputs("-(", cx->f); emit_expr(cx, operand); fputc(')', cx->f); break;
                default:        emit_expr(cx, operand); break;
            }
            break;
        }
        default:
            fputs("0/*unhandled*/", cx->f);
            break;
    }
}

/* -------------------------------------------------------------------------
 * Statement emission
 * ---------------------------------------------------------------------- */

static void emit_block_body(CgCtx *cx, NlNode *block) {
    if (!block) return;
    if (block->kind == ND_BLOCK) {
        for (uint32_t i = 0; i < block->children.count; i++)
            emit_stmt(cx, block->children.items[i]);
    } else {
        emit_stmt(cx, block);
    }
}

static void emit_stmt(CgCtx *cx, NlNode *n) {
    if (!n) return;
    switch (n->kind) {
        case ND_BLOCK:
            emit_block_body(cx, n);
            break;
        case ND_VAR_DECL: case ND_CONST_DECL: {
            cg_indent(cx);
            int ci = 0;
            NlNode *type_node = NULL;
            if (n->children.count > 0 &&
                (n->children.items[0]->kind == ND_TYPE_NAME ||
                 n->children.items[0]->kind == ND_NULLABLE_TYPE ||
                 n->children.items[0]->kind == ND_ARRAY_TYPE)) {
                type_node = n->children.items[0]; ci = 1;
            }
            if (type_node)  emit_c_type_node(cx, type_node);
            else             emit_default_c_type(cx, n->type_ref);
            fprintf(cx->f, " %s", cg_str(cx, n->val.str_id));
            /* initializer */
            if ((uint32_t)ci < n->children.count) {
                fputs(" = ", cx->f);
                emit_expr(cx, n->children.items[ci]);
            } else {
                fputs(" = 0", cx->f); /* zero-initialize */
            }
            fputs(";\n", cx->f);
            break;
        }
        case ND_ASSIGN_STMT: case ND_COMPOUND_ASSIGN: {
            cg_indent(cx);
            NlNode *lhs = n->children.count > 0 ? n->children.items[0] : NULL;
            NlNode *rhs = n->children.count > 1 ? n->children.items[1] : NULL;
            emit_expr(cx, lhs);
            if (n->kind == ND_COMPOUND_ASSIGN) {
                switch (n->val.op) {
                    case OP_PLUS_EQ:  fputs(" += ", cx->f); break;
                    case OP_MINUS_EQ: fputs(" -= ", cx->f); break;
                    case OP_MUL_EQ:   fputs(" *= ", cx->f); break;
                    case OP_DIV_EQ:   fputs(" /= ", cx->f); break;
                    case OP_AMP_EQ: /* &= means string concat-assign */
                        fputs(" = nl_cat(", cx->f);
                        emit_expr(cx, lhs);
                        fputs(", ", cx->f);
                        emit_expr(cx, rhs);
                        fputs(");\n", cx->f);
                        return;
                    default: fputs(" = ", cx->f); break;
                }
            } else {
                fputs(" = ", cx->f);
            }
            emit_expr(cx, rhs);
            fputs(";\n", cx->f);
            break;
        }
        case ND_CALL_STMT:
            cg_indent(cx);
            if (n->children.count > 0) emit_expr(cx, n->children.items[0]);
            fputs(";\n", cx->f);
            break;
        case ND_RETURN_STMT:
            cg_indent(cx);
            fputs("return", cx->f);
            if (n->children.count > 0) {
                fputc(' ', cx->f);
                emit_expr(cx, n->children.items[0]);
            }
            fputs(";\n", cx->f);
            break;
        case ND_IF_STMT: {
            /* children: [cond, then_block, (ND_ELSEIF_CLAUSE | ND_BLOCK)*] */
            NlNode *cond       = n->children.count > 0 ? n->children.items[0] : NULL;
            NlNode *then_block = n->children.count > 1 ? n->children.items[1] : NULL;
            cg_indent(cx);
            fputs("if (", cx->f); emit_expr(cx, cond); fputs(") {\n", cx->f);
            cx->indent++;
            emit_block_body(cx, then_block);
            cx->indent--;
            for (uint32_t i = 2; i < n->children.count; i++) {
                NlNode *c = n->children.items[i];
                if (c->kind == ND_ELSEIF_CLAUSE) {
                    NlNode *ei_cond  = c->children.count > 0 ? c->children.items[0] : NULL;
                    NlNode *ei_block = c->children.count > 1 ? c->children.items[1] : NULL;
                    cg_indent(cx);
                    fputs("} else if (", cx->f); emit_expr(cx, ei_cond); fputs(") {\n", cx->f);
                    cx->indent++;
                    emit_block_body(cx, ei_block);
                    cx->indent--;
                } else if (c->kind == ND_BLOCK) {
                    cg_indent(cx); fputs("} else {\n", cx->f);
                    cx->indent++;
                    emit_block_body(cx, c);
                    cx->indent--;
                }
            }
            cg_indent(cx); fputs("}\n", cx->f);
            break;
        }
        case ND_WHILE_STMT: {
            NlNode *cond  = n->children.count > 0 ? n->children.items[0] : NULL;
            NlNode *body  = n->children.count > 1 ? n->children.items[1] : NULL;
            cg_indent(cx);
            fputs("while (", cx->f); emit_expr(cx, cond); fputs(") {\n", cx->f);
            cx->indent++;
            emit_block_body(cx, body);
            cx->indent--;
            cg_indent(cx); fputs("}\n", cx->f);
            break;
        }
        case ND_FOR_STMT: {
            /* children: [var_ident, from, to, (step)?, body] */
            NlNode *var_node = n->children.count > 0 ? n->children.items[0] : NULL;
            NlNode *from     = n->children.count > 1 ? n->children.items[1] : NULL;
            NlNode *to       = n->children.count > 2 ? n->children.items[2] : NULL;
            NlNode *body     = n->children.items[n->children.count - 1];
            NlNode *step     = (n->children.count == 5) ? n->children.items[3] : NULL;

            const char *var_name = var_node ? cg_str(cx, var_node->val.str_id) : "_i";
            NlNode *type_node = (var_node && var_node->children.count > 0) ?
                                var_node->children.items[0] : NULL;

            cg_indent(cx); fputc('{', cx->f); fputc('\n', cx->f);
            cx->indent++;
            cg_indent(cx);
            if (type_node) emit_c_type_node(cx, type_node);
            else           fputs("long long", cx->f);
            fprintf(cx->f, " %s = ", var_name);
            emit_expr(cx, from);
            fputs(";\n", cx->f);

            cg_indent(cx);
            fprintf(cx->f, "for (; %s <= ", var_name);
            emit_expr(cx, to);
            fprintf(cx->f, "; %s += ", var_name);
            if (step) emit_expr(cx, step); else fputs("1", cx->f);
            fputs(") {\n", cx->f);
            cx->indent++;
            emit_block_body(cx, body);
            cx->indent--;
            cg_indent(cx); fputs("}\n", cx->f);
            cx->indent--;
            cg_indent(cx); fputs("}\n", cx->f);
            break;
        }
        case ND_SELECT_STMT: {
            /* children: [selector, case_clause*] */
            NlNode *sel = n->children.count > 0 ? n->children.items[0] : NULL;
            /* emit as if/else if chain to avoid C switch type restrictions */
            int first = 1;
            for (uint32_t i = 1; i < n->children.count; i++) {
                NlNode *cc = n->children.items[i];
                NlNode *body = cc->children.count > 0 ?
                               cc->children.items[cc->children.count - 1] : NULL;
                cg_indent(cx);
                if (cc->flags == 1) {
                    /* Case Else */
                    fputs(first ? "{\n" : "} else {\n", cx->f);
                } else {
                    fputs(first ? "if (" : "} else if (", cx->f);
                    /* emit: (sel == expr1 || sel == expr2 ...) */
                    uint32_t expr_count = cc->children.count - 1;
                    if (expr_count > 1) fputc('(', cx->f);
                    for (uint32_t j = 0; j < expr_count; j++) {
                        if (j > 0) fputs(" || ", cx->f);
                        fputc('(', cx->f);
                        emit_expr(cx, sel);
                        fputs(" == ", cx->f);
                        emit_expr(cx, cc->children.items[j]);
                        fputc(')', cx->f);
                    }
                    if (expr_count > 1) fputc(')', cx->f);
                    fputs(") {\n", cx->f);
                }
                first = 0;
                cx->indent++;
                emit_block_body(cx, body);
                cx->indent--;
            }
            if (!first) { cg_indent(cx); fputs("}\n", cx->f); }
            break;
        }
        case ND_EXIT_STMT:
            cg_indent(cx);
            switch (n->val.op) {
                case KW_FOR: case KW_WHILE: case KW_DO: fputs("break;\n", cx->f); break;
                case KW_FUNCTION: case KW_SUB: fputs("return;\n", cx->f); break;
                default: fputs("break;\n", cx->f); break;
            }
            break;
        case ND_CONTINUE_STMT:
            cg_indent(cx); fputs("continue;\n", cx->f);
            break;
        default:
            /* fall through for unhandled statements */
            break;
    }
}

/* -------------------------------------------------------------------------
 * Top-level declaration emission
 * ---------------------------------------------------------------------- */

static void emit_func_params(CgCtx *cx, NlNode *param_list) {
    if (!param_list) { fputs("void", cx->f); return; }
    int first = 1;
    for (uint32_t i = 0; i < param_list->children.count; i++) {
        NlNode *par = param_list->children.items[i];
        if (!first) fputs(", ", cx->f);
        first = 0;
        NlNode *type_node = par->children.count > 0 ? par->children.items[0] : NULL;
        if (type_node) emit_c_type_node(cx, type_node);
        else           fputs("long long", cx->f);
        fprintf(cx->f, " %s", cg_str(cx, par->val.str_id));
    }
    if (first) fputs("void", cx->f);
}

static void emit_decl(CgCtx *cx, NlNode *n);

static void emit_struct_decl(CgCtx *cx, NlNode *n) {
    fprintf(cx->f, "typedef struct {\n");
    cx->indent++;
    for (uint32_t i = 0; i < n->children.count; i++) {
        NlNode *c = n->children.items[i];
        if (c->kind == ND_VAR_DECL) {
            cg_indent(cx);
            NlNode *tn = c->children.count > 0 ? c->children.items[0] : NULL;
            if (tn) emit_c_type_node(cx, tn); else fputs("long long", cx->f);
            fprintf(cx->f, " %s;\n", cg_str(cx, c->val.str_id));
        }
    }
    cx->indent--;
    fprintf(cx->f, "} %s;\n\n", cg_str(cx, n->val.str_id));
}

static void emit_enum_decl(CgCtx *cx, NlNode *n) {
    fprintf(cx->f, "typedef enum {\n");
    for (uint32_t i = 0; i < n->children.count; i++) {
        NlNode *m = n->children.items[i];
        if (m->kind != ND_ENUM_MEMBER) continue;
        fprintf(cx->f, "    %s", cg_str(cx, m->val.str_id));
        if (m->children.count > 0) {
            fputs(" = ", cx->f); emit_expr(cx, m->children.items[0]);
        }
        fputs(",\n", cx->f);
    }
    fprintf(cx->f, "} %s;\n\n", cg_str(cx, n->val.str_id));
}

static void emit_func_decl(CgCtx *cx, NlNode *n, int is_sub) {
    /* find: param_list, return_type_node, body */
    NlNode *param_list = NULL, *ret_type = NULL, *body = NULL;
    for (uint32_t i = 0; i < n->children.count; i++) {
        NlNode *c = n->children.items[i];
        if (c->kind == ND_PARAM_LIST) { param_list = c; continue; }
        if (c->kind == ND_BLOCK)      { body = c; continue; }
        if (c->kind == ND_TYPE_NAME || c->kind == ND_NULLABLE_TYPE ||
            c->kind == ND_ARRAY_TYPE) { ret_type = c; continue; }
    }
    const char *name = cg_str(cx, n->val.str_id);
    int is_main = (strcasecmp(name, "main") == 0);

    if (is_main) {
        fputs("int main(int __argc, char** __argv) {\n", cx->f);
    } else if (is_sub) {
        fputs("void ", cx->f);
        fprintf(cx->f, "%s(", name);
        emit_func_params(cx, param_list);
        fputs(") {\n", cx->f);
    } else {
        if (ret_type) emit_c_type_node(cx, ret_type);
        else          fputs("long long", cx->f);
        fprintf(cx->f, " %s(", name);
        emit_func_params(cx, param_list);
        fputs(") {\n", cx->f);
    }
    cx->indent++;
    if (body) emit_block_body(cx, body);
    if (is_main) { cg_indent(cx); fputs("return 0;\n", cx->f); }
    cx->indent--;
    fputs("}\n\n", cx->f);
}

static void emit_decl(CgCtx *cx, NlNode *n) {
    if (!n) return;
    switch (n->kind) {
        case ND_COMPILATION_UNIT:
            for (uint32_t i = 0; i < n->children.count; i++)
                emit_decl(cx, n->children.items[i]);
            break;
        case ND_IMPORTS_STMT: /* ignore */
            break;
        case ND_MODULE_DECL: case ND_NAMESPACE_DECL:
            /* emit a forward-declaration pass first */
            for (uint32_t i = 0; i < n->children.count; i++) {
                NlNode *c = n->children.items[i];
                if (c->kind == ND_FUNCTION_DECL || c->kind == ND_SUB_DECL) {
                    /* forward declaration */
                    NlNode *pl = NULL, *rt = NULL;
                    for (uint32_t j = 0; j < c->children.count; j++) {
                        NlNode *ch = c->children.items[j];
                        if (ch->kind == ND_PARAM_LIST) pl = ch;
                        if (ch->kind == ND_TYPE_NAME || ch->kind == ND_NULLABLE_TYPE ||
                            ch->kind == ND_ARRAY_TYPE) rt = ch;
                    }
                    const char *nm = cg_str(cx, c->val.str_id);
                    if (c->kind == ND_SUB_DECL)      fputs("void ", cx->f);
                    else if (rt) { emit_c_type_node(cx, rt); fputc(' ', cx->f); }
                    else  fputs("long long ", cx->f);
                    fprintf(cx->f, "%s(", nm);
                    emit_func_params(cx, pl);
                    fputs(");\n", cx->f);
                }
            }
            fputc('\n', cx->f);
            /* then emit full definitions */
            for (uint32_t i = 0; i < n->children.count; i++)
                emit_decl(cx, n->children.items[i]);
            break;
        case ND_STRUCT_DECL:
            emit_struct_decl(cx, n);
            break;
        case ND_CLASS_DECL:
            /* minimal: treat as struct */
            emit_struct_decl(cx, n);
            break;
        case ND_ENUM_DECL:
            emit_enum_decl(cx, n);
            break;
        case ND_SUB_DECL:
            emit_func_decl(cx, n, 1);
            break;
        case ND_FUNCTION_DECL:
            emit_func_decl(cx, n, 0);
            break;
        case ND_VAR_DECL: case ND_CONST_DECL:
            /* module-level variable → static global */
            if (n->children.count > 0 &&
                (n->children.items[0]->kind == ND_TYPE_NAME ||
                 n->children.items[0]->kind == ND_NULLABLE_TYPE)) {
                fputs("static ", cx->f);
                emit_c_type_node(cx, n->children.items[0]);
            } else {
                fputs("static long long", cx->f);
            }
            fprintf(cx->f, " %s", cg_str(cx, n->val.str_id));
            if (n->children.count > 1) {
                fputs(" = ", cx->f);
                emit_expr(cx, n->children.items[1]);
            } else if (n->children.count == 1 &&
                       n->children.items[0]->kind != ND_TYPE_NAME) {
                fputs(" = ", cx->f);
                emit_expr(cx, n->children.items[0]);
            } else {
                fputs(" = 0", cx->f);
            }
            fputs(";\n\n", cx->f);
            break;
        default:
            break;
    }
}

/* -------------------------------------------------------------------------
 * Public entry point
 * ---------------------------------------------------------------------- */
int nl_codegen(NlNode *unit, NlLexer *lex, NlTypeChecker *tc,
               const NlCgOptions *opts) {
    FILE *f = fopen(opts->output_c, "w");
    if (!f) {
        fprintf(stderr, "codegen: cannot write %s\n", opts->output_c);
        return 1;
    }
    /* runtime comment header */
    fprintf(f, "/* Generated by NovaLang %s from %s */\n\n",
            NOVALANG_VERSION, opts->input_path ? opts->input_path : "?");
    fputs(nl_runtime, f);

    CgCtx cx;
    cx.f      = f;
    cx.lex    = lex;
    cx.tc     = tc;
    cx.indent = 0;

    emit_decl(&cx, unit);
    fclose(f);

    /* invoke GCC */
    char cmd[4096];
#ifdef _WIN32
    snprintf(cmd, sizeof(cmd),
             "gcc -O1 -std=c11 -o \"%s\" \"%s\" -lm 2>&1",
             opts->output_exe, opts->output_c);
#else
    snprintf(cmd, sizeof(cmd),
             "gcc -O1 -std=c11 -o '%s' '%s' -lm 2>&1",
             opts->output_exe, opts->output_c);
#endif
    int rc = system(cmd);
    remove(opts->output_c);
    return rc;
}

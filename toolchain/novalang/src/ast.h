#ifndef NL_AST_H
#define NL_AST_H

#include "lexer.h"
#include <stdint.h>

/* -------------------------------------------------------------------------
 * Forward declarations
 * ---------------------------------------------------------------------- */
typedef struct NlNode     NlNode;
typedef struct NlNodeList NlNodeList;
typedef uint32_t          NlTypeRef;  /* index into type table; 0 = unresolved */

/* -------------------------------------------------------------------------
 * Node kinds
 * ---------------------------------------------------------------------- */
typedef enum NlNodeKind {

    /* Compilation unit */
    ND_COMPILATION_UNIT,    /* children: imports[], decls[] */
    ND_IMPORTS_STMT,        /* name: dotted name */
    ND_OPTION_STMT,         /* val: option text */

    /* Declarations */
    ND_NAMESPACE_DECL,
    ND_MODULE_DECL,
    ND_CLASS_DECL,
    ND_STRUCT_DECL,
    ND_INTERFACE_DECL,
    ND_ENUM_DECL,
    ND_ENUM_MEMBER,
    ND_DELEGATE_DECL,
    ND_FUNCTION_DECL,       /* val.str = name; children[0] = param_list, [1] = return_type, [2] = body */
    ND_SUB_DECL,            /* val.str = name; children[0] = param_list, [1] = body */
    ND_PROPERTY_DECL,
    ND_EVENT_DECL,
    ND_VAR_DECL,            /* Dim / Static */
    ND_CONST_DECL,          /* Const */
    ND_PARAM,               /* parameter in sub/function */
    ND_PARAM_LIST,

    /* Statements */
    ND_BLOCK,               /* list of statements */
    ND_ASSIGN_STMT,         /* lhs = rhs */
    ND_COMPOUND_ASSIGN,     /* lhs op= rhs */
    ND_CALL_STMT,           /* call expression used as statement */
    ND_RETURN_STMT,
    ND_IF_STMT,             /* cond, then-block, elseif[]*, else-block? */
    ND_ELSEIF_CLAUSE,
    ND_SELECT_STMT,
    ND_CASE_CLAUSE,
    ND_FOR_STMT,            /* var, from, to, step, body */
    ND_FOR_EACH_STMT,       /* var, collection, body */
    ND_WHILE_STMT,
    ND_DO_STMT,             /* condition, body, is_until */
    ND_EXIT_STMT,           /* val.int = KW_FOR / KW_WHILE / KW_DO / KW_FUNCTION / KW_SUB */
    ND_CONTINUE_STMT,
    ND_THROW_STMT,
    ND_TRY_STMT,            /* body, catch[]*, finally? */
    ND_CATCH_CLAUSE,
    ND_FINALLY_CLAUSE,
    ND_USING_STMT,
    ND_RAISEEVENT_STMT,
    ND_ADDHANDLER_STMT,
    ND_REMOVEHANDLER_STMT,
    ND_WITH_STMT,

    /* Expressions */
    ND_INT_LIT,
    ND_FLOAT_LIT,
    ND_STRING_LIT,
    ND_CHAR_LIT,
    ND_BOOL_LIT,
    ND_NOTHING,
    ND_IDENT,               /* simple name */
    ND_MEMBER_ACCESS,       /* obj.member */
    ND_CALL_EXPR,           /* callee, arg_list */
    ND_INDEX_EXPR,          /* array(index) */
    ND_NEW_EXPR,            /* New T(...) */
    ND_CAST_EXPR,           /* CType(expr, T) / DirectCast */
    ND_TYPEOF_EXPR,         /* TypeOf x Is T */
    ND_BINARY_EXPR,         /* op, left, right */
    ND_UNARY_EXPR,          /* op, operand */
    ND_CONDITIONAL_EXPR,    /* If(cond, then, else) */
    ND_LAMBDA_EXPR,
    ND_AWAIT_EXPR,
    ND_ARG_LIST,
    ND_ARG,                 /* possibly named: name := value */

    /* Type references in AST */
    ND_TYPE_NAME,           /* simple or qualified type name */
    ND_GENERIC_TYPE,        /* T(Of A, B) */
    ND_NULLABLE_TYPE,       /* T? */
    ND_ARRAY_TYPE,          /* T() */

    ND_KIND_COUNT
} NlNodeKind;

/* -------------------------------------------------------------------------
 * Access modifier flags
 * ---------------------------------------------------------------------- */
#define NL_ACC_PUBLIC      0x01
#define NL_ACC_PRIVATE     0x02
#define NL_ACC_PROTECTED   0x04
#define NL_ACC_FRIEND      0x08
#define NL_ACC_SHARED      0x10
#define NL_ACC_READONLY    0x20
#define NL_ACC_OVERRIDABLE 0x40
#define NL_ACC_MUSTOVERRIDE 0x80
#define NL_ACC_OVERRIDES   0x100
#define NL_ACC_ASYNC       0x200
#define NL_ACC_STATIC      0x400

/* -------------------------------------------------------------------------
 * Node list (dynamic array of NlNode*)
 * ---------------------------------------------------------------------- */
struct NlNodeList {
    NlNode  **items;
    uint32_t  count;
    uint32_t  cap;
};

/* -------------------------------------------------------------------------
 * AST Node
 * ---------------------------------------------------------------------- */
struct NlNode {
    NlNodeKind kind;
    NlRange    range;
    NlTypeRef  type_ref;   /* filled by type-checker */
    uint32_t   flags;      /* NL_ACC_* */

    /* children */
    NlNodeList children;

    /* value payload */
    union {
        int64_t  int_val;
        double   float_val;
        uint32_t str_id;    /* interned string index */
        NlTokenKind op;     /* for binary/unary nodes */
    } val;
};

/* -------------------------------------------------------------------------
 * Arena allocator for AST nodes
 * ---------------------------------------------------------------------- */
typedef struct NlArena {
    char    **blocks;
    uint32_t  block_count;
    uint32_t  block_cap;
    uint32_t  block_size;
    uint32_t  used;        /* bytes used in current block */
} NlArena;

void     nl_arena_init(NlArena *a);
void    *nl_arena_alloc(NlArena *a, uint32_t size);
void     nl_arena_free(NlArena *a);

NlNode  *nl_node_new(NlArena *a, NlNodeKind kind, NlRange range);
int      nl_node_push(NlArena *a, NlNodeList *list, NlNode *child);

#endif /* NL_AST_H */

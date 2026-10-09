#ifndef NL_TYPECHECKER_H
#define NL_TYPECHECKER_H

#include "ast.h"
#include "diagnostic.h"
#include <stdint.h>

/* -------------------------------------------------------------------------
 * Built-in type IDs (fixed indices into the type table)
 * ---------------------------------------------------------------------- */
#define NL_TY_VOID       0u
#define NL_TY_BOOL       1u
#define NL_TY_BYTE       2u
#define NL_TY_SBYTE      3u
#define NL_TY_SHORT      4u
#define NL_TY_USHORT     5u
#define NL_TY_INTEGER    6u
#define NL_TY_UINTEGER   7u
#define NL_TY_LONG       8u
#define NL_TY_ULONG      9u
#define NL_TY_SINGLE    10u
#define NL_TY_DOUBLE    11u
#define NL_TY_DECIMAL   12u
#define NL_TY_CHAR      13u
#define NL_TY_STRING    14u
#define NL_TY_OBJECT    15u
#define NL_TY_BUILTIN_COUNT 16u

typedef enum NlTypeKind {
    TY_PRIMITIVE,
    TY_STRING,
    TY_CLASS,
    TY_STRUCT,
    TY_INTERFACE,
    TY_ENUM,
    TY_DELEGATE,
    TY_ARRAY,
    TY_NULLABLE,
    TY_GENERIC,
    TY_UNRESOLVED
} NlTypeKind;

typedef struct NlTypeDesc {
    NlTypeRef  id;
    NlTypeKind kind;
    uint32_t   name_id;
    NlTypeRef  base_type;   /* 0 = none */
    NlTypeRef  elem_type;   /* for array / nullable */
    uint8_t    is_value_type;
    uint8_t    is_nullable;
} NlTypeDesc;

typedef struct NlSymbol {
    uint32_t    name_id;
    NlTypeRef   type;
    NlNode     *decl_node;
    struct NlSymbol *next;
} NlSymbol;

typedef struct NlScope {
    NlSymbol       *symbols;
    struct NlScope *parent;
} NlScope;

typedef struct NlTypeTable {
    NlTypeDesc *types;
    uint32_t    count;
    uint32_t    cap;
} NlTypeTable;

typedef struct NlTypeChecker {
    NlTypeTable  *table;
    NlScope      *scope;
    NlArena      *arena;
    NlDiagList   *diags;
    char        **strings;
    uint32_t      string_count;
    NlTypeRef     current_return_type;
} NlTypeChecker;

void      nl_tc_init(NlTypeChecker *tc, NlArena *a, NlDiagList *diags,
                     char **strings, uint32_t string_count);
void      nl_tc_check(NlTypeChecker *tc, NlNode *unit);
NlTypeRef nl_tc_resolve_type_node(NlTypeChecker *tc, NlNode *type_node);
NlTypeRef nl_tc_check_expr(NlTypeChecker *tc, NlNode *expr);

/* helpers */
NlTypeRef nl_type_register(NlTypeTable *t, NlArena *a, NlTypeKind kind,
                            uint32_t name_id, uint8_t is_value);
int       nl_type_is_numeric(NlTypeRef t);
int       nl_type_is_assignable(NlTypeRef dest, NlTypeRef src);

#endif /* NL_TYPECHECKER_H */

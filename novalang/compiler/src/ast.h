#ifndef NOVAC_AST_H
#define NOVAC_AST_H

#include <stdint.h>

/* AST fuer NovaLang bis v0.3 - siehe specifications/NOVALANG_SPEC_v0.3.md
   fuer die aktuelle Grammatik. Gewachsen aus der bewusst minimalen v0.1-
   Fassung (nur STMT_WRITELINE/STMT_RETURN); mit echten Laufzeit-
   Integer-Variablen, Zuweisungen und If/Else lohnt sich jetzt eine echte
   Knotenhierarchie statt der fruehren Zwei-Varianten-Struct. */

typedef enum NcStmtKind {
    STMT_WRITELINE,
    STMT_RETURN,
    STMT_ASSIGN_INT, /* "x = IntExpr" (nur fuer bereits per Dim deklarierte Integer-Variablen) */
    STMT_IF,
    STMT_ASSIGN_SYSCALL /* "x = SysCall(service, operation, arg1, arg2)", siehe NOVALANG_SPEC_v0.4.md */
} NcStmtKind;

/* Ein Operand von SysCall(...) - absichtlich nur Literal/Variable (kein
   verschachtelter Ausdruck, siehe NOVALANG_SPEC_v0.4.md Abschnitt 3 fuer
   die Begruendung der Registerkonflikt-Vermeidung). */
typedef struct NcSysArg {
    int      is_var;
    int64_t  literal;
    uint32_t var_index;
} NcSysArg;

/* Ein einzelner Term in einer Int-Expression ("+"/"-"-verkettete Folge von
   Literalen/Variablen, siehe NOVALANG_SPEC_v0.3.md). op ist '+' oder '-'
   fuer alle Terme AUSSER dem ersten (dessen op ignoriert wird). */
typedef struct NcIntTerm {
    char       op;         /* '+' oder '-', ignoriert beim ersten Term */
    int        is_var;     /* 0 = Literal (in `literal`), 1 = Variable (in `var_index`) */
    int64_t    literal;
    uint32_t   var_index;
    struct NcIntTerm *next;
} NcIntTerm;

/* Vergleichsoperatoren fuer "If lhs <op> rhs Then" (siehe Parser). */
typedef enum NcCmpOp {
    CMP_EQ, CMP_NE, CMP_LT, CMP_GT, CMP_LE, CMP_GE
} NcCmpOp;

typedef struct NcStmt {
    NcStmtKind kind;

    /* STMT_WRITELINE */
    char      *string_arg; /* der Literaltext (ohne \r\n) */
    uint32_t   string_len;

    /* STMT_RETURN */
    int64_t    int_arg;    /* Rueckgabewert - siehe Spec: wird NICHT propagiert */

    /* STMT_ASSIGN_INT */
    uint32_t   assign_var_index;
    NcIntTerm *assign_expr;

    /* STMT_ASSIGN_SYSCALL - teilt sich assign_var_index mit STMT_ASSIGN_INT */
    NcSysArg   syscall_args[4]; /* service, operation, arg1, arg2 */

    /* STMT_IF */
    uint32_t   if_lhs_var_index;
    NcCmpOp    if_cmp_op;
    int        if_rhs_is_var;
    int64_t    if_rhs_literal;
    uint32_t   if_rhs_var_index;
    struct NcStmt *if_then_body;
    struct NcStmt *if_else_body; /* NULL = kein Else */

    struct NcStmt *next;
} NcStmt;

typedef struct NcFunction {
    char   *name;
    NcStmt *body; /* verkettete Liste */
} NcFunction;

typedef struct NcModule {
    char       *name;
    NcFunction *main_func; /* v0.1-v0.3: genau eine Function "Main" */

    /* Laufzeit-Integer-Variablen (siehe NOVALANG_SPEC_v0.3.md): jede
       "Dim x As Integer = N" bekommt einen festen 4-Byte-Slot in der
       DATA-Section, Reihenfolge = Deklarationsreihenfolge. Anders als
       String-"Dim" (v0.2, rein compile-time) braucht das ECHTEN
       Laufzeit-Speicher, daher hier auf Modulebene statt nur in der
       Parser-eigenen Symboltabelle. */
    char      *int_var_names[64];
    int64_t    int_var_initial[64];
    uint32_t   int_var_count;
} NcModule;

#endif

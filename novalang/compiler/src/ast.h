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
    STMT_ASSIGN_SYSCALL, /* "x = SysCall(service, operation, arg1, arg2)", siehe NOVALANG_SPEC_v0.4.md */
    STMT_POKE /* "PokeInt(buffer, byteOffset, value)", siehe NOVALANG_SPEC_v0.5.md */
} NcStmtKind;

/* Ein Operand von SysCall(...)/PokeInt(...) - absichtlich nur Literal/
   Variable/"AddressOf(Puffer)" (letzteres wird VOM PARSER sofort zu
   einem Literal aufgeloest, siehe parser.c - Pufferadressen stehen zur
   Compile-Zeit fest, es gibt keine Laufzeit-Zeigerarithmetik). Kein
   verschachtelter Ausdruck - siehe NOVALANG_SPEC_v0.4.md Abschnitt 3
   fuer die Begruendung der Registerkonflikt-Vermeidung bei SysCall. */
typedef struct NcSysArg {
    int      is_var;
    int      is_peek;   /* 1 = PeekInt(...), hat Vorrang vor is_var/literal */
    uint32_t peek_addr;
    int64_t  literal;
    uint32_t var_index;
} NcSysArg;

/* Ein einzelner Term in einer Int-Expression ("+"/"-"-verkettete Folge von
   Literalen/Variablen, siehe NOVALANG_SPEC_v0.3.md). op ist '+' oder '-'
   fuer alle Terme AUSSER dem ersten (dessen op ignoriert wird).
   is_peek (v0.5): "PeekInt(buffer, byteOffset)" - liest 4 Byte aus einer
   zur Compile-Zeit bekannten absoluten Adresse (peek_addr), genau wie
   is_var aus einer Variablenadresse liest, nur mit direkt eingebetteter
   Adresse statt Var-Index. */
typedef struct NcIntTerm {
    char       op;         /* '+' oder '-', ignoriert beim ersten Term */
    int        is_var;     /* 0 = Literal (in `literal`), 1 = Variable (in `var_index`) */
    int        is_peek;    /* 1 = PeekInt(...) - hat Vorrang vor is_var/literal */
    uint32_t   peek_addr;
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

    /* STMT_POKE: "PokeInt(buffer, byteOffset, value)" - poke_addr ist
       bereits die fertige absolute Adresse (Pufferadresse + Offset,
       vom Parser berechnet). */
    uint32_t   poke_addr;
    NcSysArg   poke_value;

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

    /* Byte-Puffer fuer Syscall-Argument-Structs (v0.5, siehe
       NOVALANG_SPEC_v0.5.md): "Dim buf As Buffer(N)" reserviert N
       zero-initialisierte Bytes in einer EIGENEN DATA-Unterregion
       (siehe codegen_x86.h NC_DATA_BUFFER_REGION_OFFSET) - getrennt von
       den Integer-Variablen-Slots, damit die Reihenfolge von Integer-
       und Buffer-"Dim"-Anweisungen keine Adressen verschiebt.
       buf_byte_offset[i] ist relativ zum Beginn der Pufferregion. */
    char      *buf_names[16];
    uint32_t   buf_byte_offset[16];
    uint32_t   buf_size[16];
    uint32_t   buf_count;

    /* "PokeString(buf, byteOffset, \"literal\")" (v0.5, siehe
       NOVALANG_SPEC_v0.5.md) - schreibt compile-time-bekannte String-
       Bytes (z.B. einen VFS-Pfad) in einen Puffer. Rein eine Daten-
       Initialisierung (wie der Dim-Integer-Initialwert), KEIN Laufzeit-
       Statement - deshalb hier auf Modulebene statt als NcStmt, wird
       direkt beim Parsen aufgeloest und am Ende des Codegens auf die
       bereits mit Nullen gefuellte Pufferregion angewendet. */
    char      *patch_bytes[16];
    uint32_t   patch_addr[16];
    uint32_t   patch_len[16];
    uint32_t   patch_count;
} NcModule;

#endif

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
    STMT_POKE, /* "PokeInt(buffer, byteOffset, value)" bzw. "PokeByte(...)", siehe NOVALANG_SPEC_v0.5.md/v0.7.md */
    STMT_WHILE, /* "While lhs <op> rhs ... End While", siehe NOVALANG_SPEC_v0.6.md - nutzt dieselben if_*-Felder wie STMT_IF (siehe unten) */
    STMT_WRITEBUFFER, /* "Console.WriteBuffer(buffer, lengthExpr)", siehe NOVALANG_SPEC_v0.7.md - Laufzeit-Textausgabe mit Laufzeit-Laenge, KEIN automatisches "\r\n" (anders als WriteLine) */
    STMT_ASSIGN_CALL, /* "x = FuncName(arg1, arg2, ...)" - Aufruf einer anderen Function, siehe NOVALANG_SPEC_v0.9.md */
    STMT_FUNC_RETURN /* "Return IntExpr" INNERHALB einer Nicht-Main-Function - echtes call/ret, siehe NOVALANG_SPEC_v0.9.md. ANDERS als STMT_RETURN (das bleibt Mains Spezialfall: literal-only, YIELD-Endlosschleife) */
} NcStmtKind;

/* Ein Operand von SysCall(...)/PokeInt(...) - absichtlich nur Literal/
   Variable/"AddressOf(Puffer)" (letzteres wird VOM PARSER sofort zu
   einem Literal aufgeloest, siehe parser.c - Pufferadressen stehen zur
   Compile-Zeit fest, es gibt keine Laufzeit-Zeigerarithmetik). Kein
   verschachtelter Ausdruck - siehe NOVALANG_SPEC_v0.4.md Abschnitt 3
   fuer die Begruendung der Registerkonflikt-Vermeidung bei SysCall. */
typedef struct NcSysArg {
    int      is_var;
    int      is_peek;   /* 1 = PeekInt(...)/PeekByte(...), hat Vorrang vor is_var/literal */
    uint32_t peek_addr; /* Literal-Offset (v0.5/v0.7): fertige Adresse. Variable-Offset (v0.15): nur die Puffer-BASISADRESSE, siehe peek_offset_is_var. */
    int      peek_width; /* 1 = PeekByte, 4 = PeekInt (nur gueltig wenn is_peek) */
    int      peek_offset_is_var; /* v0.15, siehe NOVALANG_SPEC_v0.15.md - analog zu poke_offset_is_var */
    uint32_t peek_offset_var_index;
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
    int        is_peek;    /* 1 = PeekInt(...)/PeekByte(...) - hat Vorrang vor is_var/literal */
    uint32_t   peek_addr;  /* Literal-Offset: fertige Adresse. Variable-Offset (v0.15): nur die Basisadresse. */
    int        peek_width; /* 1 = PeekByte, 4 = PeekInt (nur gueltig wenn is_peek) */
    int        peek_offset_is_var; /* v0.15, siehe NOVALANG_SPEC_v0.15.md */
    uint32_t   peek_offset_var_index;
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

    /* STMT_POKE: "PokeInt(buffer, byteOffset, value)" bzw.
       "PokeByte(...)" - poke_width: 1 = PokeByte (schreibt nur das
       niedrigste Byte des Werts), 4 = PokeInt.
       poke_offset_is_var==0 (Literal-Offset, v0.5/v0.7): poke_addr ist
       bereits die fertige absolute Adresse (Pufferadresse + Offset,
       vom Parser zur Compile-Zeit berechnet und bereits bereichs-
       geprueft).
       poke_offset_is_var==1 (Variable-Offset, v0.12, siehe
       NOVALANG_SPEC_v0.12.md): poke_addr ist NUR die Puffer-BASISADRESSE
       (ohne Offset) - der tatsaechliche Offset steckt zur Laufzeit in
       der Integer-Variable poke_offset_var_index (z.B. eine mitgefuehrte
       Zeilenlaenge) und wird erst im Codegen per Register-indirekter
       Adressierung ("mov [ecx+disp32], al/eax") addiert. KEINE
       Bereichspruefung moeglich (Offset erst zur Laufzeit bekannt) -
       bewusste, dokumentierte Grenze, wie bei SysCall-Argumenten auch. */
    uint32_t   poke_addr;
    int        poke_width;
    int        poke_offset_is_var;
    uint32_t   poke_offset_var_index;
    NcSysArg   poke_value;

    /* STMT_WRITEBUFFER: "Console.WriteBuffer(buffer, lengthExpr)"
       (v0.7, siehe NOVALANG_SPEC_v0.7.md) - writebuf_addr ist bereits
       die fertige absolute Pufferadresse, writebuf_len die Laenge
       (haeufig eine Variable - die eigentliche Pointe: die Laenge ist
       eine LAUFZEIT-Groesse, nicht wie bei WriteLine zur Compile-Zeit
       aus dem Literal bekannt). */
    uint32_t   writebuf_addr;
    NcSysArg   writebuf_len;

    /* STMT_ASSIGN_CALL (v0.9, siehe NOVALANG_SPEC_v0.9.md): "x =
       FuncName(arg1, ...)" - teilt sich assign_var_index mit
       STMT_ASSIGN_INT/_SYSCALL fuer den Rueckgabewert (EAX nach dem
       `call`). call_func_index indiziert mod->functions[]. */
    uint32_t   call_func_index;
    NcSysArg   call_args[8];
    uint32_t   call_arg_count;

    /* STMT_FUNC_RETURN (v0.9): echter Funktionsrueckgabewert, beliebiger
       IntExpr (nicht nur ein Literal wie bei Mains STMT_RETURN). */
    NcIntTerm *return_expr;

    /* STMT_IF UND STMT_WHILE (v0.6, siehe NOVALANG_SPEC_v0.6.md) - beide
       teilen sich diese Felder, da ihre Bedingung syntaktisch und
       codegen-seitig identisch ist (nur die Sprunglogik um den Koerper
       herum unterscheidet sich: If ueberspringt einmal, While wiederholt).
       if_then_body ist bei STMT_WHILE der Schleifenkoerper,
       if_else_body bleibt dort immer NULL (While kennt kein Else). */
    uint32_t   if_lhs_var_index;
    NcCmpOp    if_cmp_op;
    int        if_rhs_is_var;
    int64_t    if_rhs_literal;
    uint32_t   if_rhs_var_index;
    struct NcStmt *if_then_body;
    struct NcStmt *if_else_body; /* NULL = kein Else */

    struct NcStmt *next;
} NcStmt;

/* NcFunction (v0.9, siehe NOVALANG_SPEC_v0.9.md): Parameter sind intern
   GANZ NORMALE Integer-Variablen (siehe NcModule::int_var_*), nur mit
   mangeltem Namen ("<FuncName>.<ParamName>", kollisionsfrei ueber
   Funktionen hinweg) VOR dem Parsen des Funktionskoerpers registriert -
   dadurch funktioniert jede bestehende Variable-Referenz (Zuweisung,
   If-Bedingung, SysCall-Argument, ...) auf einen Parameter OHNE
   zusaetzlichen Code, es ist einfach ein Int-Var wie jeder andere.
   KEINE echte Stackframe-/Rekursionsunterstuetzung: ein rekursiver oder
   wiedereintretender Aufruf wuerde sich seine eigenen Parameter-Slots
   ueberschreiben - bewusste, dokumentierte Grenze dieser Version. */
typedef struct NcFunction {
    char      *name;
    NcStmt    *body; /* verkettete Liste */
    uint32_t   param_var_index[8]; /* Indizes in mod->int_var_*, in Aufrufreihenfolge */
    uint32_t   param_count;
    int        is_main; /* 1 fuer die Entry-Point-Function (immer die ERSTE im Modul) */
} NcFunction;

typedef struct NcModule {
    char       *name;
    NcFunction *functions[16]; /* v0.9: mehrere Functions, functions[0] MUSS "Main" sein */
    uint32_t    function_count;

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

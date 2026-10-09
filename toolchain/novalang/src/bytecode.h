#ifndef NL_BYTECODE_H
#define NL_BYTECODE_H

#include <stdint.h>

/* -------------------------------------------------------------------------
 * NovaLang Bytecode (NLB) – stack-based VM format
 * ---------------------------------------------------------------------- */

/* File magic: "NLB\x01" */
#define NLB_MAGIC   0x014C4E4Eu  /* "NLB\x01" little-endian */
#define NLB_VERSION 1

typedef enum NlBcOp {
    /* Stack manipulation */
    BC_NOP,
    BC_POP,
    BC_DUP,
    BC_SWAP,

    /* Constants */
    BC_LDINT,       /* push int64 constant (8 bytes follow) */
    BC_LDFLT,       /* push double constant (8 bytes follow) */
    BC_LDSTR,       /* push string constant (4-byte index follow) */
    BC_LDTRUE,
    BC_LDFALSE,
    BC_LDNULL,

    /* Locals */
    BC_LDLOC,       /* push local[idx] (2 bytes) */
    BC_STLOC,       /* pop to local[idx] */
    BC_LDARG,       /* push arg[idx] */
    BC_STARG,

    /* Fields / arrays */
    BC_LDFLD,       /* pop obj, push field[id] */
    BC_STFLD,       /* pop value + obj, store field[id] */
    BC_LDELEM,      /* pop index + array, push element */
    BC_STELEM,
    BC_NEWARRAY,    /* pop length, push new array of type[id] */

    /* Arithmetic */
    BC_ADD, BC_SUB, BC_MUL, BC_DIV, BC_IDIV, BC_MOD, BC_POW, BC_NEG,
    BC_AND, BC_OR,  BC_XOR, BC_NOT, BC_SHL, BC_SHR,
    BC_CONCAT,

    /* Comparison */
    BC_CEQ, BC_CNE, BC_CLT, BC_CLE, BC_CGT, BC_CGE,

    /* Control flow */
    BC_JMP,         /* 4-byte absolute offset */
    BC_JMPTRUE,
    BC_JMPFALSE,

    /* Calls */
    BC_CALL,        /* 4-byte method index, 2-byte argc */
    BC_CALLVIRT,
    BC_RET,
    BC_RETVOID,

    /* Object */
    BC_NEWOBJ,      /* 4-byte type_id, 2-byte argc */
    BC_CASTOBJ,
    BC_ISINST,

    BC_OP_COUNT
} NlBcOp;

/* NLB file header */
typedef struct NlbHeader {
    uint32_t magic;
    uint16_t version;
    uint16_t flags;
    uint32_t entry_method;  /* index of main entry point */
    uint32_t method_count;
    uint32_t string_count;
    uint32_t type_count;
} NlbHeader;

/* NLB method record */
typedef struct NlbMethod {
    uint32_t name_str;      /* index into string pool */
    uint32_t code_offset;   /* offset into code section */
    uint32_t code_len;
    uint16_t local_count;
    uint16_t param_count;
    uint32_t return_type;   /* type index */
} NlbMethod;

#endif /* NL_BYTECODE_H */

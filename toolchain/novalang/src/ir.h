#ifndef NL_IR_H
#define NL_IR_H

#include "ast.h"
#include <stdint.h>

/* -------------------------------------------------------------------------
 * Nova Intermediate Representation (Nova IR)
 *
 * A simple 3-address, SSA-style IR. Each function is a list of basic
 * blocks; each block is a list of instructions.
 * ---------------------------------------------------------------------- */

typedef uint32_t NlIrVal;   /* value id; 0 = void/undefined */
typedef uint32_t NlIrBlock; /* block id */

typedef enum NlIrOpcode {
    /* Arithmetic */
    IR_ADD, IR_SUB, IR_MUL, IR_DIV, IR_IDIV, IR_MOD, IR_POW, IR_NEG,
    /* Bitwise */
    IR_AND, IR_OR, IR_XOR, IR_NOT, IR_SHL, IR_SHR,
    /* Comparison → boolean */
    IR_EQ, IR_NEQ, IR_LT, IR_LE, IR_GT, IR_GE,
    /* String */
    IR_CONCAT,
    /* Memory */
    IR_LOAD,        /* dst = *ptr */
    IR_STORE,       /* *ptr = src */
    IR_ALLOCA,      /* dst = alloca(type) */
    /* Calls */
    IR_CALL,        /* dst = call func(args...) */
    IR_CALL_VOID,   /* call func(args...) */
    IR_RET,         /* return val */
    IR_RET_VOID,
    /* Control flow */
    IR_JMP,         /* unconditional jump to block */
    IR_JMPIF,       /* jump to block_true if cond, else block_false */
    /* Values */
    IR_CONST_INT,   /* dst = int_val */
    IR_CONST_FLOAT, /* dst = float_val */
    IR_CONST_STR,   /* dst = string_id */
    IR_CONST_BOOL,
    IR_CONST_NULL,
    IR_PHI,         /* dst = phi(v1 from b1, v2 from b2, ...) */
    /* Object */
    IR_FIELD_LOAD,  /* dst = obj.field_id */
    IR_FIELD_STORE, /* obj.field_id = src */
    IR_NEW,         /* dst = new TypeRef(args...) */
    IR_CAST,        /* dst = cast<type> src */
    IR_TYPEOF,      /* dst = typeof(src) is type */

    IR_OPCODE_COUNT
} NlIrOpcode;

typedef struct NlIrInstr {
    NlIrOpcode opcode;
    NlIrVal    dst;
    NlIrVal    src[3];   /* operands */
    union {
        int64_t  int_val;
        double   float_val;
        uint32_t str_id;
        uint32_t field_id;
        NlIrBlock target_block;
        NlTypeRef type_ref;
    } imm;
    uint32_t     arg_count;   /* for CALL */
    NlIrVal     *args;        /* for CALL (arena-allocated) */
    struct NlIrInstr *next;
} NlIrInstr;

typedef struct NlIrBasicBlock {
    NlIrBlock         id;
    NlIrInstr        *head;
    NlIrInstr        *tail;
    uint32_t          instr_count;
    struct NlIrBasicBlock *next;
} NlIrBasicBlock;

typedef struct NlIrParam {
    NlIrVal   val;
    NlTypeRef type;
    uint32_t  name_id;
} NlIrParam;

typedef struct NlIrFunction {
    uint32_t          name_id;
    NlTypeRef         return_type;
    NlIrParam        *params;
    uint32_t          param_count;
    NlIrBasicBlock   *entry;
    NlIrBasicBlock   *block_list;
    uint32_t          block_count;
    uint32_t          next_val;     /* SSA value counter */
    struct NlIrFunction *next;
} NlIrFunction;

typedef struct NlIrModule {
    NlIrFunction *functions;
    uint32_t      function_count;
    NlArena      *arena;
} NlIrModule;

/* -------------------------------------------------------------------------
 * Builder API
 * ---------------------------------------------------------------------- */
void          nl_ir_module_init(NlIrModule *m, NlArena *a);
NlIrFunction *nl_ir_add_function(NlIrModule *m, uint32_t name_id, NlTypeRef ret);
NlIrBasicBlock *nl_ir_add_block(NlIrFunction *f, NlArena *a);
NlIrInstr    *nl_ir_emit(NlIrFunction *f, NlIrBasicBlock *b, NlArena *a, NlIrOpcode op);
NlIrVal       nl_ir_next_val(NlIrFunction *f);
void          nl_ir_print(const NlIrModule *m);

#endif /* NL_IR_H */

#include "bytecode.h"
#include "ir.h"
#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* -------------------------------------------------------------------------
 * Dynamic byte buffer
 * ---------------------------------------------------------------------- */

typedef struct ByteBuf {
    uint8_t *data;
    uint32_t len;
    uint32_t cap;
} ByteBuf;

static void bb_init(ByteBuf *b) { memset(b, 0, sizeof(*b)); }

static void bb_push_u8(ByteBuf *b, uint8_t v) {
    if (b->len >= b->cap) {
        b->cap = b->cap ? b->cap * 2 : 256;
        b->data = realloc(b->data, b->cap);
    }
    b->data[b->len++] = v;
}

static void bb_push_u16(ByteBuf *b, uint16_t v) {
    bb_push_u8(b, (uint8_t)(v & 0xFF));
    bb_push_u8(b, (uint8_t)(v >> 8));
}

static void bb_push_u32(ByteBuf *b, uint32_t v) {
    bb_push_u16(b, (uint16_t)(v & 0xFFFF));
    bb_push_u16(b, (uint16_t)(v >> 16));
}

static void bb_push_u64(ByteBuf *b, uint64_t v) {
    bb_push_u32(b, (uint32_t)(v & 0xFFFFFFFFu));
    bb_push_u32(b, (uint32_t)(v >> 32));
}

static void bb_free(ByteBuf *b) { free(b->data); memset(b, 0, sizeof(*b)); }

/* -------------------------------------------------------------------------
 * IR → bytecode translation
 * ---------------------------------------------------------------------- */

static void emit_op(ByteBuf *code, NlBcOp op) {
    bb_push_u8(code, (uint8_t)op);
}

static void translate_instr(ByteBuf *code, const NlIrInstr *ins) {
    switch (ins->opcode) {
        case IR_CONST_INT:
            emit_op(code, BC_LDINT);
            bb_push_u64(code, (uint64_t)ins->imm.int_val);
            break;
        case IR_CONST_FLOAT:
            emit_op(code, BC_LDFLT); {
                uint64_t bits;
                memcpy(&bits, &ins->imm.float_val, 8);
                bb_push_u64(code, bits);
            }
            break;
        case IR_CONST_STR:
            emit_op(code, BC_LDSTR);
            bb_push_u32(code, ins->imm.str_id);
            break;
        case IR_CONST_BOOL:
            emit_op(code, ins->imm.int_val ? BC_LDTRUE : BC_LDFALSE);
            break;
        case IR_CONST_NULL:
            emit_op(code, BC_LDNULL);
            break;
        case IR_ADD:    emit_op(code, BC_ADD); break;
        case IR_SUB:    emit_op(code, BC_SUB); break;
        case IR_MUL:    emit_op(code, BC_MUL); break;
        case IR_DIV:    emit_op(code, BC_DIV); break;
        case IR_IDIV:   emit_op(code, BC_IDIV);break;
        case IR_MOD:    emit_op(code, BC_MOD); break;
        case IR_POW:    emit_op(code, BC_POW); break;
        case IR_NEG:    emit_op(code, BC_NEG); break;
        case IR_AND:    emit_op(code, BC_AND); break;
        case IR_OR:     emit_op(code, BC_OR);  break;
        case IR_XOR:    emit_op(code, BC_XOR); break;
        case IR_NOT:    emit_op(code, BC_NOT); break;
        case IR_EQ:     emit_op(code, BC_CEQ); break;
        case IR_NEQ:    emit_op(code, BC_CNE); break;
        case IR_LT:     emit_op(code, BC_CLT); break;
        case IR_LE:     emit_op(code, BC_CLE); break;
        case IR_GT:     emit_op(code, BC_CGT); break;
        case IR_GE:     emit_op(code, BC_CGE); break;
        case IR_CONCAT: emit_op(code, BC_CONCAT); break;
        case IR_LOAD:
            emit_op(code, BC_LDLOC);
            bb_push_u16(code, (uint16_t)ins->src[0]);
            break;
        case IR_STORE:
            emit_op(code, BC_STLOC);
            bb_push_u16(code, (uint16_t)ins->src[0]);
            break;
        case IR_CALL: case IR_CALL_VOID:
            emit_op(code, BC_CALL);
            bb_push_u32(code, ins->src[0]);
            bb_push_u16(code, (uint16_t)ins->arg_count);
            break;
        case IR_RET:    emit_op(code, BC_RET); break;
        case IR_RET_VOID: emit_op(code, BC_RETVOID); break;
        case IR_JMP:
            emit_op(code, BC_JMP);
            bb_push_u32(code, ins->imm.target_block);
            break;
        case IR_JMPIF:
            emit_op(code, BC_JMPTRUE);
            bb_push_u32(code, ins->imm.target_block);
            break;
        case IR_FIELD_LOAD:
            emit_op(code, BC_LDFLD);
            bb_push_u32(code, ins->imm.field_id);
            break;
        case IR_FIELD_STORE:
            emit_op(code, BC_STFLD);
            bb_push_u32(code, ins->imm.field_id);
            break;
        case IR_NEW:
            emit_op(code, BC_NEWOBJ);
            bb_push_u32(code, ins->imm.type_ref);
            bb_push_u16(code, (uint16_t)ins->arg_count);
            break;
        default:
            emit_op(code, BC_NOP);
            break;
    }
}

/* -------------------------------------------------------------------------
 * Write NLB file
 * ---------------------------------------------------------------------- */

int nl_bytecode_write(const NlIrModule *m, const char *path) {
    FILE *f = fopen(path, "wb");
    if (!f) return -1;

    /* collect per-function code */
    uint32_t func_count = m->function_count;
    ByteBuf  *codes = calloc(func_count, sizeof(ByteBuf));
    uint32_t  fi = 0;
    for (NlIrFunction *func = m->functions; func; func = func->next, fi++) {
        bb_init(&codes[fi]);
        for (NlIrBasicBlock *b = func->block_list; b; b = b->next)
            for (NlIrInstr *ins = b->head; ins; ins = ins->next)
                translate_instr(&codes[fi], ins);
    }

    /* calculate code offsets */
    uint32_t code_base = sizeof(NlbHeader) + func_count * sizeof(NlbMethod);
    uint32_t *offsets = calloc(func_count, sizeof(uint32_t));
    uint32_t off = code_base;
    for (uint32_t i = 0; i < func_count; i++) { offsets[i] = off; off += codes[i].len; }

    /* header */
    NlbHeader hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.magic        = NLB_MAGIC;
    hdr.version      = NLB_VERSION;
    hdr.entry_method = 0;
    hdr.method_count = func_count;
    fwrite(&hdr, sizeof(hdr), 1, f);

    /* method table */
    fi = 0;
    for (NlIrFunction *func = m->functions; func; func = func->next, fi++) {
        NlbMethod meth;
        memset(&meth, 0, sizeof(meth));
        meth.name_str   = func->name_id;
        meth.code_offset= offsets[fi];
        meth.code_len   = codes[fi].len;
        meth.param_count= (uint16_t)func->param_count;
        fwrite(&meth, sizeof(meth), 1, f);
    }

    /* code sections */
    for (uint32_t i = 0; i < func_count; i++)
        fwrite(codes[i].data, 1, codes[i].len, f);

    for (uint32_t i = 0; i < func_count; i++) bb_free(&codes[i]);
    free(codes);
    free(offsets);
    fclose(f);
    return 0;
}

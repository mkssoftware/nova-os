#include "ir.h"
#include <stdio.h>
#include <string.h>

void nl_ir_module_init(NlIrModule *m, NlArena *a) {
    memset(m, 0, sizeof(*m));
    m->arena = a;
}

NlIrFunction *nl_ir_add_function(NlIrModule *m, uint32_t name_id, NlTypeRef ret) {
    NlIrFunction *f = nl_arena_alloc(m->arena, sizeof(NlIrFunction));
    memset(f, 0, sizeof(*f));
    f->name_id     = name_id;
    f->return_type = ret;
    f->next_val    = 1;
    /* append */
    NlIrFunction **p = &m->functions;
    while (*p) p = &(*p)->next;
    *p = f;
    m->function_count++;
    return f;
}

NlIrBasicBlock *nl_ir_add_block(NlIrFunction *f, NlArena *a) {
    NlIrBasicBlock *b = nl_arena_alloc(a, sizeof(NlIrBasicBlock));
    memset(b, 0, sizeof(*b));
    b->id = f->block_count++;
    NlIrBasicBlock **p = &f->block_list;
    while (*p) p = &(*p)->next;
    *p = b;
    if (!f->entry) f->entry = b;
    return b;
}

NlIrInstr *nl_ir_emit(NlIrFunction *f, NlIrBasicBlock *b, NlArena *a, NlIrOpcode op) {
    NlIrInstr *instr = nl_arena_alloc(a, sizeof(NlIrInstr));
    memset(instr, 0, sizeof(*instr));
    instr->opcode = op;
    (void)f;
    if (!b->head) b->head = b->tail = instr;
    else { b->tail->next = instr; b->tail = instr; }
    b->instr_count++;
    return instr;
}

NlIrVal nl_ir_next_val(NlIrFunction *f) {
    return f->next_val++;
}

static const char *opcode_name(NlIrOpcode op) {
    static const char *names[] = {
        "add","sub","mul","div","idiv","mod","pow","neg",
        "and","or","xor","not","shl","shr",
        "eq","neq","lt","le","gt","ge","concat",
        "load","store","alloca","call","call_void","ret","ret_void",
        "jmp","jmpif",
        "const_int","const_float","const_str","const_bool","const_null",
        "phi","field_load","field_store","new","cast","typeof"
    };
    if ((unsigned)op < IR_OPCODE_COUNT) return names[op];
    return "?";
}

void nl_ir_print(const NlIrModule *m) {
    for (NlIrFunction *f = m->functions; f; f = f->next) {
        printf("function #%u {\n", f->name_id);
        for (NlIrBasicBlock *b = f->block_list; b; b = b->next) {
            printf("  bb%u:\n", b->id);
            for (NlIrInstr *ins = b->head; ins; ins = ins->next) {
                if (ins->dst)
                    printf("    v%u = %s", ins->dst, opcode_name(ins->opcode));
                else
                    printf("    %s", opcode_name(ins->opcode));
                if (ins->src[0]) printf(" v%u", ins->src[0]);
                if (ins->src[1]) printf(" v%u", ins->src[1]);
                if (ins->src[2]) printf(" v%u", ins->src[2]);
                printf("\n");
            }
        }
        printf("}\n");
    }
}

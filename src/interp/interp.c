#include "interp.h"
#include <stdint.h>
#include <stdio.h>

#include "../tables.h"

web49_interp_block_t *web49_interp_import(web49_interp_t *interp, const char *mod, const char *sym) {
    if (!strcmp(mod, "wasi_snapshot_preview1")) {
        web49_interp_instr_t iit;
        uint64_t nargs = UINT64_MAX;
        uint64_t nreturns = UINT64_MAX;
        if (!strcmp(sym, "proc_exit")) {
            iit = WEB49_INTERP_INSTR_WASI_PROC_EXIT;
            nargs = 1;
            nreturns = 0;
        } else if (!strcmp(sym, "fd_write")) {
            iit = WEB49_INTERP_INSTR_WASI_FD_WRITE;
            nargs = 4;
            nreturns = 1;
        // } else if (!strcmp(sym, "fd_close")) {
        //     iit = WEB49_INTERP_INSTR_WASI_FD_CLOSE;
        // } else if (!strcmp(sym, "fd_seek")) {
        //     iit = WEB49_INTERP_INSTR_WASI_FD_SEEK;
        } else {
            fprintf(stderr, "unknown wasi import symbol: %s\n", sym);
            exit(1);
        }
        web49_interp_opcode_t *instrs = web49_malloc(sizeof(web49_interp_opcode_t) * 2);
        instrs[0].opcode = iit;
        instrs[1].opcode = WEB49_INTERP_INSTR_RETURN;
        web49_interp_block_t *block = web49_malloc(sizeof(web49_interp_block_t));
        block->code = instrs;
        block->ncode = 2;
        block->nlocals = 0;
        block->nreturns = nreturns;
        block->nparams = nargs;
        return block;
    }
    fprintf(stderr, "unknown import module: %s\n", mod);
    exit(1);
}

web49_interp_block_t *web49_interp_read_block(web49_interp_t *interp, web49_interp_instr_buf_t *instrs) {
    bool isfunc = instrs->head == 0;
    uint64_t alloc = 128;
    web49_interp_opcode_t *code = web49_malloc(sizeof(web49_interp_opcode_t) * alloc);
    uint64_t ncode = 0;
    while (instrs->head < instrs->len) {
        web49_instr_t cur = instrs->instrs[instrs->head++];
        if (cur.opcode == WEB49_OPCODE_END || cur.opcode == WEB49_OPCODE_ELSE) {
            break;
        }
        switch (cur.opcode) {
            case WEB49_OPCODE_NOP:
                break;
            case WEB49_OPCODE_UNREACHABLE:
                break;
            case WEB49_OPCODE_RETURN:
                code[ncode++].opcode = WEB49_INTERP_INSTR_RETURN;
                break;
            case WEB49_OPCODE_DROP:
                code[ncode++].opcode = WEB49_INTERP_INSTR_DROP;
                break;
            case WEB49_OPCODE_GET_GLOBAL:
                code[ncode++].opcode = WEB49_INTERP_INSTR_GET_GLOBAL;
                code[ncode++].data.i32_u = cur.immediate.varuint32;
                break;
            case WEB49_OPCODE_SET_GLOBAL:
                code[ncode++].opcode = WEB49_INTERP_INSTR_SET_GLOBAL;
                code[ncode++].data.i32_u = cur.immediate.varuint32;
                break;
            case WEB49_OPCODE_I32_WRAP_I64:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_WRAP_I64;
                break;
            case WEB49_OPCODE_I64_CONST:
                code[ncode++].opcode = WEB49_INTERP_INSTR_DATA_CONST;
                code[ncode++].data.i64_s = cur.immediate.varint64;
                break;
            case WEB49_OPCODE_I32_CONST:
                code[ncode++].opcode = WEB49_INTERP_INSTR_DATA_CONST;
                code[ncode++].data.i32_s = cur.immediate.varint32;
                break;
            case WEB49_OPCODE_I32_ADD:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_ADD;
                break;
            case WEB49_OPCODE_I32_SUB:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_SUB;
                break;
            case WEB49_OPCODE_I32_MUL:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_MUL;
                break;
            case WEB49_OPCODE_I32_DIV_S:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_DIV_S;
                break;
            case WEB49_OPCODE_I32_DIV_U:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_DIV_S;
                break;
            case WEB49_OPCODE_I32_EQ:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_EQ;
                break;
            case WEB49_OPCODE_I32_NE:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_NE;
                break;
            case WEB49_OPCODE_I32_LT_S:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_LT_S;
                break;
            case WEB49_OPCODE_I32_GT_S:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_GT_S;
                break;
            case WEB49_OPCODE_I32_LE_S:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_LE_S;
                break;
            case WEB49_OPCODE_I32_GE_S:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_GE_S;
                break;
            case WEB49_OPCODE_I32_LT_U:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_LT_U;
                break;
            case WEB49_OPCODE_I32_GT_U:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_GT_U;
                break;
            case WEB49_OPCODE_I32_LE_U:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_LE_U;
                break;
            case WEB49_OPCODE_I32_GE_U:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_GE_U;
                break;
            case WEB49_OPCODE_I32_SHL:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_SHL;
                break;
            case WEB49_OPCODE_I32_SHR_S:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_SHR_S;
                break;
            case WEB49_OPCODE_I32_SHR_U:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_SHR_U;
                break;
            case WEB49_OPCODE_I32_EQZ:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_EQZ;
                break;
            case WEB49_OPCODE_I32_OR:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_OR;
                break;
            case WEB49_OPCODE_I32_XOR:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_XOR;
                break;
            case WEB49_OPCODE_I32_AND:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_AND;
                break;
            case WEB49_OPCODE_I32_STORE:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_STORE;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_I32_LOAD:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_LOAD;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_I32_STORE8:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_STORE8;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_I32_LOAD8_S:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_LOAD8_S;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_I32_LOAD8_U:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_LOAD8_U;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_I32_STORE16:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_STORE16;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_I32_LOAD16_S:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_LOAD16_S;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_I32_LOAD16_U:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I32_LOAD16_U;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_I64_ADD:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_ADD;
                break;
            case WEB49_OPCODE_I64_SUB:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_SUB;
                break;
            case WEB49_OPCODE_I64_MUL:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_MUL;
                break;
            case WEB49_OPCODE_I64_DIV_S:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_DIV_S;
                break;
            case WEB49_OPCODE_I64_DIV_U:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_DIV_S;
                break;
            case WEB49_OPCODE_I64_EQ:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_EQ;
                break;
            case WEB49_OPCODE_I64_NE:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_NE;
                break;
            case WEB49_OPCODE_I64_LT_S:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_LT_S;
                break;
            case WEB49_OPCODE_I64_GT_S:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_GT_S;
                break;
            case WEB49_OPCODE_I64_LE_S:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_LE_S;
                break;
            case WEB49_OPCODE_I64_GE_S:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_GE_S;
                break;
            case WEB49_OPCODE_I64_LT_U:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_LT_U;
                break;
            case WEB49_OPCODE_I64_GT_U:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_GT_U;
                break;
            case WEB49_OPCODE_I64_LE_U:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_LE_U;
                break;
            case WEB49_OPCODE_I64_GE_U:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_GE_U;
                break;
            case WEB49_OPCODE_I64_SHL:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_SHL;
                break;
            case WEB49_OPCODE_I64_SHR_S:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_SHR_S;
                break;
            case WEB49_OPCODE_I64_SHR_U:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_SHR_U;
                break;
            case WEB49_OPCODE_I64_EQZ:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_EQZ;
                break;
            case WEB49_OPCODE_I64_OR:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_OR;
                break;
            case WEB49_OPCODE_I64_AND:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_AND;
                break;
            case WEB49_OPCODE_I64_XOR:
                code[ncode++].opcode = WEB49_OPCODE_I64_XOR;
                break;
            case WEB49_OPCODE_I64_STORE:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_STORE;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_I64_LOAD:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_LOAD;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_I64_STORE8:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_STORE8;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_I64_LOAD8_S:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_LOAD8_S;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_I64_LOAD8_U:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_LOAD8_U;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_I64_STORE16:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_STORE16;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_I64_LOAD16_S:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_LOAD16_S;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_I64_LOAD16_U:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_LOAD16_U;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_I64_STORE32:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_STORE32;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_I64_LOAD32_S:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_LOAD32_S;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_I64_LOAD32_U:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_LOAD32_U;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            case WEB49_OPCODE_TEE_LOCAL:
                code[ncode++].opcode = WEB49_INTERP_INSTR_TEE_LOCAL;
                code[ncode++].data.i32_u = cur.immediate.varuint32;
                break;
            case WEB49_OPCODE_GET_LOCAL:
                code[ncode++].opcode = WEB49_INTERP_INSTR_GET_LOCAL;
                code[ncode++].data.i32_u = cur.immediate.varuint32;
                break;
            case WEB49_OPCODE_SET_LOCAL:
                code[ncode++].opcode = WEB49_INTERP_INSTR_SET_LOCAL;
                code[ncode++].data.i32_u = cur.immediate.varuint32;
                break;
            case WEB49_OPCODE_CALL:
                code[ncode++].opcode = WEB49_INTERP_INSTR_CALL;
                code[ncode++].data.i32_u = cur.immediate.varuint32;
                break;
            case WEB49_OPCODE_CALL_INDIRECT:
                code[ncode++].opcode = WEB49_INTERP_INSTR_CALL_INDIRECT;
                break;
            case WEB49_OPCODE_BR:
                code[ncode++].opcode = WEB49_INTERP_INSTR_BR;
                code[ncode++].data.i32_u = cur.immediate.varuint32;
                break;
            case WEB49_OPCODE_BR_TABLE:
                code[ncode++].opcode = WEB49_INTERP_INSTR_BR_TABLE;
                code[ncode++].data.i32_u = cur.immediate.br_table.num_targets;
                for (uint64_t i = 0; i < cur.immediate.br_table.num_targets; i++) {
                    code[ncode++].data.i32_u = cur.immediate.br_table.targets[i];
                }
                code[ncode++].data.i32_u = cur.immediate.br_table.default_target;
                break;
            case WEB49_OPCODE_BR_IF:
                code[ncode++].opcode = WEB49_INTERP_INSTR_BR_IF;
                code[ncode++].data.i32_u = cur.immediate.varuint32;
                break;
            case WEB49_OPCODE_SELECT:
                code[ncode++].opcode = WEB49_INTERP_INSTR_SELECT;
                break;
            case WEB49_OPCODE_IF:
                code[ncode++].opcode = WEB49_INTERP_INSTR_IF;
                code[ncode++].block = web49_interp_read_block(interp, instrs);
                if (instrs->instrs[instrs->head - 1].opcode == WEB49_OPCODE_ELSE) {
                    code[ncode++].block = web49_interp_read_block(interp, instrs);
                } else {
                    web49_interp_opcode_t *instrs = web49_malloc(sizeof(web49_interp_opcode_t));
                    instrs[0].opcode = WEB49_INTERP_INSTR_END;
                    web49_interp_block_t *block = web49_malloc(sizeof(web49_interp_block_t));
                    block->code = instrs;
                    block->ncode = 1;
                    block->nlocals = 0;
                    block->nparams = 0;
                    code[ncode++].block = block;
                }
                break;
            case WEB49_OPCODE_LOOP:
                code[ncode++].opcode = WEB49_INTERP_INSTR_LOOP;
                code[ncode++].block = web49_interp_read_block(interp, instrs);
                break;
            case WEB49_OPCODE_BLOCK:
                code[ncode++].opcode = WEB49_INTERP_INSTR_BLOCK;
                code[ncode++].block = web49_interp_read_block(interp, instrs);
                break;
            case WEB49_OPCODE_I64_EXTEND_S_I32:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_EXTEND_I32_S;
                break;
            case WEB49_OPCODE_I64_EXTEND_U_I32:
                code[ncode++].opcode = WEB49_INTERP_INSTR_I64_EXTEND_I32_U;
                break;
            case WEB49_OPCODE_F64_LOAD:
                code[ncode++].opcode = WEB49_INTERP_INSTR_F64_LOAD;
                code[ncode++].data.i32_u = cur.immediate.memory_immediate.offset;
                break;
            default:
                fprintf(stderr, "not implemented: %s\n", web49_opcode_to_name(cur.opcode));
                exit(1);
        }
        if (ncode + 64 >= alloc) {
            alloc = (ncode + 64) * 2;
            code = web49_realloc(code, sizeof(web49_interp_opcode_t) * alloc);
        }
    }
    if (isfunc) {
        code[ncode++].opcode = WEB49_INTERP_INSTR_RETURN;
    } else {
        code[ncode++].opcode = WEB49_INTERP_INSTR_END;
    }
    web49_interp_block_t *block = web49_malloc(sizeof(web49_interp_block_t));
    block->code = code;
    block->ncode = ncode;
    block->nlocals = 0;
    block->nparams = 0;
    return block;
}

int32_t web49_interp_block_run(web49_interp_t interp, web49_interp_block_t *restrict block) {
    // static size_t depth = 0;
    web49_interp_opcode_t *head = block->code;
    int32_t val = -1;
    // fprintf(stderr, "BEGIN: %zu\n", ++depth);
    while (true) {
        // fprintf(stderr, "code[%zu] = %zu\n", head - block->code, head->opcode);
        switch (head++->opcode) {
        case WEB49_INTERP_INSTR_DROP: {
            interp.stack -= 1;
            break;
        }
        case WEB49_INTERP_INSTR_RETURN: {
            if (interp.nreturns > 0) {
                interp.locals[0] = interp.stack[-1];
            }
            val = INT32_MAX;
            goto ret;
        }
        case WEB49_INTERP_INSTR_SET_GLOBAL: {
            interp.globals[head++->data.i32_u] = *--interp.stack;
            break;
        }
        case WEB49_INTERP_INSTR_GET_GLOBAL: {
            *interp.stack++ = interp.globals[head++->data.i32_u];
            break;
        }
        case WEB49_INTERP_INSTR_TEE_LOCAL: {
            interp.locals[head++->data.i32_u] = interp.stack[-1];
            break;
        }
        case WEB49_INTERP_INSTR_GET_LOCAL: {
            *interp.stack++ = interp.locals[head++->data.i32_u];
            break;
        }
        case WEB49_INTERP_INSTR_SET_LOCAL: {
            interp.locals[head++->data.i32_u] = *--interp.stack;
            break;
        }
        case WEB49_INTERP_INSTR_I32_ADD: {
            uint32_t rhs = (--interp.stack)->i32_u;
            uint32_t lhs = (--interp.stack)->i32_u;
            interp.stack++->i32_u = (uint32_t) (lhs + rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_SUB: {
            uint32_t rhs = (--interp.stack)->i32_u;
            uint32_t lhs = (--interp.stack)->i32_u;
            interp.stack++->i32_u = (uint32_t) (lhs - rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_MUL: {
            uint32_t rhs = (--interp.stack)->i32_u;
            uint32_t lhs = (--interp.stack)->i32_u;
            interp.stack++->i32_u = (uint32_t) (lhs * rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_DIV_S: {
            int32_t rhs = (--interp.stack)->i32_s;
            int32_t lhs = (--interp.stack)->i32_s;
            interp.stack++->i32_s = (int32_t) (lhs / rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_DIV_U: {
            uint32_t rhs = (--interp.stack)->i32_u;
            uint32_t lhs = (--interp.stack)->i32_u;
            interp.stack++->i32_u = (uint32_t) (lhs / rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_EQ: {
            uint32_t rhs = (--interp.stack)->i32_u;
            uint32_t lhs = (--interp.stack)->i32_u;
            interp.stack++->i64_u = (uint64_t)(lhs == rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_NE: {
            uint32_t rhs = (--interp.stack)->i32_u;
            uint32_t lhs = (--interp.stack)->i32_u;
            interp.stack++->i64_u = (uint64_t)(lhs != rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_LT_S: {
            int32_t rhs = (--interp.stack)->i32_s;
            int32_t lhs = (--interp.stack)->i32_s;
            interp.stack++->i64_u = (uint64_t)(lhs < rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_GT_S: {
            int32_t rhs = (--interp.stack)->i32_s;
            int32_t lhs = (--interp.stack)->i32_s;
            interp.stack++->i64_u = (uint64_t)(lhs > rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_LE_S: {
            int32_t rhs = (--interp.stack)->i32_s;
            int32_t lhs = (--interp.stack)->i32_s;
            interp.stack++->i64_u = (uint64_t)(lhs <= rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_GE_S: {
            int32_t rhs = (--interp.stack)->i32_s;
            int32_t lhs = (--interp.stack)->i32_s;
            interp.stack++->i64_u = (uint64_t)(lhs >= rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_LT_U: {
            uint32_t rhs = (--interp.stack)->i32_u;
            uint32_t lhs = (--interp.stack)->i32_u;
            interp.stack++->i64_u = (uint64_t)(lhs < rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_GT_U: {
            uint32_t rhs = (--interp.stack)->i32_u;
            uint32_t lhs = (--interp.stack)->i32_u;
            interp.stack++->i64_u = (uint64_t)(lhs > rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_LE_U: {
            uint32_t rhs = (--interp.stack)->i32_u;
            uint32_t lhs = (--interp.stack)->i32_u;
            interp.stack++->i64_u = (uint64_t)(lhs <= rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_GE_U: {
            uint32_t rhs = (--interp.stack)->i32_u;
            uint32_t lhs = (--interp.stack)->i32_u;
            interp.stack++->i64_u = (uint64_t)(lhs >= rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_SHL: {
            uint32_t rhs = (--interp.stack)->i32_u;
            uint32_t lhs = (--interp.stack)->i32_u;
            interp.stack++->i32_u = (lhs << rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_SHR_U: {
            uint32_t rhs = (--interp.stack)->i32_u;
            uint32_t lhs = (--interp.stack)->i32_u;
            interp.stack++->i32_u = (lhs >> rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_SHR_S: {
            uint32_t rhs = (--interp.stack)->i32_u;
            int32_t lhs = (--interp.stack)->i32_s;
            interp.stack++->i32_s = (lhs >> rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_EQZ: {
            interp.stack[-1].i64_u = interp.stack[-1].i32_u == 0 ? 1 : 0; 
            break;
        }
        case WEB49_INTERP_INSTR_I32_AND: {
            uint32_t rhs = (--interp.stack)->i32_u;
            uint32_t lhs = (--interp.stack)->i32_u;
            interp.stack++->i32_u = (uint32_t) (lhs & rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_OR: {
            uint32_t rhs = (--interp.stack)->i32_u;
            uint32_t lhs = (--interp.stack)->i32_u;
            interp.stack++->i32_u = (uint32_t) (lhs | rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_XOR: {
            uint32_t rhs = (--interp.stack)->i32_u;
            uint32_t lhs = (--interp.stack)->i32_u;
            interp.stack++->i32_u = (uint32_t) (lhs ^ rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I32_STORE8: {
            uint32_t val = (--interp.stack)->i32_u;
            uint32_t ptr = (--interp.stack)->i32_u;
            uint32_t off = head++->data.i32_u;
            * (uint8_t*) &interp.memory[ptr + off] = (uint8_t) val;
            break;
        }
        case WEB49_INTERP_INSTR_I32_LOAD8_S: {
            uint32_t ptr = (--interp.stack)->i32_u;
            uint32_t off = head++->data.i32_u;
            interp.stack++->i32_s = (int32_t)*(int8_t *)&interp.memory[ptr + off];
            break;
        }
        case WEB49_INTERP_INSTR_I32_LOAD8_U: {
            uint32_t ptr = (--interp.stack)->i32_u;
            uint32_t off = head++->data.i32_u;
            interp.stack++->i32_u = (uint32_t)*(uint8_t *)&interp.memory[ptr + off];
            break;
        }
        case WEB49_INTERP_INSTR_I32_STORE16: {
            uint32_t val = (--interp.stack)->i32_u;
            uint32_t ptr = (--interp.stack)->i32_u;
            uint32_t off = head++->data.i32_u;
            * (uint16_t*) &interp.memory[ptr + off] = (uint16_t) val;
            break;
        }
        case WEB49_INTERP_INSTR_I32_LOAD16_S: {
            uint32_t ptr = (--interp.stack)->i32_u;
            uint32_t off = head++->data.i32_u;
            interp.stack++->i32_s = (int32_t)*(int16_t *)&interp.memory[ptr + off];
            break;
        }
        case WEB49_INTERP_INSTR_I32_LOAD16_U: {
            uint32_t ptr = (--interp.stack)->i32_u;
            uint32_t off = head++->data.i32_u;
            interp.stack++->i32_u = (uint32_t)*(uint16_t *)&interp.memory[ptr + off];
            break;
        }
        case WEB49_INTERP_INSTR_I32_STORE: {
            uint32_t val = (--interp.stack)->i32_u;
            uint32_t ptr = (--interp.stack)->i32_u;
            uint32_t off = head++->data.i32_u;
            * (uint32_t*) &interp.memory[ptr + off] = (uint32_t) val;
            break;
        }
        case WEB49_INTERP_INSTR_I32_LOAD: {
            uint32_t ptr = (--interp.stack)->i32_u;
            uint32_t off = head++->data.i32_u;
            interp.stack++->i32_u = * (uint32_t*) &interp.memory[ptr + off];
            break;
        }
        case WEB49_INTERP_INSTR_I64_EXTEND_I32_U: {
            uint32_t val = (--interp.stack)->i64_u;
            interp.stack++->i64_s = (int64_t) val;
            break;
        }
        case WEB49_INTERP_INSTR_I64_EXTEND_I32_S: {
            int32_t val = (--interp.stack)->i64_s;
            interp.stack++->i64_s = (int64_t) val;
            break;
        }
        case WEB49_INTERP_INSTR_I32_WRAP_I64: {
            int64_t val = (--interp.stack)->i64_u;
            interp.stack++->i32_s = (int32_t) val;
            break;
        }
        case WEB49_INTERP_INSTR_I64_ADD: {
            uint64_t rhs = (--interp.stack)->i64_u;
            uint64_t lhs = (--interp.stack)->i64_u;
            interp.stack++->i64_u = (uint64_t) (lhs + rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I64_SUB: {
            uint64_t rhs = (--interp.stack)->i64_u;
            uint64_t lhs = (--interp.stack)->i64_u;
            interp.stack++->i64_u = (uint64_t) (lhs - rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I64_MUL: {
            uint64_t rhs = (--interp.stack)->i64_u;
            uint64_t lhs = (--interp.stack)->i64_u;
            interp.stack++->i64_u = (uint64_t) (lhs * rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I64_DIV_S: {
            int64_t rhs = (--interp.stack)->i64_s;
            int64_t lhs = (--interp.stack)->i64_s;
            interp.stack++->i64_s = (int64_t) (lhs / rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I64_DIV_U: {
            uint64_t rhs = (--interp.stack)->i64_u;
            uint64_t lhs = (--interp.stack)->i64_u;
            interp.stack++->i64_u = (uint64_t) (lhs / rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I64_EQ: {
            uint64_t rhs = (--interp.stack)->i64_u;
            uint64_t lhs = (--interp.stack)->i64_u;
            interp.stack++->i64_u = (uint64_t) (lhs == rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I64_NE: {
            uint64_t rhs = (--interp.stack)->i64_u;
            uint64_t lhs = (--interp.stack)->i64_u;
            interp.stack++->i64_u = (uint64_t) (lhs != rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I64_LT_S: {
            int64_t rhs = (--interp.stack)->i64_s;
            int64_t lhs = (--interp.stack)->i64_s;
            interp.stack++->i64_u = (uint64_t) (lhs < rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I64_GT_S: {
            int64_t rhs = (--interp.stack)->i64_s;
            int64_t lhs = (--interp.stack)->i64_s;
            interp.stack++->i64_u = (uint64_t) (lhs > rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I64_LE_S: {
            int64_t rhs = (--interp.stack)->i64_s;
            int64_t lhs = (--interp.stack)->i64_s;
            interp.stack++->i64_u = (uint64_t) (lhs <= rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I64_GE_S: {
            int64_t rhs = (--interp.stack)->i64_s;
            int64_t lhs = (--interp.stack)->i64_s;
            interp.stack++->i64_u = (uint64_t) (lhs >= rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I64_LT_U: {
            uint64_t rhs = (--interp.stack)->i64_u;
            uint64_t lhs = (--interp.stack)->i64_u;
            // fprintf(stderr, "%zu < %zu\n", (size_t) lhs, (size_t) rhs);
            interp.stack++->i64_u = (uint64_t) (lhs < rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I64_GT_U: {
            uint64_t rhs = (--interp.stack)->i64_u;
            uint64_t lhs = (--interp.stack)->i64_u;
            interp.stack++->i64_u = (uint64_t) (lhs > rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I64_LE_U: {
            uint64_t rhs = (--interp.stack)->i64_u;
            uint64_t lhs = (--interp.stack)->i64_u;
            interp.stack++->i64_u = (uint64_t) (lhs <= rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I64_GE_U: {
            uint64_t rhs = (--interp.stack)->i64_u;
            uint64_t lhs = (--interp.stack)->i64_u;
            interp.stack++->i64_u = (uint64_t) (lhs >= rhs);
            break;
        }
        case WEB49_INTERP_INSTR_I64_SHL: {
            fprintf(stderr, "unimplemented: i64_shl\n");
            exit(1);
            break;
        }
        case WEB49_INTERP_INSTR_I64_SHR_U: {
            fprintf(stderr, "unimplemented: i64_shr_u\n");
            exit(1);
            break;
        }
        case WEB49_INTERP_INSTR_I64_SHR_S: {
            fprintf(stderr, "unimplemented: i64_shr_s\n");
            exit(1);
            break;
        }
        case WEB49_INTERP_INSTR_I64_EQZ: {
            interp.stack[-1].i64_u = interp.stack[-1].i64_u == 0 ? 1 : 0; 
            break;
        }
        case WEB49_INTERP_INSTR_I64_AND: {
            fprintf(stderr, "unimplemented: i64_and\n");
            exit(1);
            break;
        }
        case WEB49_INTERP_INSTR_I64_OR: {
            fprintf(stderr, "unimplemented: i64_or\n");
            exit(1);
            break;
        }
        case WEB49_INTERP_INSTR_I64_XOR: {
            fprintf(stderr, "unimplemented: i64_xor\n");
            exit(1);
            break;
        }
        case WEB49_INTERP_INSTR_I64_STORE8: {
            fprintf(stderr, "unimplemented: i64_store8\n");
            exit(1);
            break;
        }
        case WEB49_INTERP_INSTR_I64_LOAD8_S: {
            uint32_t ptr = (--interp.stack)->i32_u;
            uint32_t off = head++->data.i32_u;
            interp.stack++->i64_s = (int64_t)*(int8_t *)&interp.memory[ptr + off];
            break;
        }
        case WEB49_INTERP_INSTR_I64_LOAD8_U: {
            uint32_t ptr = (--interp.stack)->i32_u;
            uint32_t off = head++->data.i32_u;
            interp.stack++->i64_u = (uint64_t)*(uint8_t *)&interp.memory[ptr + off];
            break;
        }
        case WEB49_INTERP_INSTR_I64_STORE16: {
            fprintf(stderr, "unimplemented: i64_store16\n");
            exit(1);
            break;
        }
        case WEB49_INTERP_INSTR_I64_LOAD16_S: {
            uint32_t ptr = (--interp.stack)->i32_u;
            uint32_t off = head++->data.i32_u;
            interp.stack++->i64_s = (int64_t)*(int16_t *)&interp.memory[ptr + off];
            break;
        }
        case WEB49_INTERP_INSTR_I64_LOAD16_U: {
            uint32_t ptr = (--interp.stack)->i32_u;
            uint32_t off = head++->data.i32_u;
            interp.stack++->i64_u = (uint64_t)*(uint16_t *)&interp.memory[ptr + off];
            break;
        }
        case WEB49_INTERP_INSTR_I64_STORE32: {
            fprintf(stderr, "unimplemented: i64_store32\n");
            exit(1);
            break;
        }
        case WEB49_INTERP_INSTR_I64_LOAD32_S: {
            uint32_t ptr = (--interp.stack)->i32_u;
            uint32_t off = head++->data.i32_u;
            interp.stack++->i64_s = (int64_t)*(int32_t *)&interp.memory[ptr + off];
            break;
        }
        case WEB49_INTERP_INSTR_I64_LOAD32_U: {
            uint32_t ptr = (--interp.stack)->i32_u;
            uint32_t off = head++->data.i32_u;
            interp.stack++->i64_u = (uint64_t)*(uint32_t *)&interp.memory[ptr + off];
            break;
        }
        case WEB49_INTERP_INSTR_I64_STORE: {
            uint64_t val = (--interp.stack)->i64_u;
            uint32_t ptr = (--interp.stack)->i32_u;
            uint32_t off = head++->data.i32_u;
            * (uint64_t*) &interp.memory[ptr + off] = (uint64_t) val;
            break;
        }
        case WEB49_INTERP_INSTR_I64_LOAD: {
            uint32_t ptr = (--interp.stack)->i32_u;
            uint32_t off = head++->data.i32_u;
            interp.stack++->i32_u = * (uint64_t*) &interp.memory[ptr + off];
            break;
        }
        case WEB49_INTERP_INSTR_F64_LOAD: {
            fprintf(stderr, "unimplemented: f64_load\n");
            exit(1);
            break;
        }
        case WEB49_INTERP_INSTR_DATA_CONST: {
            *interp.stack++ = head++->data;
            break;
        }
        case WEB49_INTERP_INSTR_CALL: {
            uint32_t funcno = head++->data.i32_u;
            web49_interp_block_t *func = interp.funcs[funcno];
            web49_interp_data_t *old_locals = interp.locals;
            web49_interp_data_t *old_stack = interp.stack;
            uint64_t old_nreturns = interp.nreturns;
            interp.locals = interp.stack - func->nparams;
            // for (uint64_t i = 0; i < func->nparams; i++) {
            //     fprintf(stderr, "args[%zu] <- %"PRIu32"\n", (size_t) i, interp.locals[i].i32_u);
            // }
            interp.stack += func->nlocals;
            interp.nreturns = func->nreturns;
            web49_interp_block_run(interp, func);
            interp.stack = old_stack - func->nparams + func->nreturns;
            // for (uint64_t i = 0; i < func->nparams; i++) {
            //     fprintf(stderr, " -> %zu\n", (old_stack - i - func->nparams)->i32_u);
            // }
            interp.locals = old_locals;
            interp.nreturns = old_nreturns;
            break;
        }
        case WEB49_INTERP_INSTR_CALL_INDIRECT: {
            uint32_t funcno = (--interp.stack)->i32_u;
            web49_interp_block_t *func = interp.table[funcno];
            web49_interp_data_t *old_locals = interp.locals;
            web49_interp_data_t *old_stack = interp.stack;
            uint64_t old_nreturns = interp.nreturns;
            interp.locals = interp.stack - func->nparams;
            interp.stack += func->nlocals;
            interp.nreturns = func->nreturns;
            web49_interp_block_run(interp, func);
            interp.stack = old_stack - func->nparams + func->nreturns;
            interp.locals = old_locals;
            interp.nreturns = old_nreturns;
            break;
        }
        case WEB49_INTERP_INSTR_BLOCK: {
            int32_t ret = web49_interp_block_run(interp, head++->block);
            if (ret >= 0) {
                val = ret-1;
                goto ret;
            }
            break;
        }
        case WEB49_INTERP_INSTR_END: {
            val = INT32_MIN;
            goto ret;
        }
        case WEB49_INTERP_INSTR_LOOP: {
            while (true) {
                int32_t ret = web49_interp_block_run(interp, head->block);
                if (ret >= 0) {
                    val = ret-1;
                    goto ret;
                } else if (ret == -1) {
                    continue;
                } else {
                    break;
                }
            }
            head += 1;
            break;
        }
        case WEB49_INTERP_INSTR_SELECT: {
            web49_interp_data_t ift = *--interp.stack;
            web49_interp_data_t iff = *--interp.stack;
            web49_interp_data_t cond = *--interp.stack;
            if (cond.i32_u) {
                *interp.stack++ = ift;
            } else {
                *interp.stack++ = iff;
            }
            break;
        }
        case WEB49_INTERP_INSTR_IF: {
            int32_t ret;
            if ((--interp.stack)->i32_u) {
                ret = web49_interp_block_run(interp, head[0].block);
            } else {
                ret = web49_interp_block_run(interp, head[1].block);
            }
            if (ret >= 0) {
                val = ret-1;
                goto ret;
            }
            head += 2;
            break;
        }
        case WEB49_INTERP_INSTR_BR_IF: {
            if ((--interp.stack)->i32_u) {
                val = head[0].data.i32_u-1;
                goto ret;
            }
            head += 1;
            break;
        }
        case WEB49_INTERP_INSTR_BR: {
            val = head[0].data.i32_u-1;
            goto ret;
        }
        case WEB49_INTERP_INSTR_BR_TABLE: {
            web49_interp_data_t data = *--interp.stack;
            uint32_t max = head++->data.i32_u;
            if (data.i32_u >= max) {
                val = head[max].data.i32_u-1;
                goto ret;
            } else {
                val = head[data.i32_u].data.i32_u-1;
                goto ret;
            }
            break;
        }
        case WEB49_INTERP_INSTR_WASI_PROC_EXIT: {
            exit(0);
            break;
        }
        case WEB49_INTERP_INSTR_WASI_FD_WRITE: {
            uint32_t fd = interp.locals[0].i32_u;
            uint32_t iovs = interp.locals[1].i32_u;
            uint32_t iovs_len = interp.locals[2].i32_u;
            uint32_t nwritten = interp.locals[3].i32_u;
            *(uint32_t *) &interp.memory[nwritten] = 0;
            if (fd == 1) {
                for (size_t i = 0; i < iovs_len; i++) {
                    uint32_t ptr = *(uint32_t *) &interp.memory[iovs+i*8];
                    uint32_t len = *(uint32_t *) &interp.memory[iovs+i*8+4];
                    for (size_t i = 0; i < len; i++) {
                        char chr = (char) interp.memory[ptr++];
                        if (!isprint(chr)) {
                            break;
                        }
                        fprintf(stdout, "%c", chr);
                        *(uint32_t *) &interp.memory[nwritten] += 1;
                    }
                }
            } else {
                fprintf(stdout, "| .fd = 0x%zx\n", (size_t) fd);
                __builtin_trap();
            }
            interp.stack++->i32_u = *(uint32_t *) &interp.memory[nwritten];
            break;
        }
        case WEB49_INTERP_INSTR_WASI_FD_CLOSE: {
            fprintf(stderr, "unimplemented: wasi_fd_close\n");
            exit(1);
            break;
        }
        case WEB49_INTERP_INSTR_WASI_FD_SEEK: {
            fprintf(stderr, "unimplemented: wasi_fd_seek\n");
            exit(1);
            break;
        }
        default:
            fprintf(stderr, "unhandled: %"PRIu64" @%zu\n", (head[-1].opcode), &head[-1]-block->code);
            __builtin_trap();
        }
        // fprintf(stderr, "- OP\n");
    }
ret:;
    // fprintf(stderr, "END: %zu\n", depth--);
    return val;
}

void web49_interp_module(web49_module_t mod) {
    web49_section_type_t type_section = {0};
    web49_section_import_t import_section = {0};
    web49_section_code_t code_section = {0};
    web49_section_function_t function_section = {0};
    web49_section_global_t global_section = {0};
    web49_section_data_t data_section = {0};
    web49_section_table_t table_section = {0};
    web49_section_element_t element_section = {0};
    uint64_t start = 0;
    for (size_t i = 0; i < mod.num_sections; i++) {
        web49_section_t section = mod.sections[i];
        if (section.header.id == WEB49_SECTION_ID_EXPORT) {
            for (size_t j = 0; j < section.export_section.num_entries; j++) {
                web49_section_export_entry_t entry = section.export_section.entries[j];
                if (!strcmp(entry.field_str, "_start")) {
                    start = entry.index;
                }
            }
        }
        if (mod.sections[i].header.id == WEB49_SECTION_ID_TYPE) {
            type_section = mod.sections[i].type_section;
        }
        if (mod.sections[i].header.id == WEB49_SECTION_ID_IMPORT) {
            import_section = mod.sections[i].import_section;
        }
        if (mod.sections[i].header.id == WEB49_SECTION_ID_FUNCTION) {
            function_section = mod.sections[i].function_section;
        }
        if (mod.sections[i].header.id == WEB49_SECTION_ID_CODE) {
            code_section = mod.sections[i].code_section;
        }
        if (mod.sections[i].header.id == WEB49_SECTION_ID_GLOBAL) {
            global_section = mod.sections[i].global_section;
        }
        if (mod.sections[i].header.id == WEB49_SECTION_ID_DATA) {
            data_section = mod.sections[i].data_section;
        }
        if (mod.sections[i].header.id == WEB49_SECTION_ID_TABLE) {
            table_section = mod.sections[i].table_section;
        }
        if (mod.sections[i].header.id == WEB49_SECTION_ID_ELEMENT) {
            element_section = mod.sections[i].element_section;
        }
    }
    uint64_t num_funcs = code_section.num_entries;
    for (size_t j = 0; j < import_section.num_entries; j++) {
        web49_section_import_entry_t entry = import_section.entries[j];
        if (entry.kind == WEB49_EXTERNAL_KIND_FUNCTION) {
            num_funcs += 1;
        }
    }
    uint64_t cur_func = 0;
    web49_interp_block_t **blocks = web49_malloc(sizeof(web49_interp_block_t *) * num_funcs);
    web49_section_type_entry_t *ftypes = web49_malloc(sizeof(web49_section_type_entry_t) * num_funcs);
    web49_interp_data_t *globals = web49_malloc(sizeof(web49_interp_data_t) * (global_section.num_entries));
    web49_interp_data_t *stack = web49_malloc(sizeof(web49_interp_data_t) * (1 << 16));
    uint8_t *memory = web49_malloc(1 << 24);
    memset(memory, 0, 1<<24);
    web49_interp_t interp = (web49_interp_t){
        .locals = stack,
        .stack = stack,
        .funcs = blocks,
        .ftypes = ftypes,
        .globals = globals,
        .memory = memory,
    };
    for (size_t j = 0; j < table_section.num_entries; j++) {
        web49_type_table_t entry = table_section.entries[j];
        if (entry.element_type == WEB49_TYPE_ANYFUNC) {
            interp.table = web49_malloc(sizeof(web49_interp_block_t *) * entry.limits.maximum);
        }
    }
    for (size_t j = 0; j < import_section.num_entries; j++) {
        web49_section_import_entry_t entry = import_section.entries[j];
        if (entry.kind == WEB49_EXTERNAL_KIND_FUNCTION) {
            ftypes[cur_func] = type_section.entries[entry.func_type.data];
            web49_interp_block_t *block= web49_interp_import(&interp, entry.module_str, entry.field_str);
            blocks[cur_func] = block;
            cur_func += 1;
        }
    }
    for (size_t j = 0; j < global_section.num_entries; j++) {
        web49_section_global_entry_t global = global_section.entries[j];
        if (global.init_expr.opcode == WEB49_OPCODE_I32_CONST) {
            globals[j].i32_s = global.init_expr.immediate.varint32;
        } else {
            globals[j].i64_s = global.init_expr.immediate.varint64;
        }
    }
    for (size_t j = 0; j < data_section.num_entries; j++) {
        web49_section_data_entry_t entry = data_section.entries[j];
        for (uint64_t i = 0; i < entry.size; i++) {
            memory[entry.offset.immediate.varint32+i] = entry.data[i];
        }
    }
    for (size_t j = 0; j < code_section.num_entries; j++) {
        web49_section_code_entry_t entry = code_section.entries[j];
        web49_interp_instr_buf_t buf;
        buf.head = 0;
        buf.len = entry.num_instrs;
        buf.instrs = entry.instrs;
        web49_interp_block_t *block = web49_interp_read_block(&interp, &buf);
        block->nlocals = entry.num_locals;
        block->nparams = type_section.entries[function_section.entries[j]].num_params;
        block->nreturns = type_section.entries[function_section.entries[j]].num_returns;
        blocks[cur_func] = block;
        cur_func += 1;
    }
    for (size_t j = 0; j < element_section.num_entries; j++) {
        web49_section_element_entry_t entry = element_section.entries[j];
        for (uint64_t i = 0; i < entry.num_elems; i++) {
            interp.table[i+entry.offset.immediate.varint32] = interp.funcs[entry.elems[i]];
        }
    }
    web49_interp_block_run(interp, blocks[start]);
}
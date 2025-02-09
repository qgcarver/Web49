#if !defined(WEB49_HEADER_INTERP_INTERP)
#define WEB49_HEADER_INTERP_INTERP

#include "../ast.h"

typedef uint64_t web49_interp_instr_t;

enum web49_interp_instr_enum_t {
    WEB49_MIN_OPCODE_INTERP = WEB49_MAX_OPCODE_NUM,
    WEB49_OPCODE_RETURN0,
    WEB49_OPCODE_FFI_CALL,
    WEB49_OPCODE_IF_I32_EQ,
    WEB49_OPCODE_IF_I32_LT_S,
    WEB49_OPCODE_IF_I32_LT_U,
    WEB49_OPCODE_IF_I32_GT_S,
    WEB49_OPCODE_IF_I32_GT_U,
    WEB49_OPCODE_IF_I64_EQ,
    WEB49_OPCODE_IF_I64_LT_S,
    WEB49_OPCODE_IF_I64_LT_U,
    WEB49_OPCODE_IF_I64_GT_S,
    WEB49_OPCODE_IF_I64_GT_U,
    WEB49_MAX_OPCODE_INTERP,
#if defined(WEB49_OPT_CONST0)
    WEB49_OPCODE_WITH_CONST0 = WEB49_MAX_OPCODE_INTERP * 1,
    WEB49_OPCODE_WITH_CONST1 = WEB49_MAX_OPCODE_INTERP * 2,
    WEB49_MAX_OPCODE_INTERP_NUM = WEB49_MAX_OPCODE_INTERP * 4,
#else
    WEB49_OPCODE_WITH_CONST1 = WEB49_MAX_OPCODE_INTERP * 1,
    WEB49_MAX_OPCODE_INTERP_NUM = WEB49_MAX_OPCODE_INTERP * 2,
#endif
};

union web49_interp_data_t;
typedef union web49_interp_data_t web49_interp_data_t;

struct web49_interp_t;
typedef struct web49_interp_t web49_interp_t;

union web49_interp_opcode_t;
typedef union web49_interp_opcode_t web49_interp_opcode_t;

struct web49_interp_block_t;
typedef struct web49_interp_block_t web49_interp_block_t;

struct web49_interp_instr_buf_t;
typedef struct web49_interp_instr_buf_t web49_interp_instr_buf_t;

struct web49_read_block_state_t;
typedef struct web49_read_block_state_t web49_read_block_state_t;

struct web49_interp_build_t;
typedef struct web49_interp_build_t web49_interp_build_t;

struct web49_interp_link_t;
typedef struct web49_interp_link_t web49_interp_link_t;

struct web49_interp_link_t {
    uint32_t *box;
    uint32_t out;
};

struct web49_interp_build_t {
    uint64_t alloc;
    web49_interp_opcode_t *code;
    uint64_t ncode;
};

union web49_interp_data_t {
    int64_t i64_s;
    int32_t i32_s;

    uint64_t i64_u;
    uint32_t i32_u;

    float f32;
    double f64;
};

typedef web49_interp_data_t (*web49_env_func_t)(web49_interp_t interp);
typedef web49_env_func_t (*web49_env_table_t)(void *state, const char *mod, const char *sym);

struct web49_interp_t {
    web49_interp_data_t *restrict locals;
    uint8_t *restrict memory;
    web49_interp_data_t *restrict globals;
    web49_interp_block_t **table;
    web49_interp_block_t *funcs;
    const char **args;
    void *import_state;
    uint64_t memsize;
    web49_env_table_t import_func;
    void *locals_base;
    web49_interp_data_t **restrict stacks;
    web49_interp_opcode_t **restrict returns;
    web49_interp_data_t *yield_base;
};

union web49_interp_opcode_t {
    void *opcode;
    void *ptr;
    web49_interp_data_t data;
    web49_interp_block_t *block;
    size_t link;
};

struct web49_interp_block_t {
    web49_interp_opcode_t *code;
    uint32_t nlocals: 32;
    uint16_t nparams: 16;
    uint16_t nreturns: 15;
    bool is_code: 1;
    union {
        struct {
            web49_instr_t *instrs;
            uint32_t num_instrs;
        };
        struct {
            const char *module_str;
            const char *field_str;
        };
    };
};

struct web49_read_block_state_t {
    void *ptrs;
    web49_interp_t *interp;
    web49_interp_build_t build;
    web49_interp_link_t *links;
    uint32_t **bufs_base;
    uint32_t bufs_alloc;
    uint32_t bufs_head;
    uint32_t depth;
    uint32_t nlocals;
    uint32_t alloc_links;
    uint32_t nlinks;
};

uint32_t *web49_interp_link_box(void);
void web49_interp_link_get(web49_read_block_state_t *state, uint32_t out, uint32_t *from);
web49_interp_t web49_interp_module(web49_module_t mod, const char **args);
web49_interp_data_t *web49_interp_block_run(web49_interp_t *ptr_interp, web49_interp_block_t *block);
uint32_t web49_interp_read_instr(web49_read_block_state_t *state, web49_instr_t cur, uint32_t local);

void web49_free_interp(web49_interp_t interp);

#if defined(WEB49_NO_BOUNDS)
#define WEB49_INTERP_BOUNDS(low, add) (__builtin_unreachable())
#else
#define WEB49_INTERP_BOUNDS(low, add) ({ fprintf(stderr, "memmory access 0x%zx of size 0x%zx out of bounds\n", (size_t) (low), (size_t) (add)); __builtin_trap(); })
#endif
#define WEB49_INTERP_ADDR(ptrtype, interp, dest, size) ({uint32_t xptr_ = (dest); web49_interp_t sub_ = (interp); if (sub_.memsize < xptr_ + size) { WEB49_INTERP_BOUNDS(xptr_, size); }; (ptrtype) &sub_.memory[xptr_]; })
#define WEB49_INTERP_READ(elemtype, interp, dest) (*WEB49_INTERP_ADDR(elemtype *, interp, dest, sizeof(elemtype)))
#define WEB49_INTERP_WRITE(elemtype, interp, dest, src) (*WEB49_INTERP_ADDR(elemtype *, interp, dest, sizeof(elemtype)) = (src))

#endif

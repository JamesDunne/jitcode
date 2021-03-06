#include <stdlib.h>
#include <stdint.h>
#include "myjit/jitlib.h"

typedef struct {
    unsigned short code;
    unsigned char spec;
    jit_value arg1;
    jit_value arg2;
    jit_value arg3;
    unsigned char arg_size;
} jit_bin_op_t;

#define C_IR(n) ((n << 4) | (JIT_RTYPE_INT << 1) | JIT_RTYPE_REG)
#define C_FR(n) ((n << 4) | (JIT_RTYPE_FLOAT << 1) | JIT_RTYPE_REG)

// Define our program in serialized format for testing:
const jit_bin_op_t code[] = {
    { JIT_DECL_ARG,     SPEC(IMM, IMM, NO),     JIT_SIGNED_NUM, sizeof(int64_t),    0,              0 },
    { JIT_GETARG,       SPEC(TREG, IMM, NO),    C_IR( 0),       (jit_value)(0),     0,              0 },
    { JIT_ADD | IMM,    SPEC(TREG, REG, IMM),   C_IR( 1),       C_IR( 0),           (jit_value)1,   0 },
    { JIT_RET | REG,    SPEC(REG, NO, NO),      C_IR( 1),       0,                  0,              0 },
    { 0xFFFF,           0,                      0,              0,                  0,              0 },
};
const int code_length = sizeof(code) / sizeof(code[0]);

typedef int64_t (*func_t)(int64_t);

void *jit_bin_load_func(struct jit *ctx, const jit_bin_op_t opcodes[], int *op)
{
    // Create the function prolog:
    void *func;
    jit_prolog(ctx, &func);

    // Enter in the function opcodes from the data:
    for (; ; (*op)++)
    {
        int i = *op;
        if (opcodes[i].code == 0xFFFF || opcodes[i].code == JIT_CODESTART)
            break;
        jit_add_op(ctx, opcodes[i].code, opcodes[i].spec, opcodes[i].arg1, opcodes[i].arg2, opcodes[i].arg3, opcodes[i].arg_size);
    }

    // Compile the function and copy the native machine code to `func`:
    jit_generate_code(ctx);

    return func;
}

int main(void)
{
    struct jit *ctx = jit_init();

    int op = 0;
    func_t func = (func_t) jit_bin_load_func(ctx, code, &op);
    printf("%d opcodes\n", op);

    int64_t b = func(1);
    printf("%lu\n", b);

    jit_free(ctx);
    return 0;
}

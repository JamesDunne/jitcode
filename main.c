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
};
const int code_length = sizeof(code) / sizeof(code[0]);

typedef int64_t (*func_t)(int64_t);

int main(void)
{
    struct jit *ctx = jit_init();

    // Create the function prolog:
    func_t func;
    jit_prolog(ctx, &func);

    // Enter in the function opcodes from the data:
    for (int i = 0; i < code_length; ++i)
    {
        jit_add_op(ctx, code[i].code, code[i].spec, code[i].arg1, code[i].arg2, code[i].arg3, code[i].arg_size);
    }

    jit_generate_code(ctx);

    int64_t b = func(1);
    printf("%lu\n", b);

    jit_free(ctx);
    return 0;
}
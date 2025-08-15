#include <sclsh/commands.h>
#include <sclsh/sclsh.h>
#include <sclsh/expr.h>
#include "value.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static SclshValue* cmd_exit(SclshContext* ctx, size_t argc, SclshValue** argv, void* user_data) {
    // Exit command implementation
    (void)ctx; (void)argc; (void)argv; (void)user_data; // Suppress unused parameter warnings
    exit(0);
    return NULL; // No return value needed for exit command
}

static SclshValue* cmd_puts(SclshContext* ctx, size_t argc, SclshValue** argv, void* user_data) {
    // Puts command implementation
    (void)ctx; (void)user_data; // Suppress unused parameter warnings
    if (argc < 1) {
        fprintf(stderr, "Usage: puts <string>\n");
        return NULL;
    }
    for (size_t i = 0; i < argc; i++) {
        SclshStringBuffer str_buf = sclsh_value_as_string(argv[i]);
        printf("%.*s", (int)str_buf.length, str_buf.string);
    }
    printf("\n");
    return sclsh_value_new("", 0);
}
static SclshValue* cmd_expr(SclshContext* ctx, size_t argc, SclshValue** argv, void* user_data) {
    // Placeholder for expression evaluation command
    (void)user_data;  // Suppress unused parameter warning
    if (argc < 1) {
        fprintf(stderr, "Usage: expr <expression>\n");
        return NULL;
    }
    SclshValue* expr = sclsh_value_new_from_list(argv, argc);
    SclshValue* res = sclsh_expr_eval(ctx, expr);
    if (!res) {
        fprintf(stderr, "Expression evaluation failed\n");
        return NULL;
    }
    sclsh_value_unref(expr);
    return res;
}

static SclshValue* cmd_set(SclshContext* ctx, size_t argc, SclshValue** argv, void* user_data) {
    // Set command implementation
    (void)user_data; // Suppress unused parameter warning
    if (argc != 2) {
        fprintf(stderr, "Usage: set <variable> <value>\n");
        return NULL;
    }
    SclshStringBuffer var_name_buf = sclsh_value_as_string(argv[0]);
    sclsh_context_set_variable(ctx, var_name_buf.string, argv[1]);
    return argv[1]; // No return value needed for set command
}

void sclsh_register_core_commands(SclshInterpreter* interp) {
    if (!interp) {
        return;  // Interpreter must not be NULL
    }

    sclsh_command_new(interp, "exit", cmd_exit, NULL, NULL);
    sclsh_command_new(interp, "puts", cmd_puts, NULL, NULL);
    sclsh_command_new(interp, "expr", cmd_expr, NULL, NULL);
    sclsh_command_new(interp, "set", cmd_set, NULL, NULL);
}

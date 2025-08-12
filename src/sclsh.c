/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

 #include <sclsh/sclsh.h>
 #include <sclsh/ast.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

 struct SclshInterpreter_s {
    SclshContext* global_context;  // Global context for the interpreter
 };

SclshInterpreter* sclsh_create_interpreter(void) {
    SclshInterpreter* interp = malloc(sizeof(SclshInterpreter));
    if (!interp) return NULL;

    interp->global_context = sclsh_create_context(interp);
    if (!interp->global_context) {
        free(interp);
        return NULL;
    }

    return interp;
}

void sclsh_destroy_interpreter(SclshInterpreter* interp) {
    if (interp) {
        sclsh_destroy_context(interp->global_context);
        free(interp);
    }
}
SclshContext* sclsh_global_context(SclshInterpreter* interp) {
    if (interp) {
        return interp->global_context;
    }
    return NULL;
}

struct SclshContext_s {
    SclshInterpreter* interp;  // Pointer to the interpreter
};

SclshContext* sclsh_create_context(SclshInterpreter* interp) {
    SclshContext* ctx = malloc(sizeof(SclshContext));
    if (!ctx) return NULL;

    ctx->interp = interp;

    return ctx;
}
void sclsh_destroy_context(SclshContext* ctx) {
    if (!ctx) {
        return;
    }
    free(ctx);
}

SclshValue* sclsh_eval(SclshContext* ctx, SclshValue* ast) {
    if (!ctx || !ast) {
        return NULL;
    }

    SclshNodeList* node_list = sclsh_value_as_command_line(ast);
    if (!node_list) {
        return NULL;  // Failed to parse as command line
    }

    for (size_t i = 0; i < node_list->count; i++) {
        SclshNode* node = &node_list->nodes[i];
        fprintf(stderr, "# node %zu: type=%s, value=%p\n", i, sclsh_node_type_to_string(node->type), node->value);
        SclshStringBuffer str_buf = sclsh_value_as_string(node->value);
        if (str_buf.string) {
            fprintf(stderr, "## length: %zu, string: %.*s\n", str_buf.length, (int)str_buf.length, str_buf.string);
            free(str_buf.string);  // Free the string buffer after use
        } else {
            fprintf(stderr, "## length: 0, string: NULL\n");
        }
    }

    return sclsh_value_ref(ast);
}
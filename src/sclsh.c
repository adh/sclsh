/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: MIT
 */

#include <sclsh/sclsh.h>
#include <sclsh/ast.h>
#include <sclsh/util.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct SclshInterpreter_s {
    SclshContext* global_context;
    SclshHashMap* commands;
};

struct SclshCommand_s {
    char* name;  // Command name
    SclshCommandFunc func;  // Function to execute the command
    void* user_data;  // User data for the command
    SclshUserDataDestructor* user_data_destructor;  // Destructor for user data
};

SclshInterpreter* sclsh_create_interpreter(void) {
    SclshInterpreter* interp = malloc(sizeof(SclshInterpreter));
    if (!interp) return NULL;

    interp->commands = sclsh_hash_map_new();
    interp->global_context = sclsh_create_context(interp);
    if (!interp->global_context) {
        free(interp);
        return NULL;
    }

    return interp;
}

void free_command(const char* key, void* value, void* user_data) {
    SclshCommand* command = (SclshCommand*)value;
    if (command->user_data_destructor) {
        command->user_data_destructor(command->user_data);
    }
    free(command->name);
    free(command);
}

void sclsh_destroy_interpreter(SclshInterpreter* interp) {
    if (interp) {
        sclsh_destroy_context(interp->global_context);
        sclsh_hash_map_for_each(interp->commands, free_command, NULL);
        sclsh_hash_map_free(interp->commands);
        free(interp);
    }
}

SclshCommand* sclsh_command_new(
    SclshInterpreter* interp, 
    char* name, 
    SclshCommandFunc func,
    void* user_data,
    SclshUserDataDestructor* user_data_destructor
) {
    if (!interp || !name || !func) {
        return NULL;
    }

    SclshCommand* command = malloc(sizeof(SclshCommand));
    if (!command) {
        return NULL;
    }

    command->name = strdup(name);
    command->func = func;
    command->user_data = user_data;
    command->user_data_destructor = user_data_destructor;

    sclsh_hash_map_set(interp->commands, command->name, command);
    return command;
}

SclshCommand* sclsh_get_command(SclshInterpreter* interp, const char* name) {
    if (!interp || !name) {
        return NULL;
    }
    return (SclshCommand*)sclsh_hash_map_get(interp->commands, name);
}

SclshContext* sclsh_global_context(SclshInterpreter* interp) {
    if (interp) {
        return interp->global_context;
    }
    return NULL;
}

struct SclshContext_s {
    SclshInterpreter* interp;  // Pointer to the interpreter
    SclshHashMap* variables;  // Hash map for storing variables
};

SclshContext* sclsh_create_context(SclshInterpreter* interp) {
    SclshContext* ctx = malloc(sizeof(SclshContext));
    if (!ctx) return NULL;

    ctx->variables = sclsh_hash_map_new();
    if (!ctx->variables) {
        free(ctx);
        return NULL;
    }

    ctx->interp = interp;

    return ctx;
}
static void free_variable(const char* key, void* value, void* user_data) {
    SclshValue* var_value = (SclshValue*)value;
    sclsh_value_unref(var_value);
}
void sclsh_destroy_context(SclshContext* ctx) {
    if (!ctx) {
        return;
    }
    sclsh_hash_map_for_each(ctx->variables, free_variable, NULL);
    sclsh_hash_map_free(ctx->variables);
    free(ctx);
}

void sclsh_context_set_variable(SclshContext* ctx, const char* name, SclshValue* value) {
    if (!ctx || !name || !value) {
        return;
    }
    sclsh_value_ref(value);  // Increment reference count
    sclsh_hash_map_set(ctx->variables, name, value);
}
SclshValue* sclsh_context_get_variable(SclshContext* ctx, const char* name) {
    if (!ctx || !name) {
        return NULL;
    }
    return (SclshValue*)sclsh_hash_map_get(ctx->variables, name);
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
/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: MIT
 */

#include <sclsh/sclsh.h>
#include <sclsh/parse.h>
#include <sclsh/util.h>
#include <sclsh/commands.h>
#include <stdio.h>
#include <editline/readline.h>

int main(int argc, char* argv[]) {
    SclshInterpreter* interp = sclsh_create_interpreter();
    if (!interp) {
        fprintf(stderr, "Failed to create interpreter\n");
        return 1;
    }

    sclsh_register_core_commands(interp);

    SclshContext* ctx = sclsh_global_context(interp);
    if (!ctx) {
        fprintf(stderr, "Failed to create global context\n");
        sclsh_destroy_interpreter(interp);
        return 1;
    }

    char* line;
    while ((line = readline("sclsh> ")) != NULL) {
        if (*line) {
            add_history(line);
            if (!sclsh_command_line_complete(SCLSH_STRING_BUFFER(line))) {
                fprintf(stderr, "Incomplete command\n");
            } else {
                SclshValue* input = sclsh_value_from_cstr(line);
                if (!input) {
                    fprintf(stderr, "Failed to create value from input\n");
                    exit(EXIT_FAILURE);
                }
                SclshValue* result = sclsh_eval(ctx, input);
                sclsh_value_unref(input);
                if (result) {
                    SclshStringBuffer str_buf = sclsh_value_as_string(result);
                    printf("Result: %.*s\n", (int)str_buf.length, str_buf.string);
                    sclsh_value_unref(result);
                } else {
                    fprintf(stderr, "Evaluation failed\n");
                }
            }
        }
        free(line);
    }

    sclsh_destroy_interpreter(interp);
    return 0;
}
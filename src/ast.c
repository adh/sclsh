/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <sclsh/ast.h>
#include "value.h"

SclshCommandLine* sclsh_value_as_command_line(SclshValue* value) {
    if (value && value->as_command_line) {
        return value->as_command_line;
    }
    return NULL;
}

void sclsh_command_line_free(SclshCommandLine* cmd_line) {
    if (!cmd_line) {
        return;
    }
    for (size_t i = 0; i < cmd_line->count; i++) {
        SclshCommandWord* word = &cmd_line->words[i];
        if (word->value) {
            sclsh_value_unref(word->value);
        }
    }
    free(cmd_line);
}

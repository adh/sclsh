/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <sclsh/ast.h>
#include "value.h"

SclshNodeList* sclsh_value_as_command_line(SclshValue* value) {
    if (value && value->as_command_line) {
        return value->as_command_line;
    }
    return NULL;
}

void sclsh_node_list_free(SclshNodeList* node_list) {
    if (!node_list) {
        return;
    }
    for (size_t i = 0; i < node_list->count; i++) {
        SclshNode* node = &node_list->nodes[i];
        if (node->value) {
            sclsh_value_unref(node->value);
        }
    }
    free(node_list);
}

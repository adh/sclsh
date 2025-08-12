/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <sclsh/value.h>
#include <sclsh/ast.h>
#include "value.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>


SclshValue* sclsh_value_new(const char* string, 
                            size_t length) {
    SclshValue* value = malloc(sizeof(SclshValue));
    if (!value) return NULL;

    value->ref_count = 1;
    assert(value->string = malloc(length + 1));
    strncpy(value->string, string, length);
    value->string[length] = '\0';
    value->length = length;

    return value;
}

SclshValue* sclsh_value_ref(SclshValue* value) {
    if (value) {
        value->ref_count++;
    }
    return value;
}

static void value_free(SclshValue* value) {
    if (!value) {
        return;
    }
    if (value->string) {
        free(value->string);
    }
    if (value->as_list) {
        sclsh_value_list_free(value->as_list);
    }
    if (value->as_proc) {
        sclsh_value_list_free(value->as_proc);
    }
    if (value->as_command_line) {
        sclsh_node_list_free(value->as_command_line);
    }
    if (value->as_interpolation) {
        sclsh_node_list_free(value->as_interpolation);
    }
    free(value);
}

void sclsh_value_unref(SclshValue* value) {
    if (value && --value->ref_count == 0) {
        value_free(value);
    }
}

void sclsh_value_list_free(SclshValueList* list) {
    if (!list) {
        return;
    }

    for (size_t i = 0; i < list->count; i++) {
        sclsh_value_unref(list->items[i]);
    }
    free(list);
}
/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <sclsh/value.h>
#include <sclsh/ast.h>
#include <stdlib.h>
#include <string.h>

SclshValue* sclsh_value_new(SclshValueType type, 
                            const char* string, 
                            size_t length,
                            void* data) {
    SclshValue* value = malloc(sizeof(SclshValue));
    if (!value) return NULL;

    value->ref_count = 1;
    value->type = type;

    switch (type) {
        case SCLSH_VALUE_TYPE_BYTES:
            value->length = length;
            value->string = malloc(length);
            if (!value->string) {
                free(value);
                return NULL;
            }
            memcpy(value->string, string, length);
            value->data = data;
            break;

        case SCLSH_VALUE_TYPE_LIST:
            value->data = data;
            break;

        case SCLSH_VALUE_TYPE_AST_NODE:
            value->data = data;
            break;

        default:
            free(value);
            return NULL;
    }

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
    if (value->data) {
        switch (value->type) {
            case SCLSH_VALUE_TYPE_LIST: {
                SclshListData* list_data = (SclshListData*)value->data;
                for (size_t i = 0; i < list_data->count; i++) {
                    sclsh_value_unref(list_data->items[i]);
                }
                break;
            }
            case SCLSH_VALUE_TYPE_AST_NODE:
                SclshAstNodeData* ast_data = (SclshAstNodeData*)value->data;
                sclsh_value_unref(ast_data->value);
                break;
            default:
                break;
        }
        free(value->data);
    }   
    free(value);
}

void sclsh_value_unref(SclshValue* value) {
    if (value && --value->ref_count == 0) {
        value_free(value);
    }
}

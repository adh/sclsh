/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: MIT
 */

#include <sclsh/ast.h>
#include <sclsh/parse.h>
#include "value.h"

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

SclshNodeList* sclsh_value_as_interpolation(SclshValue* value) {
    if (!value){
        return NULL;
    }
    if (!value->as_interpolation) {
        value->as_interpolation = sclsh_parse_interpolation(sclsh_value_as_string(value));
        if (!value->as_interpolation) {
            return NULL;  // Failed to parse as interpolation
        }
    }
    return value->as_interpolation;
}
SclshNodeList* sclsh_value_as_command_line(SclshValue* value) {
    if (!value) {
        return NULL;
    }
    if (!value->as_command_line) {
        value->as_command_line = sclsh_parse_command_line(sclsh_value_as_string(value));
        if (!value->as_command_line) {
            return NULL;  // Failed to parse as command line
        }
    }
    return value->as_command_line;
}

struct SclshNodeListBuilder_s {
    size_t count;  // Number of nodes in the builder
    size_t capacity;  // Capacity of the nodes array
    SclshNode* nodes;  // Array of nodes
};
SclshNodeListBuilder* sclsh_node_list_builder_new(void) {
    SclshNodeListBuilder* builder = malloc(sizeof(SclshNodeListBuilder));
    if (!builder) return NULL;

    builder->count = 0;
    builder->capacity = 8;  // Initial capacity
    builder->nodes = malloc(sizeof(SclshNode) * builder->capacity);
    if (!builder->nodes) {
        free(builder);
        return NULL;
    }

    return builder;
}
void sclsh_node_list_builder_free(SclshNodeListBuilder* builder) {
    if (!builder) {
        return;
    }
    for (size_t i = 0; i < builder->count; i++) {
        if (builder->nodes[i].value) {
            sclsh_value_unref(builder->nodes[i].value);
        }
    }

    free(builder->nodes);
    free(builder);
}
void sclsh_node_list_builder_append(
    SclshNodeListBuilder* builder,
    SclshValue* value,
    SclshNodeType type
) {
    if (!builder || !value) {
        return;
    }

    if (builder->count >= builder->capacity) {
        builder->capacity *= 2;
        builder->nodes = realloc(builder->nodes, sizeof(SclshNode) * builder->capacity);
        if (!builder->nodes) {
            return;  // Memory allocation failed
        }
    }

    builder->nodes[builder->count].type = type;
    builder->nodes[builder->count].value = sclsh_value_ref(value);
    builder->count++;
}

SclshNodeList* sclsh_node_list_builder_value(SclshNodeListBuilder* builder) {
    SclshNodeList* list = malloc(sizeof(SclshNodeList) + sizeof(SclshNode) * (builder->count));
    if (!list) {
        return NULL;
    }

    list->count = builder->count;

    for (size_t i = 0; i < builder->count; i++) {
        list->nodes[i] = builder->nodes[i];
        sclsh_value_ref(list->nodes[i].value);  // Increase ref count
    }

    return list;
}

char* sclsh_node_type_to_string(SclshNodeType type) {
    switch (type) {
        case SCLSH_WORD_BARE: return "bare";
        case SCLSH_WORD_QUOTED: return "quoted";
        case SCLSH_WORD_BRACE: return "brace";
        case SCLSH_WORD_BRACKET: return "bracket";
        case SCLSH_WORD_VARIABLE: return "variable";
        default: return "unknown";
    }
}
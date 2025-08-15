/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: MIT
 */

#include <sclsh/value.h>
#include <sclsh/ast.h>
#include <sclsh/util.h>
#include <sclsh/parse.h>
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

SclshValue* sclsh_value_from_cstr(const char* str) {
    return sclsh_value_new(str, strlen(str));
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

SclshValueList* sclsh_value_as_list(SclshValue* value) {
    if (!value) {
        return NULL;
    }
    if (!value->as_list) {
        value->as_list = sclsh_parse_list(sclsh_value_as_string(value));
        if (!value->as_list) {
            return NULL;  // Failed to parse as list
        }

    }
    return value->as_list;
}   

SclshValueList* sclsh_value_as_proc(SclshValue* value) {
    if (!value) {
        return NULL;
    }
    if (!value->as_proc) {
        value->as_proc = sclsh_parse_commands(sclsh_value_as_string(value));
        if (!value->as_proc) {
            return NULL;  // Failed to parse as procedure
        }
    }
    return value->as_proc;
}
SclshStringBuffer sclsh_value_as_string(SclshValue* value) {
    SclshStringBuffer buffer = { .string = NULL, .length = 0 };
    if (!value || !value->string) {
        return buffer;  // Empty value
    }

    buffer.string = malloc(value->length + 1);
    if (!buffer.string) {
        return buffer;  // Memory allocation failed
    }
    memcpy(buffer.string, value->string, value->length);
    buffer.string[value->length] = '\0';  // Null-terminate
    buffer.length = value->length;

    return buffer;
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

typedef struct ListBuilderEntry_s {
    SclshValue* value;
    struct ListBuilderEntry_s* next;
} ListBuilderEntry;

struct SclshListBuilder_s {
    size_t count;  // Number of items in the builder
    ListBuilderEntry* head;
    ListBuilderEntry* tail;
};

SclshListBuilder* sclsh_list_builder_new(void) {
    SclshListBuilder* builder = malloc(sizeof(SclshListBuilder));
    if (!builder) return NULL;

    builder->count = 0;
    builder->head = NULL;
    builder->tail = NULL;

    return builder;
}
void sclsh_list_builder_free(SclshListBuilder* builder) {
    if (!builder) {
        return;
    }

    ListBuilderEntry* entry = builder->head;
    while (entry) {
        ListBuilderEntry* next = entry->next;
        sclsh_value_unref(entry->value);
        free(entry);
        entry = next;
    }
    free(builder);
}
void sclsh_list_builder_append(SclshListBuilder* builder, SclshValue* value) {
    if (!builder || !value) {
        return;
    }

    ListBuilderEntry* entry = malloc(sizeof(ListBuilderEntry));
    if (!entry) return;

    entry->value = sclsh_value_ref(value);
    entry->next = NULL;

    if (builder->tail) {
        builder->tail->next = entry;
    } else {
        builder->head = entry;
    }
    builder->tail = entry;
    builder->count++;
}
SclshValueList* sclsh_list_builder_value_list(SclshListBuilder* builder) {
    if (!builder || builder->count == 0) {
        SclshValueList* empty_list = malloc(sizeof(SclshValueList));
        if (!empty_list) return NULL;
        empty_list->count = 0;
        return empty_list;
    }

    SclshValueList* list = malloc(sizeof(SclshValueList) + sizeof(SclshValue*) * (builder->count - 1));
    if (!list) return NULL;

    list->count = builder->count;
    ListBuilderEntry* entry = builder->head;
    size_t index = 0;

    while (entry) {
        sclsh_value_ref(entry->value);  // Increase ref count for each value
        list->items[index++] = entry->value;
        entry = entry->next;
    }

    return list;
}

SclshStringBuffer value_list_to_string(SclshValueList* list) {
    SclshStringBuffer buffer = { .string = NULL, .length = 0 };
    if (!list || list->count == 0) {
        return buffer;  // Empty list
    }

    SclshStringBuilder* sb = sclsh_string_builder_new();
    if (!sb) return buffer;

    for (size_t i = 0; i < list->count; i++) {
        SclshValue* value = list->items[i];
        sclsh_string_builder_append_str(sb, "{");
        sclsh_string_builder_append_bytes(sb, value->string, value->length);
        sclsh_string_builder_append_str(sb, "}");
        if (i < list->count - 1) {
            sclsh_string_builder_append_str(sb, " ");  // Add space between items
        }
    }

    buffer = sclsh_string_builder_value(sb);
    sclsh_string_builder_free(sb);
    return buffer;
}

SclshValue* sclsh_list_builder_value(SclshListBuilder* builder) {
    SclshValueList* list = sclsh_list_builder_value_list(builder);
    if (!list) return NULL;

    SclshValue* value = malloc(sizeof(SclshValue));
    if (!value) {
        sclsh_value_list_free(list);
        return NULL;
    }

    SclshStringBuffer buffer = value_list_to_string(list);

    value->ref_count = 1;
    value->string = buffer.string;
    value->length = buffer.length;
    value->as_list = list;
    value->as_proc = NULL;
    value->as_command_line = NULL;
    value->as_interpolation = NULL;

    return value;
}


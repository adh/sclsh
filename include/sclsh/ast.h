/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: MIT
 */

 #ifndef H__SCLSH__AST_H
#define H__SCLSH__AST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sclsh/value.h>

typedef enum {
    SCLSH_WORD_BARE,
    SCLSH_WORD_QUOTED,
    SCLSH_WORD_BRACE,
    SCLSH_WORD_BRACKET,
    SCLSH_WORD_VARIABLE,
} SclshNodeType;

typedef struct s_SclshNode_s {
    SclshNodeType type;  // Type of the AST node
    SclshValue* value;  // Associated value (e.g., command, expression)
} SclshNode;

typedef struct s_SclshNodeList_s {
    size_t count;
    SclshNode nodes[];
} SclshNodeList;

SclshNodeList* sclsh_value_as_command_line(SclshValue* value);
SclshNodeList* sclsh_value_as_interpolation(SclshValue* value);

void sclsh_node_list_free(SclshNodeList* node_list);

typedef struct SclshNodeListBuilder_s SclshNodeListBuilder;

SclshNodeListBuilder* sclsh_node_list_builder_new(void);
void sclsh_node_list_builder_free(SclshNodeListBuilder* builder);
void sclsh_node_list_builder_append(
    SclshNodeListBuilder* builder,
    SclshValue* value,
    SclshNodeType type
);
SclshNodeList* sclsh_node_list_builder_value(SclshNodeListBuilder* builder);

char* sclsh_node_type_to_string(SclshNodeType type);

#ifdef __cplusplus
}
#endif

#endif // H__SCLSH__AST_H

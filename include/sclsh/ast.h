/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

 #ifndef H__SCLSH__AST_H
#define H__SCLSH__AST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sclsh/value.h>

typedef enum {
    SCLSH_AST_NODE_TYPE_COMMAND,
    SCLSH_AST_NODE_TYPE_BRACE_EXPRESSION,
    SCLSH_AST_NODE_TYPE_PIPELINE,
    SCLSH_AST_NODE_TYPE_VARIABLE,
    SCLSH_AST_NODE_TYPE_REDIRECTION,
} SclshAstNodeType;

typedef struct s_SclshAstNodeData_s {
    SclshAstNodeType type;  // Type of the AST node
    SclshValue* value;  // Associated value (e.g., command, expression)
} SclshAstNodeData;


#ifdef __cplusplus
}
#endif

#endif // H__SCLSH__AST_H

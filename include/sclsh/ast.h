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
    SCLSH_WORD_BARE,
    SCLSH_WORD_QUOTED,
    SCLSH_WORD_BRACE,
    SCLSH_WORD_BRACKET,
    SCLSH_WORD_VARIABLE,
} SclshWordType;

typedef struct s_SclshCommandWord_s {
    SclshWordType type;  // Type of the AST node
    SclshValue* value;  // Associated value (e.g., command, expression)
} SclshCommandWord;

typedef struct SclshCommandLine_s {
    size_t count;
    SclshCommandWord words[];
} SclshCommandLine;

SclshCommandLine* sclsh_value_as_command_line(SclshValue* value);

void sclsh_command_line_free(SclshCommandLine* cmd_line);

#ifdef __cplusplus
}
#endif

#endif // H__SCLSH__AST_H

/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <sclsh/parse.h>
#include <ctype.h>

int skip_comments_and_whitespace(SclshStringBuffer* buffer, size_t* pos) {
    while (*pos < buffer->length) {
        char ch = buffer->string[*pos];
        if (ch == '#') {
            // Skip to end of line
            while (*pos < buffer->length && buffer->string[*pos] != '\n') {
                (*pos)++;
            }
        } else if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') {
            // Skip whitespace
            (*pos)++;
        } else {
            return 0; // Found non-whitespace character
        }
    }
    return -1; // Reached end of buffer
}

int skip_comments_and_whitespace_to_eol(SclshStringBuffer* buffer, size_t* pos) {
    while (*pos < buffer->length) {
        char ch = buffer->string[*pos];
        if (ch == '#') {
            // Skip to end of line
            while (*pos < buffer->length && buffer->string[*pos] != '\n') {
                (*pos)++;
            }
            return 1;
        } else if (ch == ' ' || ch == '\t') {
            // Skip whitespace
            (*pos)++;
        } else if (ch == '\n' || ch == '\r') {
            if (*pos > 0 && buffer->string[*pos - 1] == '\\') {
                // If the newline is escaped, just skip it
                (*pos)++;
            } else {
                return 1;
            }
        } else {
            return 1;
        }
    }
    return 1;
}

int skip_bare_word(SclshStringBuffer* buffer, size_t* pos) {
    while (*pos < buffer->length) {
        char ch = buffer->string[*pos];
        if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '#') {
            return 1; // Found whitespace or comment start
        }
        (*pos)++;
    }
    return 1; // Reached end of buffer
}

SclshValue* parse_bare_word(SclshStringBuffer* buffer, size_t* pos) {
    size_t start = *pos;
    if (!skip_bare_word(buffer, pos)) {
        return NULL; // Error or end of buffer
    }
    size_t length = *pos - start;
    if (length == 0) {
        return NULL; // No bare word found
    }
    return sclsh_value_new(buffer->string + start, length);
}

int skip_braces(SclshStringBuffer* buffer, size_t* pos) { 
    int brace_count = 1;
    int escape = 0;
    if (*pos >= buffer->length || buffer->string[*pos] != '{') {
        return 0; // Not a brace word
    }
    (*pos)++; // Skip the opening brace

    while (*pos < buffer->length && brace_count > 0) {
        char ch = buffer->string[*pos];
        if (ch == '{') {
            if (escape) {
                escape = 0; // Reset escape state
            } else {
                brace_count++;
            }
        } else if (ch == '}') {
            if (escape) {
                escape = 0; // Reset escape state
            } else if (brace_count > 0) {
                brace_count--;
            }
        } else if (ch == '\\') {
            escape = 1;
        } else {
            escape = 0; // Reset escape state for any other character
        }
        (*pos)++;
    }
    return brace_count == 0;
}

SclshValue* parse_brace_word(SclshStringBuffer* buffer, size_t* pos) {
    size_t start = *pos;
    if (!skip_braces(buffer, pos)) {
        return NULL; // Error or unmatched braces
    }
    size_t length = *pos - start;
    if (length == 0 || buffer->string[start] != '{' || buffer->string[*pos - 1] != '}') {
        return NULL; // Invalid brace word
    }
    return sclsh_value_new(buffer->string + start + 1, length - 2); // Exclude braces
}

int skip_brackets(SclshStringBuffer* buffer, size_t* pos) {
    int bracket_count = 1;
    int escape = 0;

    if (*pos >= buffer->length || buffer->string[*pos] != '[') {
        return 0; // Not a bracket word
    }
    (*pos)++; // Skip the opening bracket

    while (*pos < buffer->length && bracket_count > 0) {
        char ch = buffer->string[*pos];
        if (ch == '[') {
            if (escape) {
                escape = 0; // Reset escape state
            } else {
                bracket_count++;
            }
        } else if (ch == ']') {
            if (escape) {
                escape = 0; // Reset escape state
            } else if (bracket_count > 0) {
                bracket_count--;
            }
        } else if (ch == '\\') {
            escape = 1;
        } else {
            escape = 0; // Reset escape state for any other character
        }
        (*pos)++;
    }
    return bracket_count == 0;
}

SclshValue* parse_bracket_word(SclshStringBuffer* buffer, size_t* pos) {
    size_t start = *pos;
    if (!skip_brackets(buffer, pos)) {
        return NULL; // Error or unmatched brackets
    }
    size_t length = *pos - start;
    if (length == 0 || buffer->string[start] != '[' || buffer->string[*pos - 1] != ']') {
        return NULL; // Invalid bracket word
    }
    return sclsh_value_new(buffer->string + start + 1, length - 2); // Exclude brackets
}

SclshValue* parse_variable_word(SclshStringBuffer* buffer, size_t* pos) {
    if (*pos >= buffer->length || buffer->string[*pos] != '$') {
        return NULL; // Not a variable word
    }
    (*pos)++; // Skip the '$'
    
    size_t start = *pos;
    while (*pos < buffer->length && (isalnum(buffer->string[*pos]) || buffer->string[*pos] == '_')) {
        (*pos)++;
    }
    
    if (start == *pos) {
        return NULL; // No variable name found
    }
    
    return sclsh_value_new(buffer->string + start, *pos - start);
}

SclshValueList* sclsh_parse_list(SclshStringBuffer buffer) {
    SclshListBuilder* list = sclsh_list_builder_new();
    SclshValueList* res;

    size_t pos = 0;
    while (pos < buffer.length) {
        if (skip_comments_and_whitespace(&buffer, &pos) < 0) {
            break; // End of buffer or error
        }

        SclshValue* value = NULL;
        if (buffer.string[pos] == '{') {
            value = parse_brace_word(&buffer, &pos);
        } else {
            value = parse_bare_word(&buffer, &pos);
        }

        if (value) {
            sclsh_list_builder_append(list, value);
        }
    }

    res = sclsh_list_builder_value_list(list);
    sclsh_list_builder_free(list);
    return res;
}

SclshNodeList* sclsh_parse_command_line(SclshStringBuffer buffer) {
    SclshNodeListBuilder* builder = sclsh_node_list_builder_new();
    SclshNodeList* node_list = NULL;

    size_t pos = 0;
    while (pos < buffer.length) {
        if (skip_comments_and_whitespace_to_eol(&buffer, &pos) < 0) {
            break; // End of buffer or error
        }

        SclshValue* value = NULL;
        SclshNodeType type = SCLSH_WORD_BARE; // Default type
        if (buffer.string[pos] == '{') {
            value = parse_brace_word(&buffer, &pos);
            type = SCLSH_WORD_BRACE;
        } else if (buffer.string[pos] == '[') {
            value = parse_bracket_word(&buffer, &pos);
            type = SCLSH_WORD_BRACKET;
        } else if (buffer.string[pos] == '$') {
            value = parse_variable_word(&buffer, &pos);
            type = SCLSH_WORD_VARIABLE;
        } else {
            value = parse_bare_word(&buffer, &pos);
        }

        if (value) {
            sclsh_node_list_builder_append(builder, value, type);
        }
    }

    node_list = sclsh_node_list_builder_value(builder);
    sclsh_node_list_builder_free(builder);
    return node_list;
}

int skip_command_line(SclshStringBuffer* buffer, size_t* pos) {
    skip_comments_and_whitespace(buffer, pos);
    while (*pos < buffer->length) {
        if (!skip_comments_and_whitespace_to_eol(buffer, pos)) {
            return 1; // End of buffer
        }
        char ch = buffer->string[*pos];
        if (ch == '{' || ch == '[' || ch == '$') {
            // Skip to the end of the command line
            if (ch == '{') {
                if (!skip_braces(buffer, pos)) {
                    return 0; // Unmatched braces
                }
            } else if (ch == '[') {
                if (!skip_brackets(buffer, pos)) {
                    return 0; // Unmatched brackets
                }
            } else if (ch == '$') {
                // Skip variable word
                (*pos)++;
                while (*pos < buffer->length && (isalnum(buffer->string[*pos]) || buffer->string[*pos] == '_')) {
                    (*pos)++; // Skip variable name
                }
            }
        } else {
            // Skip bare word
            if (!skip_bare_word(buffer, pos)) {
                return 0; // Error or end of buffer
            }
        }
    }
    return 1; // Successfully skipped command line
}

SclshValueList* sclsh_parse_commands(SclshStringBuffer buffer) {
    SclshListBuilder* list = sclsh_list_builder_new();
    SclshValueList* res;

    size_t pos = 0;
    while (pos < buffer.length) {
        size_t start = pos;
        if (!skip_command_line(&buffer, &pos)) {
            break; // Error or end of buffer
        }
        size_t length = pos - start;
        if (length == 0) {
            continue; // No command found
        }
        SclshValue* value = sclsh_value_new(buffer.string + start, length);
        sclsh_list_builder_append(list, value);
    }

    res = sclsh_list_builder_value_list(list);
    sclsh_list_builder_free(list);
    return res;
}

int sclsh_command_line_complete(SclshStringBuffer buffer) {
    size_t pos = 0;
    while (pos < buffer.length) {
        if (!skip_command_line(&buffer, &pos)) {
            return 0; // Error or unmatched braces/brackets
        }
        if (!skip_comments_and_whitespace(&buffer, &pos)) {
            return 0; // End of buffer or error
        }
    }
    return 1;
}

SclshNodeList* sclsh_parse_interpolation(SclshStringBuffer buffer) {
    SclshNodeListBuilder* builder = sclsh_node_list_builder_new();
    SclshNodeList* node_list = NULL;

    // TODO
    sclsh_node_list_builder_append(
        builder, 
        sclsh_value_new(buffer.string, buffer.length), 
        SCLSH_WORD_BARE
    );
 
    node_list = sclsh_node_list_builder_value(builder);
    sclsh_node_list_builder_free(builder);
    return node_list;
}
#ifndef H__SCLSH__PARSE_H
#define H__SCLSH__PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sclsh/value.h>
#include <sclsh/ast.h>

#include <stdio.h>

typedef struct SclshParser_s SclshParser;

typedef int (*SclshParserCommandHandler)(
    SclshParser* parser,
    SclshValue* command,
    void* user_data
);

SclshParser* sclsh_parser_new(SclshValue* value);
void sclsh_parser_free(SclshParser* parser);
void sclsh_parser_set_command_handler(
    SclshParser* parser, 
    SclshParserCommandHandler command_handler,
    void* user_data
);

int sclsh_parser_parse(SclshParser* parser, SclshValue* value);
int sclsh_parser_parse_cstr(SclshParser* parser, const char* str);
int sclsh_parser_parse_file(SclshParser* parser, FILE* file);
int sclsh_parser_eof(SclshParser* parser);

SclshValueList* sclsh_parse_list(SclshValue* value);
SclshNodeList* sclsh_parse_command_line(SclshValue* value);
SclshNodeList* sclsh_parse_interpolation(SclshValue* value);

#ifdef __cplusplus
}
#endif

#endif // H__SCLSH__PARSE_H

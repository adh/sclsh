#ifndef H__SCLSH__PARSE_H
#define H__SCLSH__PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sclsh/value.h>
#include <sclsh/ast.h>
#include <sclsh/util.h>

#include <stdio.h>

SclshValueList* sclsh_parse_commands(SclshStringBuffer buffer);
SclshValueList* sclsh_parse_list(SclshStringBuffer buffer);
SclshNodeList* sclsh_parse_command_line(SclshStringBuffer buffer);
SclshNodeList* sclsh_parse_interpolation(SclshStringBuffer buffer);

int sclsh_command_line_complete(SclshStringBuffer buffer);

#ifdef __cplusplus
}
#endif

#endif // H__SCLSH__PARSE_H

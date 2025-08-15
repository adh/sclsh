/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef H__SCLSH__SCLSH_H
#define H__SCLSH__SCLSH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sclsh/value.h>

#include <stdlib.h>
#include <stdint.h>

typedef struct SclshInterpreter_s SclshInterpreter;

SclshInterpreter* sclsh_create_interpreter(void);
void sclsh_destroy_interpreter(SclshInterpreter* interp);

typedef struct SclshContext_s SclshContext;

SclshContext* sclsh_create_context(SclshInterpreter* interp);
void sclsh_destroy_context(SclshContext* ctx);
SclshContext* sclsh_global_context(SclshInterpreter* interp);

void sclsh_context_set_variable(SclshContext* ctx, const char* name, SclshValue* value);
SclshValue* sclsh_context_get_variable(SclshContext* ctx, const char* name);

typedef struct SclshCommand_s SclshCommand;
SclshCommand* sclsh_get_command(SclshInterpreter* interp, const char* name);

typedef SclshValue* (*SclshCommandFunc)(
    SclshContext* ctx, 
    size_t argc,
    SclshValue** argv,
    void *user_data
);
typedef void (SclshUserDataDestructor)(void* user_data);

SclshCommand* sclsh_command_new(
    SclshInterpreter* interp, 
    char* name, 
    SclshCommandFunc func,
    void* user_data,
    SclshUserDataDestructor* user_data_destructor
);

SclshValue* sclsh_eval(SclshContext* ctx, SclshValue* ast);

#ifdef __cplusplus
}   // extern "C"
#endif

#endif // H__SCLSH__SCLSH_H
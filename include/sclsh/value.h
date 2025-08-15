/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: MIT
 */

 #ifndef H__SCLSH__VALUE_H
#define H__SCLSH__VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sclsh/util.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct s_SclshValue SclshValue;
typedef struct s_SclshValueList SclshValueList;


SclshValue* sclsh_value_new(const char* string, 
                            size_t length);
SclshValue* sclsh_value_from_cstr(const char* str);
SclshValue* sclsh_value_ref(SclshValue* value);
void sclsh_value_unref(SclshValue* value);

SclshValueList* sclsh_value_as_list(SclshValue* value);
SclshValueList* sclsh_value_as_proc(SclshValue* value);

void sclsh_value_list_free(SclshValueList* list);

SclshStringBuffer sclsh_value_as_string(SclshValue* value);
SclshValue* sclsh_string_builder_to_value(SclshStringBuilder* sb);

typedef struct SclshListBuilder_s SclshListBuilder;

SclshListBuilder* sclsh_list_builder_new(void);
void sclsh_list_builder_free(SclshListBuilder* builder);
void sclsh_list_builder_append(SclshListBuilder* builder, SclshValue* value);
SclshValueList* sclsh_list_builder_value_list(SclshListBuilder* builder);
SclshValue* sclsh_list_builder_value(SclshListBuilder* builder);

#ifdef __cplusplus
}
#endif

#endif // H__SCLSH__VALUE_H

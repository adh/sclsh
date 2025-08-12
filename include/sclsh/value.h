/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

 #ifndef H__SCLSH__VALUE_H
#define H__SCLSH__VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct s_SclshValue SclshValue;
typedef struct s_SclshValueList SclshValueList;


SclshValue* sclsh_value_new(const char* string, 
                            size_t length,
                            void* data);
SclshValue* sclsh_value_ref(SclshValue* value);
void sclsh_value_unref(SclshValue* value);

SclshValueList* sclsh_value_as_list(SclshValue* value);
SclshValueList* sclsh_value_as_proc(SclshValue* value);

void sclsh_value_list_free(SclshValueList* list);

#ifdef __cplusplus
}
#endif

#endif // H__SCLSH__VALUE_H

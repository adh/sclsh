/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef H__SCLSH__UTIL_H
#define H__SCLSH__UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>

typedef struct SclshStringBuffer_s {
    char* string;
    size_t length;
} SclshStringBuffer;

extern uint32_t sclsh_fnv_hash(char* string);
extern uint32_t sclsh_pointer_hash(void* pointer);

typedef struct SclshStringBuilder_s SclshStringBuilder;
SclshStringBuilder* sclsh_string_builder_new(void);
void sclsh_string_builder_free(SclshStringBuilder* sb);

void sclsh_string_builder_append_str(SclshStringBuilder* sb, const char* str);
void sclsh_string_builder_append_bytes(SclshStringBuilder* sb, const char* bytes, size_t len);
void sclsh_string_builder_append_buffer(SclshStringBuilder* sb, const SclshStringBuffer buffer);

SclshStringBuffer sclsh_string_builder_value(SclshStringBuilder* sb);

typedef struct SclshHashMap_s SclshHashMap;

#ifdef __cplusplus
}
#endif

#endif // H__SCLSH__UTIL_H

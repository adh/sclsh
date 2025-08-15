/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef H__SCLSH__UTIL_H
#define H__SCLSH__UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct SclshStringBuffer_s {
    char* string;
    size_t length;
} SclshStringBuffer;

#define SCLSH_STRING_BUFFER(str) (SclshStringBuffer){ str, strlen(str) }

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
typedef void (*SclshHashMapIteratorCallback)(
    const char* key, 
    void* value, 
    void* user_data
);

SclshHashMap* sclsh_hash_map_new(void);
void sclsh_hash_map_free(SclshHashMap* map);
void sclsh_hash_map_set(SclshHashMap* map, const char* key, void* value);
void* sclsh_hash_map_get(SclshHashMap* map, const char* key);
void sclsh_hash_map_remove(SclshHashMap* map, const char* key);
void sclsh_hash_map_for_each(
    SclshHashMap* map, 
    SclshHashMapIteratorCallback callback, 
    void* user_data
);

#ifdef __cplusplus
}
#endif

#endif // H__SCLSH__UTIL_H

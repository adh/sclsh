/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: MIT
 */

 #ifndef H__SCLSH__IO_H
#define H__SCLSH__IO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sclsh/value.h>
#include <stdio.h>

typedef struct SclshStreamReader_s SclshStreamReader;

int sclsh_stream_reader_read_char(SclshStreamReader* reader);
void sclsh_stream_reader_unread_char(SclshStreamReader* reader, char ch);

SclshStreamReader* sclsh_stream_reader_new_from_file(FILE* file);
SclshStreamReader* sclsh_stream_reader_new_from_value(SclshValue* value);
void sclsh_stream_reader_free(SclshStreamReader* reader);

#ifdef __cplusplus
}
#endif

#endif // H__SCLSH__IO_H

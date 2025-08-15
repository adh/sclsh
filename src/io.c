/* Copyright © 2025 Ales Hakl
 *
 * SPDX-License-Identifier: MIT
 */

#include <sclsh/io.h>
#include "value.h"

struct SclshStreamReader_s {
    FILE* file;  // File pointer for reading
    SclshValue* value;  // Value to read from if not using a file
    size_t position;  // Current position in the value string
};

int sclsh_stream_reader_read_char(SclshStreamReader* reader) {
    if (reader->file) {
        return fgetc(reader->file);
    } else if (reader->value) {
        if (reader->position < reader->value->length) {
            return reader->value->string[reader->position++];
        } else {
            return EOF;  // End of value string
        }
    }
    return EOF;  // No valid source to read from
}

void sclsh_stream_reader_unread_char(SclshStreamReader* reader, char ch) {
    if (reader->file) {
        ungetc(ch, reader->file);
    } else if (reader->value && reader->position > 0) {
        reader->position--;
    }
}

SclshStreamReader* sclsh_stream_reader_new_from_file(FILE* file) {
    SclshStreamReader* reader = malloc(sizeof(SclshStreamReader));
    if (!reader) return NULL;

    reader->file = file;
    reader->value = NULL;
    reader->position = 0;

    return reader;
}

SclshStreamReader* sclsh_stream_reader_new_from_value(SclshValue* value) {
    SclshStreamReader* reader = malloc(sizeof(SclshStreamReader));
    if (!reader) return NULL;

    reader->file = NULL;
    reader->value = value;
    reader->position = 0;

    return reader;
}

void sclsh_stream_reader_free(SclshStreamReader* reader) {
    if (!reader) {
        return;
    }
    if (reader->file) {
        fclose(reader->file);
    }
    if (reader->value) {
        sclsh_value_unref(reader->value);
    }
    free(reader);
}

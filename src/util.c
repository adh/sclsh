#include <sclsh/util.h>
#include <sclsh/value.h>
#include <string.h>

uint32_t sclsh_fnv_hash(char* string) {
    uint32_t res = 0x811c9dc5;
    while(*string){
        res += (unsigned char)*string;
        res *= 0x01000193;
        string++;
    }
    return res;
}

uint32_t sclsh_pointer_hash(void* pointer) {
    uint32_t res = 0x811c9dc5;
    size_t buf = (size_t)pointer;
    buf >>= 3;
    while(buf){
        res += buf & 0xfff;
        buf >>= 12;
        res *= 0x01000193;
    }
    return res;
}

struct SclshStringBuilder_s {
    char* string;
    size_t length;
    size_t capacity;
};
SclshStringBuilder* sclsh_string_builder_new(void) {
    SclshStringBuilder* sb = malloc(sizeof(SclshStringBuilder));
    if (!sb) return NULL;

    sb->string = malloc(64);
    if (!sb->string) {
        free(sb);
        return NULL;
    }
    sb->length = 0;
    sb->capacity = 64;

    return sb;
}
void sclsh_string_builder_free(SclshStringBuilder* sb) {
    if (!sb) return;
    free(sb->string);
    free(sb);
}
void sclsh_string_builder_append_str(SclshStringBuilder* sb, const char* str) {
    if (!sb || !str) return;
    sclsh_string_builder_append_bytes(sb, str, strlen(str));
}
void sclsh_string_builder_append_bytes(SclshStringBuilder* sb, const char* bytes, size_t len) {
    if (!sb || !bytes || len == 0) return;

    if (sb->length + len >= sb->capacity) {
        sb->capacity = (sb->length + len) * 2;
        sb->string = realloc(sb->string, sb->capacity);
    }
    memcpy(sb->string + sb->length, bytes, len);
    sb->length += len;
    sb->string[sb->length] = '\0'; // Null-terminate
}
void sclsh_string_builder_append_buffer(SclshStringBuilder* sb, const SclshStringBuffer buffer) {
    if (!sb || !buffer.string || buffer.length == 0) return;
    sclsh_string_builder_append_bytes(sb, buffer.string, buffer.length);
}

SclshStringBuffer sclsh_string_builder_value(SclshStringBuilder* sb) {
    SclshStringBuffer buffer;
    if (!sb) {
        buffer.string = NULL;
        buffer.length = 0;
    } else {
        buffer.string = malloc(sb->length + 1);
        buffer.length = sb->length;
        memcpy(buffer.string, sb->string, sb->length);
        buffer.string[sb->length] = '\0';
    }
    return buffer;
}
SclshValue* sclsh_string_builder_to_value(SclshStringBuilder* sb) {
    if (!sb || sb->length == 0) {
        return sclsh_value_new("", 0);
    }
    SclshValue* value = sclsh_value_new(sb->string, sb->length);
    return value;
}

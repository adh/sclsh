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

typedef struct SclshHashMapEntry_s {
    char* key;
    void* value;
    struct SclshHashMapEntry_s* next;
} SclshHashMapEntry;
struct SclshHashMap_s {
    size_t count;
    size_t capacity;
    SclshHashMapEntry** entries;
};

SclshHashMap* sclsh_hash_map_new(void) {
    SclshHashMap* map = malloc(sizeof(SclshHashMap));
    if (!map) return NULL;

    map->count = 0;
    map->capacity = 16;
    map->entries = calloc(map->capacity, sizeof(SclshHashMapEntry*));
    if (!map->entries) {
        free(map);
        return NULL;
    }

    return map;
}
void sclsh_hash_map_free(SclshHashMap* map) {
    if (!map) return;
    for (size_t i = 0; i < map->capacity; i++) {
        SclshHashMapEntry* entry = map->entries[i];
        while (entry) {
            SclshHashMapEntry* next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
    }
    free(map->entries);
    free(map);
}
static void grow_hash_map(SclshHashMap* map) {
    size_t new_capacity = map->capacity * 2;
    SclshHashMapEntry** new_entries = calloc(new_capacity, sizeof(SclshHashMapEntry*));
    if (!new_entries) return; // Handle memory allocation failure

    for (size_t i = 0; i < map->capacity; i++) {
        SclshHashMapEntry* entry = map->entries[i];
        while (entry) {
            SclshHashMapEntry* next = entry->next;
            uint32_t hash = sclsh_fnv_hash(entry->key);
            size_t index = hash % new_capacity;

            entry->next = new_entries[index];
            new_entries[index] = entry;

            entry = next;
        }
    }

    free(map->entries);
    map->entries = new_entries;
    map->capacity = new_capacity;
}
void sclsh_hash_map_set(SclshHashMap* map, const char* key, void* value) {
    if (!map || !key) return;

    if (map->count >= map->capacity) {
        grow_hash_map(map);
    }

    uint32_t hash = sclsh_fnv_hash((char*)key);
    size_t index = hash % map->capacity;

    SclshHashMapEntry* entry = map->entries[index];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            entry->value = value; // Update existing value
            return;
        }
        entry = entry->next;
    }

    // Create a new entry
    entry = malloc(sizeof(SclshHashMapEntry));
    if (!entry) return;

    entry->key = strdup(key);
    entry->value = value;
    entry->next = map->entries[index];
    map->entries[index] = entry;
    map->count++;
}
void* sclsh_hash_map_get(SclshHashMap* map, const char* key) {
    if (!map || !key) return NULL;

    uint32_t hash = sclsh_fnv_hash((char*)key);
    size_t index = hash % map->capacity;

    SclshHashMapEntry* entry = map->entries[index];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value; // Found the value
        }
        entry = entry->next;
    }

    return NULL; // Key not found
}
void sclsh_hash_map_remove(SclshHashMap* map, const char* key) {
    if (!map || !key) return;
    uint32_t hash = sclsh_fnv_hash((char*)key);
    size_t index = hash % map->capacity;
    SclshHashMapEntry* entry = map->entries[index];
    SclshHashMapEntry* prev = NULL; 
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                map->entries[index] = entry->next; // Remove from head
            }
            free(entry->key);
            free(entry);
            map->count--;
            return;
        }
        prev = entry;
        entry = entry->next;
    }
}
void sclsh_hash_map_for_each(
    SclshHashMap* map, 
    SclshHashMapIteratorCallback callback, 
    void* user_data
) {
    if (!map || !callback) return;
    for (size_t i = 0; i < map->capacity; i++) {
        SclshHashMapEntry* entry = map->entries[i];
        while (entry) {
            callback(entry->key, entry->value, user_data);
            entry = entry->next;
        }
    }
}

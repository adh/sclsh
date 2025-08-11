#include <sclsh/util.h>

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

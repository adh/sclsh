#ifndef H__SCLSH__INTERNAL_VALUE
#define H__SCLSH__INTERNAL_VALUE

#include <sclsh/value.h>

struct s_SclshValue {
    long ref_count;  // Reference count for memory management
    
    bool is_error : 1;

    char* string;  // Pointer to the string data
    size_t length;  // Length of the string

    SclshValueList* as_list;
    SclshValueList* as_proc;
    SclshCommandLine* as_command_line;
};

struct s_SclshValueList {
    size_t count;  // Number of items in the list
    SclshValue* items[];  // Array of pointers to SclshValue
};

#endif
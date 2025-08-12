#include <sclsh/unwind.h>

SclshUnwindKind current_unwind = SCLSH_UNWIND_NONE;
SclshValue* unwind_value = NULL;
SclshListBuilder* traceback = NULL;

SclshUnwindKind sclsh_get_unwind(void) {
    return current_unwind;
}

SclshValue* sclsh_get_unwind_value(void) {
    return unwind_value;
}
void sclsh_set_unwind(SclshUnwindKind kind, SclshValue* value) {
    current_unwind = kind;
    if (unwind_value) {
        sclsh_value_unref(unwind_value);
    }
    unwind_value = sclsh_value_ref(value);
}

void sclsh_clear_unwind(void) {
    current_unwind = SCLSH_UNWIND_NONE;
    if (unwind_value) {
        sclsh_value_unref(unwind_value);
        unwind_value = NULL;
    }
    if (traceback) {
        sclsh_list_builder_free(traceback);
        traceback = NULL;
    }
}
void sclsh_record_traceback(SclshValue* line) {
    if (!traceback) {
        traceback = sclsh_list_builder_new();
    }
    sclsh_list_builder_append(traceback, line);
}

SclshValue* sclsh_get_traceback(void) {
    if (!traceback) {
        return NULL;
    }
    return sclsh_list_builder_value(traceback);
}

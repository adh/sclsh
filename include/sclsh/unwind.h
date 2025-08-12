#ifndef H__SCLSH__UNWIND_H
#define H__SCLSH__UNWIND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sclsh/value.h>

typedef enum {
    SCLSH_UNWIND_NONE,
    SCLSH_UNWIND_ERROR,
    SCLSH_UNWIND_RETURN,
    SCLSH_UNWIND_BREAK,
    SCLSH_UNWIND_CONTINUE,
} SclshUnwindKind;

SclshUnwindKind sclsh_get_unwind(void);
SclshValue* sclsh_get_unwind_value(void);
void sclsh_set_unwind(SclshUnwindKind kind, SclshValue* value);

void sclsh_clear_unwind(void);
void sclsh_record_traceback(SclshValue* line);

SclshValue* sclsh_get_traceback(void);

#ifdef __cplusplus
}
#endif

#endif // H__SCLSH__UNWIND_H

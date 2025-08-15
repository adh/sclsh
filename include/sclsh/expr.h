#ifndef H__SCLSH__EXPR_H
#define H__SCLSH__EXPR_H

#include <sclsh/value.h>
#include <sclsh/sclsh.h>

double sclsh_expr_eval_double(SclshContext* ctx, SclshValue* expr);
int sclsh_expr_eval_bool(SclshContext* ctx, SclshValue* expr);
SclshValue* sclsh_expr_eval(SclshContext* ctx, SclshValue* expr);

#endif // H__SCLSH__EXPR_H
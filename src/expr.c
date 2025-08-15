#include <sclsh/expr.h>
#include <stdio.h>
#include <math.h>
#include "value.h"

static double value_to_double(SclshContext* ctx, SclshValue* value) {
    SclshStringBuffer str_buf = sclsh_value_as_string(value);
    if (!str_buf.string) {
        return 0.0;
    }
    if (str_buf.string[0] == '$') {
        value = sclsh_context_get_variable(ctx, str_buf.string + 1);
        str_buf = sclsh_value_as_string(value);
    }
    return str_buf.length > 0 ? atof(str_buf.string) : 0.0;
}

double sclsh_expr_eval_double(SclshContext* ctx, SclshValue* expr) {
    SclshValueList *list = sclsh_value_as_list(expr);
    if (!list || list->count == 0) {
        return 0.0; // Empty or invalid expression
    }

    double result = value_to_double(ctx, list->items[0]);
    for (size_t i = 1; i < list->count; i += 2) {
        char* op = sclsh_value_as_string(list->items[i]).string;
        double next_value = value_to_double(ctx, list->items[i + 1]);
        if (strcmp(op, "+") == 0) {
            result += next_value;
        } else if (strcmp(op, "-") == 0) {
            result -= next_value;
        } else if (strcmp(op, "*") == 0) {
            result *= next_value;
        } else if (strcmp(op, "/") == 0) {
            if (next_value != 0.0) {
                result /= next_value;
            } else {
                fprintf(stderr, "Division by zero in expression\n");
                return 0.0; // Handle division by zero
            }
        } else if (strcmp(op, "%") == 0) {
            if (next_value != 0.0) {
                result = fmod(result, next_value);
            } else {
                fprintf(stderr, "Division by zero in expression\n");
                return 0.0; // Handle division by zero
            }
        } else if (strcmp(op, "==") == 0) {
            result = (result == next_value) ? 1.0 : 0.0;
        } else if (strcmp(op, "!=") == 0) {
            result = (result != next_value) ? 1.0 : 0.0;
        } else if (strcmp(op, "<") == 0) {
            result = (result < next_value) ? 1.0 : 0.0;
        } else if (strcmp(op, "<=") == 0) {
            result = (result <= next_value) ? 1.0 : 0.0;
        } else if (strcmp(op, ">") == 0) {
            result = (result > next_value) ? 1.0 : 0.0;
        } else if (strcmp(op, ">=") == 0) {
            result = (result >= next_value) ? 1.0 : 0.0;
        } else {
            fprintf(stderr, "Unknown operator: %s\n", op);
            return 0.0; // Handle unknown operator
        }
    }
    return result;
}
int sclsh_expr_eval_bool(SclshContext* ctx, SclshValue* expr) {
    return sclsh_expr_eval_double(ctx, expr) != 0.0;
}
SclshValue* sclsh_expr_eval(SclshContext* ctx, SclshValue* expr) {
    double result = sclsh_expr_eval_double(ctx, expr);
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%f", result);
    return sclsh_value_new(buffer, strlen(buffer));
}

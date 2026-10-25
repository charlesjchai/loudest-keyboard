#ifndef STACK_H
#define STACK_H
#include <stdlib.h>
#include <string.h>
typedef struct {
    char *data;
    int len;
    int capacity;
} Stack;
static inline int stk_push(Stack *stack, char *value) {
    if (stack->len >= stack->capacity) {
        char *tmp = realloc(stack->data, stack->capacity * 2);
        if (!tmp) {
            return -1;
        }
        stack->data = tmp;
        stack->capacity *= 2;
    }
    stack->len++;
    memcpy(stack->data + stack->len - 1, value, 1);
    return 0;
}
static inline void stk_pop(Stack *stack) {
    memset(stack->data + stack->len - 1, 0, 1);
    stack->len--;
}
static inline void stk_clr(Stack *stack) {
    memset(stack->data, 0, stack->len);
    stack->len = 0;
}

#endif

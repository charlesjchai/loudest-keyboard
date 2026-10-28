// clang-format Language: C
#ifndef STACK_H
#define STACK_H
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
    char *data;
    size_t len;
    size_t capacity;
    size_t element_size;
} Stack;
static inline int stk_push(Stack *restrict stack, const void *restrict value) {
    if (stack->len >= stack->capacity) {
        char *tmp = realloc(stack->data, stack->capacity * 2 * stack->element_size);
        if (!tmp) {
            return -1;
        }
        stack->data = tmp;
        stack->capacity *= 2;
    }
    stack->len++;
    memcpy(stack->data + ((stack->len - 1) * stack->element_size), value,
           stack->element_size);
    return 0;
}
static inline void *stk_pop(Stack *stack) {
    stack->len--;
    return stack->data + stack->len * stack->element_size;
}
static inline void stk_clr(Stack *stack) {
    memset(stack->data, 0, stack->len * stack->element_size);
    stack->len = 0;
}
#endif
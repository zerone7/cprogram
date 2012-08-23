#include <assert.h>
#include <stdlib.h>

#define STACK_SIZE  100

struct stack {
    char data[STACK_SIZE];
    int  top;
};

static inline struct stack *init_stack()
{
    struct stack *pstack;

    pstack = (struct stack *)malloc(sizeof(struct stack));
    if (pstack) {
        pstack->top = -1;
        return pstack;
    } else
        return NULL;
}

static inline void destroy_stack(struct stack *pstack)
{
    assert(pstack);
    free(pstack);
}

static inline int is_empty(struct stack *pstack)
{
    assert(pstack);
    return pstack->top == -1 ? -1 : 0;
}

static inline void clear_stack(struct stack *pstack)
{
    assert(pstack);
    pstack->top = -1;
}

static inline char top(struct stack *pstack)
{
    assert(pstack);
    return is_empty(pstack) ? '\0' : pstack->data[pstack->top];
}

static inline int push(struct stack *pstack, char new_data)
{
    assert(pstack);
    if (pstack->top != STACK_SIZE-1) {
        pstack->top++;
        pstack->data[pstack->top] = new_data;
        return 0;
    } else
        return -1;
}

static inline char pop(struct stack *pstack)
{
    assert(pstack);
    if (is_empty(pstack))
        return '\0';
    else {
        pstack->top--;
        return pstack->data[pstack->top+1];
    }
}

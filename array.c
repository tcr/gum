#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

#include "gum.h"

#define DARRAYSIZE 5

typedef struct js_array {
    int max_size;
    int cur_size;
    JS_VAL *items;
} JSArray;

int  js_array_new(JSArray *);
void js_array_add(JSArray *, JS_VAL);
void js_array_destroy(JSArray *);
bool  js_array_full(JSArray *);
int  js_array_grow(JSArray *);
void js_array_debug(JSArray *);

//js_array_newialise the JSArray
int js_array_new(JSArray *arr)
{
    arr->items = (JS_VAL *) malloc(sizeof(JS_VAL) * DARRAYSIZE);
    if (arr->items) {
        arr->max_size = DARRAYSIZE;
        arr->cur_size = 0;
        return 0;
    } else {
        printf("Malloc failed!\n");
        return 1;
    }
}

//add the new val into the JSArray
void js_array_add(JSArray *arr, JS_VAL val)
{
    if (!js_array_full(arr)) {
        arr->items[arr->cur_size++] = val;    
    } else if (!js_array_grow(arr)) {
        arr->items[arr->cur_size++] = val;    
    } else {
        exit(1);
    }
}

void _js_array_vadd(JSArray *arr, ...) {
    va_list args;
    va_start(args, arr);
    JS_VAL val;
    while (true) {
        val = va_arg(args, JS_VAL);
        if (val.tag == 0) {
            break;
        }
        js_array_add(arr, val);
    }
    va_end(args);
}

#define js_array_vadd(X, ...) _js_array_vadd(X, __VA_ARGS__, NULL)

//returns 1 if the DAarray is js_array_full(
bool js_array_full(JSArray *arr)
{
    assert(arr->cur_size >= 0 && arr->max_size >= 0);
    assert(arr->cur_size <= arr->max_size);
    return arr->cur_size == arr->max_size;
}

//js_array_grow(s the JSArray to double its original size
int js_array_grow(JSArray *arr)
{
    JS_VAL *temp = (JS_VAL *) realloc(arr->items, sizeof(*temp) * arr->max_size * 2);
    if (!temp) {
        printf("Realloc failed!\n");
        return 1;
    } else {
        arr->items = temp;
        arr->max_size *= 2;
        printf("Darray doubled and current contents are:\n");
        js_array_debug(arr);
        return 0;
    }
}

//js_array_destroy(s the JSArray
void js_array_destroy(JSArray *arr)
{
    free(arr->items);
    arr->items = 0;
    arr->max_size = 0;
    arr->cur_size = 0;
}

void js_array_debug(JSArray *arr) {
    int i = 0;
    printf("Max = %d; Cur = %d\n", arr->max_size, arr->cur_size);
    for (i = 0; i < arr->cur_size; i++) {
        printf("%s\t", JS_VAL_STR(arr->items[i]));
    }
    printf("\n");
}

int main(void)
{
    JSArray newDA;
    if (!js_array_new(&newDA))
    {
        js_array_vadd(&newDA, JS_NUMBER(42), JS_STRING("hihihi"));
        int i;
        for (i = 1; i < 30; i++) {
            js_array_add(&newDA, JS_NUMBER(i));
        }
    } else {
        exit(1);
    }
    js_array_destroy(&newDA);

    return 0;
}
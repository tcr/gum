#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

#include "gum.h"

#define DARRAYSIZE 5

//js_array_newialise the JSArray. Returns NULL on failure.
JSArray *js_array_new()
{
    JSArray* arr = (JSArray*) malloc(sizeof(JSArray));
    if (!arr) goto err;

    arr->items = (JSValue *) malloc(sizeof(JSValue) * DARRAYSIZE);
    if (!arr->items) goto err;

    arr->max_size = DARRAYSIZE;
    arr->cur_size = 0;
    return arr;

    err:
        if (arr)
            js_array_destroy(arr);
        return NULL;
}

//add the new val into the JSArray
void js_array_add(JSArray *arr, JSValue val)
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
    JSValue val;
    while (true) {
        val = va_arg(args, JSValue);
        if (val.tag == 0) {
            break;
        }
        js_array_add(arr, val);
    }
    va_end(args);
}

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
    JSValue *temp = (JSValue *) realloc(arr->items, sizeof(*temp) * arr->max_size * 2);
    if (!temp) {
        printf("Realloc failed!\n");
        return 1;
    } else {
        arr->items = temp;
        arr->max_size *= 2;
        return 0;
    }
}

//js_array_destroy(s the JSArray
void js_array_destroy(JSArray *arr)
{
    free(arr->items);
    free(arr);
}

/*
int main(void)
{
    JSArray* newDA = js_array_new();
    if (!js_array_new()) {
        exit(1);
    }

    js_array_vadd(newDA, JS_NUMBER(42), JS_STRING("hihihi"));
    int i;
    for (i = 1; i < 30; i++) {
        js_array_add(newDA, JS_NUMBER(i));
    }
    js_array_destroy(newDA);

    return 0;
}
*/
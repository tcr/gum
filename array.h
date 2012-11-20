#ifndef ARRAY_H
#define ARRAY_H

#include "gum.h"

struct js_array;
typedef struct js_array JSArray;

struct js_array {
    int max_size;
    int cur_size;
    JSValue *items;
};

JSArray *js_array_new();

void js_array_add(JSArray *arr, JSValue val);

void _js_array_vadd(JSArray *arr, ...);

#define js_array_vadd(X, ...) _js_array_vadd(X, __VA_ARGS__, NULL);

//returns 1 if the DAarray is js_array_full(
bool js_array_full(JSArray *arr);

//js_array_grow(s the JSArray to double its original size
int js_array_grow(JSArray *arr);

//js_array_destroy(s the JSArray
void js_array_destroy(JSArray *arr);

void js_array_debug(JSArray *arr);

#endif
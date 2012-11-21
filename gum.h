#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

#include "hashmap.h"
#include "array.h"

#define KEY_MAX_LENGTH (256)
#define KEY_PREFIX ("somekey")
#define KEY_COUNT (1024*1024)

/**
 * Typedefs
 */

#ifndef GUM_H
#define GUM_H 1

typedef struct JSValue_struct {
	char tag;
	union {
		char boolean;
		double number;
		char *string;
		void *function;
		map_t object;
		struct JSValue_struct *array;
	};
} JSValue;

typedef bool (*js_op_ptr)(void **, JSValue, JSValue);
typedef JSValue (*jsvalue_op_ptr)(void **, JSValue, JSValue);
typedef JSValue (*js_func)(JSValue, ...);

#endif

/**
 * Macros
 */

// Heap printf

#define HPRINTF(...) ({ \
		char *sptr; \
		asprintf(&sptr, __VA_ARGS__); \
		sptr; \
	})

// vargs

#define VARGS(...) va_list argp; \
	va_start(argp, this); \
	__VA_ARGS__ \
	va_end(argp)

#define VARG(NAME) JSValue NAME = va_arg(argp, JSValue);

/**
 * Struct stuff
 */

#define JS_INVALID_TAG 0
#define JS_UNDEFINED_TAG 1
#define JS_NULL_TAG 2
#define JS_NUMBER_TAG 3
#define JS_STRING_TAG 4
#define JS_BOOL_TAG 5
#define JS_FUNCTION_TAG 6
#define JS_OBJECT_TAG 7
#define JS_ARRAY_TAG 8

#define JS_NUMBER(X) ((JSValue) {JS_NUMBER_TAG, {.number = X}})
#define JS_STRING(X) ((JSValue) {JS_STRING_TAG, {.string = X}})
#define JS_BOOL(X) ((JSValue) {JS_BOOL_TAG, {.boolean = X}})
#define JS_FUNCTION(X) ((JSValue) {JS_FUNCTION_TAG, {.function = X}})
#define JS_OBJECT() ({ \
 	JSValue obj = ((JSValue) {JS_OBJECT_TAG, {.object = hashmap_new()}}); \
 	hashmap_set_proto(obj.object, _object_prototype.object); \
 	obj; \
 	})
#define JS_ARRAY(X) ((JSValue) {JS_ARRAY_TAG, {.array = X}})

const JSValue JS_UNDEF;
const JSValue JS_NULL;

/**
 * ops
 */

// operations

#define JS_OP(THEOP, X, Y) ({ \
		static js_op_ptr PTR = &THEOP; \
		(*PTR)((void **) &PTR, X, Y); \
	})

// operator <
bool JS_LT_SWITCH (void **op_cache_ptr, JSValue a, JSValue b);
#define JS_LT_VARIANT(NAME, CHECK, RET) bool JS_LT_ ## NAME (void **op_cache_ptr, JSValue a, JSValue b) {\
		return (CHECK) ? JS_LT_SWITCH(op_cache_ptr, a, b) : (RET) ? true : false; \
	}
bool JS_LT_NUMBER_NUMBER(void **, JSValue, JSValue);
#define JS_LT(A, B) JS_OP(JS_LT_NUMBER_NUMBER, A, B)

// operator ==
bool JS_EQ_SWITCH (void **op_cache_ptr, JSValue a, JSValue b);
#define JS_EQ_VARIANT(NAME, CHECK, RET) bool JS_EQ_ ## NAME (void **op_cache_ptr, JSValue a, JSValue b) {\
		return (CHECK) ? JS_EQ_SWITCH(op_cache_ptr, a, b) : (RET) ? true : false; \
	}
bool JS_EQ_NUMBER_NUMBER(void **, JSValue, JSValue);
#define JS_EQ(A, B) JS_OP(JS_EQ_NUMBER_NUMBER, A, B)

// operator ||
bool JS_OR_SWITCH (void **op_cache_ptr, JSValue a, JSValue b);
#define JS_OR_VARIANT(NAME, CHECK, RET) bool JS_OR_ ## NAME (void **op_cache_ptr, JSValue a, JSValue b) {\
		return (CHECK) ? JS_OR_SWITCH(op_cache_ptr, a, b) : (RET) ? true : false; \
	}
bool JS_OR_BOOL_BOOL(void **, JSValue, JSValue);
#define JS_OR(A, B) JS_OP(JS_OR_BOOL_BOOL, A, B)


// adding

#define JSValue_OP(THEOP, X, Y) ({ \
		static jsvalue_op_ptr PTR = &THEOP; \
		(*PTR)((void **) &PTR, X, Y); \
	})

JSValue JS_ADD_SWITCH (void **op_cache_ptr, JSValue a, JSValue b);
#define JS_ADD_VARIANT(NAME, CHECK, RET) JSValue JS_ADD_ ## NAME (void **op_cache_ptr, JSValue a, JSValue b) {\
		return (CHECK) ? JS_ADD_SWITCH(op_cache_ptr, a, b) : RET; \
	}
JSValue JS_ADD_DOUBLE_DOUBLE(void **, JSValue, JSValue);
JSValue JS_ADD_DOUBLE_STRING(void **, JSValue, JSValue);
#define JS_ADD(A, B) JSValue_OP(JS_ADD_DOUBLE_DOUBLE, A, B)

JSValue JS_SUB_SWITCH (void **op_cache_ptr, JSValue a, JSValue b);
#define JS_SUB_VARIANT(NAME, CHECK, RET) JSValue JS_SUB_ ## NAME (void **op_cache_ptr, JSValue a, JSValue b) {\
		return (CHECK) ? JS_SUB_SWITCH(op_cache_ptr, a, b) : RET; \
	}
JSValue JS_SUB_DOUBLE_DOUBLE(void **, JSValue, JSValue);
#define JS_SUB(A, B) JSValue_OP(JS_SUB_DOUBLE_DOUBLE, A, B)

JSValue JS_MUL_SWITCH (void **op_cache_ptr, JSValue a, JSValue b);
#define JS_MUL_VARIANT(NAME, CHECK, RET) JSValue JS_MUL_ ## NAME (void **op_cache_ptr, JSValue a, JSValue b) {\
		return (CHECK) ? JS_MUL_SWITCH(op_cache_ptr, a, b) : RET; \
	}
JSValue JS_MUL_DOUBLE_DOUBLE(void **, JSValue, JSValue);
#define JS_MUL(A, B) JSValue_OP(JS_MUL_DOUBLE_DOUBLE, A, B)


// calling conventions

#define VA_ARGS(...) , ##__VA_ARGS__

#define JS_CALL_FUNC(OBJ, ...) ((js_func) OBJ.function)(JS_NULL VA_ARGS(__VA_ARGS__))

#define JS_CALL_METHOD(OBJ, NAME, ...) ({ \
	JSValue value; \
	hashmap_get(OBJ.object, NAME, &value); \
	((js_func) value.function)(OBJ VA_ARGS(__VA_ARGS__));  \
	})

#define JS_GET_PROP(OBJ, NAME) ({ \
	JSValue value; \
	hashmap_get(OBJ.object, NAME, &value); \
	value; \
	})

#define JS_SET_PROP(OBJ, NAME, VAL) hashmap_put(OBJ.object, NAME, VAL);

#define JS_OBJECT_FREE(X) hashmap_free(X.object);

#define JS_DEFN(NAME) JSValue _js_fn_ ## NAME (JSValue, ...); \
	JSValue NAME = JS_FUNCTION(_js_fn_ ## NAME); \
	JSValue _js_fn_ ## NAME (JSValue this, ...)

char *JS_VAL_STR (JSValue val);
JSValue console;
JSValue module_0;
JSValue _object_prototype;
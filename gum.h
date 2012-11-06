#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

#include "hashmap.h"

#define KEY_MAX_LENGTH (256)
#define KEY_PREFIX ("somekey")
#define KEY_COUNT (1024*1024)

/**
 * Typedefs
 */

#ifndef GUM_H

typedef struct JS_VAL_STRUCT {
	char tag;
	union {
		char boolean;
		double number;
		char *string;
		void *function;
		map_t object;
		struct JS_VAL_STRUCT *array;
	};
} JS_VAL;

typedef bool (*js_op_ptr)(void **, JS_VAL, JS_VAL);
typedef JS_VAL (*js_val_op_ptr)(void **, JS_VAL, JS_VAL);
typedef JS_VAL (*js_func)(JS_VAL, ...);

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

#define VARG(NAME) JS_VAL NAME = va_arg(argp, JS_VAL);

/**
 * Struct stuff
 */

#define JS_NULL_TAG 0
#define JS_NUMBER_TAG 1
#define JS_STRING_TAG 2
#define JS_BOOL_TAG 3
#define JS_FUNCTION_TAG 4
#define JS_OBJECT_TAG 5
#define JS_ARRAY_TAG 6

#define JS_NUMBER(X) ((JS_VAL) {JS_NUMBER_TAG, {.number = X}})
#define JS_STRING(X) ((JS_VAL) {JS_STRING_TAG, {.string = X}})
#define JS_BOOL(X) ((JS_VAL) {JS_BOOL_TAG, {.boolean = X}})
#define JS_FUNCTION(X) ((JS_VAL) {JS_FUNCTION_TAG, {.function = X}})
#define JS_OBJECT(X) ((JS_VAL) {JS_OBJECT_TAG, {.object = hashmap_new()}})
#define JS_ARRAY(X) ((JS_VAL) {JS_ARRAY_TAG, {.array = X}})

const JS_VAL JS_NULL;

/**
 * ops
 */

// operations

#define JS_OP(THEOP, X, Y) ({ \
		static js_op_ptr PTR = &THEOP; \
		(*PTR)((void **) &PTR, X, Y); \
	})

// operator <
bool JS_LT_SWITCH (void **op_cache_ptr, JS_VAL a, JS_VAL b);
#define JS_LT_VARIANT(NAME, CHECK, RET) bool JS_LT_ ## NAME (void **op_cache_ptr, JS_VAL a, JS_VAL b) {\
		return (CHECK) ? JS_LT_SWITCH(op_cache_ptr, a, b) : (RET) ? true : false; \
	}
bool JS_LT_NUMBER_NUMBER(void **, JS_VAL, JS_VAL);
#define JS_LT(A, B) JS_OP(JS_LT_NUMBER_NUMBER, A, B)

// operator ==
bool JS_EQ_SWITCH (void **op_cache_ptr, JS_VAL a, JS_VAL b);
#define JS_EQ_VARIANT(NAME, CHECK, RET) bool JS_EQ_ ## NAME (void **op_cache_ptr, JS_VAL a, JS_VAL b) {\
		return (CHECK) ? JS_EQ_SWITCH(op_cache_ptr, a, b) : (RET) ? true : false; \
	}
bool JS_EQ_NUMBER_NUMBER(void **, JS_VAL, JS_VAL);
#define JS_EQ(A, B) JS_OP(JS_EQ_NUMBER_NUMBER, A, B)

// operator ||
bool JS_OR_SWITCH (void **op_cache_ptr, JS_VAL a, JS_VAL b);
#define JS_OR_VARIANT(NAME, CHECK, RET) bool JS_OR_ ## NAME (void **op_cache_ptr, JS_VAL a, JS_VAL b) {\
		return (CHECK) ? JS_OR_SWITCH(op_cache_ptr, a, b) : (RET) ? true : false; \
	}
bool JS_OR_BOOL_BOOL(void **, JS_VAL, JS_VAL);
#define JS_OR(A, B) JS_OP(JS_OR_BOOL_BOOL, A, B)


// adding

#define JS_VAL_OP(THEOP, X, Y) ({ \
		static js_val_op_ptr PTR = &THEOP; \
		(*PTR)((void **) &PTR, X, Y); \
	})

JS_VAL JS_ADD_SWITCH (void **op_cache_ptr, JS_VAL a, JS_VAL b);
#define JS_ADD_VARIANT(NAME, CHECK, RET) JS_VAL JS_ADD_ ## NAME (void **op_cache_ptr, JS_VAL a, JS_VAL b) {\
		return (CHECK) ? JS_ADD_SWITCH(op_cache_ptr, a, b) : RET; \
	}
JS_VAL JS_ADD_DOUBLE_DOUBLE(void **, JS_VAL, JS_VAL);
JS_VAL JS_ADD_DOUBLE_STRING(void **, JS_VAL, JS_VAL);
#define JS_ADD(A, B) JS_VAL_OP(JS_ADD_DOUBLE_DOUBLE, A, B)

JS_VAL JS_SUB_SWITCH (void **op_cache_ptr, JS_VAL a, JS_VAL b);
#define JS_SUB_VARIANT(NAME, CHECK, RET) JS_VAL JS_SUB_ ## NAME (void **op_cache_ptr, JS_VAL a, JS_VAL b) {\
		return (CHECK) ? JS_SUB_SWITCH(op_cache_ptr, a, b) : RET; \
	}
JS_VAL JS_SUB_DOUBLE_DOUBLE(void **, JS_VAL, JS_VAL);
#define JS_SUB(A, B) JS_VAL_OP(JS_SUB_DOUBLE_DOUBLE, A, B)

JS_VAL JS_MUL_SWITCH (void **op_cache_ptr, JS_VAL a, JS_VAL b);
#define JS_MUL_VARIANT(NAME, CHECK, RET) JS_VAL JS_MUL_ ## NAME (void **op_cache_ptr, JS_VAL a, JS_VAL b) {\
		return (CHECK) ? JS_MUL_SWITCH(op_cache_ptr, a, b) : RET; \
	}
JS_VAL JS_MUL_DOUBLE_DOUBLE(void **, JS_VAL, JS_VAL);
#define JS_MUL(A, B) JS_VAL_OP(JS_MUL_DOUBLE_DOUBLE, A, B)


// calling conventions

#define JS_CALL_FUNC(OBJ, ...) ((js_func) OBJ.function)(JS_NULL, ## __VA_ARGS__)

#define JS_CALL_METHOD(OBJ, NAME, ...) ({ \
	JS_VAL* value; \
	hashmap_get(OBJ.object, NAME, (void **)(&value)); \
	((js_func) value->function)(OBJ, ## __VA_ARGS__);  \
	})

#define JS_SET_PROP(OBJ, NAME, VAL) hashmap_put(OBJ.object, NAME, VAL);

#define JS_OBJECT_FREE(X) hashmap_free(X.object);

#define JS_DEFN(NAME) JS_VAL _js_fn_ ## NAME (JS_VAL, ...); \
	JS_VAL NAME = JS_FUNCTION(_js_fn_ ## NAME); \
	JS_VAL _js_fn_ ## NAME (JS_VAL this, ...)

/**
 * C code
 */

#ifndef GUM_H
#define GUM_H 1

const JS_VAL JS_NULL = ((JS_VAL) {JS_NULL_TAG});

char *JS_VAL_STR (JS_VAL val) {
	switch (val.tag) {
		case JS_NUMBER_TAG: return HPRINTF("%f", val.number);
		case JS_STRING_TAG: return HPRINTF("%s", val.string);
		case JS_BOOL_TAG: return HPRINTF("%s", val.boolean ? "true" : "false");
		case JS_OBJECT_TAG: return HPRINTF("[object Object]");
		case JS_FUNCTION_TAG: return HPRINTF("[function]");
		default: return "null";
	}
}

JS_LT_VARIANT(NUMBER_NUMBER, a.tag != b.tag, a.number < b.number);
bool JS_LT_SWITCH (void **op_cache_ptr, JS_VAL a, JS_VAL b) {
	if (a.tag == JS_NUMBER_TAG && b.tag == JS_NUMBER_TAG) {
		*op_cache_ptr = &JS_LT_NUMBER_NUMBER;
	}
	return ((js_op_ptr) *op_cache_ptr)(op_cache_ptr, a, b);
}

JS_EQ_VARIANT(NUMBER_NUMBER, a.tag != b.tag, a.number == b.number);
bool JS_EQ_SWITCH (void **op_cache_ptr, JS_VAL a, JS_VAL b) {
	if (a.tag == JS_NUMBER_TAG && b.tag == JS_NUMBER_TAG) {
		*op_cache_ptr = &JS_EQ_NUMBER_NUMBER;
	}
	return ((js_op_ptr) *op_cache_ptr)(op_cache_ptr, a, b);
}

JS_OR_VARIANT(BOOL_BOOL, a.tag != b.tag, a.number || b.number);
bool JS_OR_SWITCH (void **op_cache_ptr, JS_VAL a, JS_VAL b) {
	if (a.tag == JS_BOOL_TAG && b.tag == JS_BOOL_TAG) {
		*op_cache_ptr = &JS_OR_BOOL_BOOL;
	}
	return ((js_op_ptr) *op_cache_ptr)(op_cache_ptr, a, b);
}

JS_ADD_VARIANT(DOUBLE_DOUBLE, a.tag != b.tag, JS_NUMBER(a.number + b.number));
JS_ADD_VARIANT(DOUBLE_STRING, a.tag != JS_NUMBER_TAG || b.tag != JS_STRING_TAG, JS_STRING(HPRINTF("%f%s", a.number, b.string)));
JS_VAL JS_ADD_SWITCH (void **op_cache_ptr, JS_VAL a, JS_VAL b) {
	if (a.tag == JS_NUMBER_TAG && b.tag == JS_NUMBER_TAG) {
		*op_cache_ptr = &JS_ADD_DOUBLE_DOUBLE;
	} else if (a.tag == JS_NUMBER_TAG && b.tag == JS_STRING_TAG) {
		*op_cache_ptr = &JS_ADD_DOUBLE_STRING;
	}
	return ((js_val_op_ptr) *op_cache_ptr)(op_cache_ptr, a, b);
}

JS_SUB_VARIANT(DOUBLE_DOUBLE, a.tag != b.tag, JS_NUMBER(a.number - b.number));
JS_VAL JS_SUB_SWITCH (void **op_cache_ptr, JS_VAL a, JS_VAL b) {
	if (a.tag == JS_NUMBER_TAG && b.tag == JS_NUMBER_TAG) {
		*op_cache_ptr = &JS_SUB_DOUBLE_DOUBLE;
	}
	return ((js_val_op_ptr) *op_cache_ptr)(op_cache_ptr, a, b);
}

JS_MUL_VARIANT(DOUBLE_DOUBLE, a.tag != b.tag, JS_NUMBER(a.number * b.number));
JS_VAL JS_MUL_SWITCH (void **op_cache_ptr, JS_VAL a, JS_VAL b) {
	if (a.tag == JS_NUMBER_TAG && b.tag == JS_NUMBER_TAG) {
		*op_cache_ptr = &JS_MUL_DOUBLE_DOUBLE;
	}
	return ((js_val_op_ptr) *op_cache_ptr)(op_cache_ptr, a, b);
}

/** 
 * Globals
 */

JS_DEFN(console_log) {
	VARGS(VARG(str));
	printf("%s\n", JS_VAL_STR(str));
	return JS_NULL;
}

JS_VAL console;

void initialze_globals() {
	// Setup console.
	console = JS_OBJECT();
	JS_SET_PROP(console, "log", &console_log);
}

void destroy_globals() {
	JS_OBJECT_FREE(console);
}

JS_VAL module_0;

int main () {
	initialze_globals();
	JS_CALL_FUNC(module_0);
	destroy_globals();
	return 0;
}

#endif
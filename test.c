#include <stdio.h>
#include <stdbool.h>

/*

function lessthan (a, b) {
	console.log(a < b ? 'a < b' : 'c < d');
}

lessthan(3, 4);
lessthan("a", "b");

*/

typedef struct {
	int tag;
	union {
		int intValue;
		float floatValue;
	};
} JS_VAL;

typedef bool (*js_op_ptr)(void **, JS_VAL *, JS_VAL *);

#define JS_OP(THEOP, X, Y) ({ \
		static js_op_ptr PTR = &THEOP; \
		(*PTR)((void **) &PTR, X, Y); \
	})
#define BOOL_COND(X) ((X) ? true : false)
#define JS_OP_VARIANT(NAME, CHECK, RET) bool JS_LT_ ## NAME (void **op_cache_ptr, JS_VAL *a, JS_VAL *b) {\
		printf("[[js_lt " # NAME "]]\n"); \
		return (CHECK) ? JS_LT_SWITCH(op_cache_ptr, a, b) : BOOL_COND(RET); \
	}

bool JS_LT_SWITCH (void **op_cache_ptr, JS_VAL *a, JS_VAL *b);

#define JS_LT(A, B) JS_OP(JS_LT_SWITCH, A, B)

JS_OP_VARIANT(INT_FLOAT, a->tag != 0 || b->tag != 1, a->intValue < b->floatValue);
JS_OP_VARIANT(INT_INT, a->tag != b->tag, a->intValue < b->intValue);

bool JS_LT_SWITCH (void **op_cache_ptr, JS_VAL *a, JS_VAL *b) {
	printf("[[js_lt switch]]\n");
	if (a->tag == 0 && b->tag == 0) {
		*op_cache_ptr = &JS_LT_INT_INT;
	} else if (a->tag == 0 && b->tag == 1) {
		*op_cache_ptr = &JS_LT_INT_FLOAT;
	}
	return ((js_op_ptr) *op_cache_ptr)(op_cache_ptr, a, b);
}

#define JS_INT(X) {0, {.intValue = X}}
#define JS_FLOAT(X) {1, {.floatValue = X}}

/******

CHECK IT

*********/

void console_log (char *str) {
	printf("%s\n", str);
}

void lessthan (JS_VAL *a, JS_VAL *b) {
	console_log(JS_LT(a, b) ? "a < b" : "a > b");
}

int main () {
	JS_VAL _a = JS_INT(5), _b = JS_INT(6);
	lessthan(&_a, &_b);
	JS_VAL _c = JS_FLOAT(4.4);
	lessthan(&_a, &_c);
	lessthan(&_a, &_c);
	lessthan(&_a, &_c);
	lessthan(&_a, &_c);
	lessthan(&_a, &_c);
	lessthan(&_a, &_c);
	lessthan(&_a, &_c);
	lessthan(&_a, &_c);
	return 0;
}
#include "gum.h"

/**
 * Your Module
 */

JS_DEFN(fac) {
	VARGS(VARG(n));
	if (JS_EQ(n, JS_NUMBER(1)) || JS_EQ(n, JS_NUMBER(0))) {
		return JS_NUMBER(1);
	}
	return JS_MUL(n, JS_CALL_FUNC(fac, JS_ADD(n, JS_NUMBER(-1))));
}

JS_DEFN(module_0) {
	JS_CALL_METHOD(console, "log", JS_CALL_FUNC(fac, JS_NUMBER(5)));
}
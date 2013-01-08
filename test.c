#include "gum.h"

// TODO (Delete test-generated code from repo): This doesn't belong here.

JS_DEFN(fac) {
	VARGS(VARG(n));
	if (JS_BOOL(JS_OR(JS_BOOL(JS_EQ(n, JS_NUMBER(1))), JS_BOOL(JS_EQ(n, JS_NUMBER(0))))).boolean) {
		return JS_NUMBER(1);
	}
	return JS_MUL(n, JS_CALL_FUNC(fac, JS_SUB(n, JS_NUMBER(1))));
}

JS_DEFN(module_0) {
	JS_CALL_METHOD(console, "log", JS_CALL_FUNC(fac, JS_NUMBER(5)));
}

int main() {
	return js_main();
}

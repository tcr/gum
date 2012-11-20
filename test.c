#include "gum.h"

JS_DEFN(fac) {
	VARGS(VARG(n));
	if (JS_BOOL(JS_OR(JS_BOOL(JS_EQ(n, JS_NUMBER(1))), JS_BOOL(JS_EQ(n, JS_NUMBER(0))))).boolean) {
		return JS_NUMBER(1);
	}
	return JS_MUL(n, JS_CALL_FUNC(fac, JS_SUB(n, JS_NUMBER(1))));
}

JS_DEFN(module_0) {
	JSValue a = ({ JSValue obj = JS_OBJECT(); JS_SET_PROP(obj, "b", fac); obj; });
	hashmap_set_proto(a.object, console.object);
	JS_CALL_METHOD(a, "log", JS_CALL_METHOD(a, "b", JS_NUMBER(5)));
}

int main() {
	return js_main();
}

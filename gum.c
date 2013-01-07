#include "gum.h"

const JSValue JS_UNDEF = ((JSValue) {JS_UNDEFINED_TAG});
const JSValue JS_NULL = ((JSValue) {JS_NULL_TAG});

char *JSValue_STR (JSValue val) {
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
bool JS_LT_SWITCH (void **op_cache_ptr, JSValue a, JSValue b) {
	if (a.tag == JS_NUMBER_TAG && b.tag == JS_NUMBER_TAG) {
		*op_cache_ptr = &JS_LT_NUMBER_NUMBER;
	}
	return ((js_op_ptr) *op_cache_ptr)(op_cache_ptr, a, b);
}

JS_EQ_VARIANT(NUMBER_NUMBER, a.tag != b.tag, a.number == b.number);
bool JS_EQ_SWITCH (void **op_cache_ptr, JSValue a, JSValue b) {
	if (a.tag == JS_NUMBER_TAG && b.tag == JS_NUMBER_TAG) {
		*op_cache_ptr = &JS_EQ_NUMBER_NUMBER;
	}
	return ((js_op_ptr) *op_cache_ptr)(op_cache_ptr, a, b);
}

JS_OR_VARIANT(BOOL_BOOL, a.tag != b.tag, a.number || b.number);
bool JS_OR_SWITCH (void **op_cache_ptr, JSValue a, JSValue b) {
	if (a.tag == JS_BOOL_TAG && b.tag == JS_BOOL_TAG) {
		*op_cache_ptr = &JS_OR_BOOL_BOOL;
	}
	return ((js_op_ptr) *op_cache_ptr)(op_cache_ptr, a, b);
}

JS_ADD_VARIANT(DOUBLE_DOUBLE, a.tag != b.tag, JS_NUMBER(a.number + b.number));
JS_ADD_VARIANT(DOUBLE_STRING, a.tag != JS_NUMBER_TAG || b.tag != JS_STRING_TAG, JS_STRING(HPRINTF("%f%s", a.number, b.string)));
JSValue JS_ADD_SWITCH (void **op_cache_ptr, JSValue a, JSValue b) {
	if (a.tag == JS_NUMBER_TAG && b.tag == JS_NUMBER_TAG) {
		*op_cache_ptr = &JS_ADD_DOUBLE_DOUBLE;
	} else if (a.tag == JS_NUMBER_TAG && b.tag == JS_STRING_TAG) {
		*op_cache_ptr = &JS_ADD_DOUBLE_STRING;
	}
	return ((jsvalue_op_ptr) *op_cache_ptr)(op_cache_ptr, a, b);
}

JS_SUB_VARIANT(DOUBLE_DOUBLE, a.tag != b.tag, JS_NUMBER(a.number - b.number));
JSValue JS_SUB_SWITCH (void **op_cache_ptr, JSValue a, JSValue b) {
	if (a.tag == JS_NUMBER_TAG && b.tag == JS_NUMBER_TAG) {
		*op_cache_ptr = &JS_SUB_DOUBLE_DOUBLE;
	}
	return ((jsvalue_op_ptr) *op_cache_ptr)(op_cache_ptr, a, b);
}

JS_MUL_VARIANT(DOUBLE_DOUBLE, a.tag != b.tag, JS_NUMBER(a.number * b.number));
JSValue JS_MUL_SWITCH (void **op_cache_ptr, JSValue a, JSValue b) {
	if (a.tag == JS_NUMBER_TAG && b.tag == JS_NUMBER_TAG) {
		*op_cache_ptr = &JS_MUL_DOUBLE_DOUBLE;
	}
	return ((jsvalue_op_ptr) *op_cache_ptr)(op_cache_ptr, a, b);
}

/** 
 * Globals
 */

JS_DEFN(console_log) {
	VARGS(VARG(str));
	printf("%s\n", JSValue_STR(str));
	return JS_NULL;
}

JSValue console;
JSValue _object_prototype;

void initialze_globals() {
	// Object prototypes.
	_object_prototype = ((JSValue) {JS_OBJECT_TAG, {.object = hashmap_new()}});

	// Setup console.
	console = JS_OBJECT();
	JS_SET_PROP(console, "log", console_log);
}

void destroy_globals() {
	JS_OBJECT_FREE(console);
}

JSValue module_0;

// TODO (Separate js_main function): This should be in its own file.
int js_main () {
	initialze_globals();
	JS_CALL_FUNC(module_0);
	destroy_globals();
	return 0;
}

var falafel = require('falafel');

/**** SOURCE ****/

function fac (n) {
	if (n == 1 || n == 0) {
		return 1;
	}
	return n * fac(n-1);
}

function module_0 () {
	console.log(fac(5));
}

/**** COMPILER ****/

function translate (fn) {
	return falafel(String(fn), function (node) {
		switch (node.type) {
			case 'Literal':
				switch (typeof node.value) {
					case 'number': node.update('JS_NUMBER(' + node.value + ')'); break;
				}
				break;
			case 'BinaryExpression':
			case 'LogicalExpression':
				var prefix = '', middle = ', '; suffix = '';
				switch (node.operator) {
					case '+': prefix = 'JS_ADD('; suffix = ')'; break;
					case '-': prefix = 'JS_SUB('; suffix = ')'; break;
					case '*': prefix = 'JS_MUL('; suffix = ')'; break;
					case '/': prefix = 'JS_DIV('; suffix = ')'; break;
					case '==': prefix = 'JS_BOOL(JS_EQ('; suffix = '))'; break;
					case '||': prefix = 'JS_BOOL(JS_OR('; suffix = '))'; break;
				}
				node.update(prefix + node.left.source() + middle + node.right.source() + suffix)
				break;
			case 'IfStatement':
				node.update('if (' + node.test.source() + '.boolean) ' + node.consequent.source());
				break;
			case 'FunctionDeclaration':
				node.update('JS_DEFN(' + node.id.source() + ') {' + 
					(!node.params.length ? '' : 
						'\n\tVARGS(' + node.params.map(function (arg) { return 'VARG(' + arg.source() + ')'; }).join(' ') + ');') + 
					node.body.source().substr(1));
				break;
			case 'CallExpression':
				if (node.callee.type == 'MemberExpression') {
					node.update('JS_CALL_METHOD(' + [node.callee.object.source(), JSON.stringify(node.callee.property.source())].concat(node.arguments.map(function (arg) { return arg.source(); })).join(', ') + ')');
				} else {
					node.update('JS_CALL_FUNC(' + [node.callee.source()].concat(node.arguments.map(function (arg) { return arg.source(); })).join(', ') + ')');
				}
				break;
			case 'MemberExpression':
			case 'BlockStatement':
			case 'ReturnStatement':
			case 'Identifier':
			case 'Program':
			case 'ExpressionStatement':
				break;
			default:
				console.log(node.type);
		}
	});
}

console.log('#include "gum.h"')
console.log()
console.log(translate(fac));
console.log()
console.log(translate(module_0));
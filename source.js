function fac (n) {
	if (n == 1 || n == 0) {
		return 1;
	}
	return n * fac(n-1);
}

function module_0 () {
	var a = {b: 5};
	console.log(fac(a.b));
}
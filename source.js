function fac (n) {
	if (n == 1 || n == 0) {
		return 1;
	}
	return n * fac(n-1);
}

function module_0 () {
	console.log(fac(5));
}
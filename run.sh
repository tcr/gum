node compile.js > test.c
gcc hashmap.c test.c -O3 -o test && strip test && ./test

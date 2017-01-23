#include <stdio.h>
void global_func()
{
	extern int x;
	x = 4;
	printf("function's x value is %d\n", x);
	printf("function's x address is %p\n", &x);
}

void global_func2()
{
	extern int x;
	x = 8;
	printf("function's x value is %d\n", x);
	printf("function's x address is %p\n", &x);
}
#include <stdio.h>
int x = 9;
void global_func();
void global_func2();
int main()
{
	printf("Pre main's x value is %d\n", x);
	printf("Pre main's x address is %p\n", &x);
	global_func();
	printf("Post main's x value is %d\n", x);
	printf("Post main's x address is %p\n", &x);
	global_func2();
	
}
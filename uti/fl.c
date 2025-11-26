/* Format and List all files */
#include <stdio.h>
#include <stdbool.h>

#include "libarg.h"

void reg(void)
{
	argo("default", "a", "all");
}

void guide(void)
{
	printf("this is an unfinished guide.......\n");
}

struct runtime {
	int a;
	char b;
};

int main(int argc, char **argv)
{
	argi("fl", argc, argv, reg, guide); 
	bool tmp = arge("default", "all");
	printf("%d\n", tmp);

	return 0;
};

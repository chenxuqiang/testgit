#include <stdio.h>
#include <stdlib.h>

void clear(char *str)
{
	free(str);
	str = NULL;
}

int main(void)
{
	char *str;
	str = (char *)malloc(100);
	clear(str);

	return 0;
}
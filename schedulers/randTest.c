#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
	srand((unsigned)time(NULL));
	for(int i = 0; i < 10 ;i++)
	{
		printf("%d\t", rand()%100 + 1);
	}

	return 0;
}
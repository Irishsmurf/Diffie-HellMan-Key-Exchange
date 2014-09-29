#include <stdlib.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
	int i;
	for(i = 0; i < argc; i++)
	{
		if(*argv[i] == 'C')
			printf("OH MAH GAWDS, ITS A C ->>> ");
		printf("argv[%d] = %s\n", i, argv[i]);
	}

	return 0;
}

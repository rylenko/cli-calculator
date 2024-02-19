#include <stdio.h>
#include <stdlib.h>
#include "calculator.h"
#include "error.h"

int
main(int argc, const char **argv)
{
	if (argc != 2)
		error(1, "Invalid arguments count.");
	printf("%ld\n", calculate(argv[1]));
	return EXIT_SUCCESS;
}

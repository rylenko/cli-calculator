#include <stdio.h>
#include <stdlib.h>
#include "error.h"

void
error(int code, char *reason)
{
	fprintf(stderr, "Error: %s\n", reason);
	exit(code);
}

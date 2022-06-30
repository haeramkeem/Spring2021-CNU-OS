#include "io.h"

void read_file(FILE* fp) {
	char buf[10] = {0,};

	fseek(fp, 0, SEEK_SET);
	fscanf(fp, "%s", buf);
	printf("read = %s\n", buf);
}

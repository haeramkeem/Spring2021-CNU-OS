#include <stdio.h>

void write_file(FILE*);
void read_file(FILE*);

int main() {
	FILE *fp = NULL;

	fp = fopen("./test.txt", "w+");
	if(fp == NULL) {
		printf("file open error!\n");
		return -1;
	}

	write_file(fp);
	read_file(fp);

	return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]){
	printf("\n[proces potomny] Argumenty procesu potomnego: ");
	for (int i = 0; i < argc; i++)
		printf("%s\t", argv[i]);
	printf("\n");
}


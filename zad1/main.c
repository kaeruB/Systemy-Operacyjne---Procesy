#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>



int main(int argc, char *argv[]){
	//extern char** environ;
	char* name;
	//int i = 0;
	
	if (argc != 2){
		printf("\nJako argument wywolania nalezy wpisac nazwe zmiennej srodowiskowej.\n");
		exit(1);
	}

	name = argv[1];

/*	printf("\n\n\n%s\n\n\n", name);
	
	while (environ[i] != NULL){
		printf("%s\n", environ[i]);
		i++;
	}
	printf("\n\n\n\n\n");
*/

	name[strlen(name) - 1] = '\0';
	
	if (getenv(name) == NULL) printf("\nNie ma takiej zmiennej srodowiskowej w srodowisku programu.\n");
	else printf("\nWartosc zmiennej srodowiskowej %s: %s\n\n", name, getenv(name));
	
	return (0);
}


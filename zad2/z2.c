#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>


static const int MAXEL = 5;

int main(int argc, char *argv[]){
	FILE* file;
	ssize_t check;
	char* line = NULL;
	size_t length = 0;
	int pom = 0;
	//extern char** environ;
	char* name;
	char* value;
	int checkEnv, checkPr;
	pid_t processID;
	char* argTab[MAXEL];  
	int i, k;
	char charactersToSkip[] = " #\n";
	int wCheck;
	struct rusage rusageStruct;
	int cpuL, memoryL;
	struct rlimit* rlimitStruct;
	int setLimitCheck;
	float timeToSubstract1 = 0;
	float timeToSubstract2 = 0;
	
	
	k = MAXEL;
	while (k > 0){
		k--;
		argTab[k] = '\0';
	}

	

	if (argc != 4 || atoi(argv[3]) < 0 || atoi(argv[2]) < 0){
		printf("\nJako argument wywolania nalezy wpisac:\n [sciezka do pliku] [czas w s] [pamiec w MB]\n");
		exit(1);
	}
	cpuL = atoi(argv[2]);
	memoryL = atoi(argv[3]);
	//na B	
	memoryL *= (1024 * 1024);

	file = fopen(argv[1], "r");
	if (file == NULL){
		printf("\nNie udalo sie otworzyc pliku.\nZla sciezka.\n");
		exit(1);
	}

	
	
	//wczytujemy po linijce tekst z pliku
	while ((check = getline(&line, &length, file)) != -1){
		if (line[0] == '\n') continue;
		//printf("%c\n", line[0]);
		if (line[0] == '#') {
			while (line[pom] != '\0' && line[pom] != ' ') {
				pom++;
			}
				
			//zmienna srodowiskowa do usuniecia 
			if (line[pom] == '\0') {
				name = strtok (line, charactersToSkip);		//oddzielamy z line nazwe zmiennej
				//sprawdzam, czy taka istnieje
				//name[strlen(name) - 1] = '\0';
				if (getenv(name) == NULL)
					printf("\nZmienna %s nie istnieje, nie moze zostac usunieta.\n", name);
		
				else {
					checkEnv = unsetenv(name);
					if (checkEnv == -1) printf("\nNie udalo sie usunac zmiennej %s\n", name);
					else printf("\nUsunieto zmienna srodowiskowa: %s\n", name);
				}
			}
			//zmienna srodowiskowa postaci #nazwa wartosc
			else {
				name = strtok (line, charactersToSkip);
				value = strtok (NULL, charactersToSkip);	//oddzielamy z line wartosc zmiennej
			
				//name[strlen(name)] = '\0';
				
				//nie istnieje
				if (getenv(name) == NULL) {
					checkEnv = setenv(name, value, 0);	//dodanie do srodowiska zmiennej sr.
					if (checkEnv == -1) printf("\nNie udalo sie ustawic zmiennej %s\n", name);
					else if (checkEnv == 0) printf("\nUdalo sie dodac zmienna %s.\n", name);
					
				}
				//istnieje, podmieniamy wartosc
				else {
					checkEnv = setenv(name, value, 1); //zeby ustawilo bezwarunkowo
					if (checkEnv == -1) printf("\nNie udalo sie ustawic zmiennej %s\n", name);
				}
			}
			
		}	
		else { 
			argTab[0] = strtok (line, charactersToSkip);
			name = argTab[0];
			i = 1;
			
			while (((value = strtok(NULL, charactersToSkip)) != NULL) && (i < MAXEL)){
				argTab[i] = value;
				i++;
			}
 		
			processID = fork();
			//proces potomny
			if (processID == 0){
				rlimitStruct = calloc(1, sizeof(struct rlimit));

				rlimitStruct -> rlim_max = (unsigned) memoryL;
				rlimitStruct -> rlim_cur = (unsigned) memoryL;

				setLimitCheck = setrlimit(RLIMIT_AS, rlimitStruct);
				free(rlimitStruct);
				if (setLimitCheck != 0) {
					printf("\nNie udalo sie ustawic limitu na rozmiar pamieci wirtualnej.\n");
					exit(1);
				}
				

				rlimitStruct = calloc(1, sizeof(struct rlimit));
				rlimitStruct -> rlim_max = (unsigned) cpuL;
				rlimitStruct -> rlim_cur = (unsigned) cpuL;
				setLimitCheck = setrlimit(RLIMIT_CPU, rlimitStruct);
				free(rlimitStruct);
				if (setLimitCheck != 0) {
					printf("\nNie udalo sie ustawic limitu na dostep do procesora\n");
					exit(1);
				}

				
				checkPr = execvp(name, argTab);
				if (checkPr == -1){
					printf("\nexecvp error\n");
					exit(1);
				}
			}
			//proces macierzysty
			else if (processID > 0) {
				wait(&wCheck);
				getrusage(RUSAGE_CHILDREN, &rusageStruct);
			
				if (!WIFEXITED(wCheck)) printf ("\nProces %s zostal zakonczony z bledem %d\n", name, WEXITSTATUS(wCheck));

				printf("\n%s:\nPamiec: %dMB\nCzasy:\n-systemowy: %fs\n-uzytkownika: %fs\n", name, (int) rusageStruct.ru_maxrss/1024 ,(float) rusageStruct.ru_stime.tv_sec - timeToSubstract1,(float) rusageStruct.ru_utime.tv_sec - timeToSubstract2);				
				timeToSubstract1 = (float) rusageStruct.ru_stime.tv_sec;
				//printf("\nto subs1%f, %f", timeToSubstract1, (float) rusageStruct.ru_stime.tv_sec);
				timeToSubstract2 = (float) rusageStruct.ru_utime.tv_sec;
				//printf("\nto subs2%f, %f", timeToSubstract2, (float) rusageStruct.ru_utime.tv_sec);
				
			}
			//blad
			else {
				printf("\nBlad, nie udalo sie przeprowadzic procesu potomnego.\n");
				printf("Nazwa nieudanego procesu: %s\n", name);
				exit(1);
			}
			k = MAXEL;
			while (k > 0){
				k--;
				argTab[k] = '\0';
			}
		}	
	}
	

	fclose(file);
	return (0);
}


#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[])
{
	char tab[5000001];
	int i = 0;
	while (i < 5000001) {
		tab[i] = 's';
		i++;
	}
	while (i > 1){
		if(tab[i] == tab[i-1]) tab[i] = tab[i-1];
		i--;
	}
	while (1) {}
}

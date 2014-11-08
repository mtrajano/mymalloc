#include "../mymalloc.h"
#include <stdio.h>
#include <time.h>

/*
 TEST: SIMPLE TEST
 */

int main(int argc, char **argv){
	printf("Regular tests that creates small fragments, then large, then a mixture of both:\n");
	
	printf("\nCreate as many 10 byte chunks as we can:\n");
	char *parray[1000];
	int nump=0;
	while((parray[nump] = malloc(10)) != NULL){
		nump++;
	}
	
	int i=0;
	while(i<nump){
		free(parray[i]);
		i++;
	}
	printf("\nNumber of pointers created: %d\n", nump);
	printf("Number of pointers freed: %d\n", i);


	printf("\nCreate as many 500 byte chunks as we can:\n");
	nump=0;
	while((parray[nump] = malloc(500)) != NULL){
		nump++;
	}
	
	i=0;
	while(i<nump){
		free(parray[i]);
		i++;
	}
	printf("\nNumber of pointers created: %d\n", nump);
	printf("Number of pointers freed: %d\n", i);


	printf("\nRandom allocations of powers of 2:\n");
	nump=0;
	srand(time(NULL));
	int exp;
	unsigned int base;
	while(1){
		base = 1;
		exp = rand() % 11;
		base <<= exp;

		if((parray[nump] = malloc(base)) == NULL)
			break;
		nump++;
	}
	
	i=0;
	while(i<nump){
		free(parray[i]);
		i++;
	}
	printf("\nNumber of pointers created: %d\n", nump);
	printf("Number of pointers freed: %d\n", i);
	
	
}
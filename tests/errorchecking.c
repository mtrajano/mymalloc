#include "../mymalloc.h"
#include <stdio.h>

/*
 TEST: TEST FOR THE SIMPLE ERRORS REQUIRED
 */

int main(int argc, char **argv){
	printf("Tests the common errors that malloc could run into.. Such as:\n");
	printf("\nAsking for more memory than available in the virtual memory:\n");
	char *p = (char *) malloc(6000);

	printf("\nFreeing a NULL pointer:\n");
	free(NULL);

	printf("\nFreeing variables that were never allocated:\n");
	int x;
	free(x);

	printf("\nWorks for unrelated pointers as well:\n");
	free(&x);

	printf("\nFreeing memory that was never allocated by malloc:\n");
	p = (char *) malloc(50);
	free(p+10);

	printf("\nRedundant freeing of a pointer:\n");
	printf("First free:\n");
	free(p);
	printf("Second free:\n");
	free(p);

	printf("\nAllocating the pointer again gives us the correct result though:\n");
	p = (char *) malloc(50);
	free(p);
	printf("And once again:\n");
	p = (char *) malloc(50);
	free(p);

	printf("\nAnd finally saturation:\n");
	int i;
	for (i = 0; i < 50; i++){
		p = malloc(100);
	}
}
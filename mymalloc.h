#ifndef MYMALLOC_H
#define MYMALLOC_H

/*
 * Prints a trace of the code, where memory was allocated, etc..
 * Uncomment to see trace messages, comment out to silence the code
 */
#define DEBUG 

/*
 * Suppresses warnings for required test cases
 * Such as passing integer to free or passing an unitialized variable
 * Was commented out until I felt confident with  my code, works as expected
 * Comment out the supress definition to bring the warnings back
 */
#define SUPRESS


#ifdef SUPRESS
#pragma GCC diagnostic ignored "-Wint-conversion"
#pragma GCC diagnostic ignored "-Wuninitialized"
#endif

#ifdef DEBUG
#define DEBUG_VARIABLES(fmt, args...) printf(fmt, args)
#define PRINT_TRACE(fmt) printf(fmt)
#else
#define DEBUG_VARIABLES(fmt, args...)
#define PRINT_TRACE(fmt, args...)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//macros redefine alloc functions and report location of error
#define malloc(x) mymalloc((x), __FILE__, __LINE__)
#define calloc(x, y) mycalloc((x), (y), __FILE__, __LINE__)
#define realloc(x, y) myrealloc((x), (y), __FILE__, __LINE__)
#define free(x) myfree((x), __FILE__, __LINE__)

//debugging and error macros
#define ALLOCERR(err,file,line) printf(err, file, line)

//maximum size of simulated virtual memory
#define MAX_MEM_SIZE 5000

typedef struct header_struct{
	struct header_struct *prev; 
	struct header_struct *next;
	unsigned int size;
	bool isAllocated;
	unsigned short typeflag;
}header;

/*
 * 2 byte of overhead
 * Can help prevent incorrect casting from free
 */
#define DIRTYFLAG 0x1234
#define SETTYPE(p) (p->typeflag = DIRTYFLAG)
#define CHECKTYPE(p) (((p->typeflag) == DIRTYFLAG) ? (1):(0))
#define DELTYPE(p) (p->typeflag = 0)


void *mymalloc(unsigned int memsize, char *file, unsigned int line);

void myfree(void *p, char *file, unsigned int line);

void split(unsigned int memsize, header **ptr);

#endif

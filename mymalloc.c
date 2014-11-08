#include "mymalloc.h"

//virtual memory (scope limited to this file)
static char virtualmem[MAX_MEM_SIZE];

void *mymalloc(unsigned int memsize, char *file, unsigned int line){
	static bool isInitialized = false;
	static header *memhead; //points to the small bytes
	header *traverse; //looks through free list for appropriate size
	void *retpointer; //pointer to the list returned

	//first call to function initialize free list etc..
	if(!isInitialized){
		isInitialized = true;

		//initialize a free block that holds all memory (will be split as necessary)
		memhead = (header *) &virtualmem;
		memhead->next = memhead->prev = NULL;
		memhead->size = (MAX_MEM_SIZE - sizeof(header));
		memhead->isAllocated = false;
		SETTYPE(memhead);
	}

	//size allocated cannot be larger than the 5KB we contain
	if(memsize > MAX_MEM_SIZE){
		ALLOCERR("ERROR: Size requested is larger than ammount of virtual memory. FILE: \"%s\" LINE:%u\n", file, line);
		return NULL;
	}

	traverse = memhead;

	do{
		//look for chunk larger than size requested, must not be allocated
		if((traverse->size < memsize) && !traverse->isAllocated)
			traverse = traverse->next;
		else{
			//chunk of memory is a good fit for the requested size
			if(traverse->size <= (memsize + sizeof(header)) && !traverse->isAllocated){
				DEBUG_VARIABLES("Size requested of (%u) fit well in the block\n", memsize);
				traverse->isAllocated = true;
				traverse->size = memsize;
				retpointer = (void *)((char *)traverse + sizeof(header));
				return retpointer;
			}
			else if(!traverse->isAllocated){ //split the large chunk
				DEBUG_VARIABLES("Size requested (%u), while the block was of size(%u), splitting the block\n", memsize, traverse->size);
				split(memsize, &traverse);

				traverse->isAllocated = true;
				traverse->size = memsize;
				retpointer = (void *)((char *)traverse + sizeof(header));
				return retpointer;
			}
			else{//memory block is being used
				traverse = traverse->next;
			}
		}
	}while(traverse != NULL);
	PRINT_TRACE("Reached the end of the loop, no memory was found for requested size\n");

	//traverse reached the end of the list
	//and so a suitable block of memory was not found
	ALLOCERR("ERROR: No more memory available. FILE: \"%s\" LINE:%u\n", file, line);
	return NULL;
}

void myfree(void *ptr, char *file, unsigned int line){
	//basic error checking for free
	if(ptr == NULL){
		ALLOCERR("ERROR: Undefined behavior, cannot free NULL pointer. FILE: \"%s\" LINE:%u\n", file, line);
		return;
	}

	//check bounds and if valid pointer
	header *ptrblk = (header *)ptr - 1;
	if(((size_t) ptrblk < (size_t) virtualmem) || (size_t) ptrblk > (size_t) (virtualmem + MAX_MEM_SIZE)){
		ALLOCERR("ERROR: Freeing illegal memory. FILE: \"%s\" LINE:%u\n", file, line);
		return;
	}
	else if(!(CHECKTYPE(ptrblk)) || !ptrblk->isAllocated){
		ALLOCERR("ERROR: Trying to free unallocated pointer. FILE: \"%s\" LINE:%u\n", file, line);
		return;
	}

	//free pointer
	PRINT_TRACE("Passed basic requirements for free, now freeing the pointer\n");
	ptrblk->isAllocated = false;
	DEBUG_VARIABLES("Check: TYPEFLAG: (%hx) ADDR: (0x%zx) NADDR: (0x%zx) PADDR: (0x%zx) VADDR(0x%zx)\n",ptrblk->typeflag, (size_t)ptrblk, (size_t)ptrblk->next, (size_t)ptrblk->prev, (size_t)virtualmem);
	DELTYPE(ptrblk);

	/*
     * Check for coalescing
     * CASES: (only one of these 4 is possible)
     *	1: previous and next block are not allocated
     *	2: previous block is not allocated but next is
     *	3: previous block is allocated but next isnt
     *	4: previous and next blocks are allocated
     */
	header *prevb = ptrblk->prev;
	header *nextb = ptrblk->next;
	if((prevb != NULL) && !prevb->isAllocated){
		if((nextb != NULL) && !nextb->isAllocated){ //case 1
			PRINT_TRACE("Both the previous and next pointers were free, coalescing both with pointer\n");
			prevb->size += (sizeof(header) + ptrblk->size);
			prevb->size += (sizeof(header) + nextb->size);
			
			//free memory back to list
			prevb->next = nextb->next;
			if(nextb->next != NULL) //join it to the previous
				(nextb->next)->prev = prevb;
			return;
		}
		else{//case 2
			PRINT_TRACE("Only the previous pointer was free, coalescing it with pointer\n");
			prevb->size += (sizeof(header) + ptrblk->size);

			prevb->next = nextb;
			if(nextb != NULL)
				nextb->prev = prevb;
			return;
		}
	}
	else if((nextb != NULL) && !nextb->isAllocated){//case 3
		PRINT_TRACE("Only the next pointer was free, coalescing it with pointer\n");
		ptrblk->size += (sizeof(header) + nextb->size);

		ptrblk->next = nextb->next;
		if(nextb->next != NULL)
			(nextb->next)->prev = ptrblk;
		return;
	}
	else{//case 4
		PRINT_TRACE("Neither the previous nor the next pointer were free, no coalescing was performed\n");
		return;
	}
}

/*
 * Extra credit
 */
void *mycalloc(unsigned int num, unsigned int size, char *file, unsigned int line){
    char *retptr = mymalloc((num * size), file, line); 
    if(!retptr){
        return NULL;
    }
    else{
        header *blk_header = (header *)retptr - 1;
        for(int i=0; i<blk_header->size; i++){
            retptr[i] = 0;
        }
        return retptr;
    }
}

void *myrealloc(void *ptr, unsigned int size, char *file, unsigned int line){
    if(!ptr){
        return mymalloc(size, file, line);
    }

    header *blk_header = (header *)ptr - 1;

    /*
     * One way to handle if size requested is smaller
     * than current block is to simply return the block
     * back to the caller
     */
    if(blk_header->size >= size){
        return ptr;
    }
    else{
        void *retptr = mymalloc(size, file, line);
        if(!retptr){
            return NULL;
        }
        else{
            memcpy(retptr, ptr, blk_header->size);
            myfree(ptr, file, line);
            return retptr;
        }
    }
}


/*
 * Helper functions
 */
void split(unsigned int memsize, header **ptr){
	header *newchunk;

	//add new chunk to the list of blocks
	newchunk = (header *)((char *)(*ptr) + sizeof(header) + memsize);
	newchunk->next = (*ptr)->next;
	newchunk->prev = (*ptr);
	(*ptr)->next = newchunk;

	//have to be careful in the situation where ptr->next points to null;
	if((*ptr)->next != NULL){
		((*ptr)->next)->prev = newchunk;
	}

	//update header for the newchunk
	newchunk->size = (*ptr)->size - (sizeof(header) + memsize);
	newchunk->isAllocated = false;
	SETTYPE(newchunk);
}

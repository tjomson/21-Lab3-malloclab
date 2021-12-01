/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "group12",
    /* First member's full name */
    "Thomas Hoffmann Kilbak",
    /* First member's email address */
    "thhk@itu.dk",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define ISCOLORED(i)  (i & 1)

#define DECOLOR(i)   (i & ~1)

#define COLOR(i)   (i | 1)

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    mem_init();
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    // void *start = mem_heap_lo();
    // printf("start address: %p\n", start);
    // printf("start value: %ld\n", *(long*)start);


/*
    printf("lmaooo\n");
    void *maxheap = mem_heap_hi();
    printf("maxheap: %p\n", maxheap);
    void *current = start;
    printf("bruh\n");
    while (current < maxheap) {
        long header = *(long*) current;
        current = (void*) DECOLOR(header);
        if (!OCCUPIED(header) && DECOLOR(header) > newsize) {
            printf("found!!");
            *(long*) current = COLOR(newsize);
            void *footer = current + (newsize - SIZE_T_SIZE);
            *(long*) footer = COLOR(newsize);
            return current + SIZE_T_SIZE;
        }
    }

    printf("current address: %p\n", current);

    printf("hej\n");
    void *newptr = mem_sbrk(newsize);
    printf("yoo\n");
    *(long*) newptr = COLOR(newsize);
    printf("new pointer value: %ld\n", *(long*) newptr);
    void *footer = newptr + (newsize - SIZE_T_SIZE);
    printf("footer address: %p\n", footer);
    *(long *) footer = COLOR(newsize);
    printf("new pointer: %p\n", newptr);
    return newptr;
*/


    // *(int *) start = newsize + 1;
    // printf("start value3: %ld\n", *(long*)start);

    // void *footer = start + newsize + SIZE_T_SIZE + SIZE_T_SIZE;
    // printf("footer address: %p\n", footer);
    // *(int*)footer = newsize + 1;
    // printf("footer value: %ld\n", *(long*)footer);

    // printf("malloc returned: %p\n", start + SIZE_T_SIZE);
    // printf("FINISHED MALLOC \n\n");
    // return start + SIZE_T_SIZE;

    int newsize = ALIGN(size + SIZE_T_SIZE + SIZE_T_SIZE); // the bytes for payload-size and SIZE_T_SIZE for the header and footer, aligned to 8 bytes
    void *current = mem_heap_lo();
    void *max = mem_heap_hi();

    while (current < max) {
        long header = *(long *) current;
        if (header == 0) break;
        current += header;
        if (!ISCOLORED(header) && newsize <= header) {
            void *footer_ptr = current + header - SIZE_T_SIZE;
            if (newsize + (3 * SIZE_T_SIZE) > header) { // if there is not enough free space to also fit a free block
                *(long *) current = COLOR(header);
                *(long *) footer_ptr = COLOR(header);
            } else {
                void *new_footer_ptr = current + newsize - SIZE_T_SIZE;
                *(long *) current = COLOR(newsize);
                *(long *) new_footer_ptr = COLOR(newsize);

                // create free block afterwards
                long other_block_size = header - newsize;
                void *other_header_ptr = current + newsize;
                *(long *) footer_ptr = DECOLOR(other_block_size);
                *(long *) other_header_ptr = DECOLOR(other_block_size);
            }
            return current + SIZE_T_SIZE;
        }
    }


    void *newptr = mem_sbrk(newsize);
    if (newptr == (void *)-1) {
        return NULL;
    }
    
    *(size_t *)newptr = COLOR(newsize); // mark on the header that the memory is being used

    void *footer = newptr + newsize - SIZE_T_SIZE; // footer starts SIZE_T_SIZE bytes from the end of the new memory
    *(size_t *) footer = COLOR(newsize); // mark on the footer that the memory is being used

    void *ptr_to_return = newptr + SIZE_T_SIZE; // payload starts after the header
    return ptr_to_return;
    
    
}

long dereference(void *ptr) {
    return DECOLOR(*(long *) ptr);
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    void *header = ptr - SIZE_T_SIZE;
    void *footer = header + dereference(header) - SIZE_T_SIZE;

    void *prev_footer = header - SIZE_T_SIZE;
    void *next_header = header + dereference(header);

    int coalesce_prev = !ISCOLORED(*(long *) prev_footer) && header > mem_heap_lo();
    int coalesce_next = !ISCOLORED(*(long *) next_header) && footer < mem_heap_hi();

    void *prev_header = header - DECOLOR(*(long *) prev_footer);
    void *next_footer = prev_footer + dereference(next_header);

    if (coalesce_prev && coalesce_next) {
        *(long *) prev_header = DECOLOR(dereference(prev_footer) + dereference(header) + dereference(next_header));
        *(long *) next_footer = dereference(prev_header);
    } else if (coalesce_prev) {
        *(long *) prev_header = DECOLOR(dereference(prev_footer) + dereference(header));
        *(long *) footer = dereference(prev_header);
    } else if (coalesce_next) {
        *(long *) header = DECOLOR(dereference(header) + dereference(next_header));
        *(long *) next_footer = dereference(header);
    } else {
        *(long *) header = DECOLOR(*(long *) header);
        *(long *) footer = dereference(header);
    }

}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}















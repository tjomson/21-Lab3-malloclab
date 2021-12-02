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

void *heaplo;

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    heaplo = mem_heap_lo();
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    int newsize = ALIGN(size + SIZE_T_SIZE + SIZE_T_SIZE); // the bytes for payload-size and SIZE_T_SIZE for the header and footer, aligned to 8 bytes
    void *current = heaplo;
    void *max = mem_heap_hi();

    while (current < max) {
        size_t header = *(size_t *) current;
        // printf("check header %lx\n", header);
        if (header == 0) break;
        // printf("current new addr: %p\n", current);
        if (!ISCOLORED(header) && newsize <= DECOLOR(header)) {
            // printf("found\n");
            void *footer_ptr = current + header - SIZE_T_SIZE;
            if (newsize + (3 * SIZE_T_SIZE) > header) { // if there is not enough free space to also fit a free block
                // printf("no room for free block\n");
                *(size_t *) current = COLOR(header);
                *(size_t *) footer_ptr = COLOR(header);
            } else {
                void *new_footer_ptr = current + newsize - SIZE_T_SIZE;
                *(size_t *) current = COLOR(newsize);
                *(size_t *) new_footer_ptr = COLOR(newsize);

                // create free block afterwards
                size_t other_block_size = header - newsize;
                void *other_header_ptr = current + newsize;
                *(size_t *) footer_ptr = DECOLOR(other_block_size);
                *(size_t *) other_header_ptr = DECOLOR(other_block_size);
            }
            // printf("return addr: %p\n", current + SIZE_T_SIZE);
            return current + SIZE_T_SIZE;
        }
        current += DECOLOR(header);
    }

    // printf("expand heap\n");

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

size_t dereference(void *ptr) {
    return DECOLOR(*(size_t *) ptr);
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

    int coalesce_prev = !ISCOLORED(*(size_t *) prev_footer) && header > mem_heap_lo();
    int coalesce_next = !ISCOLORED(*(size_t *) next_header) && (footer + SIZE_T_SIZE) < mem_heap_hi();

    void *prev_header = header - dereference(prev_footer);
    void *next_footer = next_header + dereference(next_header) - SIZE_T_SIZE;
    if (coalesce_prev && coalesce_next) {
        *(size_t *) prev_header = dereference(prev_footer) + dereference(header) + dereference(next_header);
        *(size_t *) next_footer = dereference(prev_header);
    } else if (coalesce_prev) {
        *(size_t *) prev_header = dereference(prev_footer) + dereference(header);
        *(size_t *) footer = dereference(prev_header);
    } else if (coalesce_next) {
        *(size_t *) header = dereference(header) + dereference(next_header);
        *(size_t *) next_footer = dereference(header);
    } else {
        *(size_t *) header = dereference(header);
        *(size_t *) footer = dereference(header);
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















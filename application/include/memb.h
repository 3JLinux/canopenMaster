#ifndef MEMB_H
#define MEMB_H

#include <unistd.h>

#define CC_CONCAT2(s1, s2) s1##s2
/**
 * A C preprocessing macro for concatenating to
 * strings.
 *
 * We need use two macros (CC_CONCAT and CC_CONCAT2) in order to allow
 * concatenation of two #defined macros.
 */
#define CC_CONCAT(s1, s2) CC_CONCAT2(s1, s2)

#define MEMB(name, structure, num) \
        static char CC_CONCAT(name,_memb_count)[num]; \
        static structure CC_CONCAT(name,_memb_mem)[num]; \
        static struct memb name = {sizeof(structure), num, \
                                          CC_CONCAT(name,_memb_count), \
                                          (void *)CC_CONCAT(name,_memb_mem)}
/*
#define MEMB(name, structure, num)
{
	static char (name_memb_count)[num];
	static structure (name_memb_mem)[num];
    static struct memb name = {sizeof(structure), num, (name_memb_count), (void *)(name_memb_mem)}	
}
*/                                          

struct memb {
  unsigned short size;
  unsigned short num;
  char *count;
  void *mem;
};

/**
 * Initialize a memory block that was declared with MEMB().
 *
 * \param m A memory block previously declared with MEMB().
 */
void  memb_init(struct memb *m);

/**
 * Allocate a memory block from a block of memory declared with MEMB().
 *
 * \param m A memory block previously declared with MEMB().
 */
void *memb_alloc(struct memb *m);

/**
 * Deallocate a memory block from a memory block previously declared
 * with MEMB().
 *
 * \param m m A memory block previously declared with MEMB().
 *
 * \param ptr A pointer to the memory block that is to be deallocated.
 *
 * \return The new reference count for the memory block (should be 0
 * if successfully deallocated) or -1 if the pointer "ptr" did not
 * point to a legal memory block.
 */
signed char  memb_free(struct memb *m, void *ptr);

int memb_inmemb(struct memb *m, void *ptr);

int  memb_numfree(struct memb *m);



#endif

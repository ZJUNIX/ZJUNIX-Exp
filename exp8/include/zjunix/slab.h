#ifndef _ZJUNIX_SLAB_H
#define _ZJUNIX_SLAB_H

#include <zjunix/list.h>
#include <zjunix/buddy.h>

#define SIZE_INT 4
#define SLAB_AVAILABLE 0x0
#define SLAB_USED 0xff

/*
 * slab_head makes the allocation accessible from end_ptr to the end of the page
 * @end_ptr : points to the head of the rest of the page
 * @nr_objs : keeps the numbers of memory segments that has been allocated
 */
struct slab_head {
    void *end_ptr;
    unsigned int nr_objs;
};

/*
 * slab pages is chained in this struct
 * @partial keeps the list of un-totally-allocated pages
 * @full keeps the list of totally-allocated pages
 */
struct kmem_cache_node {
    struct list_head partial;
    struct list_head full;
};

/*
 * current being allocated page unit
 */
struct kmem_cache_cpu {
    void **freeobj;  // points to the free-space head addr inside current page
    struct page *page;
};

struct kmem_cache {
    unsigned int size;
    unsigned int objsize;
    unsigned int offset;
    struct kmem_cache_node node;
    struct kmem_cache_cpu cpu;
    unsigned char name[16];
};

// extern struct kmem_cache kmalloc_caches[PAGE_SHIFT];
extern void init_slab();
extern void *kmalloc(unsigned int size);
extern void kfree(void *obj);

#endif

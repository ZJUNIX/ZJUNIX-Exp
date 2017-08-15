#ifndef _ZJUNIX_BUDDY_H
#define _ZJUNIX_BUDDY_H

#include <zjunix/list.h>
#include <zjunix/lock.h>

#define _PAGE_RESERVED (1 << 31)
#define _PAGE_ALLOCED (1 << 30)
#define _PAGE_SLAB (1 << 29)

/*
 * struct buddy page is one info-set for the buddy group of pages
 */
struct page {
    unsigned int flag;       // the declaration of the usage of this page
    unsigned int reference;  //
    struct list_head list;   // double-way list
    void *virtual;           // default 0x(-1)
    unsigned int bplevel;    /* the order level of the page
                              *
                              * unsigned int sl_objs;
                              * 		represents the number of objects in current
                              * if the page is of _PAGE_SLAB, then bplevel is the sl_objs
                              */
    unsigned int slabp;      /* if the page is used by slab system,
                              * then slabp represents the base-addr of free space
                              */
};

#define PAGE_SHIFT 12
/*
 * order means the size of the set of pages, e.g. order = 1 -> 2^1
 * pages(consequent) are free In current system, we allow the max order to be
 * 4(2^4 consequent free pages)
 */
#define MAX_BUDDY_ORDER 4

struct freelist {
    unsigned int nr_free;
    struct list_head free_head;
};

struct buddy_sys {
    unsigned int buddy_start_pfn;
    unsigned int buddy_end_pfn;
    struct page *start_page;
    struct lock_t lock;
    struct freelist freelist[MAX_BUDDY_ORDER + 1];
};

#define _is_same_bpgroup(page, bage) (((*(page)).bplevel == (*(bage)).bplevel))
#define _is_same_bplevel(page, lval) ((*(page)).bplevel == (lval))
#define set_bplevel(page, lval) ((*(page)).bplevel = (lval))
#define set_flag(page, val) ((*(page)).flag |= (val))
#define clean_flag(page, val) ((*(page)).flag &= ~(val))
#define has_flag(page, val) ((*(page)).flag & val)
#define set_ref(page, val) ((*(page)).reference = (val))
#define inc_ref(page, val) ((*(page)).reference += (val))
#define dec_ref(page, val) ((*(page)).reference -= (val))

extern struct page *pages;
extern struct buddy_sys buddy;

extern void __free_pages(struct page *page, unsigned int order);
extern struct page *__alloc_pages(unsigned int order);

extern void free_pages(void *addr, unsigned int order);

extern void *alloc_pages(unsigned int order);

extern void init_buddy();

extern void buddy_info();

#endif

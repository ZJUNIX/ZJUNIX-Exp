#ifndef _ZJUNIX_BOOTMM_H
#define _ZJUNIX_BOOTMM_H

extern unsigned char __end[];

enum mm_usage { _MM_KERNEL, _MM_MMMAP, _MM_VGABUFF, _MM_PDTABLE, _MM_PTABLE, _MM_DYNAMIC, _MM_RESERVED, _MM_COUNT };

// record every part of mm's information
struct bootmm_info {
    unsigned int start;
    unsigned int end;
    unsigned int type;
};
// typedef struct bootmm_info * p_mminfo;

// 4K per page
#define PAGE_SHIFT 12
#define PAGE_FREE 0x00
#define PAGE_USED 0xff

#define PAGE_ALIGN (~((1 << PAGE_SHIFT) - 1))

#define MAX_INFO 10

struct bootmm {
    unsigned int phymm;    // the actual physical memory
    unsigned int max_pfn;  // record the max page number
    unsigned char* s_map;  // map begin place
    unsigned char* e_map;
    unsigned int last_alloc_end;
    unsigned int cnt_infos;  // get number of infos stored in bootmm now
    struct bootmm_info info[MAX_INFO];
};
// typedef struct bootmm * p_bootmm;

extern unsigned int firstusercode_start;
extern unsigned int firstusercode_len;

extern struct bootmm bmm;

extern unsigned int get_phymm_size();

extern void set_mminfo(struct bootmm_info* info, unsigned int start, unsigned int end, unsigned int type);

extern unsigned int insert_mminfo(struct bootmm* mm, unsigned int start, unsigned int end, unsigned int type);

extern unsigned int split_mminfo(struct bootmm* mm, unsigned int index, unsigned int split_start);

extern void remove_mminfo(struct bootmm* mm, unsigned int index);

extern void init_bootmm();

extern void set_maps(unsigned int s_pfn, unsigned int cnt, unsigned char value);

extern unsigned char* find_pages(unsigned int page_cnt, unsigned int s_pfn, unsigned int e_pfn, unsigned int align_pfn);

extern unsigned char* bootmm_alloc_pages(unsigned int size, unsigned int type, unsigned int align);

extern void bootmap_info(unsigned char* msg);

#endif

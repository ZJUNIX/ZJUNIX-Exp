#include "debug.h"

#ifdef FS_DEBUG
#include <driver/vga.h>
void dump_bpb_info(struct BPB_attr* BPB) {
    kernel_printf("BPB size: %d\n", sizeof(struct BPB_attr));
    kernel_printf("Sector size: %x\n", BPB->sector_size);
}

void dump_fat_info(struct fs_info* info) {
    kernel_printf("Max root dir entries: %x\n", info->BPB.attr.max_root_dir_entries);
    kernel_printf("Num of sectors per FAT table: %x\n", info->BPB.attr.num_of_sectors_per_fat);
    kernel_printf("Reserved sectors: %x\n", info->BPB.attr.reserved_sectors);
    kernel_printf("First data sector: %x\n", info->first_data_sector);
    kernel_printf("Total data clusters: %x\n", info->total_data_clusters);
}
#endif  // ! FS_DEBUG
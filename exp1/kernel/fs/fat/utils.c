#include "utils.h"
#include <driver/sd.h>
#include "fat.h"

/* Read/Write block for FAT (starts from first block of partition 1) */
u32 read_block(u8 *buf, u32 addr, u32 count) {
    return sd_read_block(buf, addr, count);
}

u32 write_block(u8 *buf, u32 addr, u32 count) {
    return sd_write_block(buf, addr, count);
}

/* char to u16/u32 */
u16 get_u16(u8 *ch) {
    return (*ch) + ((*(ch + 1)) << 8);
}

u32 get_u32(u8 *ch) {
    return (*ch) + ((*(ch + 1)) << 8) + ((*(ch + 2)) << 16) + ((*(ch + 3)) << 24);
}

/* u16/u32 to char */
void set_u16(u8 *ch, u16 num) {
    *ch = (u8)(num & 0xFF);
    *(ch + 1) = (u8)((num >> 8) & 0xFF);
}

void set_u32(u8 *ch, u32 num) {
    *ch = (u8)(num & 0xFF);
    *(ch + 1) = (u8)((num >> 8) & 0xFF);
    *(ch + 2) = (u8)((num >> 16) & 0xFF);
    *(ch + 3) = (u8)((num >> 24) & 0xFF);
}

/* work around */
u32 fs_wa(u32 num) {
    // return the bits of `num`
    u32 i;
    for (i = 0; num > 1; num >>= 1, i++)
        ;
    return i;
}

u32 get_entry_filesize(u8 *entry) {
    return get_u32(entry + 28);
}

u32 get_entry_attr(u8 *entry) {
    return entry[11];
}

/* DIR_FstClusHI/LO to clus */
u32 get_start_cluster(const FILE *file) {
    return (file->entry.attr.starthi << 16) + (file->entry.attr.startlow);
}

/* Get fat entry value for a cluster */
u32 get_fat_entry_value(u32 clus, u32 *ClusEntryVal) {
    u32 ThisFATSecNum;
    u32 ThisFATEntOffset;
    u32 index;

    cluster_to_fat_entry(clus, &ThisFATSecNum, &ThisFATEntOffset);

    index = read_fat_sector(ThisFATSecNum);
    if (index == 0xffffffff)
        goto get_fat_entry_value_err;

    *ClusEntryVal = get_u32(fat_buf[index].buf + ThisFATEntOffset) & 0x0FFFFFFF;

    return 0;
get_fat_entry_value_err:
    return 1;
}

/* modify fat for a cluster */
u32 fs_modify_fat(u32 clus, u32 ClusEntryVal) {
    u32 ThisFATSecNum;
    u32 ThisFATEntOffset;
    u32 fat32_val;
    u32 index;

    cluster_to_fat_entry(clus, &ThisFATSecNum, &ThisFATEntOffset);

    index = read_fat_sector(ThisFATSecNum);
    if (index == 0xffffffff)
        goto fs_modify_fat_err;

    fat_buf[index].state = 3;

    ClusEntryVal &= 0x0FFFFFFF;
    fat32_val = (get_u32(fat_buf[index].buf + ThisFATEntOffset) & 0xF0000000) | ClusEntryVal;
    set_u32(fat_buf[index].buf + ThisFATEntOffset, fat32_val);

    return 0;
fs_modify_fat_err:
    return 1;
}

/* Determine FAT entry for cluster */
void cluster_to_fat_entry(u32 clus, u32 *ThisFATSecNum, u32 *ThisFATEntOffset) {
    u32 FATOffset = clus << 2;
    *ThisFATSecNum = fat_info.BPB.attr.reserved_sectors + (FATOffset >> 9) + fat_info.base_addr;
    *ThisFATEntOffset = FATOffset & 511;
}

/* data cluster num <==> sector num */
u32 fs_dataclus2sec(u32 clus) {
    return ((clus - 2) << fs_wa(fat_info.BPB.attr.sectors_per_cluster)) + fat_info.first_data_sector;
}

u32 fs_sec2dataclus(u32 sec) {
    return ((sec - fat_info.first_data_sector) >> fs_wa(fat_info.BPB.attr.sectors_per_cluster)) + 2;
}
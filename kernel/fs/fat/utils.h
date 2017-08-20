#ifndef _FAT_UTILS_H
#define _FAT_UTILS_H

#include <zjunix/fs/fat.h>
#include <zjunix/type.h>

// Manipulate data through pointers
u16 get_u16(u8 *ch);
u32 get_u32(u8 *ch);
void set_u16(u8 *ch, u16 num);
void set_u32(u8 *ch, u32 num);
u32 fs_wa(u32 num);

// Get file entry info
u32 get_start_cluster(const FILE *file);
u32 get_fat_entry_value(u32 clus, u32 *ClusEntryVal);
u32 fs_modify_fat(u32 clus, u32 ClusEntryVal);

void cluster_to_fat_entry(u32 clus, u32 *ThisFATSecNum, u32 *ThisFATEntOffset);
u32 fs_dataclus2sec(u32 clus);
u32 fs_sec2dataclus(u32 sec);

#endif  // ! _FAT_UTILS_H
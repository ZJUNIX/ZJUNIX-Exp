#ifndef _FAT_DEBUG_H
#define _FAT_DEBUG_H

#ifdef FS_DEBUG
#include <zjunix/fs/fat.h>
void dump_bpb_info(struct BPB_attr* BPB);
void dump_fat_info(struct fs_info* info);
#endif  // ! FS_DEBUG

#endif  //_FAT_DEBUG_H

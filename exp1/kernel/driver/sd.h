#ifndef _SD_H
#define _SD_H

#include <driver/sd.h>

int sd_write_sector_blocking(int id, void* buffer);
int sd_read_sector_blocking(int id, void* buffer);

#endif

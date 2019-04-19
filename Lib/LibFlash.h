#ifndef _FLASHUPD_H
#define _FLASHUPD_H

typedef int (*FLASH_PROGRESS)(int percent);

int flash_init (void);
int flash_get_size(void);
int flash_erase (int start, int length, FLASH_PROGRESS progress,int base,int portion);
int flash_write (unsigned short *p, int start, int length, FLASH_PROGRESS progress, int base, int portion);
int flash_exit (void);

#endif


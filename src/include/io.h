#ifndef FILE_H
#define FILE_H
#include <types.h>



void KlIo_printf(char* s,...);



char* KlIo_read(struct File f,unsigned long l,unsigned long* r);



unsigned long KlIo_write(struct File f,char* s);



bool KlIo_flush(struct File f);



char* KlIo_default_read_func(void* p,unsigned char m,unsigned long l,unsigned long* r);



unsigned long KlIo_default_write_func(void* p,unsigned char m,char* s);



bool KlIo_default_flush_func(void* p,unsigned char m);



#endif

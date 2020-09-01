#ifndef FILE_H
#define FILE_H
#include <shared.h>



void KlIo_printf(char* s,...);



char* KlIo_read(struct File f,size_t l,size_t* r);



size_t KlIo_write(struct File f,char* s);



bool KlIo_flush(struct File f);



char* KlIo_default_read_func(void* p,unsigned char m,size_t l,size_t* r);



size_t KlIo_default_write_func(void* p,unsigned char m,char* s);



bool KlIo_default_flush_func(void* p,unsigned char m);



#endif

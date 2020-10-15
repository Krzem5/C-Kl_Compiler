#ifndef FILE_H
#define FILE_H
#include <types.h>



char* KlFileApi_read(struct File f,unsigned long l,unsigned long* r);



unsigned long KlFileApi_write(struct File f,char* s);



bool KlFileApi_flush(struct File f);



char* KlFileApi_default_read_func(void* p,unsigned char m,unsigned long l,unsigned long* r);



unsigned long KlFileApi_default_write_func(void* p,unsigned char m,char* s);



bool KlFileApi_default_flush_func(void* p,unsigned char m);



#endif

#ifndef NUMBER_H
#define NUMBER_H
#include <types.h>



#define BASE_EXP 32
#define BASE 4294967296



struct Number* KlNumApi_create_num(bool s,unsigned int i);



struct Number* KlNumApi_create_numL(bool s,unsigned long l);



struct Number* KlNumApi_create_numLL(bool s,unsigned long long ll);



#endif

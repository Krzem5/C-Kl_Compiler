#ifndef STRING_H
#define STRING_H
#include <types.h>



unsigned long str_len(char* s);



char* str_clone(char* s);



char* str_append(char* s,char c);



char* str_concat(char* a,char* b);



char* str_substr(char* s,unsigned long i,unsigned long j);



unsigned long str_find(char* s,char c,unsigned long i);



unsigned long str_rfind(char* s,char c,unsigned long i);



bool str_cmp(char* a,char* b,unsigned long s,unsigned long l);



char* str_format(char* s,...);



char* str_format_va(char* t,va_list a);



char* str_escape_ansi(unsigned long long int f);



#endif

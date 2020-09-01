#ifndef STRING_H
#define STRING_H
#include <shared.h>



size_t str_len(const char* s);



char* str_clone(const char* s);



char* str_append(const char* s,char c);



char* str_concat(const char* a,char* b);



char* str_substr(const char* s,size_t i,size_t j);



size_t str_find(const char* s,char c,size_t i);



size_t str_rfind(const char* s,char c,size_t i);



bool str_cmp(const char* a,char* b,size_t s,size_t l);



char* str_format(const char* s,...);



char* str_format_va(const char* t,va_list a);



char* str_escape_ansi(uint64_t f);



#endif

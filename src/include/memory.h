#ifndef MEMORY_H
#define MEMORY_H
#include <shared.h>



#ifndef NDEBUG
#define WARN_MEMCPY_TO_UNKNOWN false
#define WARN_MEMCPY_FROM_UNKNOWN false
#define KlMem_malloc(sz) KlMem_malloc_((sz),__FILE__,__LINE__,__func__)
#define KlMem_calloc(ln,sz) KlMem_calloc_((ln),(sz),__FILE__,__LINE__,__func__)
#define KlMem_realloc(s,sz) KlMem_realloc_((s),(sz),__FILE__,__LINE__,__func__)
#define KlMem_memcpy(o,s,sz) KlMem_memcpy_((o),(s),(sz),__FILE__,__LINE__,__func__)
#define KlMem_free(p) KlMem_free_((p),__FILE__,__LINE__,__func__)
#define KlMem_const(p,sz) KlMem_memcpy_(KlMem_malloc_((sz),__FILE__,__LINE__,__func__),(p),(sz),__FILE__,__LINE__,__func__)
#define KlMem_trace(p) KlMem_trace_((p),#p,__FILE__,__LINE__,__func__)
#define KlMem_ret(p) KlMem_ret_(p,__FILE__,__LINE__,__func__)
#define KlMem_ret_null() KlMem_ret_null_(__FILE__,__LINE__,__func__)
#define KlMem_enter_func() KlMem_enter_func_(__FILE__,__LINE__,__func__)
#define KlMem_check_allocated() KlMem_check_allocated_(__FILE__,__LINE__,__func__)
#else
#include <stdlib.h>
#define KlMem_malloc(sz) malloc((sz))
#define KlMem_calloc(ln,sz) calloc((ln),(sz))
#define KlMem_realloc(s,sz) realloc((s),(sz))
#define KlMem_memcpy(o,s,sz) KlMem_memcpy_((o),(s),(sz))
#define KlMem_free(p) free((p))
#define KlMem_const(p,sz) KlMem_memcpy(malloc((sz)),(p),(sz))
#define KlMem_trace(p)
#define KlMem_ret(p)
#define KlMem_ret_null()
#define KlMem_enter_func()
#define KlMem_check_allocated()
#endif
#ifndef NDEBUG



void* KlMem_malloc_(size_t sz,const char* f,unsigned int ln,const char* fn);



void* KlMem_calloc_(size_t ln_,size_t sz,const char* f,unsigned int ln,const char* fn);



void* KlMem_realloc_(void* s,size_t sz,const char* f,unsigned int ln,const char* fn);



void* KlMem_memcpy_(void* o,void* s,size_t sz,const char* f,unsigned int ln,const char* fn);



void KlMem_free_(void* p,const char* f,unsigned int ln,const char* fn);



void KlMem_trace_(void* p,const char* p_nm,const char* f,unsigned int ln,const char* fn);



void KlMem_ret_(void* p,const char* f,unsigned int ln,const char* fn);



void KlMem_ret_null_(const char* f,unsigned int ln,const char* fn);



void KlMem_enter_func_(const char* f,unsigned int ln,const char* fn);



void KlMem_check_allocated_(const char* f,unsigned int ln,const char* fn);



void KlMem_check_all_allocated_(void);



#else



void* KlMem_memcpy_(void* o,void* s,size_t sz);



#endif
#endif

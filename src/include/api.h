#ifndef API_H
#define API_H
#include <shared.h>



struct Object* KlApi_default_get_attr_str_func(struct Object* a,const char* b);



struct Object* KlApi_default_set_attr_str_func(struct Object* a,const char* b,struct Object* c);



struct Object* KlApi_default_del_attr_str_func(struct Object* a,const char* b);



char* KlApi_default_repr_func(struct Object* a);



uint64_t KlApi_default_hash(struct Object* a);



void KlApi_bigint_dealloc_f(struct Object* a);



struct Object* KlApi_bigint_add_f(struct Object* a,struct Object* b);



struct Object* KlApi_bigint_mlt_f(struct Object* a,struct Object* b);



struct Object* KlApi_bigint_iroot_f(struct Object* a,struct Object* b);



#endif

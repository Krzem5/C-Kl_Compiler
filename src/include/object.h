#ifndef OBJECT_H
#define OBJECT_H
#include <error.h>
#include <shared.h>



struct Object* KlObject_new(const struct Type t);



struct Object* KlObject_new_args(const struct Type t,struct Object** a,size_t al);



size_t KlObject_get_size(struct Object* o);



#endif

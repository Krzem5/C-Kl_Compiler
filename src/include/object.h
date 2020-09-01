#ifndef OBJECT_H
#define OBJECT_H
#include <shared.h>



struct Object* KlObject_new(const struct Type t);



struct Object* KlObject_new_args(const struct Type t,struct Object** a,size_t al);



#endif

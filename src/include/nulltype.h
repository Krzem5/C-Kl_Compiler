#ifndef NULL_TYPE_H
#define NULL_TYPE_H
#include <type.h>
#include <shared.h>



struct NullObject{
	OBJECT_HEAD
};



static struct Type NullType={
	.nm="null",
	.base=&BaseType,
	.sz=sizeof(struct NullObject)
};



#endif

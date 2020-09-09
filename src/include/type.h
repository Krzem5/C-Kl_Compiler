#ifndef TYPES_H
#define TYPES_H
#include <shared.h>
#include <api.h>



struct CharObject{
	OBJECT_HEAD
	char c;
};



struct StrObject{
	OBJECT_HEAD
	char* s;
	size_t ln;
};



struct BigIntObject{
	OBJECT_HEAD
	struct BigInt* i;
};



static struct Type BaseType={
	TYPE_HEAD_INIT
	.nm="<base_type>",
	.base=NULL,
	.sz=sizeof(struct Object),
	.get_attr_str_f=KlApi_default_get_attr_str_func,
	.set_attr_str_f=KlApi_default_set_attr_str_func,
	.del_attr_str_f=KlApi_default_del_attr_str_func,
	.repr_f=KlApi_default_repr_func,
	.hash_f=KlApi_default_hash,
};



static struct Type CharType={
	TYPE_HEAD_INIT
	.nm="char",
	.base=&BaseType,
	.sz=sizeof(struct CharObject)
};



static struct Type StrType={
	TYPE_HEAD_INIT
	.nm="str",
	.base=&BaseType,
	.sz=sizeof(struct StrObject)
};



static struct Type BigIntType={
	TYPE_HEAD_INIT
	.nm="int",
	.base=&BaseType,
	.sz=sizeof(struct BigIntObject),
	.dealloc_f=KlApi_bigint_dealloc_f,
	.add_f=KlApi_bigint_add_f,
	.mlt_f=KlApi_bigint_mlt_f,
	.iroot_f=KlApi_bigint_iroot_f
};



// Types: null,char,str,int,float,func,native_func,list,array,map



#endif

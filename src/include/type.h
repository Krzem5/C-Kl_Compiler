#ifndef TYPES_H
#define TYPES_H
#include <shared.h>
#include <api.h>



//struct Object* KlType_type_call_func(struct Object* a,struct Object** b,size_t c);



struct TypeObject{
	OBJECT_HEAD
	char* t;
};



static const struct Type BaseType={
	.nm="<base_type>",
	.base=NULL,
	.sz=sizeof(struct Object),
	.get_attr_str_f=KlApi_default_get_attr_str_func,
	.set_attr_str_f=KlApi_default_set_attr_str_func,
	.del_attr_str_f=KlApi_default_del_attr_str_func,
	.repr_f=KlApi_default_repr_func,
	.hash_f=KlApi_default_hash,
};



static const struct Type TypeObjectType={
	.nm="type",
	.base=&BaseType,
	.sz=sizeof(struct TypeObject),
	//.call_f=KlType_type_call_func,
};



// Types: null,char,str,int,float,func,native_func,list,array,map



#endif

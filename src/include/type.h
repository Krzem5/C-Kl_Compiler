#ifndef TYPES_H
#define TYPES_H
#include <shared.h>
#include <api.h>



struct TypeObject{
	OBJECT_HEAD
};



static const struct Type BaseType={
	.nm="<base_type>",
	.base=NULL,
	.sz=sizeof(struct TypeObject),
	// .get_attr_f=KlApi_default_get_attr_func,
	// .set_attr_f=KlApi_default_set_attr_func,
	// .del_attr_f=KlApi_default_det_attr_func,
	.get_attr_str_f=KlApi_default_get_attr_str_func,
	.set_attr_str_f=KlApi_default_set_attr_str_func,
	.del_attr_str_f=KlApi_default_del_attr_str_func,
	.repr_f=KlApi_default_repr_func,
	.hash_f=KlApi_default_hash,
};



// Types: null,char,str,int,float,func,native_func,list,array,map



static const struct Type TypeType={
	"type",
	&BaseType,
	sizeof(struct TypeObject)
};



#endif

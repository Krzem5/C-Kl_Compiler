#include <api.h>
#include <object.h>
#include <error.h>
#include <crypt.h>
#include <io.h>
#include <shared.h>
#include <memory.h>
#include <string_utils.h>



struct Object* KlApi_default_get_attr_str_func(struct Object* a,const char* b){
	KlMem_enter_func();
	if (OBJECT_TYPE_P(a)->get_set_del!=NULL){
		for (struct TypeSlot* i=OBJECT_TYPE_P(a)->get_set_del;i->nm!=NULL;i++){
			if (str_cmp(i->nm,b)==true){
				return i->get(a);
			}
		}
	}
	return(NULL);
}



struct Object* KlApi_default_set_attr_str_func(struct Object* a,const char* b,struct Object* c){
	KlMem_enter_func();
	if (OBJECT_TYPE_P(a)->get_set_del!=NULL){
		for (struct TypeSlot* i=OBJECT_TYPE_P(a)->get_set_del;i->nm!=NULL;i++){
			if (str_cmp(i->nm,b)==true){
				return i->set(a,c);
			}
		}
	}
	return(NULL);
}



struct Object* KlApi_default_del_attr_str_func(struct Object* a,const char* b){
	KlMem_enter_func();
	if (OBJECT_TYPE_P(a)->get_set_del!=NULL){
		for (struct TypeSlot* i=OBJECT_TYPE_P(a)->get_set_del;i->nm!=NULL;i++){
			if (str_cmp(i->nm,b)==true){
				return i->del(a);
			}
		}
	}
	return(NULL);
}



char* KlApi_default_repr_func(struct Object* a){
	KlMem_enter_func();
	assert(OBJECT_TYPE_P(a)!=NULL);
	return(str_format("<object '%s' at 0x%p>",OBJECT_TYPE_P(a)->nm,a));
}



uint64_t KlApi_default_hash(struct Object* a){
	KlMem_enter_func();
	assert(OBJECT_TYPE_P(a)!=NULL);
	uint256_t h=KlCrypt_quick_sha256((unsigned char*)(void*)a,KlObject_get_size(a));
	return(h.a^h.b^h.c^h.d);
}



struct Object* KlApi_char_to_object(char c);



struct Object* KlApi_str_to_object(char* s);



struct Object* KlApi_int_to_object(struct BigInt* i);

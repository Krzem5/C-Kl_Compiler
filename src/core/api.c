#include <api.h>
#include <object.h>
#include <error.h>
#include <crypt.h>
#include <io.h>
#include <shared.h>
#include <string_utils.h>



struct Object* KlApi_default_get_attr_str_func(struct Object* a,const char* b){
	if (a->t->get_set_del!=NULL){
		for (struct TypeSlot* i=a->t->get_set_del;i->nm!=NULL;i++){
			if (str_cmp(i->nm,b)==true){
				return i->get(a);
			}
		}
	}
	return NULL;
}



struct Object* KlApi_default_set_attr_str_func(struct Object* a,const char* b,struct Object* c){
	if (a->t->get_set_del!=NULL){
		for (struct TypeSlot* i=a->t->get_set_del;i->nm!=NULL;i++){
			if (str_cmp(i->nm,b)==true){
				return i->set(a,c);
			}
		}
	}
	return NULL;
}



struct Object* KlApi_default_del_attr_str_func(struct Object* a,const char* b){
	if (a->t->get_set_del!=NULL){
		for (struct TypeSlot* i=a->t->get_set_del;i->nm!=NULL;i++){
			if (str_cmp(i->nm,b)==true){
				return i->del(a);
			}
		}
	}
	return NULL;
}



char* KlApi_default_repr_func(struct Object* a){
	assert(a->t!=NULL);
	return str_format("<object '%s' at 0x%p>",a->t->nm,a);
}



uint64_t KlApi_default_hash(struct Object* a){
	assert(a->t!=NULL);
	uint256_t h=KlCrypt_quick_sha256((unsigned char*)(void*)a,KlObject_get_size(a));
	return h.a^h.b^h.c^h.d;
}

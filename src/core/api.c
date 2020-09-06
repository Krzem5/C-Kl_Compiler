#include <api.h>
#include <object.h>
#include <error.h>
#include <crypt.h>
#include <io.h>
#include <shared.h>
#include <type.h>
#include <bigint.h>
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



void KlApi_bigint_dealloc_f(struct Object* a){
	KlMem_enter_func();
	struct BigIntObject* ia=KlObject_to_bigint(a);
	if (ia==NULL){
		return();
	}
	KlBigInt_free(ia->i);
	return();
}



struct Object* KlApi_bigint_add_f(struct Object* a,struct Object* b){
	KlMem_enter_func();
	struct BigIntObject* ia=KlObject_to_bigint(a);
	if (ia==NULL){
		return(NULL);
	}
	struct BigIntObject* ib=KlObject_to_bigint(b);
	if (ib==NULL){
		return(NULL);
	}
	struct BigInt* oi=KlBigInt_add(ia->i,ib->i);
	if (oi==NULL){
		return(NULL);
	}
	struct BigIntObject* o=(struct BigIntObject*)KlObject_new(BigIntType);
	o->i=oi;
	return((struct Object*)o);
}



struct Object* KlApi_bigint_iroot_f(struct Object* a,struct Object* b){
	KlMem_enter_func();
	struct BigIntObject* ia=KlObject_to_bigint(a);
	if (a==NULL){
		return(NULL);
	}
	struct BigIntObject* ib=KlObject_to_bigint(b);
	if (b==NULL){
		return(NULL);
	}
	struct BigInt* oi=KlBigInt_iroot(ia->i,ib->i);
	if (oi==NULL){
		return(NULL);
	}
	struct BigIntObject* o=(struct BigIntObject*)KlObject_new(BigIntType);
	o->i=oi;
	return((struct Object*)o);
}

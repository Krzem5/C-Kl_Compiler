#include <object.h>
#include <memory.h>
#include <shared.h>



struct Object* KlObject_new(const struct Type t){
	struct Object* o=(t.alloc_f!=NULL?t.alloc_f():KlMem_malloc(t.m_sz));
	if (t.init_f!=NULL){
		t.init_f(o,NULL,0);
	}
	KlMem_ret(o);
	return o;
}



struct Object* KlObject_new_args(const struct Type t,struct Object** a,size_t al){
	struct Object* o=(t.alloc_f!=NULL?t.alloc_f():KlMem_malloc(t.m_sz));
	if (t.init_f!=NULL){
		t.init_f(o,a,al);
	}
	KlMem_ret(o);
	return o;
}

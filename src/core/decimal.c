#include <decimal.h>
#include <memory.h>
#include <types.h>
#include <io.h>
#include <stdint.h>



struct Decimal* KlDec_create_dec(bool s,unsigned int n,unsigned int f){
	KlMem_enter_func();
	struct Decimal o;
	o.l=(s==true?-1:1);
	o.fl=1;
	o.v=KlMem_const(&(uint32_t)n,sizeof(uint32_t));
	o.fv=KlMem_const(&(uint32_t)f,sizeof(uint32_t));
	KlMem_ret(o.v);
	KlMem_ret(o.fv);
	return(KlMem_const(&o,sizeof(struct Decimal)));
}



struct Decimal* KlDec_from_num(struct Number* n){
	KlMem_enter_func();
	struct Decimal o;
	o.l=n->l;
	o.fl=1;
	o.v=KlMem_malloc(n->l*sizeof(uint32_t));
	KlMem_memcpy(o.v,n->v,n->l*sizeof(uint32_t));
	o.fv=KlMem_malloc(sizeof(uint32_t));
	*(o.fv)=0;
	KlMem_ret(o.v);
	KlMem_ret(o.fv);
	return(KlMem_const(&o,sizeof(struct Decimal)));
}



#include <stdio.h>
void KlDec_add_frac_digit(struct Decimal* o,unsigned char d){
	KlMem_enter_func();
	o--;// C4100
	d--;// C4100
	assert(0);
	return();
}

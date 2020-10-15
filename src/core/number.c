#include <number.h>
#include <memory.h>
#include <stdint.h>



struct Number* KlNumApi_create_num(bool s,unsigned int i){
	struct Number o;
	o.l=(s==true?-1:1);
	o.v=&(uint32_t)i;
	return KlMemApi_const_storage(&o,sizeof(struct Number));
}



struct Number* KlNumApi_create_numL(bool s,unsigned long l){
	struct Number o;
	o.l=(s==true?-1:1);
	o.v=&(uint32_t)l;
	return KlMemApi_const_storage(&o,sizeof(struct Number));
}



struct Number* KlNumApi_create_numLL(bool s,unsigned long long ll){
	struct Number o;
	o.l=(s==true?-1:1);
	o.v=malloc(2*sizeof(uint32_t));
	*o.v=ll>>BASE_EXP;
	*(o.v+1)=ll%BASE;
	return KlMemApi_const_storage(&o,sizeof(struct Number));
}

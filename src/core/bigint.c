#include <error.h>
#include <bigint.h>
#include <memory.h>
#include <stdint.h>
#include <io.h>
#include <shared.h>



struct BigInt* KlBigInt_from_long(long long ll){
	KlMem_enter_func();
	struct BigInt o;
	signed char s=(ll<0?-1:1);
	unsigned long long t=ll*s;
	unsigned char ln=0;
	while (t!=0){
		ln++;
		t>>=NUM_SHIFT;
	}
	if (ln==0){
		ln=1;
	}
	o.l=ln*s;
	o.v=KlMem_malloc(ln*sizeof(uint32_t));
	t=ll*s;
	unsigned long i=0;
	*o.v=(uint32_t)(t&NUM_MASK);
	t>>=NUM_SHIFT;
	while (t!=0){
		*(o.v+i)=(uint32_t)(t&NUM_MASK);
		t>>=NUM_SHIFT;
		i++;
	}
	KlMem_ret(o.v);
	return(KlMem_const(&o,sizeof(struct BigInt)));
}



struct BigInt* KlBigInt_assign(struct BigInt* o,struct BigInt* v){
	KlMem_enter_func();
	assert(v!=NULL);
	if (o==NULL){
		o=KlMem_malloc(sizeof(struct BigInt));
		o->v=NULL;
	}
	o->l=v->l;
	o->v=KlMem_memcpy(KlMem_realloc(o->v,o->l*sizeof(uint32_t)),v->v,o->l*sizeof(uint32_t));
	KlMem_ret(o->v);
	KlMem_ret(o);
	return(o);
}



struct BigInt* KlBigInt_assign_long(struct BigInt* o,long v){
	KlMem_enter_func();
	if (o==NULL){
		o=KlMem_malloc(sizeof(struct BigInt));
		KlMem_ret(o);
	}
	o->l=(v<0?-1:1);
	intmax_t uv=v*o->l;
	o->v=KlMem_const(&(uint32_t)uv,sizeof(uint32_t));
	KlMem_ret(o->v);
	return(o);
}



void KlBigInt_free(struct BigInt* n){
	KlMem_enter_func();
	if (n!=NULL){
		if (n->v!=NULL){
			KlMem_free(n->v);
		}
		KlMem_free(n);
	}
	return();
}



struct BigInt* KlBigInt_trunc(struct BigInt* o){
	KlMem_enter_func();
	assert(o!=NULL);
	signed char s=(o->l<0?-1:1);
	intmax_t l=o->l*s;
	while (l>1&&o->v[l-1]==0){
		l--;
	}
	if (l!=(intmax_t)(o->l*s)){
		o->l=l*s;
		o->v=KlMem_realloc(o->v,l*sizeof(uint32_t));
	}
	return(o);
}



bool KlBigInt_is_zero(struct BigInt* n){
	KlMem_enter_func();
	assert(n!=NULL);
	KlBigInt_trunc(n);
	return(((n->l==1||n->l==-1)&&*n->v==0));
}



bool KlBigInt_is_one(struct BigInt* n){
	KlMem_enter_func();
	assert(n!=NULL);
	KlBigInt_trunc(n);
	return((n->l==1&&*n->v==1));
}



struct BigInt* KlBigInt_abs(struct BigInt* a){
	KlMem_enter_func();
	assert(a!=NULL);
	struct BigInt o;
	o.l=(a->l<0?-a->l:a->l);
	o.v=KlMem_memcpy(KlMem_malloc(o.l*sizeof(uint32_t)),a->v,o.l*sizeof(uint32_t));
	assert(o.l>0);
	return(KlMem_const(&o,sizeof(struct BigInt)));
}



struct BigInt* KlBigInt_neg(struct BigInt* a){
	KlMem_enter_func();
	assert(a!=NULL);
	struct BigInt o;
	if (KlBigInt_is_zero(a)==true){
		return(KlBigInt_assign(NULL,a));
	}
	o.l=(a->l<0?a->l:-a->l);
	o.v=KlMem_memcpy(KlMem_malloc(o.l*sizeof(uint32_t)),a->v,o.l*sizeof(uint32_t));
	return(KlMem_const(&o,sizeof(struct BigInt)));
}



struct BigInt* KlBigInt_add(struct BigInt* a,struct BigInt* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	if ((a->l<0)==(b->l<0)){
		struct BigInt* o=KlBigInt_add_abs(a,b);
		o->l*=(a->l<0?-1:1);
		return(o);
	}
	else{
		struct BigInt* o=KlBigInt_sub_abs(a,b);
		if (KlBigInt_is_zero(o)){
			o->l=1;
		}
		else{
			o->l*=((KlBigInt_cmp_abs(a,b)>=0?a->l<0:b->l<0)==true?-1:1);
		}
		return(o);
	}
}



struct BigInt* KlBigInt_add_long(struct BigInt* a,long b){
	KlMem_enter_func();
	assert(a!=NULL);
	if ((a->l<0)==(b<0)){
		struct BigInt* o=KlBigInt_add_long_abs(a,b);
		o->l*=(a->l<0?-1:1);
		return(o);
	}
	else{
		struct BigInt* o=KlBigInt_sub_long_abs(a,b);
		if (KlBigInt_is_zero(o)){
			o->l=1;
		}
		else{
			o->l*=((KlBigInt_cmp_long_abs(a,b)>=0?a->l<0:b<0)==true?-1:1);
		}
		return(o);
	}
}



struct BigInt* KlBigInt_add_abs(struct BigInt* a,struct BigInt* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	intmax_t lna=(a->l<0?-a->l:a->l);
	intmax_t lnb=(b->l<0?-b->l:b->l);
	if (lna<lnb){
		struct BigInt* t=a;
		a=b;
		b=t;
		intmax_t lnt=lna;
		lna=lnb;
		lnb=lnt;
	}
	struct BigInt o;
	o.l=(lna+1)*(a->l<0?-1:1);
	o.v=KlMem_malloc((lna+1)*sizeof(uint32_t));
	uint32_t c=0;
	intmax_t i;
	for (i=0;i<lnb;i++){
		c+=a->v[i]+b->v[i];
		o.v[i]=c&NUM_MASK;
		c>>=NUM_SHIFT;
	}
	for (;i<lna;i++){
		c+=a->v[i];
		o.v[i]=c&NUM_MASK;
		c>>=NUM_SHIFT;
	}
	o.v[i]=c;
	KlMem_ret(o.v);
	return(KlMem_const(KlBigInt_trunc(&o),sizeof(struct BigInt)));
}



struct BigInt* KlBigInt_add_long_abs(struct BigInt* a,unsigned long b){
	KlMem_enter_func();
	assert(a!=NULL);
	intmax_t lna=(a->l<0?-a->l:a->l);
	struct BigInt o;
	o.l=(lna+1)*(a->l<0?-1:1);
	o.v=KlMem_malloc((lna+1)*sizeof(uint32_t));
	uint32_t c=a->v[0]+b;
	o.v[0]=c&NUM_MASK;
	c>>=NUM_SHIFT;
	intmax_t i=1;
	for (;i<lna;i++){
		c+=a->v[i];
		o.v[i]=c&NUM_MASK;
		c>>=NUM_SHIFT;
	}
	o.v[i]=c;
	KlMem_ret(o.v);
	return(KlMem_const(KlBigInt_trunc(&o),sizeof(struct BigInt)));
}



struct BigInt* KlBigInt_sub(struct BigInt* a,struct BigInt* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	struct BigInt* o=((a->l<0)==(b->l<0)?KlBigInt_sub_abs(a,b):KlBigInt_add_abs(a,b));
	if (KlBigInt_is_zero(o)){
		o->l=1;
	}
	else{
		o->l*=(KlBigInt_cmp(a,b)>=0?1:-1);
	}
	return(o);
}



struct BigInt* KlBigInt_sub_long(struct BigInt* a,long b){
	KlMem_enter_func();
	assert(a!=NULL);
	struct BigInt* o=((a->l<0)==(b<0)?KlBigInt_sub_long_abs(a,b):KlBigInt_add_long_abs(a,b));
	if (KlBigInt_is_zero(o)){
		o->l=1;
	}
	else{
		o->l*=(KlBigInt_cmp_long(a,b)>=0?1:-1);
	}
	return(o);
}



struct BigInt* KlBigInt_sub_abs(struct BigInt* a,struct BigInt* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	intmax_t lna=(a->l<0?-a->l:a->l);
	intmax_t lnb=(b->l<0?-b->l:b->l);
	if (lna<lnb){
		struct BigInt* t=a;
		a=b;
		b=t;
		intmax_t lnt=lna;
		lna=lnb;
		lnb=lnt;
	}
	else if (lna==lnb){
		intmax_t i=lna;
		while (a->v[i]==b->v[i]){
			if (i==0){
				return(KlBigInt_from_long(0));
			}
			i--;
		}
		if (a->v[i]<b->v[i]){
			struct BigInt* t=a;
			a=b;
			b=t;
		}
		lna=lnb=i+1;
	}
	struct BigInt o;
	o.l=lna;
	o.v=KlMem_malloc(lna*sizeof(uint32_t));
	uint32_t br=0;
	intmax_t i=0;
	for (;i<lnb;i++){
		br=a->v[i]-b->v[i]-br;
		o.v[i]=br&NUM_MASK;
		br>>=NUM_SHIFT;
		br&=1;
	}
	for (;i<lna;i++){
		br=a->v[i]-br;
		o.v[i]=br&NUM_MASK;
		br>>=NUM_SHIFT;
		br&=1;
	}
	KlMem_ret(o.v);
	return(KlMem_const(KlBigInt_trunc(&o),sizeof(struct BigInt)));
}



struct BigInt* KlBigInt_sub_long_abs(struct BigInt* a,unsigned long b){
	KlMem_enter_func();
	assert(a!=NULL);
	intmax_t lna=(a->l<0?-a->l:a->l);
	if (lna==1){
		if (*a->v==b){
			return(KlBigInt_from_long(0));
		}
		if (*a->v<b){
			uint32_t t=a->v[0];
			*a->v=(uint32_t)b;
			b=t;
		}
	}
	struct BigInt o;
	o.l=lna;
	o.v=KlMem_malloc(lna*sizeof(uint32_t));
	uint32_t br=*a->v-b;
	*o.v=br&NUM_MASK;
	br>>=NUM_SHIFT;
	br&=1;
	for (intmax_t i=1;i<lna;i++){
		br=a->v[i]-br;
		o.v[i]=br&NUM_MASK;
		br>>=NUM_SHIFT;
		br&=1;
	}
	KlMem_ret(o.v);
	return(KlMem_const(KlBigInt_trunc(&o),sizeof(struct BigInt)));
}



struct BigInt* KlBigInt_mult(struct BigInt* a,struct BigInt* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	struct BigInt* o=KlBigInt_mult_abs(a,b);
	if (KlBigInt_is_zero(o)){
		o->l=1;
	}
	else{
		o->l*=((a->l<0)==(b->l<0)?1:-1);
	}
	return(o);
}



struct BigInt* KlBigInt_mult_long(struct BigInt* a,long b){
	KlMem_enter_func();
	assert(a!=NULL);
	struct BigInt* o=KlBigInt_mult_long_abs(a,b);
	if (KlBigInt_is_zero(o)){
		o->l=1;
	}
	else{
		o->l*=((a->l<0)==(b<0)?1:-1);
	}
	return(o);
}



struct BigInt* KlBigInt_mult_abs(struct BigInt* a,struct BigInt* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	if (KlBigInt_is_zero(a)||KlBigInt_is_zero(b)){
		return(KlBigInt_from_long(0));
	}
	if (KlBigInt_is_one(a)){
		return(KlBigInt_assign(NULL,b));
	}
	if (KlBigInt_is_one(b)){
		return(KlBigInt_assign(NULL,a));
	}
	intmax_t lna=(a->l<0?-a->l:a->l);
	intmax_t lnb=(b->l<0?-b->l:b->l);
	struct BigInt o;
	o.l=lna+lnb;
	o.v=KlMem_calloc(o.l,sizeof(uint32_t));
	uint64_t c;
	intmax_t j;
	intmax_t bi;
	for (intmax_t i=0;i<lna;i++){
		c=0;
		j=i;
		bi=0;
		while (bi<lnb){
			c+=*(o.v+j)+*(b->v+bi)*(uint64_t)*(a->v+i);
			*(o.v+j)=(uint32_t)(c&NUM_MASK);
			c>>=NUM_SHIFT;
			bi++;
			j++;
		}
		if (c>0){
			*(o.v+j)=(uint32_t)(c&NUM_MASK);
		}
	}
	KlMem_ret(o.v);
	return(KlMem_const(KlBigInt_trunc(&o),sizeof(struct BigInt)));
}



struct BigInt* KlBigInt_mult_long_abs(struct BigInt* a,unsigned long b){
	KlMem_enter_func();
	assert(a!=NULL);
	if (KlBigInt_is_zero(a)||b==0){
		return(KlBigInt_from_long(0));
	}
	if (KlBigInt_is_one(a)){
		return(KlBigInt_from_long(b));
	}
	if (b==1){
		return(KlBigInt_assign(NULL,a));
	}
	intmax_t lna=(a->l<0?-a->l:a->l);
	struct BigInt o;
	o.l=lna+1;
	o.v=KlMem_calloc(o.l,sizeof(uint32_t));
	intmax_t j;
	for (intmax_t i=0;i<lna;i++){
		j=i;
		uint64_t c=*(o.v+i)+b*(*(a->v+i));
		*(o.v+j)=(uint32_t)(c&NUM_MASK);
		c>>=NUM_SHIFT;
		j++;
		if (c>0){
			*(o.v+j)=(uint32_t)(c&NUM_MASK);
		}
	}
	KlMem_ret(o.v);
	return(KlMem_const(KlBigInt_trunc(&o),sizeof(struct BigInt)));
}



struct BigInt* KlBigInt_div(struct BigInt* a,struct BigInt* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL&&KlBigInt_is_zero(b)==false);
	KlError_unimplemented_code();
	return(NULL);
}



struct BigInt* KlBigInt_fdiv(struct BigInt* a,struct BigInt* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL&&KlBigInt_is_zero(b)==false);
	if (KlBigInt_is_zero(b)==true){
		KlError_unimplemented_error();
		return(NULL);
	}
	if ((a->l==1||a->l==-1)&&(b->l==1||b->l==-1)){
		return(KlBigInt_from_long((a->l==b->l?*a->v/(*b->v):-1-(*a->v-1)/(*b->v))));
	}
	struct BigInt* o=NULL;
	if (KlBigInt_divmod(a,b,&o,NULL)!=0){
		return(NULL);
	}
	return(o);
}



struct BigInt* KlBigInt_fdiv_long(struct BigInt* a,long b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=0);
	if (b==0){
		KlError_unimplemented_error();
		return(NULL);
	}
	if (a->l==1||a->l==-1){
		return(KlBigInt_from_long((a->l==(b<0?-1:1)?*a->v/b:-1-(*a->v-1)/b)));
	}
	struct BigInt* o=NULL;
	if (KlBigInt_divmod_long(a,b,&o,NULL)!=0){
		return(NULL);
	}
	return(o);
}



struct BigInt* KlBigInt_mod(struct BigInt* a,struct BigInt* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL&&KlBigInt_is_zero(b)==false);
	if (KlBigInt_is_zero(b)==true){
		KlError_unimplemented_error();
		return(NULL);
	}
	if ((a->l==1||a->l==-1)&&(b->l==1||b->l==-1)){
		return(KlBigInt_from_long((a->l==b->l?*a->v%*b->v:*b->v-1-(*a->v-1)%*b->v)*(int32_t)b->l));
	}
	struct BigInt* o=NULL;
	if (KlBigInt_divmod(a,b,NULL,&o)!=0){
		return(NULL);
	}
	return(o);
}



struct BigInt* KlBigInt_mod_long(struct BigInt* a,long b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=0);
	if (b==0){
		KlError_unimplemented_error();
		return(NULL);
	}
	if (a->l==1||a->l==-1){
		return(KlBigInt_from_long((a->l==(b<0?-1:1)?*a->v%b:b-1-(*a->v-1)%b)*(int32_t)(b<0?-1:1)));
	}
	struct BigInt* o=NULL;
	if (KlBigInt_divmod_long(a,b,NULL,&o)!=0){
		return(NULL);
	}
	return(o);
}



unsigned char KlBigInt_divrem(struct BigInt* a,struct BigInt* b,struct BigInt** div,struct BigInt** rem){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL&&KlBigInt_is_zero(b)==false);
	assert(div!=NULL||rem!=NULL);
	if (KlBigInt_is_zero(b)){
		KlError_unimplemented_error();
		return(1);
	}
	intmax_t lna=(a->l<0?-a->l:a->l);
	intmax_t lnb=(b->l<0?-b->l:b->l);
	if (lna<lnb||(lna==lnb&&*(a->v+lna-1)<*(b->v+lnb-1))){
		if (div!=NULL){
			*div=KlBigInt_assign_long(*div,0);
			assert(*div!=NULL);
		}
		if (rem!=NULL){
			*rem=KlBigInt_assign(*rem,a);
			assert(*rem!=NULL);
		}
		return(0);
	}
	if (lnb==1){
		uint64_t rm=0;
		intmax_t tln=lna;
		uint32_t hi;
		if (div!=NULL){
			if (*div==NULL){
				*div=KlMem_malloc(sizeof(struct BigInt));
			}
			(*div)->l=lna;
			(*div)->v=(uint32_t*)KlMem_malloc(lna*sizeof(uint32_t))+tln;
			(uint32_t*)a->v+=tln;
			while (tln>0){
				a->v--;
				(*div)->v--;
				rm=(rm<<NUM_SHIFT)|(*a->v);
				*((*div)->v)=hi=(uint32_t)(rm/(*b->v));
				rm-=(uint64_t)hi*(*b->v);
				tln--;
			}
		}
		else{
			a->v+=tln;
			while (tln>0){
				a->v--;
				rm=(rm<<NUM_SHIFT)|(*a->v);
				hi=(uint32_t)(rm/(*b->v));
				rm-=(uint64_t)hi*(*b->v);
				tln--;
			}
		}
		if (rem!=NULL){
			*rem=KlBigInt_assign_long(*rem,(uint32_t)rm);
			assert(*rem!=NULL);
		}
	}
	else{
		if (KlBigInt_divrem_abs(a,b,div,rem)!=0){
			return(1);
		}
	}
	if (div!=NULL&&(a->l<0)!=(b->l<0)){
		(*div)->l*=-1;
	}
	if (rem!=NULL&&a->l<0&&KlBigInt_is_zero(*rem)==false){
		(*rem)->l*=-1;
	}
	assert((div!=NULL&&(*div)!=NULL)||(rem!=NULL&&(*rem)!=NULL));
	return(0);
}



unsigned char KlBigInt_divrem_long(struct BigInt* a,long b,struct BigInt** div,struct BigInt** rem){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=0);
	assert(div!=NULL||rem!=NULL);
	if (b==0){
		KlError_unimplemented_error();
		return(1);
	}
	intmax_t lna=(a->l<0?-a->l:a->l);
	unsigned long ub=(b<0?-b:b);
	if (lna==1&&*(a->v+lna-1)<ub){
		if (div!=NULL){
			*div=KlBigInt_assign_long(*div,0);
		}
		if (rem!=NULL){
			*rem=KlBigInt_assign(*rem,a);
		}
		return(0);
	}
	if (*div==NULL){
		*div=KlMem_malloc(sizeof(struct BigInt));
	}
	(*div)->l=lna;
	(*div)->v=(uint32_t*)KlMem_malloc(lna*sizeof(uint32_t))+lna;
	uint64_t r=0;
	intmax_t tln=lna;
	uint32_t hi;
	(uint32_t*)a->v+=tln;
	while (tln>0){
		a->v--;
		(*div)->v--;
		r=(r<<NUM_SHIFT)|(*a->v);
		*((*div)->v)=hi=(uint32_t)(r/ub);
		r-=(uint64_t)hi*ub;
		tln--;
	}
	*rem=KlBigInt_assign_long(*rem,(uint32_t)r);
	if ((a->l<0)!=(b<0)){
		(*div)->l*=-1;
	}
	if (a->l<0&&KlBigInt_is_zero(*rem)==false){
		(*rem)->l*=-1;
	}
	assert((div!=NULL&&(*div)!=NULL)||(rem!=NULL&&(*rem)!=NULL));
	return(0);
}



unsigned char KlBigInt_divrem_abs(struct BigInt* a,struct BigInt* b,struct BigInt** div,struct BigInt** rem){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL&&KlBigInt_is_zero(b)==false);
	assert(div!=NULL||rem!=NULL);
	if (KlBigInt_is_zero(b)==true){
		KlError_unimplemented_error();
		return(1);
	}
	intmax_t lna=(a->l<0?-a->l:a->l);
	intmax_t lnb=(b->l<0?-b->l:b->l);
	struct BigInt v;
	v.l=lna+1;
	v.v=KlMem_malloc(v.l*sizeof(uint32_t));
	struct BigInt w;
	w.l=lnb;
	w.v=KlMem_malloc(w.l*sizeof(uint32_t));
	unsigned char d=NUM_SHIFT;
	uint32_t tmp=*(b->v+lnb-1);
	while (tmp>=32){
		d-=6;
		tmp>>=6;
	}
	if (tmp!=0){
		if (tmp==1){
			d--;
		}
		else if (tmp<=3){
			d-=2;
		}
		else if (tmp<=7){
			d-=3;
		}
		else if (tmp<=15){
			d-=4;
		}
		else{
			d-=5;
		}
	}
	uint32_t c=0;
	for (intmax_t i=0;i<lnb;i++){
		uint64_t s=(uint64_t)*(b->v+i)<<d|c;
		*(w.v+i)=(uint32_t)s&NUM_MASK;
		c=(uint32_t)(s>>NUM_SHIFT);
	}
	c=0;
	for (intmax_t i=0;i<lna;i++){
		uint64_t s=(uint64_t)*(a->v+i)<<d|c;
		*(v.v+i)=(uint32_t)s&NUM_MASK;
		c=(uint32_t)(s>>NUM_SHIFT);
	}
	if (c!=0||*(v.v+lna-1)>=*(w.v+lnb-1)){
		v.v[lna]=c;
		lna++;
	}
	intmax_t k=lna-lnb;
	struct BigInt t;
	t.l=lna-lnb;
	t.v=KlMem_malloc(t.l*sizeof(uint32_t));
	uint32_t* ak=t.v+k;
	uint32_t* vk=v.v+k;
	while (true){
		uint32_t vtop=*(vk+lnb);
		uint64_t vv=((uint64_t)vtop<<NUM_SHIFT)|*(vk+lnb-1);
		uint32_t q=(uint32_t)(vv/(*(w.v+lnb-1)));
		uint32_t r=(uint32_t)(vv-(uint64_t)*(w.v+lnb-1)*q);
		while ((uint64_t)*(w.v+lnb-2)*q>(((uint64_t)r<<NUM_SHIFT)|*(vk+lnb-2))){
			q--;
			r+=*(w.v+lnb-1);
			if (r>=NUM_BASE){
				break;
			}
		}
		int32_t zhi=0;
		for (intmax_t i=0;i<lnb;i++){
			int64_t z=(int32_t)*(vk+i)+zhi-(int64_t)q*(int64_t)*(w.v+i);
			*(vk+i)=(uint32_t)z&NUM_MASK;
			zhi=(int32_t)z>>NUM_SHIFT;
		}
		if ((int32_t)vtop+zhi<0){
			c=0;
			for (intmax_t i=0;i<lnb;i++){
				c+=*(vk+i)+*(w.v+i);
				*(vk+i)=c&NUM_MASK;
				c>>=NUM_SHIFT;
			}
			q--;
		}
		ak--;
		*ak=q;
		vk--;
		if (vk<=v.v){
			break;
		}
	}
	c=0;
	uint32_t ms=((uint32_t)1<<d)-1;
	for (intmax_t i=lnb-1;i>0;i--){
		uint64_t s=(uint64_t)c<<NUM_SHIFT|*(w.v+i);
		c=(uint32_t)s&ms;
		*(v.v+i)=(uint32_t)(s>>d);
	}
	assert(c==0);
	*div=KlBigInt_assign(*div,&t);
	*rem=KlBigInt_assign(*rem,&w);
	assert((div!=NULL&&(*div)!=NULL)||(rem!=NULL&&(*rem)!=NULL));
	KlMem_free(v.v);
	KlMem_free(w.v);
	KlMem_free(t.v);
	return(0);
}



unsigned char KlBigInt_divmod(struct BigInt* a,struct BigInt* b,struct BigInt** div,struct BigInt** mod){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL&&KlBigInt_is_zero(b)==false);
	assert(div!=NULL||mod!=NULL);
	if (KlBigInt_is_zero(b)==true){
		KlError_unimplemented_error();
		return(1);
	}
	if ((a->l==1||a->l==-1)&&(b->l==1||b->l==-1)){
		if (div!=NULL){
			KlBigInt_assign_long(*div,(a->l==b->l?*a->v/(*b->v):-1-(*a->v-1)/(*b->v)));
		}
		if (mod!=NULL){
			KlBigInt_assign_long(*mod,(a->l==b->l?*a->v%*b->v:*b->v-1-(*a->v-1)%*b->v)*(int32_t)b->l);
		}
		return(0);
	}
	struct BigInt* d=NULL;
	struct BigInt* m=NULL;
	struct BigInt** dp=&d;
	struct BigInt** mp=&m;
	if (KlBigInt_divrem(a,b,dp,mp)!=0){
		return(1);
	}
	assert(d!=NULL);
	assert(m!=NULL);
	if ((m->l<0&&b->l>0)||(m->l>0&&b->l<0)){
		struct BigInt* tmp=KlBigInt_add(m,b);
		KlBigInt_free(m);
		m=tmp;
		KlBigInt_free(tmp);
		tmp=KlBigInt_sub_long(d,1);
		KlBigInt_free(d);
		d=tmp;
		KlBigInt_free(tmp);
	}
	if (div!=NULL){
		*div=KlBigInt_assign(*div,d);
		assert(*div!=NULL);
	}
	if (mod!=NULL){
		*mod=KlBigInt_assign(*mod,m);
		assert(*mod!=NULL);
	}
	KlBigInt_free(d);
	KlBigInt_free(m);
	return(0);
}



unsigned char KlBigInt_divmod_long(struct BigInt* a,long b,struct BigInt** div,struct BigInt** mod){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=0);
	assert(div!=NULL||mod!=NULL);
	if (b==0){
		KlError_unimplemented_error();
		return(1);
	}
	if (a->l==1||a->l==-1){
		if (div!=NULL){
			KlBigInt_assign_long(*div,(a->l==(b<0?-1:1)?*a->v/b:-1-(*a->v-1)/b));
		}
		if (mod!=NULL){
			KlBigInt_assign_long(*mod,(a->l==(b<0?-1:1)?*a->v%b:b-1-(*a->v-1)%b)*(int32_t)(b<0?-1:1));
		}
		return(0);
	}
	struct BigInt* d=KlMem_malloc(sizeof(struct BigInt));
	struct BigInt* m=KlMem_malloc(sizeof(struct BigInt));
	if (KlBigInt_divrem_long(a,b,&d,&m)!=0){
		return(1);
	}
	assert(d!=NULL);
	assert(m!=NULL);
	if ((m->l<0&&b>0)||(m->l>0&&b<0)){
		struct BigInt* t=KlBigInt_add_long(m,b);
		KlBigInt_assign(m,t);
		KlBigInt_free(t);
		t=KlBigInt_sub_long(d,1);
		KlBigInt_assign(d,t);
		KlBigInt_free(t);
	}
	if (div!=NULL){
		*div=KlBigInt_assign(*div,d);
		assert(*div!=NULL);
	}
	if (mod!=NULL){
		*mod=KlBigInt_assign(*mod,m);
		assert(*mod!=NULL);
	}
	KlBigInt_free(d);
	KlBigInt_free(m);
	return(0);
}



struct BigInt* KlBigInt_pow(struct BigInt* a,struct BigInt* b,struct BigInt* c){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	unsigned char n=0;
	if (b->l<0&&c==NULL){
		KlError_unimplemented_code();
		return(NULL);
	}
	struct BigInt* tmp;
	if (c!=NULL){
		if (KlBigInt_is_zero(c)==true){
			KlError_unimplemented_error();
			return(NULL);
		}
		if (c->l<0){
			n=1;
			c->l*=-1;
		}
		if (KlBigInt_is_one(c)==true){
			return(KlBigInt_from_long(0));
		}
		if (b->l<0){
			b->l*=-1;
			tmp=KlBigInt_invmod(a,c);
			KlBigInt_assign(a,tmp);
			KlBigInt_free(tmp);
		}
		if (a->l<0||a->l>c->l){
			if (KlBigInt_divmod(a,c,NULL,&tmp)!=0){
				return(NULL);
			}
			KlBigInt_assign(a,tmp);
			KlBigInt_free(tmp);
		}
	}
	assert((c!=NULL||a->l>0));
	assert(b->l>0);
	assert((c==NULL||c->l>0));
	struct BigInt* o=KlBigInt_from_long(1);
	intmax_t i;
	uint32_t j;
	unsigned char k;
	if (b->l<=8){
		i=b->l-1;
		while (true){
			for (j=(uint32_t)1<<(NUM_SHIFT-1);j!=0;j>>=1){
				tmp=KlBigInt_mult(o,o);
				KlBigInt_assign(o,tmp);
				KlBigInt_free(tmp);
				if (c!=NULL){
					if (KlBigInt_divmod(o,c,NULL,&tmp)!=0){
						KlBigInt_free(o);
						KlBigInt_free(tmp);
						return(NULL);
					}
					KlBigInt_assign(o,tmp);
					KlBigInt_free(tmp);
				}
				if ((*(b->v+i)&j)!=0){
					tmp=KlBigInt_mult(o,a);
					KlBigInt_assign(o,tmp);
					KlBigInt_free(tmp);
					if (c!=NULL){
						if (KlBigInt_divmod(o,c,NULL,&tmp)!=0){
							KlBigInt_free(o);
							KlBigInt_free(tmp);
							return(NULL);
						}
						KlBigInt_assign(o,tmp);
						KlBigInt_free(tmp);
					}
				}
			}
			if (i==0){
				break;
			}
			i--;
		}
	}
	else{
		struct BigInt** t=KlMem_malloc(32*sizeof(struct BigInt));
		*t=o;
		for (i=1;i<32;i++){
			tmp=KlBigInt_mult(*(t+i-1),a);
			KlBigInt_assign(*(t+i),tmp);
			KlBigInt_free(tmp);
			if (c!=NULL){
				if (KlBigInt_divmod(*(t+i),c,NULL,&tmp)!=0){
					KlBigInt_free(o);
					KlBigInt_free(tmp);
					KlMem_free(t);
					return(NULL);
				}
				KlBigInt_assign(*(t+i),tmp);
				KlBigInt_free(tmp);
			}
		}
		i=b->l-1;
		while (true){
			j=NUM_SHIFT-5;
			while (true){
				for (k=0;k<5;k++){
					tmp=KlBigInt_mult(o,o);
					KlBigInt_assign(o,tmp);
					KlBigInt_free(tmp);
					if (c!=NULL){
						if (KlBigInt_divmod(o,c,NULL,&tmp)!=0){
							KlBigInt_free(o);
							KlBigInt_free(tmp);
							KlMem_free(t);
							return(NULL);
						}
						KlBigInt_assign(o,tmp);
						KlBigInt_free(tmp);
					}
				}
				if (((*(b->v+i)>>j)&0x1f)!=0){
					tmp=KlBigInt_mult(o,*(t+((*(b->v+i)>>j)&0x1f)));
					KlBigInt_assign(o,tmp);
					KlBigInt_free(tmp);
					if (c!=NULL){
						if (KlBigInt_divmod(o,c,NULL,&tmp)!=0){
							KlBigInt_free(o);
							KlBigInt_free(tmp);
							KlMem_free(t);
							return(NULL);
						}
						KlBigInt_assign(o,tmp);
						KlBigInt_free(tmp);
					}
				}
				if (j==0){
					break;
				}
				j--;
			}
			if (i==0){
				break;
			}
			i--;
		}
		for (i=1;i<32;i++){
			KlBigInt_free(*(t+i));
		}
	}
	if (n==1&&KlBigInt_is_zero(o)==false){
		tmp=KlBigInt_sub(o,c);
		KlBigInt_assign(o,tmp);
		KlBigInt_free(tmp);
	}
	return(o);
}



struct BigInt* KlBigInt_pow_long(struct BigInt* a,long b,struct BigInt* c){
	KlMem_enter_func();
	assert(a!=NULL);
	unsigned char n=0;
	if (b<0&&c==NULL){
		KlError_unimplemented_code();
		return(NULL);
	}
	struct BigInt* tmp;
	if (c!=NULL){
		if (KlBigInt_is_zero(c)==true){
			KlError_unimplemented_error();
			return(NULL);
		}
		if (c->l<0){
			n=1;
			c->l*=-1;
		}
		if (KlBigInt_is_one(c)==true){
			return(KlBigInt_from_long(0));
		}
		if (b<0){
			b*=-1;
			tmp=KlBigInt_invmod(a,c);
			KlBigInt_assign(a,tmp);
			KlBigInt_free(tmp);
		}
		if (a->l<0||a->l>c->l){
			if (KlBigInt_divmod(a,c,NULL,&tmp)!=0){
				return(NULL);
			}
			KlBigInt_assign(a,tmp);
			KlBigInt_free(tmp);
		}
	}
	assert((c!=NULL||a->l>0));
	assert(b>=0);
	assert((c==NULL||c->l>0));
	struct BigInt* o=KlBigInt_from_long(1);
	for (uint32_t i=(uint32_t)1<<(NUM_SHIFT-1);i!=0;i>>=1){
		tmp=KlBigInt_mult(o,o);
		KlBigInt_assign(o,tmp);
		KlBigInt_free(tmp);
		if (c!=NULL){
			if (KlBigInt_divmod(o,c,NULL,&tmp)!=0){
				KlBigInt_free(o);
				KlBigInt_free(tmp);
				return(NULL);
			}
			KlBigInt_assign(o,tmp);
			KlBigInt_free(tmp);
		}
		if ((b&i)!=0){
			tmp=KlBigInt_mult(o,a);
			KlBigInt_assign(o,tmp);
			KlBigInt_free(tmp);
			if (c!=NULL){
				if (KlBigInt_divmod(o,c,NULL,&tmp)!=0){
					KlBigInt_free(o);
					KlBigInt_free(tmp);
					return(NULL);
				}
				KlBigInt_assign(o,tmp);
				KlBigInt_free(tmp);
			}
		}
	}
	if (n==1&&KlBigInt_is_zero(o)==false){
		tmp=KlBigInt_sub(o,c);
		KlBigInt_assign(o,tmp);
		KlBigInt_free(tmp);
	}
	return(o);
}



struct BigInt* KlBigInt_invmod(struct BigInt* a,struct BigInt* n){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(n!=NULL);
	struct BigInt* a1=KlBigInt_assign(NULL,a);
	struct BigInt* b=KlBigInt_from_long(1);
	struct BigInt* c=KlBigInt_from_long(0);
	struct BigInt* q=NULL;
	struct BigInt* r=NULL;
	struct BigInt* t=NULL;
	struct BigInt* s=NULL;
	while (KlBigInt_is_zero(n)==false){
		if (KlBigInt_divmod(a,n,&q,&r)!=0){
			KlBigInt_free(b);
			KlBigInt_free(c);
			KlBigInt_free(q);
			KlBigInt_free(r);
			KlBigInt_free(t);
			KlBigInt_free(s);
			return(NULL);
		}
		a=n;
		n=r;
		KlBigInt_free(t);
		KlBigInt_free(s);
		t=KlBigInt_mult(q,c);
		s=KlBigInt_sub(b,t);
		b=c;
		c=s;
	}
	KlBigInt_free(q);
	KlBigInt_free(r);
	KlBigInt_free(t);
	KlBigInt_free(s);
	KlBigInt_free(c);
	if (KlBigInt_is_one(a1)==false){
		KlBigInt_free(a1);
		KlBigInt_free(b);
		KlError_unimplemented_error();
		return(NULL);
	}
	KlBigInt_free(a1);
	return(b);
}



struct BigInt* KlBigInt_iroot(struct BigInt* r,struct BigInt* dg){
	KlMem_enter_func();
	assert(r!=NULL);
	assert(dg!=NULL);
	assert(r->l>0);
	if (r->l<0){
		KlError_unimplemented_error();
		return(NULL);
	}
	if (r->l==1&&*r->v<2){
		return(KlBigInt_assign(NULL,r));
	}
	if (KlBigInt_cmp(r,dg)==-1){
		return(KlBigInt_from_long(0));
	}
	if (KlBigInt_is_zero(dg)==true){
		return(KlBigInt_from_long(1));
	}
	struct BigInt* dg1=KlBigInt_sub_long(dg,1);
	struct BigInt* c=KlBigInt_from_long(1);
	struct BigInt* tmp=KlBigInt_add(r,dg1);
	struct BigInt* d=KlBigInt_fdiv(tmp,dg);
	KlBigInt_free(tmp);
	tmp=KlBigInt_pow(d,dg1,NULL);
	struct BigInt* tmp2=KlBigInt_fdiv(r,tmp);
	KlBigInt_free(tmp);
	tmp=KlBigInt_mult(d,dg1);
	struct BigInt* tmp3=KlBigInt_add(tmp,tmp2);
	KlBigInt_free(tmp);
	KlBigInt_free(tmp2);
	struct BigInt* e=KlBigInt_fdiv(tmp3,dg);
	KlBigInt_free(tmp3);
	if (KlBigInt_is_one(d)||KlBigInt_is_one(e)){
		KlBigInt_free(dg1);
		KlBigInt_free(c);
		KlBigInt_free(d);
		KlBigInt_free(e);
		return(KlBigInt_from_long(1));
	}
	while (KlBigInt_neq(c,d)&&KlBigInt_neq(c,e)){
		KlBigInt_free(c);
		c=d;
		d=e;
		tmp=KlBigInt_pow(e,dg1,NULL);
		tmp2=KlBigInt_fdiv(r,tmp);
		KlBigInt_free(tmp);
		tmp=KlBigInt_mult(e,dg1);
		tmp3=KlBigInt_add(tmp,tmp2);
		KlBigInt_free(tmp);
		KlBigInt_free(tmp2);
		e=KlBigInt_fdiv(tmp3,dg);
		KlBigInt_free(tmp3);
	}
	KlBigInt_free(dg1);
	KlBigInt_free(c);
	if (KlBigInt_cmp(d,e)==-1){
		KlBigInt_free(e);
		return(d);
	}
	KlBigInt_free(d);
	return(e);
}



struct BigInt* KlBigInt_iroot_long(struct BigInt* r,unsigned long dg){
	KlMem_enter_func();
	assert(r!=NULL);
	assert(r->l>0);
	if (r->l<0){
		KlError_unimplemented_error();
		return(NULL);
	}
	if (r->l==1&&*r->v<2){
		return(KlBigInt_assign(NULL,r));
	}
	if (KlBigInt_cmp_long(r,dg)==-1){
		return(KlBigInt_from_long(0));
	}
	if (dg==0){
		return(KlBigInt_from_long(1));
	}
	if (dg==1){
		return(r);
	}
	struct BigInt* c=KlBigInt_from_long(1);
	struct BigInt* tmp=KlBigInt_add_long(r,dg-1);
	struct BigInt* d=KlBigInt_fdiv_long(tmp,dg);
	tmp=KlBigInt_pow_long(d,dg-1,NULL);
	struct BigInt* tmp2=KlBigInt_fdiv(r,tmp);
	KlBigInt_free(tmp);
	tmp=KlBigInt_mult_long(d,dg-1);
	struct BigInt* tmp3=KlBigInt_add(tmp,tmp2);
	KlBigInt_free(tmp);
	KlBigInt_free(tmp2);
	struct BigInt* e=KlBigInt_fdiv_long(tmp3,dg);
	KlBigInt_free(tmp3);
	if (KlBigInt_is_one(d)||KlBigInt_is_one(e)){
		KlBigInt_free(c);
		KlBigInt_free(d);
		KlBigInt_free(e);
		return(KlBigInt_from_long(1));
	}
	while (KlBigInt_neq(c,d)&&KlBigInt_neq(c,e)){
		KlBigInt_free(c);
		c=d;
		d=e;
		tmp=KlBigInt_pow_long(e,dg-1,NULL);
		tmp2=KlBigInt_fdiv(r,tmp);
		KlBigInt_free(tmp);
		tmp=KlBigInt_mult_long(e,dg-1);
		tmp3=KlBigInt_add(tmp,tmp2);
		KlBigInt_free(tmp);
		KlBigInt_free(tmp2);
		e=KlBigInt_fdiv_long(tmp3,dg);
		KlBigInt_free(tmp3);
	}
	KlBigInt_free(c);
	if (KlBigInt_cmp(d,e)==-1){
		KlBigInt_free(e);
		return(d);
	}
	KlBigInt_free(d);
	return(e);
}



/*struct BigInt* KlBigInt_root(struct BigInt* n,struct BigInt* b){
	return(KlBigInt_pow(n,KlBigInt_div(1,b)));
}*/



bool KlBigInt_eq(struct BigInt* a,struct BigInt* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	if (a==b){
		return(true);
	}
	if (a->l!=b->l){
		return(false);
	}
	for (intmax_t i=0;i<(intmax_t)a->l*(a->l<0?-1:1);i++){
		if (*(a->v+i)!=*(b->v+i)){
			return(false);
		}
	}
	return(true);
}



bool KlBigInt_neq(struct BigInt* a,struct BigInt* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	if (a==b){
		return(false);
	}
	if (a->l!=b->l){
		return(true);
	}
	for (intmax_t i=0;i<(intmax_t)a->l*(a->l<0?-1:1);i++){
		if (*(a->v+i)!=*(b->v+i)){
			return(true);
		}
	}
	return(false);
}



signed char KlBigInt_cmp(struct BigInt* a,struct BigInt* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	if (a==b){
		return(0);
	}
	if (a->l>0||a->v[0]>0||b->l>0||b->v[0]>0){
		if (a->l<0&&b->l>0){
			return(-1);
		}
		if (a->l>0&&b->l<0){
			return(1);
		}
	}
	return(0);
}



signed char KlBigInt_cmp_long(struct BigInt* a,long b){
	KlMem_enter_func();
	assert(a!=NULL);
	if (a->l>0||*a->v>0||b!=0){
		if (a->l<0&&b>=0){
			return(-1);
		}
		if (a->l>0&&b<0){
			return(1);
		}
	}
	return(0);
}



signed char KlBigInt_cmp_abs(struct BigInt* a,struct BigInt* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	if (a==b){
		return(0);
	}
	intmax_t lna=(a->l<0?-a->l:a->l);
	intmax_t lnb=(b->l<0?-b->l:b->l);
	if (lna>lnb){
		return(1);
	}
	if (lna<lnb){
		return(-1);
	}
	for (intmax_t i=lna-1;i>=0;i++){
		if (*(a->v+i)>*(b->v+i)){
			return(1);
		}
		if (*(a->v+i)<*(b->v+i)){
			return(-1);
		}
	}
	return(0);
}



signed char KlBigInt_cmp_long_abs(struct BigInt* a,unsigned long b){
	KlMem_enter_func();
	assert(a!=NULL);
	intmax_t lna=(a->l<0?-a->l:a->l);
	if (lna>1){
		return(1);
	}
	if (lna<1){
		return(-1);
	}
	if (*a->v>b){
		return(1);
	}
	if (*a->v<b){
		return(-1);
	}
	return(0);
}



char* KlBigInt_print(struct BigInt* n){
	KlMem_enter_func();
	assert(n!=NULL);
	intmax_t ln=(n->l<0?-n->l:n->l);
	uint32_t* tmp=KlMem_malloc((1+ln+ln/((33*DEC_SHIFT)/(10*NUM_SHIFT-33*DEC_SHIFT)))*sizeof(uint32_t));
	size_t sz=0;
	size_t i=ln-1;
	size_t j;
	while (true){
		uint32_t hi=*(n->v+i);
		for (j=0;j<sz;j++){
			uint64_t z=(uint64_t)*(tmp+j)<<NUM_SHIFT|hi;
			hi=(uint32_t)(z/DEC_BASE);
			*(tmp+j)=(uint32_t)(z-(uint64_t)hi*DEC_BASE);
		}
		while (hi){
			*(tmp+sz)=hi%DEC_BASE;
			hi/=DEC_BASE;
			sz++;
		}
		if (i==0){
			break;
		}
		i--;
	}
	if (sz==0){
		*tmp=0;
		sz++;
	}
	size_t o_ln=(n->l<0?1:0)+1+(sz-1)*DEC_SHIFT;
	uint32_t pw=10;
	uint32_t rm=*(tmp+sz-1);
	while (rm>=pw){
		pw*=10;
		o_ln++;
	}
	char* o=(char*)KlMem_malloc(o_ln+1)+o_ln;
	*o=0;
	for (i=0;i<sz-1;i++){
		rm=*(tmp+i);
		for (j=0;j<DEC_SHIFT;j++){
			o--;
			*o=rm%10+48;
			rm/=10;
		}
	}
	rm=*(tmp+i);
	KlMem_free(tmp);
	while (true){
		o--;
		*o=rm%10+48;
		rm/=10;
		if (rm==0){
			break;
		}
	}
	if (n->l<0){
		o--;
		*o='-';
	}
	KlMem_ret(o);
	return(o);
}

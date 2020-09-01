#include <error.h>
#include <number.h>
#include <memory.h>
#include <stdint.h>
#include <io.h>
#include <types.h>



struct Number* KlNum_from_long(long long ll){
	KlMem_enter_func();
	struct Number o;
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
	return(KlMem_const(&o,sizeof(struct Number)));
}



struct Number* KlNum_assign(struct Number* o,struct Number* v){
	KlMem_enter_func();
	assert(v!=NULL);
	if (o==NULL){
		o=KlMem_malloc(sizeof(struct Number));
		o->v=NULL;
	}
	o->l=v->l;
	o->v=KlMem_memcpy(KlMem_realloc(o->v,o->l*sizeof(uint32_t)),v->v,o->l*sizeof(uint32_t));
	KlMem_ret(o->v);
	KlMem_ret(o);
	return(o);
}



struct Number* KlNum_assign_long(struct Number* o,long v){
	KlMem_enter_func();
	if (o==NULL){
		o=KlMem_malloc(sizeof(struct Number));
	}
	o->l=(v<0?-1:1);
	unsigned long uv=v*o->l;
	o->v=KlMem_const(&(uint32_t)uv,sizeof(uint32_t));
	return(o);
}



void KlNum_free(struct Number* n){
	KlMem_enter_func();
	if (n!=NULL){
		if (n->v!=NULL){
			KlMem_free(n->v);
		}
		KlMem_free(n);
	}
	return();
}



struct Number* KlNum_trunc(struct Number* o){
	KlMem_enter_func();
	assert(o!=NULL);
	signed char s=(o->l<0?-1:1);
	unsigned long l=o->l*s;
	while (l>1&&o->v[l-1]==0){
		l--;
	}
	if (l!=(unsigned long)(o->l*s)){
		o->l=l*s;
		o->v=KlMem_realloc(o->v,l*sizeof(uint32_t));
	}
	return(o);
}



bool KlNum_is_zero(struct Number* n){
	KlMem_enter_func();
	assert(n!=NULL);
	KlNum_trunc(n);
	return(((n->l==1||n->l==-1)&&*n->v==0));
}



bool KlNum_is_one(struct Number* n){
	KlMem_enter_func();
	assert(n!=NULL);
	KlNum_trunc(n);
	return((n->l==1&&*n->v==1));
}



struct Number* KlNum_abs(struct Number* a){
	KlMem_enter_func();
	assert(a!=NULL);
	struct Number o;
	o.l=(a->l<0?-a->l:a->l);
	o.v=KlMem_memcpy(KlMem_malloc(o.l*sizeof(uint32_t)),a->v,o.l*sizeof(uint32_t));
	assert(o.l>0);
	return(KlMem_const(&o,sizeof(struct Number)));
}



struct Number* KlNum_neg(struct Number* a){
	KlMem_enter_func();
	assert(a!=NULL);
	struct Number o;
	if (KlNum_is_zero(a)==true){
		return(KlNum_assign(NULL,a));
	}
	o.l=(a->l<0?a->l:-a->l);
	o.v=KlMem_memcpy(KlMem_malloc(o.l*sizeof(uint32_t)),a->v,o.l*sizeof(uint32_t));
	return(KlMem_const(&o,sizeof(struct Number)));
}



struct Number* KlNum_add(struct Number* a,struct Number* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	if ((a->l<0)==(b->l<0)){
		struct Number* o=KlNum_add_abs(a,b);
		o->l*=(a->l<0?-1:1);
		return(o);
	}
	else{
		struct Number* o=KlNum_sub_abs(a,b);
		if (KlNum_is_zero(o)){
			o->l=1;
		}
		else{
			o->l*=((KlNum_cmp_abs(a,b)>0?a->l<0:b->l<0)==true?-1:1);
		}
		return(o);
	}
}



struct Number* KlNum_add_long(struct Number* a,long b){
	KlMem_enter_func();
	assert(a!=NULL);
	if ((a->l<0)==(b<0)){
		struct Number* o=KlNum_add_long_abs(a,b);
		o->l*=(a->l<0?-1:1);
		return(o);
	}
	else{
		struct Number* o=KlNum_sub_long_abs(a,b);
		if (KlNum_is_zero(o)){
			o->l=1;
		}
		else{
			o->l*=((KlNum_cmp_long_abs(a,b)>0?a->l<0:b<0)==true?-1:1);
		}
		return(o);
	}
}



struct Number* KlNum_add_abs(struct Number* a,struct Number* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	unsigned long lna=(a->l<0?-a->l:a->l);
	unsigned long lnb=(b->l<0?-b->l:b->l);
	if (lna<lnb){
		struct Number* t=a;
		a=b;
		b=t;
		unsigned long lnt=lna;
		lna=lnb;
		lnb=lnt;
	}
	struct Number o;
	o.l=(lna+1)*(a->l<0?-1:1);
	o.v=KlMem_malloc((lna+1)*sizeof(uint32_t));
	uint32_t c=0;
	unsigned long i;
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
	return(KlMem_const(KlNum_trunc(&o),sizeof(struct Number)));
}



struct Number* KlNum_add_long_abs(struct Number* a,unsigned long b){
	KlMem_enter_func();
	assert(a!=NULL);
	unsigned long lna=(a->l<0?-a->l:a->l);
	struct Number o;
	o.l=(lna+1)*(a->l<0?-1:1);
	o.v=KlMem_malloc((lna+1)*sizeof(uint32_t));
	uint32_t c=a->v[0]+b;
	o.v[0]=c&NUM_MASK;
	c>>=NUM_SHIFT;
	unsigned long i=1;
	for (;i<lna;i++){
		c+=a->v[i];
		o.v[i]=c&NUM_MASK;
		c>>=NUM_SHIFT;
	}
	o.v[i]=c;
	KlMem_ret(o.v);
	return(KlMem_const(KlNum_trunc(&o),sizeof(struct Number)));
}



struct Number* KlNum_sub(struct Number* a,struct Number* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	struct Number* o=((a->l<0)==(b->l<0)?KlNum_sub_abs(a,b):KlNum_add_abs(a,b));
	if (KlNum_is_zero(o)){
		o->l=1;
	}
	else{
		o->l*=(KlNum_cmp(a,b)>0?1:-1);
	}
	return(o);
}



struct Number* KlNum_sub_long(struct Number* a,long b){
	KlMem_enter_func();
	assert(a!=NULL);
	struct Number* o=((a->l<0)==(b<0)?KlNum_sub_long_abs(a,b):KlNum_add_long_abs(a,b));
	if (KlNum_is_zero(o)){
		o->l=1;
	}
	else{
		o->l*=(KlNum_cmp_long(a,b)>0?1:-1);
	}
	return(o);
}



struct Number* KlNum_sub_abs(struct Number* a,struct Number* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	unsigned long lna=(a->l<0?-a->l:a->l);
	unsigned long lnb=(b->l<0?-b->l:b->l);
	if (lna<lnb){
		struct Number* t=a;
		a=b;
		b=t;
		unsigned long lnt=lna;
		lna=lnb;
		lnb=lnt;
	}
	else if (lna==lnb){
		unsigned long i=lna;
		while (a->v[i]==b->v[i]){
			if (i==0){
				return(KlNum_from_long(0));
			}
			i--;
		}
		if (a->v[i]<b->v[i]){
			struct Number* t=a;
			a=b;
			b=t;
		}
		lna=lnb=i+1;
	}
	struct Number o;
	o.l=lna;
	o.v=KlMem_malloc(lna*sizeof(uint32_t));
	uint32_t br=0;
	unsigned long i=0;
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
	return(KlMem_const(KlNum_trunc(&o),sizeof(struct Number)));
}



struct Number* KlNum_sub_long_abs(struct Number* a,unsigned long b){
	KlMem_enter_func();
	assert(a!=NULL);
	unsigned long lna=(a->l<0?-a->l:a->l);
	if (lna==1){
		if (*a->v==b){
			return(KlNum_from_long(0));
		}
		if (*a->v<b){
			unsigned long t=a->v[0];
			*a->v=(uint32_t)b;
			b=t;
		}
	}
	struct Number o;
	o.l=lna;
	o.v=KlMem_malloc(lna*sizeof(uint32_t));
	uint32_t br=*a->v-b;
	*o.v=br&NUM_MASK;
	br>>=NUM_SHIFT;
	br&=1;
	for (unsigned long i=1;i<lna;i++){
		br=a->v[i]-br;
		o.v[i]=br&NUM_MASK;
		br>>=NUM_SHIFT;
		br&=1;
	}
	return(KlMem_const(KlNum_trunc(&o),sizeof(struct Number)));
}



struct Number* KlNum_mult(struct Number* a,struct Number* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	struct Number* o=KlNum_mult_abs(a,b);
	if (KlNum_is_zero(o)){
		o->l=1;
	}
	else{
		o->l*=((a->l<0)==(b->l<0)?1:-1);
	}
	return(o);
}



struct Number* KlNum_mult_long(struct Number* a,long b){
	KlMem_enter_func();
	assert(a!=NULL);
	struct Number* o=KlNum_mult_long_abs(a,b);
	if (KlNum_is_zero(o)){
		o->l=1;
	}
	else{
		o->l*=((a->l<0)==(b<0)?1:-1);
	}
	return(o);
}



struct Number* KlNum_mult_abs(struct Number* a,struct Number* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	if (KlNum_is_zero(a)||KlNum_is_zero(b)){
		return(KlNum_from_long(0));
	}
	if (KlNum_is_one(a)){
		return(KlNum_assign(NULL,b));
	}
	if (KlNum_is_one(b)){
		return(KlNum_assign(NULL,a));
	}
	unsigned long lna=(a->l<0?-a->l:a->l);
	unsigned long lnb=(b->l<0?-b->l:b->l);
	struct Number o;
	o.l=lna+lnb;
	o.v=KlMem_calloc(o.l,sizeof(uint32_t));
	uint64_t c;
	unsigned long j;
	unsigned long bi;
	for (unsigned long i=0;i<lna;i++){
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
	return(KlMem_const(KlNum_trunc(&o),sizeof(struct Number)));
}



struct Number* KlNum_mult_long_abs(struct Number* a,unsigned long b){
	KlMem_enter_func();
	assert(a!=NULL);
	if (KlNum_is_zero(a)||b==0){
		return(KlNum_from_long(0));
	}
	if (KlNum_is_one(a)){
		return(KlNum_from_long(b));
	}
	if (b==1){
		return(KlNum_assign(NULL,a));
	}
	unsigned long lna=(a->l<0?-a->l:a->l);
	struct Number o;
	o.l=lna+1;
	o.v=KlMem_calloc(o.l,sizeof(uint32_t));
	unsigned long j;
	for (unsigned long i=0;i<lna;i++){
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
	return(KlMem_const(KlNum_trunc(&o),sizeof(struct Number)));
}



struct Number* KlNum_div(struct Number* a,struct Number* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL&&KlNum_is_zero(b)==false);
	KlError_unimplemented_code();
	return(NULL);
}



struct Number* KlNum_fdiv(struct Number* a,struct Number* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL&&KlNum_is_zero(b)==false);
	if (KlNum_is_zero(b)==true){
		KlError_unimplemented_error();
		return(NULL);
	}
	if ((a->l==1||a->l==-1)&&(b->l==1||b->l==-1)){
		return(KlNum_from_long((a->l==b->l?*a->v/(*b->v):-1-(*a->v-1)/(*b->v))));
	}
	struct Number* o=NULL;
	if (KlNum_divmod(a,b,&o,NULL)!=0){
		return(NULL);
	}
	return(o);
}



struct Number* KlNum_fdiv_long(struct Number* a,long b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=0);
	if (b==0){
		KlError_unimplemented_error();
		return(NULL);
	}
	if (a->l==1||a->l==-1){
		return(KlNum_from_long((a->l==(b<0?-1:1)?*a->v/b:-1-(*a->v-1)/b)));
	}
	struct Number* o=NULL;
	if (KlNum_divmod_long(a,b,&o,NULL)!=0){
		return(NULL);
	}
	return(o);
}



struct Number* KlNum_mod(struct Number* a,struct Number* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL&&KlNum_is_zero(b)==false);
	if (KlNum_is_zero(b)==true){
		KlError_unimplemented_error();
		return(NULL);
	}
	if ((a->l==1||a->l==-1)&&(b->l==1||b->l==-1)){
		return(KlNum_from_long((a->l==b->l?*a->v%*b->v:*b->v-1-(*a->v-1)%*b->v)*(int32_t)b->l));
	}
	struct Number* o=NULL;
	if (KlNum_divmod(a,b,NULL,&o)!=0){
		return(NULL);
	}
	return(o);
}



struct Number* KlNum_mod_long(struct Number* a,long b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=0);
	if (b==0){
		KlError_unimplemented_error();
		return(NULL);
	}
	if (a->l==1||a->l==-1){
		return(KlNum_from_long((a->l==(b<0?-1:1)?*a->v%b:b-1-(*a->v-1)%b)*(int32_t)(b<0?-1:1)));
	}
	struct Number* o=NULL;
	if (KlNum_divmod_long(a,b,NULL,&o)!=0){
		return(NULL);
	}
	return(o);
}



unsigned char KlNum_divrem(struct Number* a,struct Number* b,struct Number** div,struct Number** rem){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL&&KlNum_is_zero(b)==false);
	assert(div!=NULL||rem!=NULL);
	if (KlNum_is_zero(b)){
		KlError_unimplemented_error();
		return(1);
	}
	unsigned long lna=(a->l<0?-a->l:a->l);
	unsigned long lnb=(b->l<0?-b->l:b->l);
	if (lna<lnb||(lna==lnb&&*(a->v+lna-1)<*(b->v+lnb-1))){
		if (div!=NULL){
			*div=KlNum_assign_long(*div,0);
			assert(*div!=NULL);
		}
		if (rem!=NULL){
			*rem=KlNum_assign(*rem,a);
			assert(*rem!=NULL);
		}
		return(0);
	}
	if (lnb==1){
		uint64_t rm=0;
		unsigned long tln=lna;
		uint32_t hi;
		if (div!=NULL){
			if (*div==NULL){
				*div=KlMem_malloc(sizeof(struct Number));
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
			*rem=KlNum_assign_long(*rem,(uint32_t)rm);
			assert(*rem!=NULL);
		}
	}
	else{
		if (KlNum_divrem_abs(a,b,div,rem)!=0){
			return(1);
		}
	}
	if (div!=NULL&&(a->l<0)!=(b->l<0)){
		(*div)->l*=-1;
	}
	if (rem!=NULL&&a->l<0&&KlNum_is_zero(*rem)==false){
		(*rem)->l*=-1;
	}
	assert((div!=NULL&&(*div)!=NULL)||(rem!=NULL&&(*rem)!=NULL));
	return(0);
}



unsigned char KlNum_divrem_long(struct Number* a,long b,struct Number** div,struct Number** rem){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=0);
	assert(div!=NULL||rem!=NULL);
	if (b==0){
		KlError_unimplemented_error();
		return(1);
	}
	unsigned long lna=(a->l<0?-a->l:a->l);
	unsigned long ub=(b<0?-b:b);
	if (lna==1&&*(a->v+lna-1)<ub){
		if (div!=NULL){
			*div=KlNum_assign_long(*div,0);
		}
		if (rem!=NULL){
			*rem=KlNum_assign(*rem,a);
		}
		return(0);
	}
	if (*div==NULL){
		*div=KlMem_malloc(sizeof(struct Number));
	}
	(*div)->l=lna;
	(*div)->v=(uint32_t*)KlMem_malloc(lna*sizeof(uint32_t))+lna;
	uint64_t r=0;
	unsigned long tln=lna;
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
	*rem=KlNum_assign_long(*rem,(uint32_t)r);
	if ((a->l<0)!=(b<0)){
		(*div)->l*=-1;
	}
	if (a->l<0&&KlNum_is_zero(*rem)==false){
		(*rem)->l*=-1;
	}
	assert((div!=NULL&&(*div)!=NULL)||(rem!=NULL&&(*rem)!=NULL));
	return(0);
}



unsigned char KlNum_divrem_abs(struct Number* a,struct Number* b,struct Number** div,struct Number** rem){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL&&KlNum_is_zero(b)==false);
	assert(div!=NULL||rem!=NULL);
	if (KlNum_is_zero(b)==true){
		KlError_unimplemented_error();
		return(1);
	}
	unsigned long lna=(a->l<0?-a->l:a->l);
	unsigned long lnb=(b->l<0?-b->l:b->l);
	struct Number v;
	v.l=lna+1;
	v.v=KlMem_malloc(v.l*sizeof(uint32_t));
	struct Number w;
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
	for (unsigned long i=0;i<lnb;i++){
		uint64_t s=(uint64_t)*(b->v+i)<<d|c;
		*(w.v+i)=(uint32_t)s&NUM_MASK;
		c=(uint32_t)(s>>NUM_SHIFT);
	}
	c=0;
	for (unsigned long i=0;i<lna;i++){
		uint64_t s=(uint64_t)*(a->v+i)<<d|c;
		*(v.v+i)=(uint32_t)s&NUM_MASK;
		c=(uint32_t)(s>>NUM_SHIFT);
	}
	if (c!=0||*(v.v+lna-1)>=*(w.v+lnb-1)){
		v.v[lna]=c;
		lna++;
	}
	unsigned long k=lna-lnb;
	struct Number t;
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
		for (unsigned long i=0;i<lnb;i++){
			int64_t z=(int32_t)*(vk+i)+zhi-(int64_t)q*(int64_t)*(w.v+i);
			*(vk+i)=(uint32_t)z&NUM_MASK;
			zhi=(int32_t)z>>NUM_SHIFT;
		}
		if ((int32_t)vtop+zhi<0){
			c=0;
			for (unsigned long i=0;i<lnb;i++){
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
	for (unsigned long i=lnb-1;i>0;i--){
		uint64_t s=(uint64_t)c<<NUM_SHIFT|*(w.v+i);
		c=(uint32_t)s&ms;
		*(v.v+i)=(uint32_t)(s>>d);
	}
	assert(c==0);
	*div=KlNum_assign(*div,&t);
	*rem=KlNum_assign(*rem,&w);
	assert((div!=NULL&&(*div)!=NULL)||(rem!=NULL&&(*rem)!=NULL));
	KlMem_free(v.v);
	KlMem_free(w.v);
	KlMem_free(t.v);
	return(0);
}



unsigned char KlNum_divmod(struct Number* a,struct Number* b,struct Number** div,struct Number** mod){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL&&KlNum_is_zero(b)==false);
	assert(div!=NULL||mod!=NULL);
	if (KlNum_is_zero(b)==true){
		KlError_unimplemented_error();
		return(1);
	}
	if ((a->l==1||a->l==-1)&&(b->l==1||b->l==-1)){
		if (div!=NULL){
			KlNum_assign_long(*div,(a->l==b->l?*a->v/(*b->v):-1-(*a->v-1)/(*b->v)));
		}
		if (mod!=NULL){
			KlNum_assign_long(*mod,(a->l==b->l?*a->v%*b->v:*b->v-1-(*a->v-1)%*b->v)*(int32_t)b->l);
		}
		return(0);
	}
	struct Number* d=NULL;
	struct Number* m=NULL;
	struct Number** dp=&d;
	struct Number** mp=&m;
	if (KlNum_divrem(a,b,dp,mp)!=0){
		return(1);
	}
	assert(d!=NULL);
	assert(m!=NULL);
	if ((m->l<0&&b->l>0)||(m->l>0&&b->l<0)){
		struct Number* tmp=KlNum_add(m,b);
		KlNum_free(m);
		m=tmp;
		KlNum_free(tmp);
		tmp=KlNum_sub_long(d,1);
		KlNum_free(d);
		d=tmp;
		KlNum_free(tmp);
	}
	if (div!=NULL){
		*div=KlNum_assign(*div,d);
		assert(*div!=NULL);
	}
	if (mod!=NULL){
		*mod=KlNum_assign(*mod,m);
		assert(*mod!=NULL);
	}
	KlNum_free(d);
	KlNum_free(m);
	return(0);
}



unsigned char KlNum_divmod_long(struct Number* a,long b,struct Number** div,struct Number** mod){
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
			KlNum_assign_long(*div,(a->l==(b<0?-1:1)?*a->v/b:-1-(*a->v-1)/b));
		}
		if (mod!=NULL){
			KlNum_assign_long(*mod,(a->l==(b<0?-1:1)?*a->v%b:b-1-(*a->v-1)%b)*(int32_t)(b<0?-1:1));
		}
		return(0);
	}
	struct Number* d=KlMem_malloc(sizeof(struct Number));
	struct Number* m=KlMem_malloc(sizeof(struct Number));
	if (KlNum_divrem_long(a,b,&d,&m)!=0){
		return(1);
	}
	assert(d!=NULL);
	assert(m!=NULL);
	if ((m->l<0&&b>0)||(m->l>0&&b<0)){
		struct Number* t=KlNum_add_long(m,b);
		KlNum_assign(m,t);
		KlNum_free(t);
		t=KlNum_sub_long(d,1);
		KlNum_assign(d,t);
		KlNum_free(t);
	}
	if (div!=NULL){
		*div=KlNum_assign(*div,d);
		assert(*div!=NULL);
	}
	if (mod!=NULL){
		*mod=KlNum_assign(*mod,m);
		assert(*mod!=NULL);
	}
	KlNum_free(d);
	KlNum_free(m);
	return(0);
}



struct Number* KlNum_pow(struct Number* a,struct Number* b,struct Number* c){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	unsigned char n=0;
	if (b->l<0&&c==NULL){
		KlError_unimplemented_code();
		return(NULL);
	}
	struct Number* tmp;
	if (c!=NULL){
		if (KlNum_is_zero(c)==true){
			KlError_unimplemented_error();
			return(NULL);
		}
		if (c->l<0){
			n=1;
			c->l*=-1;
		}
		if (KlNum_is_one(c)==true){
			return(KlNum_from_long(0));
		}
		if (b->l<0){
			b->l*=-1;
			tmp=KlNum_invmod(a,c);
			KlNum_assign(a,tmp);
			KlNum_free(tmp);
		}
		if (a->l<0||a->l>c->l){
			if (KlNum_divmod(a,c,NULL,&tmp)!=0){
				return(NULL);
			}
			KlNum_assign(a,tmp);
			KlNum_free(tmp);
		}
	}
	assert((c!=NULL||a->l>0));
	assert(b->l>0);
	assert((c==NULL||c->l>0));
	struct Number* o=KlNum_from_long(1);
	unsigned long i;
	unsigned long j;
	unsigned long k;
	if (b->l<=8){
		i=b->l-1;
		while (true){
			for (j=(uint32_t)1<<(NUM_SHIFT-1);j!=0;j>>=1){
				tmp=KlNum_mult(o,o);
				KlNum_assign(o,tmp);
				KlNum_free(tmp);
				if (c!=NULL){
					if (KlNum_divmod(o,c,NULL,&tmp)!=0){
						KlNum_free(o);
						KlNum_free(tmp);
						return(NULL);
					}
					KlNum_assign(o,tmp);
					KlNum_free(tmp);
				}
				if ((*(b->v+i)&j)!=0){
					tmp=KlNum_mult(o,a);
					KlNum_assign(o,tmp);
					KlNum_free(tmp);
					if (c!=NULL){
						if (KlNum_divmod(o,c,NULL,&tmp)!=0){
							KlNum_free(o);
							KlNum_free(tmp);
							return(NULL);
						}
						KlNum_assign(o,tmp);
						KlNum_free(tmp);
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
		struct Number** t=KlMem_malloc(32*sizeof(struct Number));
		*t=o;
		for (i=1;i<32;i++){
			tmp=KlNum_mult(*(t+i-1),a);
			KlNum_assign(*(t+i),tmp);
			KlNum_free(tmp);
			if (c!=NULL){
				if (KlNum_divmod(*(t+i),c,NULL,&tmp)!=0){
					KlNum_free(o);
					KlNum_free(tmp);
					KlMem_free(t);
					return(NULL);
				}
				KlNum_assign(*(t+i),tmp);
				KlNum_free(tmp);
			}
		}
		i=b->l-1;
		while (true){
			j=NUM_SHIFT-5;
			while (true){
				for (k=0;k<5;k++){
					tmp=KlNum_mult(o,o);
					KlNum_assign(o,tmp);
					KlNum_free(tmp);
					if (c!=NULL){
						if (KlNum_divmod(o,c,NULL,&tmp)!=0){
							KlNum_free(o);
							KlNum_free(tmp);
							KlMem_free(t);
							return(NULL);
						}
						KlNum_assign(o,tmp);
						KlNum_free(tmp);
					}
				}
				if (((*(b->v+i)>>j)&0x1f)!=0){
					tmp=KlNum_mult(o,*(t+((*(b->v+i)>>j)&0x1f)));
					KlNum_assign(o,tmp);
					KlNum_free(tmp);
					if (c!=NULL){
						if (KlNum_divmod(o,c,NULL,&tmp)!=0){
							KlNum_free(o);
							KlNum_free(tmp);
							KlMem_free(t);
							return(NULL);
						}
						KlNum_assign(o,tmp);
						KlNum_free(tmp);
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
			KlNum_free(*(t+i));
		}
	}
	if (n==1&&KlNum_is_zero(o)==false){
		tmp=KlNum_sub(o,c);
		KlNum_assign(o,tmp);
		KlNum_free(tmp);
	}
	return(o);
}



struct Number* KlNum_pow_long(struct Number* a,long b,struct Number* c){
	KlMem_enter_func();
	assert(a!=NULL);
	unsigned char n=0;
	if (b<0&&c==NULL){
		KlError_unimplemented_code();
		return(NULL);
	}
	struct Number* tmp;
	if (c!=NULL){
		if (KlNum_is_zero(c)==true){
			KlError_unimplemented_error();
			return(NULL);
		}
		if (c->l<0){
			n=1;
			c->l*=-1;
		}
		if (KlNum_is_one(c)==true){
			return(KlNum_from_long(0));
		}
		if (b<0){
			b*=-1;
			tmp=KlNum_invmod(a,c);
			KlNum_assign(a,tmp);
			KlNum_free(tmp);
		}
		if (a->l<0||a->l>c->l){
			if (KlNum_divmod(a,c,NULL,&tmp)!=0){
				return(NULL);
			}
			KlNum_assign(a,tmp);
			KlNum_free(tmp);
		}
	}
	assert((c!=NULL||a->l>0));
	assert(b>=0);
	assert((c==NULL||c->l>0));
	struct Number* o=KlNum_from_long(1);
	for (unsigned long i=(uint32_t)1<<(NUM_SHIFT-1);i!=0;i>>=1){
		tmp=KlNum_mult(o,o);
		KlNum_assign(o,tmp);
		KlNum_free(tmp);
		if (c!=NULL){
			if (KlNum_divmod(o,c,NULL,&tmp)!=0){
				KlNum_free(o);
				KlNum_free(tmp);
				return(NULL);
			}
			KlNum_assign(o,tmp);
			KlNum_free(tmp);
		}
		if ((b&i)!=0){
			tmp=KlNum_mult(o,a);
			KlNum_assign(o,tmp);
			KlNum_free(tmp);
			if (c!=NULL){
				if (KlNum_divmod(o,c,NULL,&tmp)!=0){
					KlNum_free(o);
					KlNum_free(tmp);
					return(NULL);
				}
				KlNum_assign(o,tmp);
				KlNum_free(tmp);
			}
		}
	}
	if (n==1&&KlNum_is_zero(o)==false){
		tmp=KlNum_sub(o,c);
		KlNum_assign(o,tmp);
		KlNum_free(tmp);
	}
	return(o);
}



struct Number* KlNum_invmod(struct Number* a,struct Number* n){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(n!=NULL);
	struct Number* a1=KlNum_assign(NULL,a);
	struct Number* b=KlNum_from_long(1);
	struct Number* c=KlNum_from_long(0);
	struct Number* q=NULL;
	struct Number* r=NULL;
	struct Number* t=NULL;
	struct Number* s=NULL;
	while (KlNum_is_zero(n)==false){
		if (KlNum_divmod(a,n,&q,&r)!=0){
			KlNum_free(b);
			KlNum_free(c);
			KlNum_free(q);
			KlNum_free(r);
			KlNum_free(t);
			KlNum_free(s);
			return(NULL);
		}
		a=n;
		n=r;
		t=KlNum_mult(q,c);
		s=KlNum_sub(b,t);
		b=c;
		c=s;
	}
	KlNum_free(q);
	KlNum_free(r);
	KlNum_free(t);
	KlNum_free(s);
	KlNum_free(c);
	if (KlNum_is_one(a1)==false){
		KlNum_free(a1);
		KlNum_free(b);
		KlError_unimplemented_error();
		return(NULL);
	}
	KlNum_free(a1);
	return(b);
}



struct Number* KlNum_iroot(struct Number* r,struct Number* dg){
	KlMem_enter_func();
	assert(r!=NULL);
	assert(dg!=NULL);
	assert(r->l>0);
	if (r->l<0){
		KlError_unimplemented_error();
		return(NULL);
	}
	if (r->l==1&&*r->v<2){
		return(KlNum_assign(NULL,r));
	}
	if (KlNum_is_zero(dg)==true){
		return(KlNum_from_long(1));
	}
	struct Number* dg1=KlNum_sub_long(dg,1);
	struct Number* c=KlNum_from_long(1);
	struct Number* tmp=KlNum_add(r,dg1);
	struct Number* d=KlNum_fdiv(tmp,dg);
	KlNum_free(tmp);
	tmp=KlNum_pow(d,dg1,NULL);
	struct Number* tmp2=KlNum_fdiv(r,tmp);
	KlNum_free(tmp);
	tmp=KlNum_mult(d,dg1);
	struct Number* tmp3=KlNum_add(tmp,tmp2);
	KlNum_free(tmp);
	KlNum_free(tmp2);
	struct Number* e=KlNum_fdiv(tmp3,dg);
	KlNum_free(tmp3);
	if (KlNum_is_one(d)||KlNum_is_one(e)){
		KlNum_free(dg1);
		KlNum_free(c);
		KlNum_free(d);
		KlNum_free(e);
		return(KlNum_from_long(1));
	}
	while (KlNum_neq(c,d)&&KlNum_neq(c,e)){
		KlNum_free(c);
		c=d;
		d=e;
		tmp=KlNum_pow(e,dg1,NULL);
		tmp2=KlNum_fdiv(r,tmp);
		KlNum_free(tmp);
		tmp=KlNum_mult(e,dg1);
		tmp3=KlNum_add(tmp,tmp2);
		KlNum_free(tmp);
		KlNum_free(tmp2);
		e=KlNum_fdiv(tmp3,dg);
		KlNum_free(tmp3);
	}
	KlNum_free(dg1);
	KlNum_free(c);
	if (KlNum_cmp(d,e)==-1){
		KlNum_free(e);
		return(d);
	}
	KlNum_free(d);
	return(e);
}



struct Number* KlNum_iroot_long(struct Number* r,unsigned long dg){
	KlMem_enter_func();
	assert(r!=NULL);
	assert(r->l>0);
	if (r->l<0){
		KlError_unimplemented_error();
		return(NULL);
	}
	if (r->l==1&&*r->v<2){
		return(KlNum_assign(NULL,r));
	}
	if (dg==0){
		return(KlNum_from_long(1));
	}
	if (dg==1){
		return(r);
	}
	struct Number* c=KlNum_from_long(1);
	struct Number* tmp=KlNum_add_long(r,dg-1);
	struct Number* d=KlNum_fdiv_long(tmp,dg);
	tmp=KlNum_pow_long(d,dg-1,NULL);
	struct Number* tmp2=KlNum_fdiv(r,tmp);
	KlNum_free(tmp);
	tmp=KlNum_mult_long(d,dg-1);
	struct Number* tmp3=KlNum_add(tmp,tmp2);
	KlNum_free(tmp);
	KlNum_free(tmp2);
	struct Number* e=KlNum_fdiv_long(tmp3,dg);
	KlNum_free(tmp3);
	if (KlNum_is_one(d)||KlNum_is_one(e)){
		KlNum_free(c);
		KlNum_free(d);
		KlNum_free(e);
		return(KlNum_from_long(1));
	}
	while (KlNum_neq(c,d)&&KlNum_neq(c,e)){
		KlNum_free(c);
		c=d;
		d=e;
		tmp=KlNum_pow_long(e,dg-1,NULL);
		tmp2=KlNum_fdiv(r,tmp);
		KlNum_free(tmp);
		tmp=KlNum_mult_long(e,dg-1);
		tmp3=KlNum_add(tmp,tmp2);
		KlNum_free(tmp);
		KlNum_free(tmp2);
		e=KlNum_fdiv_long(tmp3,dg);
		KlNum_free(tmp3);
	}
	KlNum_free(c);
	if (KlNum_cmp(d,e)==-1){
		KlNum_free(e);
		return(d);
	}
	KlNum_free(d);
	return(e);
}



/*struct Number* KlNum_root(struct Number* n,struct Number* b){
	return(KlNum_pow(n,KlNum_div(1,b)));
}*/



bool KlNum_eq(struct Number* a,struct Number* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	if (a==b){
		return(true);
	}
	if (a->l!=b->l){
		return(false);
	}
	for (unsigned long i=0;i<(unsigned long)a->l*(a->l<0?-1:1);i++){
		if (*(a->v+i)!=*(b->v+i)){
			return(false);
		}
	}
	return(true);
}



bool KlNum_neq(struct Number* a,struct Number* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	if (a==b){
		return(false);
	}
	if (a->l!=b->l){
		return(true);
	}
	for (unsigned long i=0;i<(unsigned long)a->l*(a->l<0?-1:1);i++){
		if (*(a->v+i)!=*(b->v+i)){
			return(true);
		}
	}
	return(false);
}



signed char KlNum_cmp(struct Number* a,struct Number* b){
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



signed char KlNum_cmp_long(struct Number* a,long b){
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



signed char KlNum_cmp_abs(struct Number* a,struct Number* b){
	KlMem_enter_func();
	assert(a!=NULL);
	assert(b!=NULL);
	if (a==b){
		return(0);
	}
	unsigned long lna=(a->l<0?-a->l:a->l);
	unsigned long lnb=(b->l<0?-b->l:b->l);
	if (lna>lnb){
		return(1);
	}
	if (lna<lnb){
		return(-1);
	}
	for (unsigned long i=lna-1;i>=0;i++){
		if (*(a->v+i)>*(b->v+i)){
			return(1);
		}
		if (*(a->v+i)<*(b->v+i)){
			return(-1);
		}
	}
	return(0);
}



signed char KlNum_cmp_long_abs(struct Number* a,unsigned long b){
	KlMem_enter_func();
	assert(a!=NULL);
	unsigned long lna=(a->l<0?-a->l:a->l);
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



char* KlNum_print(struct Number* n){
	KlMem_enter_func();
	assert(n!=NULL);
	unsigned long ln=(n->l<0?-n->l:n->l);
	uint32_t* tmp=KlMem_malloc((1+ln+ln/((33*DEC_SHIFT)/(10*NUM_SHIFT-33*DEC_SHIFT)))*sizeof(uint32_t));
	unsigned long sz=0;
	unsigned long i=ln-1;
	unsigned long j;
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
	unsigned long o_ln=(n->l<0?1:0)+1+(sz-1)*DEC_SHIFT;
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

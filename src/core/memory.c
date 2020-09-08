#include <memory.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>
#include <io.h>
#include <shared.h>
#undef return



#ifndef NDEBUG
const char* _s_sig="\x63\x57\xfd\xd4\x82\x5b\x10\xb5";
const char* _e_sig="\x44\xaa\x66\x40\x7c\xcc\x54\x67";
struct _Node{
	struct _Node* p;
	struct _Node* n;
	void* ptr;
	size_t sz;
	const char* f;
	unsigned int ln;
	const char* fn;
	unsigned long fcl;
	bool t;
	char* t_v;
	const char* t_nm;
	const char* mf;
	unsigned int mln;
	const char* mfn;
	bool r;
} _head={
	NULL,
	NULL,
	NULL,
	0,
	NULL,
	0,
	NULL,
	0,
	false,
	NULL,
	NULL,
	NULL,
	0,
	NULL,
	false
};
struct _FuncLvl{
	struct _FuncLvl* n;
	const char* f;
	const char* fn;
	unsigned long l;
} _fl_head={
	NULL,
	NULL,
	NULL,
	0
};
bool _er=false;
bool _m_err=false;



void KlMem_abrt_(int p){
	_m_err=true;
	signal(SIGABRT,SIG_DFL);
	raise(SIGABRT);
}



unsigned long _get_call_lvl(const char* f,const char* fn){
	struct _FuncLvl* fl=&_fl_head;
	while (fl->f!=f||fl->fn!=fn){
		assert(fl->n!=NULL);
		fl=fl->n;
	}
	return fl->l;
}



void _dump(void* s,size_t sz){
	printf("Memory Dump Of Address 0x%016llx - 0x%016llx (+ %llu):\n",(unsigned long long int)s,(unsigned long long int)s+sz,sz);
	size_t mx_n=8*(((sz+7)>>3)-1);
	unsigned char mx=1;
	while (mx_n>10){
		mx++;
		mx_n/=10;
	}
	char* f=malloc(mx+20);
	sprintf_s(f,mx+20,"0x%%016llx + %% %ullu: ",mx);
	for (size_t i=0;i<sz;i+=8){
		printf(f,(uintptr_t)s,(uintptr_t)i);
		unsigned char j;
		for (j=0;j<8;j++){
			if (i+j>=sz){
				break;
			}
			printf("%02x",*((unsigned char*)s+i+j));
			printf(" ");
		}
		if (j==0){
			break;
		}
		while (j<8){
			printf("   ");
			j++;
		}
		printf("| ");
		for (j=0;j<8;j++){
			if (i+j>=sz){
				break;
			}
			unsigned char c=*((unsigned char*)s+i+j);
			if (c>0x1f&&c!=0x7f){
				printf("%c  ",(char)c);
			}
			else{
				printf("%02x ",c);
			}
		}
		printf("\n");
	}
	free(f);
}



void _check_heap(const char* f,unsigned int ln,const char* fn){
	struct _Node* n=&_head;
	while (true){
		if (n->ptr!=NULL){
			for (unsigned char i=0;i<8;i++){
				if (*((char*)n->ptr+i)!=*(_s_sig+i)){
					if (f==NULL){
						printf("ERROR: Address 0x%016llx Allocated at %s:%u (%s) has been Corrupted (0x%016llx-%u)!\n",((unsigned long long int)n->ptr+8),n->f,n->ln,n->fn,((unsigned long long int)n->ptr+8),8-i);
					}
					else{
						printf("ERROR: %s:%u (%s): Address 0x%016llx Allocated at %s:%u (%s) has been Corrupted (0x%016llx-%u)!\n",f,ln,fn,((unsigned long long int)n->ptr+8),n->f,n->ln,n->fn,((unsigned long long int)n->ptr+8),8-i);
					}
					_dump(n->ptr,n->sz+16);
					_m_err=true;
					raise(SIGABRT);
					return;
				}
			}
			for (unsigned char i=0;i<8;i++){
				if (*((char*)n->ptr+n->sz+i+8)!=*(_e_sig+i)){
					if (f==NULL){
						printf("ERROR: Address 0x%016llx Allocated at %s:%u (%s) has been Corrupted (0x%016llx+%llu+%u)!\n",((unsigned long long int)n->ptr+8),n->f,n->ln,n->fn,((unsigned long long int)n->ptr+8),n->sz,i+1);
					}
					else{
						printf("ERROR: %s:%u (%s): Address 0x%016llx Allocated at %s:%u (%s) has been Corrupted (0x%016llx+%llu+%u)!\n",f,ln,fn,((unsigned long long int)n->ptr+8),n->f,n->ln,n->fn,((unsigned long long int)n->ptr+8),n->sz,i+1);
					}
					_dump(n->ptr,n->sz+16);
					_m_err=true;
					raise(SIGABRT);
					return;
				}
			}
			if (n->t==true){
				bool ch=false;
				for (size_t i=0;i<n->sz;i++){
					if (*((char*)n->ptr+i+8)!=*((char*)n->t_v+i)){
						if (ch==false){
							printf("TRACE: %s:%u (%s): %s (0x%016llx): Detected Memory Change:",f,ln,fn,n->t_nm,(unsigned long long int)n->ptr);
							ch=true;
						}
						else{
							printf(";");
						}
						printf(" +%llu (%02x -> %02x)",i,*((unsigned char*)n->t_v+i),*((unsigned char*)n->ptr+i+8));
						*((char*)n->t_v+i)=*((char*)n->ptr+i+8);
					}
				}
				if (ch==true){
					printf("\n");
				}
			}
		}
		if (n->n==NULL){
			break;
		}
		n=n->n;
	}
}



void* _get(void* p,const char* msg,bool wu,const char* f,unsigned int ln,const char* fn){
	if (p==NULL){
		printf("ERROR: %s:%u (%s): %s Null Pointer!\n",f,ln,fn,msg);
		_m_err=true;
		raise(SIGABRT);
		return NULL;
	}
	struct _Node* n=&_head;
	while (n->ptr!=(char*)p-8){
		if (n->n==NULL){
			if (wu==true){
				printf("WARN:  %s:%u (%s): %s Unknown Pointer!\n",f,ln,fn,msg);
			}
			return p;
		}
		n=n->n;
	}
	for (unsigned char i=0;i<8;i++){
		if (*((char*)n->ptr+i)!=*(_s_sig+i)){
			printf("ERROR: %s:%u (%s): Address 0x%016llx Allocated at %s:%u (%s) has been Corrupted (0x%016llx-%u)!\n",f,ln,fn,((unsigned long long int)n->ptr+8),n->f,n->ln,n->fn,((unsigned long long int)n->ptr+8),8-i);
			_dump(n->ptr,n->sz+16);
			_m_err=true;
			raise(SIGABRT);
			return NULL;
		}
	}
	for (unsigned char i=0;i<8;i++){
		if (*((char*)n->ptr+n->sz+i+8)!=*(_e_sig+i)){
			printf("ERROR: %s:%u (%s): Address 0x%016llx Allocated at %s:%u (%s) has been Corrupted (0x%016llx+%llu+%u)!\n",f,ln,fn,((unsigned long long int)n->ptr+8),n->f,n->ln,n->fn,((unsigned long long int)n->ptr+8),n->sz,i+1);
			_dump(n->ptr,n->sz+16);
			_m_err=true;
			raise(SIGABRT);
			return NULL;
		}
	}
	return n;
}



void* KlMem_malloc_(size_t sz,const char* f,unsigned int ln,const char* fn){
	if (_er==false){
		atexit(KlMem_check_all_allocated_);
		signal(SIGABRT,KlMem_abrt_);
		_er=true;
	}
	_check_heap(f,ln,fn);
	assert(sz>=0);
	struct _Node* n=&_head;
	while (n->ptr!=NULL){
		if (n->n==NULL){
			n->n=malloc(sizeof(struct _Node));
			n->n->p=NULL;
			n->n->n=NULL;
			n->n->ptr=NULL;
			n->n->sz=0;
			n->n->f=NULL;
			n->n->ln=0;
			n->n->fn=NULL;
			n->n->fcl=0;
			n->n->t=false;
			n->n->t_v=NULL;
			n->n->t_nm=NULL;
			n->n->mf=NULL;
			n->n->mln=0;
			n->n->mfn=NULL;
			n->n->r=false;
		}
		n=n->n;
	}
	n->ptr=malloc(sz+16);
	if (n->ptr==NULL){
		printf("ERROR: %s: %s(%u): Out of Memory!\n",f,fn,ln);
		_m_err=true;
		raise(SIGABRT);
		return NULL;
	}
	for (size_t i=0;i<8;i++){
		*((char*)n->ptr+i)=*(_s_sig+i);
		*((char*)n->ptr+sz+i+8)=*(_e_sig+i);
	}
	n->sz=sz;
	n->f=f;
	n->ln=ln;
	n->fn=fn;
	n->fcl=_get_call_lvl(f,fn);
	n->t=false;
	n->t_v=NULL;
	n->t_nm=NULL;
	n->mf=f;
	n->mln=ln;
	n->mfn=fn;
	n->r=false;
	return (void*)((uintptr_t)n->ptr+8);
}



void* KlMem_calloc_(size_t ln_,size_t sz,const char* f,unsigned int ln,const char* fn){
	if (_er==false){
		atexit(KlMem_check_all_allocated_);
		signal(SIGABRT,KlMem_abrt_);
		_er=true;
	}
	_check_heap(f,ln,fn);
	sz*=ln_;
	assert(sz>=0);
	struct _Node* n=&_head;
	while (n->ptr!=NULL){
		if (n->n==NULL){
			n->n=malloc(sizeof(struct _Node));
			n->n->p=n;
			n->n->n=NULL;
			n->n->ptr=NULL;
			n->n->sz=0;
			n->n->f=NULL;
			n->n->ln=0;
			n->n->fn=NULL;
			n->n->fcl=0;
			n->n->t=false;
			n->n->t_v=NULL;
			n->n->t_nm=NULL;
			n->n->mf=NULL;
			n->n->mln=0;
			n->n->mfn=NULL;
			n->n->r=false;
		}
		n=n->n;
	}
	n->ptr=malloc(sz+16);
	if (n->ptr==NULL){
		printf("ERROR: %s:%u (%s): Out of Memory!\n",f,ln,fn);
		_m_err=true;
		raise(SIGABRT);
		return NULL;
	}
	for (size_t i=0;i<8;i++){
		*((char*)n->ptr+i)=*(_s_sig+i);
		*((char*)n->ptr+sz+i+8)=*(_e_sig+i);
	}
	for (size_t i=0;i<sz;i++){
		*((char*)n->ptr+i+8)=0;
	}
	n->sz=sz;
	n->f=f;
	n->ln=ln;
	n->fn=fn;
	n->fcl=_get_call_lvl(f,fn);
	n->t=false;
	n->t_v=NULL;
	n->t_nm=NULL;
	n->mf=f;
	n->mln=ln;
	n->mfn=fn;
	n->r=false;
	return (void*)((uintptr_t)n->ptr+8);
}



void* KlMem_realloc_(void* s,size_t sz,const char* f,unsigned int ln,const char* fn){
	if (_er==false){
		atexit(KlMem_check_all_allocated_);
		signal(SIGABRT,KlMem_abrt_);
		_er=true;
	}
	_check_heap(f,ln,fn);
	if (s==NULL){
		return KlMem_malloc_(sz,f,ln,fn);
	}
	assert(sz>=0);
	void* sn=_get(s,"Reallocating",true,f,ln,fn);
	struct _Node* n;
	if (sn==s){
		n=&_head;
		while (n->n==NULL){
			n=n->n;
		}
		n->n=malloc(sizeof(struct _Node));
		n->ptr=s;
	}
	else{
		n=(struct _Node*)sn;
	}
	if (n->t==true){
		printf("TRACE: %s:%u (%s): %s (0x%016llx): Reallocating Memory on Line %s:%u (%s): %llu -> %llu\n",f,ln,fn,n->t_nm,(unsigned long long int)n->ptr,f,ln,fn,n->sz,sz);
	}
	n->ptr=realloc(n->ptr,sz+16);
	if (n->ptr==NULL){
		printf("ERROR: %s:%u (%s): Out of Memory!\n",f,ln,fn);
		_m_err=true;
		raise(SIGABRT);
		return NULL;
	}
	for (size_t i=0;i<8;i++){
		*((char*)n->ptr+i)=*(_s_sig+i);
		*((char*)n->ptr+sz+i+8)=*(_e_sig+i);
	}
	n->sz=sz;
	n->f=f;
	n->ln=ln;
	n->fn=fn;
	n->fcl=_get_call_lvl(f,fn);
	return (void*)((uintptr_t)n->ptr+8);
}



void* KlMem_memcpy_(void* o,void* s,size_t sz,const char* f,unsigned int ln,const char* fn){
	if (_er==false){
		atexit(KlMem_check_all_allocated_);
		signal(SIGABRT,KlMem_abrt_);
		_er=true;
	}
	assert(o!=s);
	_check_heap(f,ln,fn);
	void* on=_get(o,"Memcpy To",WARN_MEMCPY_TO_UNKNOWN,f,ln,fn);
	if (on!=o){
		if (((struct _Node*)on)->t==true){
			printf("TRACE: %s:%u (%s): %s (0x%016llx): Memory Copy To Self on Line %s:%u (%s)\n",f,ln,fn,((struct _Node*)on)->t_nm,(unsigned long long int)((struct _Node*)on)->ptr,f,ln,fn);
		}
		o=(char*)((struct _Node*)on)->ptr+8;
	}
	void* sn=_get(s,"Memcpy From",WARN_MEMCPY_FROM_UNKNOWN,f,ln,fn);
	if (sn!=s){
		if (((struct _Node*)sn)->t==true){
			printf("TRACE: %s:%u (%s): %s (0x%016llx): Memory Copy From Self on Line %s:%u (%s)\n",f,ln,fn,((struct _Node*)sn)->t_nm,(unsigned long long int)((struct _Node*)sn)->ptr,f,ln,fn);
		}
		s=(char*)((struct _Node*)sn)->ptr+8;
	}
	for (size_t i=0;i<sz;i++){
		*((char*)o+i)=*((char*)s+i);
	}
	return o;
}



void KlMem_free_(void* p,const char* f,unsigned int ln,const char* fn){
	if (_er==false){
		atexit(KlMem_check_all_allocated_);
		signal(SIGABRT,KlMem_abrt_);
		_er=true;
	}
	_check_heap(f,ln,fn);
	struct _Node* n=_get(p,"Freeing",false,f,ln,fn);
	if (n==p){
		printf("ERROR: %s:%u (%s): Freeing Unknown Pointer!\n",f,ln,fn);
		_m_err=true;
		raise(SIGABRT);
		return;
	}
	if (n->t==true){
		printf("TRACE: %s:%u (%s): %s (0x%016llx): Freeing Pointer on Line %s:%u (%s)\n",f,ln,fn,n->t_nm,(unsigned long long int)n->ptr,f,ln,fn);
	}
	free(n->ptr);
	if (n->t_v!=NULL){
		free(n->t_v);
	}
	n->ptr=NULL;
	n->sz=0;
	n->f=NULL;
	n->ln=0;
	n->fn=NULL;
	n->fcl=0;
	n->t=false;
	n->t_v=NULL;
	n->t_nm=NULL;
	n->mf=NULL;
	n->mln=0;
	n->mfn=NULL;
	n->r=false;
	if (n->p!=NULL){
		n->p->n=n->n;
		if (n->n!=NULL){
			n->n->p=n->p;
		}
		free(n);
	}
}



void KlMem_trace_(void* p,const char* p_nm,const char* f,unsigned int ln,const char* fn){
	if (_er==false){
		atexit(KlMem_check_all_allocated_);
		signal(SIGABRT,KlMem_abrt_);
		_er=true;
	}
	_check_heap(f,ln,fn);
	struct _Node* n=_get(p,"Tracing",false,f,ln,fn);
	if (n==p){
		printf("ERROR: %s:%u (%s): Tracing Unknown Pointer!\n",f,ln,fn);
		_m_err=true;
		raise(SIGABRT);
		return;
	}
	printf("TRACE: %s:%u (%s): Started Tracing Pointer %s (0x%016llx)\n",f,ln,fn,p_nm,(unsigned long long int)n->ptr);
	n->t=true;
	n->t_v=malloc(n->sz);
	n->t_nm=p_nm
;	for (size_t i=0;i<n->sz;i++){
		*((char*)n->t_v+i)=*((char*)n->ptr+i+8);
	}
}



void KlMem_ret_(void* p,const char* f,unsigned int ln,const char* fn){
	if (_er==false){
		atexit(KlMem_check_all_allocated_);
		signal(SIGABRT,KlMem_abrt_);
		_er=true;
	}
	_check_heap(f,ln,fn);
	struct _Node* n=_get(p,"Returning",false,f,ln,fn);
	if ((void*)n==p){
		printf("ERROR: %s:%u (%s): Returning Unknown Pointer!\n",f,ln,fn);
		_m_err=true;
		raise(SIGABRT);
		return;
	}
	n->r=true;
}



void KlMem_ret_null_(const char* f,unsigned int ln,const char* fn){
	if (_er==false){
		atexit(KlMem_check_all_allocated_);
		signal(SIGABRT,KlMem_abrt_);
		_er=true;
	}
	_check_heap(f,ln,fn);
	struct _Node* n=&_head;
	while (n->n!=NULL){
		if (n->ptr!=NULL&&n->mf==f&&n->mfn==fn&&n->r==true){
			n->r=false;
		}
		n=n->n;
	}
}



void KlMem_enter_func_(const char* f,unsigned int ln,const char* fn){
	if (_er==false){
		atexit(KlMem_check_all_allocated_);
		signal(SIGABRT,KlMem_abrt_);
		_er=true;
	}
	(void)ln;
	struct _FuncLvl* fl=&_fl_head;
	while (fl->f!=f||fl->fn!=fn){
		if (fl->f==NULL&&fl->fn==NULL){
			fl->n=NULL;
			fl->f=f;
			fl->fn=fn;
			fl->l=0;
			break;
		}
		if (fl->n==NULL){
			fl->n=malloc(sizeof(struct _FuncLvl));
			fl->n->n=NULL;
			fl->n->f=f;
			fl->n->fn=fn;
			fl->n->l=0;
		}
		fl=fl->n;
	}
	fl->l++;
}



void KlMem_check_allocated_(const char* f,unsigned int ln,const char* fn){
	if (_er==false){
		atexit(KlMem_check_all_allocated_);
		signal(SIGABRT,KlMem_abrt_);
		_er=true;
	}
	_check_heap(f,ln,fn);
	struct _Node* n=&_head;
	unsigned long pc=0;
	unsigned long long int bc=0;
	while (n->n!=NULL){
		if (n->ptr!=NULL&&n->mf==f&&n->mfn==fn&&n->r==false&&n->fcl==_get_call_lvl(f,fn)){
			pc++;
			bc+=n->sz;
			if (n->t==true){
				printf("ERROR: %s:%u (%s): %s (0x%016llx): Pointer not Freed at The End of Function! (%s:%u (%s))\n",f,ln,fn,n->t_nm,(unsigned long long int)n->ptr,n->mf,n->mln,n->mfn);
			}
			else{
				printf("ERROR: %s:%u (%s): 0x%016llx: Pointer not Freed at The End of Function! (%s:%u (%s))\n",f,ln,fn,(unsigned long long int)n->ptr,n->mf,n->mln,n->mfn);
			}
		}
		n=n->n;
	}
	if (pc>0){
		printf("%lu Pointer(s) (%llu byte(s)) Not Freed, Aborting.\n",pc,bc);
		_m_err=true;
		raise(SIGABRT);
	}
}



void KlMem_check_all_allocated_(){
	if (_m_err==true){
		return;
	}
	_check_heap(NULL,0,NULL);
	unsigned long pc=0;
	unsigned long long int bc=0;
	struct _Node* n=&_head;
	while (n->n!=NULL){
		if (n->ptr!=NULL){
			pc++;
			bc+=n->sz;
			if (n->t==true){
				printf("%s (0x%016llx): Pointer not Freed at The End of Program! (%s:%u (%s))\n",n->t_nm,(unsigned long long int)n->ptr,n->mf,n->mln,n->mfn);
			}
			else{
				printf("0x%016llx: Pointer not Freed at The End of Program! (%s:%u (%s))\n",(unsigned long long int)n->ptr,n->mf,n->mln,n->mfn);
			}
			_dump((unsigned char*)n->ptr+8,n->sz);
		}
		n=n->n;
	}
	if (pc>0){
		printf("%lu Pointer(s) (%llu byte(s)) Not Freed, Aborting.\n",pc,bc);
	}
	else{
		printf("Everything Freed!");
	}
}



#else



void* KlMem_memcpy_(void* o,void* s,size_t sz){
	for (size_t i=0;i<(sz);i++){
		*((char*)(o)+i)=*((char*)(s)+i);
	}
	return o;
}



#endif

#include <constants.h>
#include <error.h>
#include <platform.h>
#include <sys.h>
#include <shared.h>
#include <string_utils.h>
#include <memory.h>
#include <stdlib.h>
#include <io.h>



struct Error _err={
	NULL,
	NULL,
	NULL
};
struct CallStack** _cs;
size_t _csl;
bool _ar;



void KlError_raise(void){
	KlMem_enter_func();
	if (_err.cs!=NULL){
		size_t j;
		size_t k;
		char* ln=NULL;
		size_t dt_ln;
		size_t sz;
		bool h;
		bool s;
		unsigned char a;
		char* cl=str_escape_ansi(CONST_COLOR_ERROR_SRC_HIGHLIGHT);
		char* c_cl=str_escape_ansi(CONST_COLOR_RESET);
		size_t cl_ln=str_len(cl);
		size_t c_cl_ln=str_len(c_cl);
		for (struct CallStackElem* i=_err.cs->e;i<_err.cs->e+_err.cs->l;i++){
			j=0;
			k=0;
			dt_ln=str_len(i->c->dt);
			while (true){
				j+=str_find(i->c->dt,'\n',j)+1;
				if (k==i->sl){
					sz=0;
					h=false;
					while (true){
						if (h==false){
							ln=KlMem_realloc(ln,sz+4);
							*(ln+sz)='\n';
							*(ln+sz+1)=' ';
							*(ln+sz+2)=' ';
							sz+=3;
						}
						else{
							ln=KlMem_realloc(ln,sz+cl_ln+4);
							*(ln+sz)='\n';
							*(ln+sz+1)=' ';
							*(ln+sz+2)=' ';
							sz+=3;
							for (size_t n=0;n<cl_ln;n++){
								*(ln+sz+n)=*(cl+n);
							}
							sz+=cl_ln;
						}
						s=false;
						for (size_t l=str_rfind(i->c->dt,'\n',j-2)+1;l<j-1;l++){
							if (s==false&&(*(i->c->dt+l)==' '||*(i->c->dt+l)=='\t'||*(i->c->dt+l)=='\r'||*(i->c->dt+l)=='\v')){
								continue;
							}
							a=(l==j-2&&*(i->c->dt+l)=='\r'?1:2);
							s=true;
							if (k==i->sl&&l==i->su){
								h=true;
								ln=KlMem_realloc(ln,sz+cl_ln+a);
								for (size_t n=0;n<cl_ln;n++){
									*(ln+sz+n)=*(cl+n);
								}
								sz+=cl_ln;
							}
							else{
								ln=KlMem_realloc(ln,sz+a);
							}
							if (a==2){
								*(ln+sz)=*(i->c->dt+l);
								sz++;
							}
							if (k==i->el&&l==i->eu){
								h=false;
								ln=KlMem_realloc(ln,sz+c_cl_ln+1);
								for (size_t n=0;n<c_cl_ln;n++){
									*(ln+sz+n)=*(c_cl+n);
								}
								sz+=c_cl_ln;
							}
						}
						if (k==i->el){
							break;
						}
						ln=KlMem_realloc(ln,sz+c_cl_ln+1);
						for (size_t n=0;n<c_cl_ln;n++){
							*(ln+sz+n)=*(c_cl+n);
						}
						sz+=c_cl_ln;
						if (j==dt_ln){
							ln=KlMem_realloc(ln,sz+cl_ln+c_cl_ln+5);
							*(ln+sz)='\n';
							*(ln+sz+1)=' ';
							*(ln+sz+2)=' ';
							sz+=3;
							for (size_t n=0;n<cl_ln;n++){
								*(ln+sz+n)=*(cl+n);
							}
							*(ln+sz+cl_ln)=' ';
							sz+=cl_ln+1;
							for (size_t n=0;n<c_cl_ln;n++){
								*(ln+sz+n)=*(c_cl+n);
							}
							sz+=c_cl_ln;
							break;
						}
						j+=str_find(i->c->dt,'\n',j)+1;
						k++;
					}
					*(ln+sz)=0;
					KlIo_write(KlSys_stderr,str_format("%fFile '%s', Line%s%s:%f%s\n",CONST_COLOR_ERROR_TXT,i->c->fp,(i->sl==i->el?str_format(" %S",i->sl+1):str_format("s %S-%S",i->sl+1,i->el+1)),(i->f!=NULL?str_format(" in '%s'",i->f):""),CONST_COLOR_RESET,ln));
					KlMem_free(ln);
					break;
				}
				k++;
			}
		}
	}
	if (_err.nm==NULL||_err.msg==NULL){
		KlIo_write(KlSys_stderr,str_format("%fInternalError: Error hasn't Been Set.%f\n",CONST_COLOR_ERROR_TXT,CONST_COLOR_RESET));
	}
	else{
		KlIo_write(KlSys_stderr,str_format("%f%s: %s%f\n",CONST_COLOR_ERROR_TXT,_err.nm,_err.msg,CONST_COLOR_RESET));
	}
	KlError_cleanup();
	return();
}



void KlError_cleanup(void){
	if (_err.nm!=NULL){
		KlMem_free(_err.nm);
		KlMem_free(_err.msg);
	}
	if (_cs!=NULL){
		for (size_t i=0;i<_csl;i++){
			KlMem_free((*(_cs+i))->e);
			KlMem_free(*(_cs+i));
		}
		KlMem_free(_cs);
	}
}



void KlError_set_error(char* nm,char* msg,struct CallStack* cs){
	KlMem_enter_func();
	_err.nm=nm;
	_err.msg=msg;
	_err.cs=cs;
#ifndef NDEBUG
	KlError_raise();
	assert(0);
#endif
	return();
}



struct CallStack* KlError_extend_call_stack(struct CallStack* cs,struct CodeFileObject* c,size_t sl,size_t el,size_t su,size_t eu,char* f){
	KlMem_enter_func();
	if (_ar==false){
		_ar=true;
		atexit(KlError_cleanup);
	}
	struct CallStack o;
	struct CallStackElem e;
	if (cs==NULL){
		e.c=c;
		e.sl=sl;
		e.el=(el==-1?sl:el);
		e.su=su;
		e.eu=(eu==-1?su:eu);
		e.f=f;
		o.e=KlMem_const(&e,sizeof(struct CallStackElem));
		KlMem_ret(o.e);
		o.l=1;
	}
	else{
		e.c=(c==NULL?(cs->l>0?cs->e[cs->l-1].c:NULL):c);
		e.sl=sl;
		e.el=(el==-1?sl:el);
		e.su=su;
		e.eu=(eu==-1?su:eu);
		e.f=(f==NULL?(cs->l>0?cs->e[cs->l-1].f:NULL):f);
		o.e=KlMem_malloc(sizeof(struct CallStackElem)*(cs->l+1));
		KlMem_ret(o.e);
		KlMem_memcpy(o.e,cs->e,sizeof(struct CallStackElem)*cs->l);
		o.e[cs->l]=e;
		o.l=cs->l+1;
	}
	_csl++;
	_cs=KlMem_realloc(_cs,_csl*sizeof(struct CallStack*));
	KlMem_ret(_cs);
	struct CallStack* op=KlMem_const(&o,sizeof(o));
	*(_cs+_csl-1)=op;
	KlMem_ret(op);
	return(op);
}



size_t KlError_offset_to_line(char* dt,size_t i){
	KlMem_enter_func();
	size_t o=0;
	for (size_t j=0;j<=i;j++){
		if (*(dt+j)=='\n'){
			o++;
		}
	}
	return(o);
}



void KlError_unimplemented(char* t,unsigned int ln){
	KlMem_enter_func();
	KlError_set_error("UnimplementedError",str_format("Unimplemented %s on Line %i.",t,ln),NULL);
	return();
}

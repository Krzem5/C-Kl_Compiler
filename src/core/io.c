#include <io.h>
#include <memory.h>
#include <shared.h>
#include <string_utils.h>
#include <sys.h>
#include <stdarg.h>
#include <stdio.h>



void KlIo_printf(char* t,...){
	KlMem_enter_func();
	va_list a;
	va_start(a,t);
	char* s=str_format_va(t,a);
	KlIo_write(KlSys_stdout,s);
	KlMem_free(s);
	va_end(a);
	return();
}



char* KlIo_read(struct File f,size_t l,size_t* r){
	KlMem_enter_func();
	if ((f.m&FILE_MODIFIER_READ)==0){
		*r=0;
		return(NULL);
	}
	return(f.rf(f.p,f.m,l,r));
}



size_t KlIo_write(struct File f,char* s){
	KlMem_enter_func();
	if ((f.m&FILE_MODIFIER_WRITE)==0){
		return(0);
	}
	return(f.wf(f.p,f.m,s));
}



bool KlIo_flush(struct File f){
	KlMem_enter_func();
	if ((f.m&FILE_MODIFIER_WRITE)==0){
		return(false);
	}
	return(f.ff(f.p,f.m));
}



char* KlIo_default_read_func(void* p,unsigned char m,size_t l,size_t* r){
	KlMem_enter_func();
	(void)m;
	char* o=KlMem_malloc(l+1);
	size_t i=0;
	while (i<l){
		if (*(o+i)==EOF){
			if (i>0){
				i--;
			}
			break;
		}
		*(o+i)=(char)fgetc(p);
		i++;
	}
	*r=i;
	if (i!=l){
		char* no=KlMem_malloc(i+1);
		KlMem_memcpy(no,o,i);
		KlMem_free(o);
		return(no);
	}
	return(o);
}



size_t KlIo_default_write_func(void* p,unsigned char m,char* s){
	KlMem_enter_func();
	(void)m;
	size_t o=0;
	size_t ln=str_len(s);
	for (size_t i=0;i<ln;i++){
		if (fputc(*(s+i),p)==EOF){
			if (o>0){
				o--;
			}
			return(o);
		}
		o++;
	}
	return(o);
}



bool KlIo_default_flush_func(void* p,unsigned char m){
	KlMem_enter_func();
	(void)m;
	fflush(p);
	return(true);
}

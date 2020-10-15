#include <file.h>
#include <memory.h>
#include <types.h>
#include <string_utils.h>
#include <stdio.h>



char* KlFileApi_read(struct File f,unsigned long l,unsigned long* r){
	if ((f.m&FILE_MODIFIER_READ)==0){
		*r=0;
		return NULL;
	}
	return f.rf(f.p,f.m,l,r);
}



unsigned long KlFileApi_write(struct File f,char* s){
	if ((f.m&FILE_MODIFIER_WRITE)==0){
		return 0;
	}
	return f.wf(f.p,f.m,s);
}



bool KlFileApi_flush(struct File f){
	if ((f.m&FILE_MODIFIER_WRITE)==0){
		return false;
	}
	return f.ff(f.p,f.m);
}



#pragma warning(disable:4100)
char* KlFileApi_default_read_func(void* p,unsigned char m,unsigned long l,unsigned long* r){
#pragma warning(default:4100)
	char* o=malloc(l+1);
	unsigned long i=0;
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
		char* no=malloc(i+1);
		memcpy(no,o,i);
		free(o);
		return no;
	}
	return o;
}



#pragma warning(disable:4100)
unsigned long KlFileApi_default_write_func(void* p,unsigned char m,char* s){
#pragma warning(default:4100)
	unsigned long o=0;
	unsigned long ln=str_len(s);
	for (unsigned long i=0;i<ln;i++){
		if (fputc(*(s+i),p)==EOF){
			if (o>0){
				o--;
			}
			return o;
		}
		o++;
	}
	return o;
}



#pragma warning(disable:4100)
bool KlFileApi_default_flush_func(void* p,unsigned char m){
#pragma warning(default:4100)
	fflush(p);
	return true;
}

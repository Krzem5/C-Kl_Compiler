#include <bytecode.h>
#include <memory.h>
#include <platform.h>
#include <error.h>
#include <shared.h>
#include <string_utils.h>
#include <io.h>



struct COFFFile* KlBytecode_from_ast_object(struct ASTObject* o){
	KlMem_enter_func();
	return(NULL);
}



struct COFFFile* KlBytecode_read_from_file(const char* fp){
	KlMem_enter_func();
	if (KlPlatform_file_exists(fp)==false){
		KlError_unimplemented_error();
		return(NULL);
	}
	size_t l=0;
	unsigned char* dt=KlPlatform_read_file_content(fp,&l);
	assert(dt!=NULL);
	struct COFFFile o;
	o.mt=*dt|(*(dt+1)<<8);
	o.sl=*(dt+2)|(*(dt+3)<<8);
	o.s=KlMem_malloc(o.sl*sizeof(struct COFFFileSection));
	KlMem_ret(o.s);
	o.tm=*(dt+4)|(*(dt+5)<<8)|(*(dt+6)<<16)|(*(dt+7)<<24);
	uint32_t sta=*(dt+8)|(*(dt+9)<<8)|(*(dt+10)<<16)|(*(dt+11)<<24);
	o.stl=*(dt+12)|(*(dt+13)<<8)|(*(dt+14)<<16)|(*(dt+15)<<24);
	o.st=KlMem_malloc(o.stl*sizeof(struct COFFFileSymbolTableEntry));
	o.f=*(dt+18)|(*(dt+19)<<8);
	KlIo_printf("LENGTH: %S, TYPE: %S, SECTIONS: %S, TIME: %S, STA: %S, STL: %S, FLAGS: %S\n",l,(size_t)o.mt,(size_t)o.sl,(size_t)o.tm,(size_t)sta,(size_t)o.stl,(size_t)o.f);
	*(dt+sta+o.stl*18)=0;
	*(dt+sta+o.stl*18+1)=0;
	*(dt+sta+o.stl*18+2)=0;
	*(dt+sta+o.stl*18+3)=0;
	size_t i=20;
	for (uint16_t j=0;j<o.sl;j++){
		(o.s+j)->nm=KlMem_malloc(9);
		KlMem_ret((o.s+j)->nm);
		for (unsigned char k=0;k<9;k++){
			*((o.s+j)->nm+k)=(k<8?*(dt+i+k):0);
			if (k==8||*(dt+i+k)==0){
				if (k!=8){
					(o.s+j)->nm=KlMem_realloc((o.s+j)->nm,k+1);
				}
				break;
			}
		}
		(o.s+j)->dt_sz=*(dt+i+16)|(*(dt+i+17)<<8)|(*(dt+i+18)<<16)|(*(dt+i+19)<<24);
		(o.s+j)->dt=KlMem_malloc((o.s+j)->dt_sz);
		uint32_t k=*(dt+i+20)|(*(dt+i+21)<<8)|(*(dt+i+22)<<16)|(*(dt+i+23)<<24);
		uint32_t l=0;
		while (l<(o.s+j)->dt_sz){
			*((o.s+j)->dt+l)=*(dt+i+k+l);
			l++;
		}
		(o.s+j)->f=*(dt+i+36)|(*(dt+i+37)<<8)|(*(dt+i+38)<<16)|(*(dt+i+39)<<24);
		KlIo_printf("SECTION#%S: NAME: %s, SZ: %S, FLAGS: %S\n",(size_t)j,(o.s+j)->nm,(size_t)(o.s+j)->dt_sz,(size_t)(size_t)(o.s+j)->f);
		i+=40;
	}
	i=sta;
	for (uint32_t j=0;j<o.stl;j++){
		(o.st+j)->nm=KlMem_malloc(9);
		KlMem_ret((o.st+j)->nm);
		for (unsigned char k=0;k<9;k++){
			*((o.st+j)->nm+k)=(k<8?*(dt+i+k):0);
		}
		if (*(o.st+j)->nm==0&&*((o.st+j)->nm+1)==0&&*((o.st+j)->nm+2)==0&&*((o.st+j)->nm+3)==0){
			uint32_t k=(*((o.st+j)->nm+4))*1000+(*((o.st+j)->nm+5))*100+(*((o.st+j)->nm+6))*10+(*((o.st+j)->nm+7));
			KlMem_free((o.st+j)->nm);
			(o.st+j)->nm=KlPlatform_demangle_name(dt+sta+o.stl*18+k);
		}
		(o.st+j)->v=*(dt+i+8)|(*(dt+i+9)<<8)|(*(dt+i+10)<<16)|(*(dt+i+11)<<24);
		(o.st+j)->sn=*(dt+i+12)|(*(dt+i+9)<<13);
		(o.st+j)->t=*(dt+i+14)|(*(dt+i+9)<<15);
		(o.st+j)->sc=*(dt+i+16);
		(o.st+j)->asl=*(dt+i+17);
		KlIo_printf("SYMBOL#%S: NAME: %s, VALUE: %S, SECTION #: %S, TYPE: %S, STORAGE CLASS: %S, AUX LEN: %S\n",(size_t)j,(o.st+j)->nm,(size_t)(o.st+j)->v,(size_t)(o.st+j)->sn,(size_t)(o.st+j)->t,(size_t)(o.st+j)->sc,(size_t)(o.st+j)->asl);
		i+=18+18*((size_t)(o.st+j)->asl);
		j+=(o.st+j)->asl;
	}
	KlMem_free(dt);
	assert(0);
	return(KlMem_const(&o,sizeof(struct COFFFile)));
}



void KlBytecode_write_to_file(const char* fp,struct COFFFile* b){
	KlMem_enter_func();
	return();
}



char* KlBytecode_demangle_string(char* nm){
	KlMem_enter_func();
	if (*nm!='?'||*(nm+1)!='?'||*(nm+2)!='_'||*(nm+3)!='C'||*(nm+4)!='@'||*(nm+5)!='_'||(*(nm+6)!='0'&&*(nm+6)!='1')){
		return(str_clone("`string'"));
	}
	unsigned char b=(*(nm+6)=='0'?0:1);
	size_t i=7;
	size_t ln=KlBytecode_demangle_string_length(nm,&i);
	KlBytecode_demangle_string_length(nm,&i);
	char* o=KlMem_malloc(ln+1);
	size_t j=0;
	while (j<ln){
		i+=b;
		if (*(nm+i)=='@'){
			o=KlMem_realloc(o,j+1);
			break;
		}
		if (*(nm+i)=='?'&&*(nm+i+1)=='$'){
			i+=2;
			*(o+j)=(*(nm+i)-'A')*16+(*(nm+i+1)-'A');
			i++;
		}
		else if (*(nm+i)=='?'&&*(nm+i+1)>='0'&&*(nm+i+1)<='9'){
			i++;
			*(o+j)=*(",/\\:. \11\10'-"+*(nm+i)-'0');
		}
		else if (*(nm+i)=='?'&&*(nm+i+1)>='A'&&*(nm+i+1)<='P'){
			i++;
			*(o+j)=*(nm+i)+0x80;
		}
		else{
			*(o+j)=*(nm+i);
		}
		i++;
		j++;
	}
	*(o+j)=0;
	KlMem_ret(o);
	return(o);
}



size_t KlBytecode_demangle_string_length(char* nm,size_t* i){
	KlMem_enter_func();
	size_t o=0;
	if (*(nm+*i)>='0'&&*(nm+*i)<='9'){
		while (*(nm+*i)>='0'&&*(nm+*i)<='9'){
			o=o*10+*(nm+*i)-'0';
			(*i)++;
		}
		return(o);
	}
	while (*(nm+*i)!='@'){
		o=o*16+*(nm+*i)-'A';
		(*i)++;
	}
	(*i)++;
	return(o);
}

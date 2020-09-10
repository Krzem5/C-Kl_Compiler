#include <bytecode.h>
#include <memory.h>
#include <platform.h>
#include <error.h>
#include <shared.h>
#include <string_utils.h>
#include <debug_utils.h>
#include <io.h>



struct BytecodeData* KlBytecode_from_ast_scope(struct CodeFileObject* fo,struct ASTScope* ast){
	KlMem_enter_func();
	assert(ast->t==AST_SCOPE_TYPE_DEFAULT);
	struct BytecodeData o;
	o.m=0x214b4c00;
	o.v=0x0001;
	o.stl=0;
	o.ftl=0;
	o.rtl=0;
	o.itl=0;
	o.etl=0;
	o.f=BYTECODE_DATA_FLAG_ENTRY|BYTECODE_DATA_FLAG_DEBUG|BYTECODE_DATA_FLAG_EMBEDDED|BYTECODE_DATA_FLAG_EMBEDDED_SRC;
	o.st=NULL;
	o.ft=NULL;
	o.rt=NULL;
	o.it=NULL;
	o.et=NULL;
	o.cl=0;
	o.c=NULL;
	if ((o.f&BYTECODE_DATA_FLAG_EMBEDDED_SRC)!=0){
		char* efp=KlBytecode_encode_string(fo->fp,str_len(fo->fp)+1);
		size_t efpl=str_len(efp)+1;
		char* edt=KlBytecode_encode_string(fo->dt,fo->l);
		size_t edtl=str_len(edt)+1;
		o.st=KlMem_realloc(o.st,o.stl+efpl+edtl);
		KlMem_ret(o.st);
		KlMem_memcpy(o.st+o.stl,efp,efpl);
		KlMem_memcpy(o.st+o.stl+efpl,edt,edtl);
		o.stl+=(uint32_t)(efpl+edtl);
		KlMem_free(efp);
		KlMem_free(edt);
	}
	uint32_t* mfp=NULL;
	uint16_t mfpl=0;
	for (size_t i=0;i<ast->ml;i++){
		if (((*(ast->m+i))->mf&OBJECT_MODIFIER_NATIVE)!=0||((*(ast->m+i))->mf&OBJECT_MODIFIER_NATIVE_BASE)!=0){
			continue;
		}
		char* efp=KlBytecode_encode_string((*(ast->m+i))->fp,str_len((*(ast->m+i))->fp)+1);
		size_t eln=str_len(efp);
		bool f=false;
		for (uint16_t j=0;j<mfpl;j++){
			if (str_cmp_sub(efp,o.st+*(mfp+j),0,eln)==true){
				f=true;
				break;
			}
		}
		if (f==true){
			KlMem_free(efp);
			continue;
		}
		o.itl++;
		mfpl++;
		o.it=KlMem_realloc(o.it,o.itl*sizeof(uint32_t));
		KlMem_ret(o.it);
		mfp=KlMem_realloc(mfp,mfpl*sizeof(uint32_t));
		*(o.it+o.itl-1)=o.stl;
		*(mfp+mfpl-1)=o.stl;
		o.st=KlMem_realloc(o.st,o.stl+eln+1);
		KlMem_ret(o.st);
		KlMem_memcpy(o.st+o.stl,efp,eln+1);
		o.stl+=(uint32_t)eln+1;
		KlMem_free(efp);
	}
	KlMem_free(mfp);
	KlDebug_print_bytecode(&o);
	return(KlMem_const(&o,sizeof(struct BytecodeData)));
}



struct BytecodeData* KlBytecode_read_from_file(const char* fp){
	KlMem_enter_func();
	if (KlPlatform_file_exists(fp)==false){
		KlError_unimplemented_error();
		return(NULL);
	}
	size_t l=0;
	unsigned char* dt=KlPlatform_read_file_content(fp,&l);
	assert(dt!=NULL);
	KlMem_free(dt);
	assert(0);
	return(NULL);
}



void KlBytecode_write_to_file(const char* fp,struct BytecodeData* b){
	KlMem_enter_func();
	return();
}



char* KlBytecode_decode_string(char* s,size_t* ln){
	KlMem_enter_func();
	*ln=str_len(s);
	char* o=KlMem_malloc(*ln+1);
	size_t i=0;
	while (i<*ln+1){
		if (*s=='$'){
			if ((*(s+1)=='0'||*(s+1)=='$')){
				(*ln)--;
				s++;
				o=KlMem_realloc(o,*ln+1);
				*(o+i)=(*s=='0'?0:'$');
			}
			else{
				*(o+i)='$';
			}
		}
		else{
			*(o+i)=*s;
		}
		i++;
		s++;
	}
	KlMem_ret(o);
	return(o);
}



char* KlBytecode_encode_string(char* s,size_t ln){
	KlMem_enter_func();
	size_t oln=ln;
	char* o=KlMem_malloc(oln);
	size_t i=0;
	while (i<ln){
		if (*s==0&&i<ln-1){
			oln++;
			o=KlMem_realloc(o,oln);
			*(o+i)='$';
			*(o+i+1)='0';
			i++;
		}
		else if (*s=='$'&&(*(s+1)=='0'||*(s+1)=='$')){
			oln++;
			o=KlMem_realloc(o,oln);
			*(o+i)='$';
			*(o+i+1)='$';
			i++;
		}
		else{
			*(o+i)=*s;
		}
		i++;
		s++;
	}
	if (*(o+i-1)!=0){
		o=KlMem_realloc(o,oln+1);
		*(o+i)=0;
	}
	KlMem_ret(o);
	return(o);
}

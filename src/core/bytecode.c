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
	struct BytecodeStackData sdt={
		NULL,
		NULL,
		0,
		0
	};
	for (size_t i=0;i<ast->cl;i++){
		KlBytecode_compile_expr(*(ast->c+i),SIZE_MAX,&o,&sdt);
	}
	assert(0);
	KlDebug_print_bytecode(&o);
	return(KlMem_const(&o,sizeof(struct BytecodeData)));
}



uint32_t KlBytecode_get_string(struct BytecodeData* bdt,char* s){
	KlMem_enter_func();
	char* e=KlBytecode_encode_string(s,str_len(s)+1);
	size_t el=str_len(e);
	for (uint32_t i=0;i<bdt->stl;i++){
		if (str_cmp_sub(e,bdt->st+i,0,el)==true){
			KlMem_free(e);
			return(i);
		}
		i+=(uint32_t)str_len(bdt->st+i);
	}
	bdt->st=KlMem_realloc(bdt->st,bdt->stl+el+1);
	KlMem_memcpy(bdt->st+bdt->stl,e,el+1);
	uint32_t i=bdt->stl;
	bdt->stl+=(uint32_t)el+1;
	return(i);
}



void KlBytecode_add_opcode_arg(struct BytecodeData* bdt,unsigned char op,uint32_t v){
	KlMem_enter_func();
	if (v<128){
		bdt->cl+=2;
		bdt->c=KlMem_realloc(bdt->c,bdt->cl);
		*(bdt->c+bdt->cl-2)=op;
		*(bdt->c+bdt->cl-1)=(unsigned char)v;
	}
	else if (v<32768){
		bdt->cl+=3;
		bdt->c=KlMem_realloc(bdt->c,bdt->cl);
		*(bdt->c+bdt->cl-3)=op;
		*(bdt->c+bdt->cl-2)=(unsigned char)((v>>8)|0x80);
		*(bdt->c+bdt->cl-1)=(unsigned char)(v&0xff);
	}
	else if (v<4194304){
		bdt->cl+=4;
		bdt->c=KlMem_realloc(bdt->c,bdt->cl);
		*(bdt->c+bdt->cl-4)=op|0x80;
		*(bdt->c+bdt->cl-3)=(unsigned char)((v>>15)|0x80);
		*(bdt->c+bdt->cl-2)=(unsigned char)((v>>8)&0xff);
		*(bdt->c+bdt->cl-1)=(unsigned char)(v&0x7f);
	}
	else{
		bdt->cl+=5;
		bdt->c=KlMem_realloc(bdt->c,bdt->cl);
		*(bdt->c+bdt->cl-5)=op|0x80;
		*(bdt->c+bdt->cl-4)=(unsigned char)((v>>23)|0x80);
		*(bdt->c+bdt->cl-3)=(unsigned char)((v>>15)&0xff);
		*(bdt->c+bdt->cl-2)=(unsigned char)(((v>>8)&0x7f)|0x80);
		*(bdt->c+bdt->cl-1)=(unsigned char)(v&0xff);
	}
	KlMem_ret(bdt->c);
	return();
}



uint32_t KlBytecode_read_opcode_arg(unsigned char* c,bool ex,uint64_t* i){
	KlMem_enter_func();
	uint32_t o=*(c+*i);
	if ((o&0x80)!=0){
		o&=~0x80;
		(*i)++;
		o=(o<<8)|*(c+*i);
		if (ex==true){
			(*i)++;
			unsigned char t=*(c+*i);
			if ((t&0x80)==0){
				o=(o<<7)|t;
			}
			else{
				(*i)++;
				o=(o<<15)|((t&(~0x80))<<8)|*(c+*i);
			}
		}
	}
	return(o);
}



void KlBytecode_compile_expr(struct ASTExpression* e,uint32_t i,struct BytecodeData* bdt,struct BytecodeStackData* sdt){
	KlMem_enter_func();
	switch (e->t){
		default:
		case AST_EXPRESSION_TYPE_UNKNOWN:
			KlError_unimplemented_error();
			return();
		case AST_EXPRESSION_TYPE_EMPTY:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_CONST:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_ARR:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_EXPAND:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_INC:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_DEC:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_CALL:
			for (size_t j=0;j<e->bl;j++){
				KlBytecode_compile_expr_arg(e->b+j,SIZE_MAX,bdt,sdt);
				sdt->n++;
			}
			KlBytecode_compile_expr_arg(&e->a,SIZE_MAX,bdt,sdt);
			KlBytecode_add_opcode_arg(bdt,OPCODE_CALL,(uint32_t)e->bl);
			KlDebug_print_bytecode(bdt);
			assert(0);
			break;
		case AST_EXPRESSION_TYPE_SLC:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_ACS:
			KlBytecode_compile_expr_arg(&e->a,SIZE_MAX,bdt,sdt);
			if (e->b->t!=AST_EXPRESSION_ARG_TYPE_IDENTIFIER){
				KlBytecode_compile_expr_arg(e->b,SIZE_MAX,bdt,sdt);
				bdt->cl+=4;
				bdt->c=KlMem_realloc(bdt->c,bdt->cl);
				*(bdt->c+bdt->cl-4)=OPCODE_BINARY_SUBS;
				*(bdt->c+bdt->cl-3)=OPCODE_ROT_THREE;
				*(bdt->c+bdt->cl-2)=OPCODE_POP;
				*(bdt->c+bdt->cl-1)=OPCODE_POP;
			}
			else{
				KlBytecode_add_opcode_arg(bdt,OPCODE_BINARY_SUBS_STR,KlBytecode_get_string(bdt,e->b->v.i));
				bdt->cl+=3;
				bdt->c=KlMem_realloc(bdt->c,bdt->cl);
				*(bdt->c+bdt->cl-3)=OPCODE_ROT_THREE;
				*(bdt->c+bdt->cl-2)=OPCODE_POP;
				*(bdt->c+bdt->cl-1)=OPCODE_POP;
			}
			break;
		case AST_EXPRESSION_TYPE_PLUS:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_MINUS:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_NOT:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_BNOT:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_POW:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_ROOT:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_IROOT:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_MLT:
			KlBytecode_compile_expr_arg(&e->a,SIZE_MAX,bdt,sdt);
			KlBytecode_compile_expr_arg(e->b,SIZE_MAX,bdt,sdt);
			bdt->cl+=4;
			bdt->c=KlMem_realloc(bdt->c,bdt->cl);
			*(bdt->c+bdt->cl-4)=OPCODE_BINARY_MLT;
			*(bdt->c+bdt->cl-3)=OPCODE_ROT_THREE;
			*(bdt->c+bdt->cl-2)=OPCODE_POP;
			*(bdt->c+bdt->cl-1)=OPCODE_POP;
			KlDebug_print_bytecode(bdt);
			break;
		case AST_EXPRESSION_TYPE_DIV:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_FDIV:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_MOD:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_MMLT:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_ADD:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_SUB:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_LSH:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_RSH:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_LT:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_LE:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_GT:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_GE:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_EQ:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_NE:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_BAND:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_BXOR:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_BOR:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_AND:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_OR:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_TCOND:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_EQU:
			if (e->a.t==AST_EXPRESSION_ARG_TYPE_IDENTIFIER){
				KlBytecode_compile_expr_arg(e->b,SIZE_MAX,bdt,sdt);
				KlBytecode_add_opcode_arg(bdt,OPCODE_BINARY_ASS,KlBytecode_get_string(bdt,e->a.v.i));
			}
			else{
				KlError_unimplemented_code();
			}
			break;
		case AST_EXPRESSION_TYPE_ADD_EQU:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_SUB_EQU:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_MLT_EQU:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_DIV_EQU:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_FDIV_EQU:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_MOD_EQU:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_MMLT_EQU:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_POW_EQU:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_ROOT_EQU:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_IROOT_EQU:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_LSH_EQU:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_RSH_EQU:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_BAND_EQU:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_BXOR_EQU:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_BOR_EQU:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_TYPE_SEP:
			KlError_unimplemented_code();
			break;
	}
	KlMem_ret(bdt->c);
	return();
}



void KlBytecode_compile_expr_arg(struct ASTExpressionArg* ea,uint32_t i,struct BytecodeData* bdt,struct BytecodeStackData* sdt){
	KlMem_enter_func();
	switch (ea->t){
		default:
		case AST_EXPRESSION_ARG_TYPE_UNKNOWN:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_ARG_TYPE_EXPRESSION:
			KlBytecode_compile_expr(ea->v.ex,SIZE_MAX,bdt,sdt);
			break;
		case AST_EXPRESSION_ARG_TYPE_SCOPE:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_ARG_TYPE_CHAR:
			bdt->cl+=2;
			bdt->c=KlMem_realloc(bdt->c,bdt->cl);
			*(bdt->c+bdt->cl-2)=OPCODE_LOAD_CONST_CHAR;
			*(bdt->c+bdt->cl-1)=(unsigned char)ea->v.c;
			break;
		case AST_EXPRESSION_ARG_TYPE_STRING:
			uint32_t i=KlBytecode_get_string(bdt,ea->v.s);
			KlBytecode_add_opcode_arg(bdt,OPCODE_LOAD_CONST,i);
			KlDebug_print_bytecode(bdt);
			break;
		case AST_EXPRESSION_ARG_TYPE_INT:
			if (ea->v.n->l==1){
				KlBytecode_add_opcode_arg(bdt,OPCODE_LOAD_CONST_INT,*ea->v.n->v);
				KlDebug_print_bytecode(bdt);
			}
			else{
				KlError_unimplemented_code();
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_FLOAT:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_ARG_TYPE_FUNCTION:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_ARG_TYPE_NATIVE_FUNCTION:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_ARG_TYPE_IDENTIFIER:
			KlBytecode_add_opcode_arg(bdt,OPCODE_REF_VAR,KlBytecode_get_string(bdt,ea->v.i));
			break;
		case AST_EXPRESSION_ARG_TYPE_MODIFIERS:
			KlError_unimplemented_code();
			break;
		case AST_EXPRESSION_ARG_TYPE_OBJECT:
			KlError_unimplemented_code();
			break;
	}
	KlMem_ret(bdt->c);
	return();
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

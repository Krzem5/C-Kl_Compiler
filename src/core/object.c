#include <object.h>
#include <memory.h>
#include <io.h>
#include <free.h>
#include <string_utils.h>
#include <api.h>
#include <type.h>
#include <bigint.h>
#include <error.h>
#include <stdlib.h>
#include <shared.h>



uint32_t* _tk;
struct Type** _tv;
size_t _tl;
bool _tfer=false;



struct Object* KlObject_new(struct Type t){
	KlMem_enter_func();
	struct Object* o=(t.alloc_f!=NULL?t.alloc_f():KlMem_malloc(t.sz));
	OBJECT_TYPE_P(o)=KlObject_get_type_ptr(t);
	OBJECT_SIZE_P(o)=t.sz;
	if (t.init_f!=NULL){
		t.init_f(o,NULL,0);
	}
	KlMem_ret(o);
	return(o);
}



struct Object* KlObject_new_args(struct Type t,struct Object** a,size_t al){
	KlMem_enter_func();
	struct Object* o=(t.alloc_f!=NULL?t.alloc_f():KlMem_malloc(t.sz));
	OBJECT_TYPE_P(o)=&t;
	OBJECT_SIZE_P(o)=t.sz;
	if (t.init_f!=NULL){
		t.init_f(o,a,al);
	}
	KlMem_ret(o);
	return(o);
}



size_t KlObject_get_size(struct Object* o){
	KlMem_enter_func();
	if (OBJECT_SIZE_P(o)!=UNKNOWN_SIZE){
		return(OBJECT_SIZE_P(o));
	}
	assert(OBJECT_TYPE_P(o)!=NULL);
	struct Type* t=OBJECT_TYPE_P(o);
	while (true){
		if (t->sz!=UNKNOWN_SIZE){
			return(t->sz);
		}
		assert(t->base);
		t=t->base;
	}
}



void* KlObject_get_op_(size_t off,struct Type* a,struct Type* b){
	KlMem_enter_func();
	while (a!=NULL||b!=NULL){
		if (a!=NULL){
			if ((void*)*((uintptr_t*)((unsigned char*)a+off))!=NULL){
				return((void*)*((uintptr_t*)((unsigned char*)a+off)));
			}
			a=a->base;
		}
		if (b!=NULL){
			if ((void*)*((uintptr_t*)((unsigned char*)b+off))!=NULL){
				return((void*)*((uintptr_t*)((unsigned char*)b+off)));
			}
			b=b->base;
		}
	}
	return(NULL);
}



struct Object* KlObject_expr_arg_to_object(struct ASTExpressionArg a){
	KlMem_enter_func();
	switch (a.t){
		default:
		case AST_EXPRESSION_ARG_TYPE_UNKNOWN:
			KlError_unimplemented_error();
			return(NULL);
		case AST_EXPRESSION_ARG_TYPE_CHAR:
			struct CharObject* co=(struct CharObject*)KlObject_new(CharType);
			co->c=a.v.c;
			return((struct Object*)co);
		case AST_EXPRESSION_ARG_TYPE_STRING:
			struct StrObject* so=(struct StrObject*)KlObject_new(StrType);
			so->s=str_clone(a.v.s);
			so->ln=str_len(a.v.s);
			return((struct Object*)so);
		case AST_EXPRESSION_ARG_TYPE_INT:
			struct BigIntObject* io=(struct BigIntObject*)KlObject_new(BigIntType);
			io->i=KlBigInt_assign(NULL,a.v.n);
			return((struct Object*)io);
		case AST_EXPRESSION_ARG_TYPE_FLOAT:
			KlError_unimplemented_code();
			return(NULL);
		case AST_EXPRESSION_ARG_TYPE_OBJECT:
			return(a.v.o);
	}
}



struct ASTExpressionArg KlObject_to_ast_expr_arg(struct Object* a){
	KlMem_enter_func();
	struct ASTExpressionArg o;
	if (OBJECT_SAME_TYPE_P(a,CharType)){
		o.t=AST_EXPRESSION_ARG_TYPE_CHAR;
		o.v.c=((struct CharObject*)a)->c;
	}
	else if (OBJECT_SAME_TYPE_P(a,StrType)){
		o.t=AST_EXPRESSION_ARG_TYPE_STRING;
		o.v.s=((struct StrObject*)a)->s;
	}
	else if (OBJECT_SAME_TYPE_P(a,BigIntType)){
		o.t=AST_EXPRESSION_ARG_TYPE_INT;
		o.v.n=KlBigInt_assign(NULL,((struct BigIntObject*)a)->i);
	}
	else{
		assert(0);
	}
	return(o);
}



struct BigIntObject* KlObject_to_bigint(struct Object* a){
	KlMem_enter_func();
	if (OBJECT_SAME_TYPE_P(a,BigIntType)){
		return((struct BigIntObject*)a);
	}
	KlError_unimplemented_error();
	return(NULL);
}



struct ASTExpressionArg KlObject_eval_unary(unsigned char t,struct ASTExpressionArg a){
	KlMem_enter_func();
	struct Object* ia=KlObject_expr_arg_to_object(a);
	UnaryFunc op_f=KlObject_get_op_(KlObject_op_to_offset(t),OBJECT_TYPE_P(ia),NULL);
	if (op_f==NULL){
		KlError_unimplemented_error();
		struct ASTExpressionArg oa={
			AST_EXPRESSION_ARG_TYPE_UNKNOWN
		};
		return(oa);
	}
	struct Object* o=op_f(ia);
	if (a.t!=AST_EXPRESSION_ARG_TYPE_OBJECT){
		KlFree_free_object(*ia);
		KlMem_free(ia);
	}
	struct ASTExpressionArg oa=KlObject_to_ast_expr_arg(o);
	KlFree_free_object(*o);
	KlMem_free(o);
	return(oa);
}



struct ASTExpressionArg KlObject_eval_binary(unsigned char t,struct ASTExpressionArg a,struct ASTExpressionArg b){
	KlMem_enter_func();
	struct Object* ia=KlObject_expr_arg_to_object(a);
	struct Object* ib=KlObject_expr_arg_to_object(b);
	BinaryFunc op_f=KlObject_get_op_(KlObject_op_to_offset(t),OBJECT_TYPE_P(ia),OBJECT_TYPE_P(ib));
	if (op_f==NULL){
		KlError_unimplemented_error();
		struct ASTExpressionArg oa={
			AST_EXPRESSION_ARG_TYPE_UNKNOWN
		};
		return(oa);
	}
	struct Object* o=op_f(ia,ib);
	if (a.t!=AST_EXPRESSION_ARG_TYPE_OBJECT){
		KlFree_free_object(*ia);
		KlMem_free(ia);
	}
	if (b.t!=AST_EXPRESSION_ARG_TYPE_OBJECT){
		KlFree_free_object(*ib);
		KlMem_free(ib);
	}
	struct ASTExpressionArg oa=KlObject_to_ast_expr_arg(o);
	KlFree_free_object(*o);
	KlMem_free(o);
	return(oa);
}



size_t KlObject_op_to_offset(unsigned char op){
	KlMem_enter_func();
	switch (op){
		default:
		case AST_EXPRESSION_TYPE_UNKNOWN:
			KlError_unimplemented_error();
			return(SIZE_MAX);
		case AST_EXPRESSION_TYPE_EXPAND:
			KlError_unimplemented_code();
			return(SIZE_MAX);
		case AST_EXPRESSION_TYPE_INC:
			return(OFFSETOF(struct Type,inc_f));
		case AST_EXPRESSION_TYPE_DEC:
			return(OFFSETOF(struct Type,dec_f));
		case AST_EXPRESSION_TYPE_CALL:
			return(OFFSETOF(struct Type,call_f));
		case AST_EXPRESSION_TYPE_ACS:
			KlError_unimplemented_code();
			return(SIZE_MAX);
		case AST_EXPRESSION_TYPE_PLUS:
			return(OFFSETOF(struct Type,pos_f));
		case AST_EXPRESSION_TYPE_MINUS:
			return(OFFSETOF(struct Type,neg_f));
		case AST_EXPRESSION_TYPE_NOT:
			return(OFFSETOF(struct Type,neg_f));
		case AST_EXPRESSION_TYPE_BNOT:
			return(OFFSETOF(struct Type,inv_f));
		case AST_EXPRESSION_TYPE_POW:
			return(OFFSETOF(struct Type,pow_f));
		case AST_EXPRESSION_TYPE_ROOT:
			return(OFFSETOF(struct Type,root_f));
		case AST_EXPRESSION_TYPE_IROOT:
			return(OFFSETOF(struct Type,iroot_f));
		case AST_EXPRESSION_TYPE_MLT:
			return(OFFSETOF(struct Type,mlt_f));
		case AST_EXPRESSION_TYPE_DIV:
			return(OFFSETOF(struct Type,div_f));
		case AST_EXPRESSION_TYPE_FDIV:
			return(OFFSETOF(struct Type,fdiv_f));
		case AST_EXPRESSION_TYPE_MOD:
			return(OFFSETOF(struct Type,mod_f));
		case AST_EXPRESSION_TYPE_MMLT:
			return(OFFSETOF(struct Type,mmlt_f));
		case AST_EXPRESSION_TYPE_ADD:
			return(OFFSETOF(struct Type,add_f));
		case AST_EXPRESSION_TYPE_SUB:
			return(OFFSETOF(struct Type,sub_f));
		case AST_EXPRESSION_TYPE_LSH:
			return(OFFSETOF(struct Type,lsh_f));
		case AST_EXPRESSION_TYPE_RSH:
			return(OFFSETOF(struct Type,rsh_f));
		case AST_EXPRESSION_TYPE_LT:
		case AST_EXPRESSION_TYPE_LE:
		case AST_EXPRESSION_TYPE_GT:
		case AST_EXPRESSION_TYPE_GE:
		case AST_EXPRESSION_TYPE_EQ:
		case AST_EXPRESSION_TYPE_NE:
			KlError_unimplemented_code();
			return(SIZE_MAX);
		case AST_EXPRESSION_TYPE_BAND:
			return(OFFSETOF(struct Type,band_f));
		case AST_EXPRESSION_TYPE_BXOR:
			return(OFFSETOF(struct Type,bxor_f));
		case AST_EXPRESSION_TYPE_BOR:
			return(OFFSETOF(struct Type,bor_f));
		case AST_EXPRESSION_TYPE_AND:
			return(OFFSETOF(struct Type,and_f));
		case AST_EXPRESSION_TYPE_OR:
			return(OFFSETOF(struct Type,or_f));
		case AST_EXPRESSION_TYPE_TCOND:
			KlError_unimplemented_code();
			return(SIZE_MAX);
		case AST_EXPRESSION_TYPE_EQU:
			return(OFFSETOF(struct Type,ass_f));
		case AST_EXPRESSION_TYPE_ADD_EQU:
			return(OFFSETOF(struct Type,inp_add_f));
		case AST_EXPRESSION_TYPE_SUB_EQU:
			return(OFFSETOF(struct Type,inp_sub_f));
		case AST_EXPRESSION_TYPE_MLT_EQU:
			return(OFFSETOF(struct Type,inp_mlt_f));
		case AST_EXPRESSION_TYPE_DIV_EQU:
			return(OFFSETOF(struct Type,inp_div_f));
		case AST_EXPRESSION_TYPE_FDIV_EQU:
			return(OFFSETOF(struct Type,inp_fdiv_f));
		case AST_EXPRESSION_TYPE_MOD_EQU:
			return(OFFSETOF(struct Type,inp_mod_f));
		case AST_EXPRESSION_TYPE_MMLT_EQU:
			return(OFFSETOF(struct Type,inp_mmlt_f));
		case AST_EXPRESSION_TYPE_POW_EQU:
			return(OFFSETOF(struct Type,inp_pow_f));
		case AST_EXPRESSION_TYPE_ROOT_EQU:
			return(OFFSETOF(struct Type,inp_root_f));
		case AST_EXPRESSION_TYPE_IROOT_EQU:
			return(OFFSETOF(struct Type,inp_iroot_f));
		case AST_EXPRESSION_TYPE_LSH_EQU:
			return(OFFSETOF(struct Type,inp_lsh_f));
		case AST_EXPRESSION_TYPE_RSH_EQU:
			return(OFFSETOF(struct Type,inp_rsh_f));
		case AST_EXPRESSION_TYPE_BAND_EQU:
			return(OFFSETOF(struct Type,inp_band_f));
		case AST_EXPRESSION_TYPE_BXOR_EQU:
			return(OFFSETOF(struct Type,inp_bxor_f));
		case AST_EXPRESSION_TYPE_BOR_EQU:
			return(OFFSETOF(struct Type,inp_bor_f));
	}
}



struct Type* KlObject_get_type_ptr(struct Type t){
	KlMem_enter_func();
	if (_tfer==false){
		atexit(KlObject_free_all_types);
		_tfer=true;
	}
	for (size_t i=0;i<_tl;i++){
		if (*(_tk+i)==TYPE_ID(t)){
			return(*(_tv+i));
		}
	}
	_tl++;
	_tk=KlMem_realloc(_tk,_tl*sizeof(uint32_t));
	_tv=KlMem_realloc(_tv,_tl*sizeof(struct Type*));
	*(_tk+_tl-1)=TYPE_ID(t);
	*(_tv+_tl-1)=KlMem_const(&t,sizeof(struct Type));
	KlMem_ret(_tk);
	KlMem_ret(_tv);
	KlMem_ret(*(_tv+_tl-1));
	return(*(_tv+_tl-1));
}



void KlObject_free_all_types(void){
	for (size_t i=0;i<_tl;i++){
		KlMem_free(*(_tv+i));
	}
	KlMem_free(_tk);
	KlMem_free(_tv);
	_tk=NULL;
	_tv=NULL;
	_tl=0;
}

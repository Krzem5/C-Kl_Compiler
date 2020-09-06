#include <object.h>
#include <memory.h>
#include <io.h>
#include <free.h>
#include <api.h>
#include <error.h>
#include <shared.h>



struct Object* KlObject_new(const struct Type t){
	KlMem_enter_func();
	struct Object* o=(t.alloc_f!=NULL?t.alloc_f():KlMem_malloc(t.sz));
	if (t.init_f!=NULL){
		t.init_f(o,NULL,0);
	}
	KlMem_ret(o);
	return(o);
}



struct Object* KlObject_new_args(const struct Type t,struct Object** a,size_t al){
	KlMem_enter_func();
	struct Object* o=(t.alloc_f!=NULL?t.alloc_f():KlMem_malloc(t.sz));
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
	const struct Type* t=OBJECT_TYPE_P(o);
	while (true){
		if (t->sz!=UNKNOWN_SIZE){
			return(t->sz);
		}
		assert(t->base);
		t=t->base;
	}
}



void* KlObject_get_op_(size_t off,const struct Type* a,const struct Type* b){
	KlMem_enter_func();
	while (a!=NULL||b!=NULL){
		if (a!=NULL){
			if ((void*)((unsigned char*)a+off)!=NULL){
				return((void*)((unsigned char*)a+off));
			}
			a=a->base;
		}
		if (b!=NULL){
			if ((void*)((unsigned char*)b+off)!=NULL){
				return((void*)((unsigned char*)b+off));
			}
			b=b->base;
		}
	}
	assert(0);
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
			return(KlApi_char_to_object(a.v.c));
		case AST_EXPRESSION_ARG_TYPE_STRING:
			return(KlApi_str_to_object(a.v.s));
		case AST_EXPRESSION_ARG_TYPE_INT:
			return(KlApi_int_to_object(a.v.n));
		case AST_EXPRESSION_ARG_TYPE_FLOAT:
			KlError_unimplemented_code();
			return(NULL);
		case AST_EXPRESSION_ARG_TYPE_OBJECT:
			return(a.v.o);
	}
}



struct ASTExpressionArg KlObject_to_ast_expr_arg(struct Object* a);



struct ASTExpressionArg KlObject_eval_unary(unsigned char t,struct ASTExpressionArg a){
	KlMem_enter_func();
	struct Object* ia=KlObject_expr_arg_to_object(a);
	struct Object* o=((UnaryFunc)KlObject_get_op_(KlObject_op_to_offset(t),OBJECT_TYPE_P(ia),NULL))(ia);
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
	struct Object* o=((BinaryFunc)KlObject_get_op_(KlObject_op_to_offset(t),OBJECT_TYPE_P(ia),OBJECT_TYPE_P(ib)))(ia,ib);
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

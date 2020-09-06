#include <free.h>
#include <error.h>
#include <memory.h>
#include <object.h>
#include <bigint.h>
#include <io.h>
#include <shared.h>



void KlFree_free_code_file_object(struct CodeFileObject fo){
	KlMem_free(fo.fp);
	KlMem_free(fo.dt);
}



size_t KlFree_free_token(struct ASTToken t){
	switch (t.t){
		case AST_TOKEN_TYPE_STRING:
		case AST_TOKEN_TYPE_IDENTIFIER:
			KlMem_free(t.v);
			break;
		case AST_TOKEN_TYPE_INT:
			KlBigInt_free(t.v);
			break;
	}
	return(t.i);
}



size_t KlFree_free_token_p(struct ASTToken* t){
	switch (t->t){
		case AST_TOKEN_TYPE_STRING:
		case AST_TOKEN_TYPE_IDENTIFIER:
			KlMem_free(t->v);
			break;
		case AST_TOKEN_TYPE_INT:
			KlBigInt_free(t->v);
			break;
	}
	return(t->i);
}



void KlFree_free_expression(struct ASTExpression ex){
	KlMem_enter_func();
	switch (ex.a.t){
		default:
		case AST_EXPRESSION_ARG_TYPE_UNKNOWN:
			KlError_unimplemented_warning();
			break;
		case AST_EXPRESSION_ARG_TYPE_EXPRESSION:
			if (ex.a.v.ex!=NULL){
				KlFree_free_expression(*ex.a.v.ex);
				KlMem_free(ex.a.v.ex);
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_SCOPE:
			if (ex.a.v.sc!=NULL){
				KlFree_free_scope(*ex.a.v.sc);
				KlMem_free(ex.a.v.sc);
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_CHAR:
			break;
		case AST_EXPRESSION_ARG_TYPE_STRING:
			if (ex.a.v.s!=NULL){
				KlMem_free(ex.a.v.s);
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_INT:
			if (ex.a.v.n!=NULL){
				KlMem_free(ex.a.v.n->v);
				KlMem_free(ex.a.v.n);
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_FLOAT:
			if (ex.a.v.d!=NULL){
				KlMem_free(ex.a.v.d->v);
				KlMem_free(ex.a.v.d->fv);
				KlMem_free(ex.a.v.d);
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_FUNCTION:///////
			if (ex.a.v.f!=NULL){
				KlMem_free(ex.a.v.f);
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_NATIVE_FUNCTION:
			break;
		case AST_EXPRESSION_ARG_TYPE_IDENTIFIER:
			if (ex.a.v.i!=NULL){
				KlMem_free(ex.a.v.i);
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_MODIFIERS:
			break;
		case AST_EXPRESSION_ARG_TYPE_OBJECT:///////
			if (ex.a.v.o!=NULL){
				KlMem_free(ex.a.v.o);
			}
			break;
	}
	if (ex.b!=NULL){
		for (size_t i=0;i<ex.bl;i++){
			switch ((ex.b+i)->t){
				default:
				case AST_EXPRESSION_ARG_TYPE_UNKNOWN:
					KlError_unimplemented_warning();
					break;
				case AST_EXPRESSION_ARG_TYPE_EXPRESSION:
					KlFree_free_expression(*(ex.b+i)->v.ex);
					KlMem_free((ex.b+i)->v.ex);
					break;
				case AST_EXPRESSION_ARG_TYPE_SCOPE:
					if ((ex.b+i)->v.sc!=NULL){
						KlFree_free_scope(*(ex.b+i)->v.sc);
						KlMem_free((ex.b+i)->v.sc);
					}
					break;
				case AST_EXPRESSION_ARG_TYPE_CHAR:
					break;
				case AST_EXPRESSION_ARG_TYPE_STRING:
					if ((ex.b+i)->v.s!=NULL){
						KlMem_free((ex.b+i)->v.s);
					}
					break;
				case AST_EXPRESSION_ARG_TYPE_INT:
					if ((ex.b+i)->v.n!=NULL){
						KlMem_free((ex.b+i)->v.n->v);
						KlMem_free((ex.b+i)->v.n);
					}
					break;
				case AST_EXPRESSION_ARG_TYPE_FLOAT:
					if ((ex.b+i)->v.d!=NULL){
						KlMem_free((ex.b+i)->v.d->v);
						KlMem_free((ex.b+i)->v.d->fv);
						KlMem_free((ex.b+i)->v.d);
					}
					break;
				case AST_EXPRESSION_ARG_TYPE_FUNCTION:///////
					if ((ex.b+i)->v.f!=NULL){
						KlMem_free((ex.b+i)->v.f);
					}
					break;
				case AST_EXPRESSION_ARG_TYPE_NATIVE_FUNCTION:
					break;
				case AST_EXPRESSION_ARG_TYPE_IDENTIFIER:
					if ((ex.b+i)->v.i!=NULL){
						KlMem_free((ex.b+i)->v.i);
					}
					break;
				case AST_EXPRESSION_ARG_TYPE_MODIFIERS:
					break;
				case AST_EXPRESSION_ARG_TYPE_OBJECT:///////
					if ((ex.b+i)->v.o!=NULL){
						KlMem_free((ex.b+i)->v.o);
					}
					break;
			}
		}
		KlMem_free(ex.b);
	}
	return();
}



void KlFree_free_unparsed_expression(struct UnparsedASTExpression e){
	KlMem_enter_func();
	for (size_t i=0;i<e.l;i++){
		switch ((e.e+i)->t){
			default:
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_UNKNOWN:
				KlError_unimplemented_warning();
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_EXPRESSION:
				KlFree_free_unparsed_expression(*(e.e+i)->v.ex);
				KlMem_free((e.e+i)->v.ex);
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_OPERATOR:
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_CHAR:
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_STRING:
				KlMem_free((e.e+i)->v.s);
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_INT:
				KlBigInt_free((e.e+i)->v.n);
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_FLOAT:
				///////
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_NATIVE_FUNCTION:
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_IDENTIFIER:
				KlMem_free((e.e+i)->v.i);
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_MODIFIERS:
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_OBJECT:
				///////
				break;
		}
	}
	KlMem_free(e.e);
	return();
}



void KlFree_free_scope(struct ASTScope sc){
	if (sc.nm!=NULL){
		KlMem_free(sc.nm);
	}
	if (sc.cl>0){
		for (size_t i=0;i<sc.cl;i++){
			KlFree_free_expression(**(sc.c+i));
			KlMem_free(*(sc.c+i));
		}
		KlMem_free(sc.c);
	}
	if (sc.fl>0){
		for (size_t i=0;i<sc.fl;i++){
			KlFree_free_scope(**(sc.f+i));
			KlMem_free(*(sc.f+i));
		}
		KlMem_free(sc.f);
	}
	if (sc.vl>0){
		for (size_t i=0;i<sc.vl;i++){
			KlMem_free(*(sc.vnm+i));
		}
		KlMem_free(sc.vnm);
		KlMem_free(sc.vm);
		KlMem_free(sc.vrc);
	}
	if (sc.al>0){
		for (size_t i=0;i<sc.al;i++){
			KlMem_free(*(sc.anm+i));
		}
		KlMem_free(sc.anm);
	}
}



void KlFree_free_sha256(struct SHA256 sha){
	KlMem_free(sha.dt);
	KlMem_free(sha.st);
}



void KlFree_free_object(struct Object o){
	UnaryVoidFunc d_f=(UnaryVoidFunc)KlObject_get_op(deinit_f,OBJECT_TYPE(o),NULL);
	if (d_f!=NULL){
		d_f(&o);
	}
	d_f=(UnaryVoidFunc)KlObject_get_op(dealloc_f,OBJECT_TYPE(o),NULL);
	if (d_f!=NULL){
		d_f(&o);
	}
}

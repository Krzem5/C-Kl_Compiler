#include <free.h>
#include <error.h>
#include <memory.h>
#include <number.h>
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
			KlNum_free(t.v);
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
			KlNum_free(t->v);
			break;
	}
	return(t->i);
}



void KlFree_free_unopt_ast_object(struct UnoptimisedASTObject o){
	KlMem_enter_func();
	for (size_t i=0;i<o.l;i++){
		switch ((o.e+i)->t){
			default:
			case AST_OBJECT_ELEM_TYPE_UNKNOWN:
				KlError_unimplemented_error();
				return();
			case AST_OBJECT_ELEM_TYPE_EXPRESSION:
				KlFree_free_expression((o.e+i)->v.e);
				break;
		}
	}
	KlMem_free(o.e);
	KlFree_free_scope(o.sc);
	return();
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
				KlNum_free((e.e+i)->v.n);
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



void KlFree_free_scope(struct Scope sc){
	if (sc.l>0){
		for (size_t i=0;i<sc.l;i++){
			KlMem_free(*(sc.k+i));
		}
		KlMem_free(sc.k);
		KlMem_free(sc.m);
	}
}

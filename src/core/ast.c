#include <ast.h>
#include <decimal.h>
#include <error.h>
#include <import.h>
#include <number.h>
#include <memory.h>
#include <shared.h>
#include <free.h>
#include <io.h>
#include <string_utils.h>
#include <stdarg.h>
#include <limits.h>



#define NOT_TEXT(i) (*(fo->dt+i)<=47||(*(fo->dt+i)>=58&&*(fo->dt+i)<=64)||(*(fo->dt+i)>=91&&*(fo->dt+i)!=95&&*(fo->dt+i)<=96)||(*(fo->dt+i)>=123&&*(fo->dt+i)<=126))
#define CMP_STR_RET(s,l,g,v_) \
	else if (str_cmp(fo->dt,s,i,l)==true&&NOT_TEXT(i+l)){ \
		o.t=g; \
		o.v=(void*)v_; \
		o.i=i+l; \
		print_token(o); \
		return(o); \
	}



#ifndef NDEBUG
#include <stdio.h>
#include <shared.h>
#include <debug_utils.h>
#define print_token(x) KlDebug_print_ast_token(x)
#define print_ast_expr(x) KlDebug_print_ast_expr(x,0)
#define print_unopt_ast_object(x) KlDebug_print_unopt_ast_object(x)
#define print_unparsed_ast_expr(x) KlDebug_print_unparsed_ast_expr(x,0)
#else
#include <debug_utils.h>
#define print_token(x)
#define print_ast_expr(x)
#define print_unopt_ast_object(x) KlDebug_print_unopt_ast_object(x)
#define print_unparsed_ast_expr(x)
#endif



struct UnoptimisedASTObject* KlAst_parse_ast(struct CodeFileObject* fo,struct CallStack* cs){
	KlMem_enter_func();
	struct ASTToken t=KlAst_next_token(fo,0,cs);
	struct ModifierData md={
		0,
		0,
		0
	};
	struct UnoptimisedASTObject o={
		NULL,
		0,
		{
			NULL,
			NULL,
			NULL,
			0
		}
	};
	while (t.t!=AST_TOKEN_TYPE_END_OF_DATA&&t.t!=AST_TOKEN_TYPE_ERROR){
		if ((t.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)t.v==AST_TOKEN_OPERATOR_SEMICOLON)||t.t==AST_TOKEN_TYPE_WHITESPACE){
			t=KlAst_next_token(fo,KlFree_free_token(t),cs);
			continue;
		}
		if (t.t==AST_TOKEN_TYPE_UNKNOWN){
			KlError_set_error("UnexpectedCharacterError",str_format("Unexpected Character '%c'.",t.v),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t.i),ULONG_MAX,t.i,ULONG_MAX,NULL));
			return(NULL);
		}
		if (t.t==AST_TOKEN_TYPE_MODIFIER){
			md=KlAst_parse_modifiers(fo,&t,cs);
			if (md.m==UCHAR_MAX){
				return(NULL);
			}
			continue;
		}
		if (t.t==AST_TOKEN_TYPE_KEYWORD){
			switch ((unsigned char)(uintptr_t)t.v){
				default:
				case AST_TOKEN_KEYWORD_UNKNOWN:
					KlError_unimplemented_error();
					return(NULL);
				case AST_TOKEN_KEYWORD_FROM:
					{
						struct ASTToken m_nm=KlAst_next_token(fo,KlFree_free_token(t),cs);
						if (m_nm.t==AST_TOKEN_TYPE_WHITESPACE){
							m_nm=KlAst_next_token(fo,KlFree_free_token(m_nm),cs);
						}
						if (m_nm.t!=AST_TOKEN_TYPE_IDENTIFIER){
							KlError_unimplemented_error();
							return(NULL);
						}
						struct ASTToken nt=KlAst_next_token(fo,m_nm.i,cs);
						struct ASTExpression* ie=KlAst_gen_expression("(i((s)i)m)",AST_EXPRESSION_TYPE_EQU,NULL,AST_EXPRESSION_TYPE_ACS,AST_EXPRESSION_TYPE_IMP,m_nm.v,NULL,OBJECT_MODIFIER_EXPORT|OBJECT_MODIFIER_FROZEN);
						struct ASTToken m_e_nm;
						m_e_nm.t=AST_TOKEN_TYPE_UNKNOWN;
						while (true){
							if (nt.t==AST_TOKEN_TYPE_WHITESPACE){
								nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
							}
							if (nt.t!=AST_TOKEN_TYPE_KEYWORD||(unsigned char)(uintptr_t)nt.v!=AST_TOKEN_KEYWORD_USE){
								KlError_unimplemented_error();
								return(NULL);
							}
							m_e_nm=KlAst_next_token(fo,nt.i,cs);
							if (m_e_nm.t==AST_TOKEN_TYPE_WHITESPACE){
								m_e_nm=KlAst_next_token(fo,KlFree_free_token(m_e_nm),cs);
							}
							if (m_e_nm.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)m_e_nm.v==AST_TOKEN_OPERATOR_STAR){
								struct ASTExpression* ex=KlAst_gen_expression("(s)",AST_EXPRESSION_TYPE_IMP_ALL,m_nm.v);
								KlAst_add_expression(&o,*ex);
								KlMem_free(ex);
								m_e_nm=KlAst_next_token(fo,KlFree_free_token(m_e_nm),cs);
								if (m_e_nm.t==AST_TOKEN_TYPE_WHITESPACE){
									m_e_nm=KlAst_next_token(fo,KlFree_free_token(m_e_nm),cs);
								}
								if (m_e_nm.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)m_e_nm.v==AST_TOKEN_OPERATOR_COMMA){
									KlError_unimplemented_error();
									return(NULL);
								}
								else if (m_e_nm.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)m_e_nm.v==AST_TOKEN_OPERATOR_SEMICOLON){
									KlFree_free_token(nt);
									nt=KlAst_next_token(fo,KlFree_free_token(m_e_nm),cs);
									break;
								}
								KlError_unimplemented_error();
								return(NULL);
							}
							else if (m_e_nm.t==AST_TOKEN_TYPE_IDENTIFIER){
								KlFree_free_token(nt);
								nt=KlAst_next_token(fo,m_e_nm.i,cs);
								if (nt.t==AST_TOKEN_TYPE_WHITESPACE){
									nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
								}
								if (ie->b->v.ex->b->v.i!=NULL){
									KlMem_free(ie->b->v.ex->b->v.i);
								}
								ie->b->v.ex->b->v.i=str_clone(m_e_nm.v);
								if (ie->a.v.i!=NULL){
									KlMem_free(ie->a.v.i);
								}
								if (nt.t==AST_TOKEN_TYPE_KEYWORD&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_KEYWORD_AS){
									nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
									if (nt.t==AST_TOKEN_TYPE_WHITESPACE){
										nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
									}
									if (nt.t!=AST_TOKEN_TYPE_IDENTIFIER){
										KlError_unimplemented_error();
										return(NULL);
									}
									ie->a.v.i=str_clone(nt.v);
									KlAst_add_expression(&o,KlAst_clone_expression(*ie));
									nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
									if (nt.t==AST_TOKEN_TYPE_WHITESPACE){
										nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
									}
								}
								else{
									ie->a.v.i=str_clone(m_e_nm.v);
									KlAst_add_expression(&o,KlAst_clone_expression(*ie));
								}
								KlFree_free_token(m_e_nm);
								if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_COMMA){
									nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
									continue;
								}
								else if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_SEMICOLON){
									nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
									break;
								}
								KlError_unimplemented_error();
								return(NULL);
							}
							KlError_unimplemented_error();
							return(NULL);
						}
						KlFree_free_expression(*ie);
						KlMem_free(ie);
						t.i=nt.i;
						KlFree_free_token(nt);
						KlFree_free_token(m_nm);
						break;
					}
				case AST_TOKEN_KEYWORD_USE:
					{
						struct ASTToken m_nm=KlAst_next_token(fo,KlFree_free_token(t),cs);
						if (m_nm.t==AST_TOKEN_TYPE_WHITESPACE){
							m_nm=KlAst_next_token(fo,KlFree_free_token(m_nm),cs);
						}
						if (m_nm.t!=AST_TOKEN_TYPE_IDENTIFIER){
							KlError_unimplemented_error();
							return(NULL);
						}
						struct ASTToken nt=KlAst_next_token(fo,m_nm.i,cs);
						if (nt.t==AST_TOKEN_TYPE_WHITESPACE){
							nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
						}
						char* m_v_nm;
						if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_SEMICOLON){
							m_v_nm=str_clone(m_nm.v);
						}
						else if (nt.t==AST_TOKEN_TYPE_KEYWORD&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_KEYWORD_AS){
							nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
							if (nt.t==AST_TOKEN_TYPE_WHITESPACE){
								nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
							}
							if (nt.t!=AST_TOKEN_TYPE_IDENTIFIER){
								KlError_unimplemented_error();
								return(NULL);
							}
							m_v_nm=str_clone(nt.v);
						}
						else{
							KlError_unimplemented_error();
							return(NULL);
						}
						t.i=nt.i;
						struct ASTExpression* ie=KlAst_gen_expression("(i(s)m)",AST_EXPRESSION_TYPE_EQU,m_v_nm,AST_EXPRESSION_TYPE_IMP,m_nm.v,OBJECT_MODIFIER_EXPORT|OBJECT_MODIFIER_FROZEN);
						KlMem_free(m_v_nm);
						KlFree_free_token(m_nm);
						KlFree_free_token(nt);
						KlAst_add_expression(&o,*ie);
						KlMem_free(ie);
						break;
					}
				case AST_TOKEN_KEYWORD_AS:
					if (md.m!=0){
						KlError_set_error("UnexpectedKeywordError",str_format("Expected a func/class/var declaration, found 'as'."),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t.i),ULONG_MAX,t.i-2,t.i-1,NULL));
						return(NULL);
					}
					else{
						KlError_set_error("UnexpectedKeywordError","Unexpected Keyword 'as'.",KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t.i),ULONG_MAX,t.i-2,t.i-1,NULL));
					}
					return(NULL);
				case AST_TOKEN_KEYWORD_DEF:
					KlError_unimplemented_code();
					return(NULL);
				case AST_TOKEN_KEYWORD_CLASS:
					KlError_unimplemented_code();
					return(NULL);
				case AST_TOKEN_KEYWORD_IF:
					KlError_unimplemented_code();
					return(NULL);
				case AST_TOKEN_KEYWORD_ELIF:
					KlError_unimplemented_code();
					return(NULL);
				case AST_TOKEN_KEYWORD_ELSE:
					KlError_unimplemented_code();
					return(NULL);
				case AST_TOKEN_KEYWORD_FOR:
					KlError_unimplemented_code();
					return(NULL);
				case AST_TOKEN_KEYWORD_WHILE:
					KlError_unimplemented_code();
					return(NULL);
				case AST_TOKEN_KEYWORD_RAISE:
					KlError_unimplemented_code();
					return(NULL);
				case AST_TOKEN_KEYWORD_ASSERT:
					KlError_unimplemented_code();
					return(NULL);
				case AST_TOKEN_KEYWORD_DELETE:
					KlError_unimplemented_code();
					return(NULL);
				case AST_TOKEN_KEYWORD_RETURN:
					KlError_unimplemented_code();
					return(NULL);
				case AST_TOKEN_KEYWORD_DEBUGGER:
					KlError_unimplemented_code();
					return(NULL);
			}
			t=KlAst_next_token(fo,KlFree_free_token(t),cs);
			continue;
		}
		struct ASTExpression* ex=KlAst_parse_expression(fo,&t,cs,&o.sc,md,true,AST_TOKEN_END_SEMICOLON);
		if (ex==NULL){
			return(NULL);
		}
		md.m=0;
		md.s=0;
		md.e=0;
		if (ex->t!=AST_EXPRESSION_TYPE_EMPTY){
			KlAst_add_expression(&o,*ex);
			KlMem_free(ex);
			continue;
		}
		KlMem_free(ex);
		char* s=KlAst_token_to_string(t);
		KlError_set_error("UnexpectedTokenError",str_format("Unexpected Token '%s'.",s),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t.i),ULONG_MAX,t.i-str_len(s),t.i-1,NULL));
		return(NULL);
	}
	if (t.t==AST_TOKEN_TYPE_ERROR){
		KlFree_free_token(t);
		return(NULL);
	}
	KlFree_free_token(t);
	print_unopt_ast_object(&o);
	return(KlMem_const(&o,sizeof(struct UnoptimisedASTObject)));
}



struct ModifierData KlAst_parse_modifiers(struct CodeFileObject* fo,struct ASTToken* t,struct CallStack* cs){
	KlMem_enter_func();
	struct ModifierData o;
	o.m=UCHAR_MAX;
	o.s=0;
	o.e=0;
	while (t->t==AST_TOKEN_TYPE_MODIFIER){
		if (t->v!=0){
			if (o.m==0){
				o.s=t->i-((unsigned char)(uintptr_t)t->v==AST_TOKEN_MODIFIER_PRIVATE?7:((unsigned char)(uintptr_t)t->v==AST_TOKEN_MODIFIER_FROZENTYPE?10:6));
			}
			o.e=t->i-1;
			o.m+=(unsigned char)(uintptr_t)t->v;
		}
		*t=KlAst_next_token(fo,KlFree_free_token_p(t),cs);
		if (t->t!=AST_TOKEN_TYPE_WHITESPACE){
			KlError_unimplemented_error();
			o.m=UCHAR_MAX;
			return(o);
		}
		*t=KlAst_next_token(fo,KlFree_free_token_p(t),cs);
	}
	if ((o.m&AST_TOKEN_MODIFIER_PUBLIC)!=0&&(o.m&AST_TOKEN_MODIFIER_PRIVATE)!=0){
		KlError_set_error("UncompatibleModifierError","Modifier 'public' isn't Compatible with Modifier 'private'.",KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t->i),ULONG_MAX,o.s,o.e,NULL));
		o.m=UCHAR_MAX;
		return(o);
	}
	if ((o.m&AST_TOKEN_MODIFIER_FROZEN)!=0&&(o.m&AST_TOKEN_MODIFIER_FROZENTYPE)!=0){
		KlError_set_error("UncompatibleModifierError","Modifier 'frozen' already Implements Modifier 'frozentype'.",KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t->i),ULONG_MAX,o.s,o.e,NULL));
		o.m=UCHAR_MAX;
		return(o);
	}
	return(o);
}



struct ASTExpression* KlAst_parse_expression(struct CodeFileObject* fo,struct ASTToken* t,struct CallStack* cs,struct Scope* sc,struct ModifierData md,bool e,unsigned char et){
	KlMem_enter_func();
	struct UnparsedASTExpression* o=KlAst_read_expression(fo,t,cs,e,et);
	if (o==NULL){
		return(NULL);
	}
	struct ASTExpression* po=KlAst_parse_unparsed_expression(fo,o,cs,sc);
	KlFree_free_unparsed_expression(*o);
	KlMem_free(o);
	if (po==NULL){
		return(NULL);
	}
	if (po->t==AST_EXPRESSION_TYPE_EQU){
		unsigned char m=KlAst_get_decl(po->a.v.i,sc);
		if (m!=UCHAR_MAX&&md.m!=0){
			KlError_unimplemented_error();
			return(NULL);
		}
		sc->l++;
		sc->k=KlMem_realloc(sc->k,sc->l*sizeof(char*));
		sc->m=KlMem_realloc(sc->m,sc->l*sizeof(char*));
		KlMem_ret(sc->k);
		KlMem_ret(sc->m);
		*(sc->k+sc->l-1)=str_clone(po->a.v.i);
		*(sc->m+sc->l-1)=md.m;
		// printf("ASSIGN: '%s' with modif '0x%02x'\n",po->a.v.i,md.m);
	}
	else if (md.m!=0){
		KlError_unimplemented_error();
		return(NULL);
	}
	return(po);
}



struct UnparsedASTExpression* KlAst_read_expression(struct CodeFileObject* fo,struct ASTToken* t,struct CallStack* cs,bool e,unsigned char et){
	KlMem_enter_func();
	struct UnparsedASTExpression o={
		NULL,
		0
	};
	assert(et==AST_TOKEN_END_SEMICOLON||et==AST_TOKEN_END_COMMA_OR_PARENTHESIS);
	if (e==false&&t->t==AST_TOKEN_TYPE_OPERATOR&&((et==AST_TOKEN_END_SEMICOLON&&(unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_SEMICOLON)||(et==AST_TOKEN_END_COMMA_OR_PARENTHESIS&&((unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_COMMA||(unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_RIGHT_PARENTHESIS)))){
		KlError_unimplemented_error();
		return(NULL);
	}
	while (true){
		if (t->t==AST_TOKEN_TYPE_OPERATOR&&((et==AST_TOKEN_END_SEMICOLON&&(unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_SEMICOLON)||(et==AST_TOKEN_END_COMMA_OR_PARENTHESIS&&((unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_COMMA||(unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_RIGHT_PARENTHESIS)))){
			break;
		}
		switch (t->t){
			default:
			case AST_TOKEN_TYPE_UNKNOWN:
				KlError_set_error("UnexpectedCharacterError",str_format("Unexpected Character '%c'.",t->v),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t->i),ULONG_MAX,t->i,ULONG_MAX,NULL));
				return(NULL);
			case AST_TOKEN_TYPE_STRING:
				{
					struct UnparsedASTExpressionElem el;
					el.t=UNPARSED_AST_EXPRESSION_ELEM_TYPE_STRING;
					el.v.s=str_clone(t->v);
					if (o.l==0){
						KlAst_add_expression_unparsed(&o,el);
						KlMem_ret(el.v.s);
						break;
					}
					if ((o.e+o.l-1)->t==UNPARSED_AST_EXPRESSION_ELEM_TYPE_STRING){
						size_t s_ln=str_len((o.e+o.l-1)->v.s);
						(o.e+o.l-1)->v.s=KlMem_realloc((o.e+o.l-1)->v.s,s_ln+str_len(el.v.s)+1);
						KlMem_memcpy((o.e+o.l-1)->v.s+s_ln,el.v.s,str_len(el.v.s)+1);
						KlMem_free(el.v.s);
						break;
					}
					if ((o.e+o.l-1)->t==UNPARSED_AST_EXPRESSION_ELEM_TYPE_OPERATOR){
						KlAst_add_expression_unparsed(&o,el);
						KlMem_ret(el.v.s);
						break;
					}
					KlError_unimplemented_error();
					return(NULL);
				}
			case AST_TOKEN_TYPE_CHAR:
				{
					struct UnparsedASTExpressionElem el;
					el.t=UNPARSED_AST_EXPRESSION_ELEM_TYPE_CHAR;
					el.v.c=(char)(uintptr_t)t->v;
					if (o.l==0){
						KlAst_add_expression_unparsed(&o,el);
						break;
					}
					if ((o.e+o.l-1)->t==UNPARSED_AST_EXPRESSION_ELEM_TYPE_OPERATOR){
						KlAst_add_expression_unparsed(&o,el);
						break;
					}
					KlError_unimplemented_error();
					return(NULL);
				}
			case AST_TOKEN_TYPE_INT:
				{
					struct UnparsedASTExpressionElem el;
					el.t=UNPARSED_AST_EXPRESSION_ELEM_TYPE_INT;
					el.v.n=KlNum_assign(NULL,t->v);
					if (o.l==0){
						KlAst_add_expression_unparsed(&o,el);
						break;
					}
					if ((o.e+o.l-1)->t==UNPARSED_AST_EXPRESSION_ELEM_TYPE_OPERATOR){
						KlAst_add_expression_unparsed(&o,el);
						break;
					}
					KlError_unimplemented_error();
					return(NULL);
				}
			case AST_TOKEN_TYPE_FLOAT:
				{
					KlError_unimplemented_code();
					return(NULL);
				}
			case AST_TOKEN_TYPE_KEYWORD:
				{
					KlError_unimplemented_error();
					return(NULL);
				}
			case AST_TOKEN_TYPE_MODIFIER:
				{
					KlError_unimplemented_error();
					return(NULL);
				}
			case AST_TOKEN_TYPE_IDENTIFIER:
				{
					struct UnparsedASTExpressionElem el;
					el.t=UNPARSED_AST_EXPRESSION_ELEM_TYPE_IDENTIFIER;
					el.v.i=KlMem_const(t->v,str_len(t->v)+1);
					if (o.l==0){
						KlAst_add_expression_unparsed(&o,el);
						KlMem_ret(el.v.i);
						break;
					}
					if ((o.e+o.l-1)->t==UNPARSED_AST_EXPRESSION_ELEM_TYPE_OPERATOR){
						KlAst_add_expression_unparsed(&o,el);
						KlMem_ret(el.v.i);
						break;
					}
					KlError_unimplemented_error();
					return(NULL);
				}
			case AST_TOKEN_TYPE_OPERATOR:
				if ((unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_LEFT_PARENTHESIS){
					struct UnparsedASTExpressionElem el;
					el.t=UNPARSED_AST_EXPRESSION_ELEM_TYPE_OPERATOR;
					el.v.op=AST_EXPRESSION_TYPE_LST;
					struct UnparsedASTExpression* el_l=KlMem_malloc(sizeof(struct UnparsedASTExpression));
					el_l->e=NULL;
					el_l->l=0;
					struct UnparsedASTExpression* el_l_e;
					struct UnparsedASTExpressionElem el_l_ea;
					el_l_ea.t=UNPARSED_AST_EXPRESSION_ELEM_TYPE_EXPRESSION;
					el_l_ea.v.ex=NULL;
					struct ASTToken nt=KlAst_next_token(fo,t->i,cs);
					while (true){
						if (nt.t==AST_TOKEN_TYPE_WHITESPACE){
							nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
						}
						el_l_e=KlAst_read_expression(fo,&nt,cs,true,AST_TOKEN_END_COMMA_OR_PARENTHESIS);
						if (el_l_e==NULL){
							return(NULL);
						}
						KlMem_ret(el_l_e);
						el_l_ea.v.ex=el_l_e;
						KlAst_add_expression_unparsed(el_l,el_l_ea);
						if (nt.t!=AST_TOKEN_TYPE_OPERATOR){
							KlError_unimplemented_error();
							return(NULL);
						}
						if ((unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_RIGHT_PARENTHESIS){
							break;
						}
						if ((unsigned char)(uintptr_t)nt.v!=AST_TOKEN_OPERATOR_COMMA){
							KlError_unimplemented_error();
							return(NULL);
						}
						nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
						if (nt.t==AST_TOKEN_TYPE_ERROR){
							return(NULL);
						}
					}
					KlMem_ret(el_l);
					if (el_l->l==0){
						KlMem_free(el_l);
					}
					else if (el_l->l==1&&((o.l>0&&(o.e+o.l-1)->t==UNPARSED_AST_EXPRESSION_ELEM_TYPE_OPERATOR)||o.l==0)){
						KlAst_add_expression_unparsed(&o,*el_l->e);
						KlMem_free(el_l->e);
						KlMem_free(el_l);
					}
					else{
						KlAst_add_expression_unparsed(&o,el);
						el.t=UNPARSED_AST_EXPRESSION_ELEM_TYPE_EXPRESSION;
						el.v.ex=el_l;
						KlAst_add_expression_unparsed(&o,el);
					}
					*t=nt;
					break;
				}
				if ((unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_OPEN_BRACKET){
					KlError_unimplemented_code();
					return(NULL);
				}
				if (o.l>0&&(o.e+o.l-1)->t==UNPARSED_AST_EXPRESSION_ELEM_TYPE_OPERATOR){
					KlError_unimplemented_error();
					return(NULL);
				}
				struct UnparsedASTExpressionElem el;
				el.t=UNPARSED_AST_EXPRESSION_ELEM_TYPE_OPERATOR;
				el.v.op=AST_EXPRESSION_TYPE_UNKNOWN;
				switch ((unsigned char)(uintptr_t)t->v){
					default:
					case AST_TOKEN_OPERATOR_UNKNOWN:
						KlError_unimplemented_error();
						return(NULL);
					case AST_TOKEN_OPERATOR_AMP:
						{
							struct ASTToken nt=KlAst_next_token(fo,t->i,cs);
							el.v.op=AST_EXPRESSION_TYPE_BAND;
							if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
								el.v.op=AST_EXPRESSION_TYPE_BAND_EQU;
								KlFree_free_token_p(t);
								*t=nt;
							}
							else if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_AMP){
								el.v.op=AST_EXPRESSION_TYPE_AND;
								KlFree_free_token_p(t);
								*t=nt;
							}
							else{
								KlFree_free_token(nt);
							}
							break;
						}
					case AST_TOKEN_OPERATOR_AT:
						{
							struct ASTToken nt=KlAst_next_token(fo,t->i,cs);
							el.v.op=AST_EXPRESSION_TYPE_MMLT;
							if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
								el.v.op=AST_EXPRESSION_TYPE_MMLT_EQU;
								KlFree_free_token_p(t);
								*t=nt;
							}
							else{
								KlFree_free_token(nt);
							}
							break;
						}
					case AST_TOKEN_OPERATOR_CIRCUMFLEX:
						{
							struct ASTToken nt=KlAst_next_token(fo,t->i,cs);
							el.v.op=AST_EXPRESSION_TYPE_BXOR;
							if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
								el.v.op=AST_EXPRESSION_TYPE_BXOR_EQU;
								KlFree_free_token_p(t);
								*t=nt;
							}
							else{
								KlFree_free_token(nt);
							}
							break;
						}
					case AST_TOKEN_OPERATOR_CLOSE_BRACKET:
						{
							KlError_unimplemented_error();
							return(NULL);
						}
					case AST_TOKEN_OPERATOR_COLON:
						{
							KlError_unimplemented_error();
							return(NULL);
						}
					case AST_TOKEN_OPERATOR_COMMA:
						{
							KlError_unimplemented_error();
							return(NULL);
						}
					case AST_TOKEN_OPERATOR_EQUALS:
						{
							struct ASTToken nt=KlAst_next_token(fo,t->i,cs);
							el.v.op=AST_EXPRESSION_TYPE_EQU;
							if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
								el.v.op=AST_EXPRESSION_TYPE_EQ;
								KlFree_free_token_p(t);
								*t=nt;
							}
							else{
								KlFree_free_token(nt);
							}
							break;
						}
					case AST_TOKEN_OPERATOR_EXCLAMATION_MARK:
						{
							struct ASTToken nt=KlAst_next_token(fo,t->i,cs);
							el.v.op=AST_EXPRESSION_TYPE_NOT;
							if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
								el.v.op=AST_EXPRESSION_TYPE_NE;
								KlFree_free_token_p(t);
								*t=nt;
							}
							else{
								KlFree_free_token(nt);
							}
							break;
						}
					case AST_TOKEN_OPERATOR_GRATER:
						{
							struct ASTToken nt=KlAst_next_token(fo,t->i,cs);
							el.v.op=AST_EXPRESSION_TYPE_GT;
							if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
								el.v.op=AST_EXPRESSION_TYPE_GE;
								*t=nt;
							}
							else if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_GRATER){
								el.v.op=AST_EXPRESSION_TYPE_RSH;
								*t=nt;
								struct ASTToken nt2=KlAst_next_token(fo,nt.i,cs);
								if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
									el.v.op=AST_EXPRESSION_TYPE_RSH_EQU;
									KlFree_free_token_p(t);
									*t=nt2;
								}
								else{
									KlFree_free_token(nt2);
								}
							}
							else{
								KlFree_free_token(nt);
							}
							break;
						}
					case AST_TOKEN_OPERATOR_LEFT_PARENTHESIS:
						{
							KlError_unimplemented_error();
							return(NULL);
						}
					case AST_TOKEN_OPERATOR_LESS:
						{
							struct ASTToken nt=KlAst_next_token(fo,t->i,cs);
							el.v.op=AST_EXPRESSION_TYPE_LT;
							if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
								el.v.op=AST_EXPRESSION_TYPE_LE;
								KlFree_free_token_p(t);
								*t=nt;
							}
							else if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_GRATER){
								el.v.op=AST_EXPRESSION_TYPE_LSH;
								KlFree_free_token_p(t);
								*t=nt;
								struct ASTToken nt2=KlAst_next_token(fo,nt.i,cs);
								if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
									el.v.op=AST_EXPRESSION_TYPE_LSH_EQU;
									KlFree_free_token_p(t);
									*t=nt2;
								}
								else{
									KlFree_free_token(nt2);
								}
							}
							else{
								KlFree_free_token(nt);
							}
							break;
						}
					case AST_TOKEN_OPERATOR_MINUS:
						{
							struct ASTToken nt=KlAst_next_token(fo,t->i,cs);
							el.v.op=(o.l==0?AST_EXPRESSION_TYPE_MINUS:AST_EXPRESSION_TYPE_SUB);
							if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
								el.v.op=AST_EXPRESSION_TYPE_SUB_EQU;
								KlFree_free_token_p(t);
								*t=nt;
							}
							else if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_MINUS){
								el.v.op=AST_EXPRESSION_TYPE_DEC;
								KlFree_free_token_p(t);
								*t=nt;
							}
							else{
								KlFree_free_token(nt);
							}
							break;
						}
					case AST_TOKEN_OPERATOR_MODULO:
						{
							struct ASTToken nt=KlAst_next_token(fo,t->i,cs);
							el.v.op=AST_EXPRESSION_TYPE_MOD;
							if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
								el.v.op=AST_EXPRESSION_TYPE_MOD_EQU;
								KlFree_free_token_p(t);
								*t=nt;
							}
							else{
								KlFree_free_token(nt);
							}
							break;
						}
					case AST_TOKEN_OPERATOR_PERIOD:
						{
							el.v.op=AST_EXPRESSION_TYPE_ACS;
							break;
						}
					case AST_TOKEN_OPERATOR_PLUS:
						{
							struct ASTToken nt=KlAst_next_token(fo,t->i,cs);
							el.v.op=(o.l==0?AST_EXPRESSION_TYPE_PLUS:AST_EXPRESSION_TYPE_ADD);
							if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
								el.v.op=AST_EXPRESSION_TYPE_ADD_EQU;
								KlFree_free_token_p(t);
								*t=nt;
							}
							else if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_PLUS){
								el.v.op=AST_EXPRESSION_TYPE_INC;
								KlFree_free_token_p(t);
								*t=nt;
							}
							else{
								KlFree_free_token(nt);
							}
							break;
						}
					case AST_TOKEN_OPERATOR_QUESTION_MARK:
						{
							KlError_unimplemented_code();
							return(NULL);
						}
					case AST_TOKEN_OPERATOR_RIGHT_PARENTHESIS:
						{
							KlError_unimplemented_error();
							return(NULL);
						}
					case AST_TOKEN_OPERATOR_SEMICOLON:
						{
							KlError_unimplemented_error();
							return(NULL);
						}
					case AST_TOKEN_OPERATOR_SLASH:
						{
							struct ASTToken nt=KlAst_next_token(fo,t->i,cs);
							el.v.op=AST_EXPRESSION_TYPE_DIV;
							if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
								el.v.op=AST_EXPRESSION_TYPE_DIV_EQU;
								KlFree_free_token_p(t);
								*t=nt;
							}
							else if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_SLASH){
								KlFree_free_token_p(t);
								*t=nt;
								struct ASTToken nt2=KlAst_next_token(fo,nt.i,cs);
								el.v.op=AST_EXPRESSION_TYPE_FDIV;
								if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
									el.v.op=AST_EXPRESSION_TYPE_FDIV_EQU;
									KlFree_free_token_p(t);
									*t=nt2;
								}
								else{
									KlFree_free_token(nt2);
								}
							}
							else{
								KlFree_free_token(nt);
							}
							break;
						}
					case AST_TOKEN_OPERATOR_STAR:
						{
							struct ASTToken nt=KlAst_next_token(fo,t->i,cs);
							el.v.op=AST_EXPRESSION_TYPE_MLT;
							if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
								el.v.op=AST_EXPRESSION_TYPE_MLT_EQU;
								KlFree_free_token_p(t);
								*t=nt;
							}
							else if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_STAR){
								KlFree_free_token_p(t);
								*t=nt;
								struct ASTToken nt2=KlAst_next_token(fo,nt.i,cs);
								el.v.op=AST_EXPRESSION_TYPE_POW;
								if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
									el.v.op=AST_EXPRESSION_TYPE_POW_EQU;
									KlFree_free_token_p(t);
									*t=nt2;
								}
								else{
									KlFree_free_token(nt2);
								}
							}
							else if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_SLASH){
								KlFree_free_token_p(t);
								*t=nt;
								struct ASTToken nt2=KlAst_next_token(fo,nt.i,cs);
								el.v.op=AST_EXPRESSION_TYPE_ROOT;
								if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
									el.v.op=AST_EXPRESSION_TYPE_ROOT_EQU;
									KlFree_free_token_p(t);
									*t=nt2;
								}
								else if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_SLASH){
									el.v.op=AST_EXPRESSION_TYPE_IROOT;
									KlFree_free_token_p(t);
									*t=nt2;
									struct ASTToken nt3=KlAst_next_token(fo,nt2.i,cs);
									if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
										el.v.op=AST_EXPRESSION_TYPE_IROOT_EQU;
										KlFree_free_token_p(t);
										*t=nt3;
									}
									else{
										KlFree_free_token(nt3);
									}
								}
								else{
									KlFree_free_token(nt2);
								}
							}
							else{
								KlFree_free_token(nt);
							}
							break;
						}
					case AST_TOKEN_OPERATOR_TILDA:
						{
							struct ASTToken nt=KlAst_next_token(fo,t->i,cs);
							el.v.op=AST_EXPRESSION_TYPE_BNOT;
							if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
								el.v.op=AST_EXPRESSION_TYPE_BNOT_EQU;
								KlFree_free_token_p(t);
								*t=nt;
							}
							else{
								KlFree_free_token(nt);
							}
							break;
						}
					case AST_TOKEN_OPERATOR_VERTICAL_BAR:
						{
							struct ASTToken nt=KlAst_next_token(fo,t->i,cs);
							el.v.op=AST_EXPRESSION_TYPE_BOR;
							if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_EQUALS){
								el.v.op=AST_EXPRESSION_TYPE_BOR_EQU;
								KlFree_free_token_p(t);
								*t=nt;
							}
							else if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_VERTICAL_BAR){
								el.v.op=AST_EXPRESSION_TYPE_OR;
								KlFree_free_token_p(t);
								*t=nt;
							}
							else{
								KlFree_free_token(nt);
							}
							break;
						}
				}
				if (o.l==0&&(el.v.op!=AST_EXPRESSION_TYPE_MINUS&&el.v.op!=AST_EXPRESSION_TYPE_PLUS)){
					KlError_unimplemented_error();
				}
				KlAst_add_expression_unparsed(&o,el);
				break;
			case AST_TOKEN_TYPE_WHITESPACE:
				break;
			case AST_TOKEN_TYPE_ERROR:
				return(NULL);
		}
		*t=KlAst_next_token(fo,KlFree_free_token_p(t),cs);
		if (t->t==AST_TOKEN_TYPE_UNKNOWN){
			KlError_set_error("UnexpectedCharacterError",str_format("Unexpected Character '%c'.",t->v),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t->i),ULONG_MAX,t->i,ULONG_MAX,NULL));
			return(NULL);
		}
		if (t->t==AST_TOKEN_TYPE_ERROR){
			return(NULL);
		}
	}
	struct UnparsedASTExpression* o_=KlMem_const(&o,sizeof(struct UnparsedASTExpression));
	KlMem_ret(o_);
	return(o_);
}



struct ASTExpression* KlAst_parse_unparsed_expression(struct CodeFileObject* fo,struct UnparsedASTExpression* e,struct CallStack* cs,struct Scope* sc){
	KlMem_enter_func();
	struct ASTExpression* c=KlMem_malloc(sizeof(struct ASTExpression));
	c->t=AST_EXPRESSION_TYPE_EMPTY;
	c->a.t=AST_EXPRESSION_TYPE_UNKNOWN;
	c->b=NULL;
	c->bl=0;
	unsigned char* c_a=KlMem_malloc(1);
	unsigned char* c_am=KlMem_malloc(1);
	struct ASTExpression** c_p=KlMem_malloc(sizeof(struct ASTExpression*));
	*c_a=0;
	*c_am=UCHAR_MAX;
	*c_p=c;
	unsigned long c_ai=0;
	for (struct UnparsedASTExpressionElem* i=e->e;i<e->e+e->l;i++){
		switch (i->t){
			default:
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_UNKNOWN:
				KlError_unimplemented_error();
				return(NULL);
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_EXPRESSION:
				{
					if (*(c_a+c_ai)==0){
						c->t=AST_EXPRESSION_TYPE_CONST;
						c->a.t=AST_EXPRESSION_ARG_TYPE_EXPRESSION;
						c->a.v.ex=KlAst_parse_unparsed_expression(fo,i->v.ex,cs,sc);
						KlMem_ret(c->a.v.ex);
					}
					else{
						c->bl++;
						c->b=KlMem_realloc(c->b,c->bl*sizeof(struct ASTExpressionArg));
						KlMem_ret(c->b);
						(c->b+c->bl-1)->t=AST_EXPRESSION_ARG_TYPE_EXPRESSION;
						(c->b+c->bl-1)->v.ex=KlAst_parse_unparsed_expression(fo,i->v.ex,cs,sc);
						KlMem_ret((c->b+c->bl-1)->v.ex);
					}
					(*(c_a+c_ai))++;
					while (*(c_a+c_ai)==*(c_am+c_ai)){
						assert(c_ai>0);
						c_a=KlMem_realloc(c_a,c_ai);
						c_am=KlMem_realloc(c_am,c_ai);
						c_p=KlMem_realloc(c_p,c_ai*sizeof(struct ASTExpression*));
						c_ai--;
						c=*(c_p+c_ai);
					}
					break;
				}
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_OPERATOR:
				{
					if (i>e->e&&i->v.op==AST_EXPRESSION_TYPE_LST&&(i-1)->t!=UNPARSED_AST_EXPRESSION_ELEM_TYPE_OPERATOR&&(i-1)->t!=UNPARSED_AST_EXPRESSION_ELEM_TYPE_EXPRESSION){
						i++;
						assert(i->t==UNPARSED_AST_EXPRESSION_ELEM_TYPE_EXPRESSION);
						struct ASTExpression* tmp=c;
						KlMem_ret(tmp);
						c=KlMem_malloc(sizeof(struct ASTExpression));
						c->t=AST_EXPRESSION_TYPE_CALL;
						c->a.t=AST_EXPRESSION_ARG_TYPE_EXPRESSION;
						c->a.v.ex=tmp;
						c->b=KlMem_malloc(i->v.ex->l*sizeof(struct ASTExpressionArg));
						c->bl=i->v.ex->l;
						for (unsigned long j=0;j<i->v.ex->l;j++){
							assert((i->v.ex->e+j)->t==UNPARSED_AST_EXPRESSION_ELEM_TYPE_EXPRESSION);
							(c->b+j)->t=AST_EXPRESSION_ARG_TYPE_EXPRESSION;
							(c->b+j)->v.ex=KlAst_parse_unparsed_expression(fo,(i->v.ex->e+j)->v.ex,cs,sc);
							if ((c->b+j)->v.ex->t==AST_EXPRESSION_TYPE_CONST){
								struct ASTExpressionArg tmp_a=(c->b+j)->v.ex->a;
								KlMem_free((c->b+j)->v.ex);
								*(c->b+j)=tmp_a;
							}
						}
						*(c_p+c_ai)=c;
						break;
					}
					if (i->v.op==AST_EXPRESSION_TYPE_LST||i->v.op==AST_EXPRESSION_TYPE_ARR){
						i++;
						struct ASTExpression* p=c;
						if (*(c_a+c_ai)>0){
							c->bl++;
							c->b=KlMem_realloc(c->b,c->bl*sizeof(struct ASTExpressionArg));
							KlMem_ret(c->b);
							(c->b+c->bl-1)->t=AST_EXPRESSION_ARG_TYPE_EXPRESSION;
							(c->b+c->bl-1)->v.ex=KlMem_malloc(sizeof(struct ASTExpression));
							p=(c->b+c->bl-1)->v.ex;
						}
						(*(c_a+c_ai))++;
						while (*(c_a+c_ai)==*(c_am+c_ai)){
							assert(c_ai>0);
							c_a=KlMem_realloc(c_a,c_ai);
							c_am=KlMem_realloc(c_am,c_ai);
							c_p=KlMem_realloc(c_p,c_ai*sizeof(struct ASTExpression*));
							c_ai--;
							c=*(c_p+c_ai);
						}
						p->t=i->v.op;
						p->b=NULL;
						p->bl=0;
						for (unsigned long j=0;j<i->v.ex->l;j++){
							struct ASTExpression* el=KlAst_parse_unparsed_expression(fo,(i->v.ex->e+j)->v.ex,cs,sc);
							KlMem_ret(el);
							if (j==0){
								p->a.t=AST_EXPRESSION_ARG_TYPE_EXPRESSION;
								p->a.v.ex=el;
							}
							else{
								p->bl++;
								p->b=KlMem_realloc(p->b,p->bl*sizeof(struct ASTExpressionArg));
								KlMem_ret(p->b);
								(p->b+p->bl-1)->t=AST_EXPRESSION_ARG_TYPE_EXPRESSION;
								(p->b+p->bl-1)->v.ex=el;
							}
						}
						break;
					}
					if (c->t==AST_EXPRESSION_TYPE_EMPTY){
						KlError_unimplemented_error();
						return(NULL);
					}
					if (c->t==AST_EXPRESSION_TYPE_CONST){
						c->t=i->v.op;
						while (*(c_a+c_ai)==*(c_am+c_ai)){
							assert(c_ai>0);
							c_a=KlMem_realloc(c_a,c_ai);
							c_am=KlMem_realloc(c_am,c_ai);
							c_p=KlMem_realloc(c_p,c_ai*sizeof(struct ASTExpression*));
							c_ai--;
							c=*(c_p+c_ai);
						}
						break;
					}
					if (KlAst_get_precedence(i->v.op)<KlAst_get_precedence(c->t)){
						struct ASTExpression* ex=KlMem_malloc(sizeof(struct ASTExpression));
						ex->t=i->v.op;
						ex->a.t=AST_EXPRESSION_ARG_TYPE_UNKNOWN;
						ex->b=NULL;
						ex->bl=0;
						KlMem_ret(ex);
						if (*(c_a+c_ai)==1){
							ex->a=c->a;
							c->a.t=AST_EXPRESSION_ARG_TYPE_EXPRESSION;
							c->a.v.ex=ex;
						}
						else{
							ex->a=*(c->b+*(c_a+c_ai)-2);
							(c->b+*(c_a+c_ai)-2)->t=AST_EXPRESSION_ARG_TYPE_EXPRESSION;
							(c->b+*(c_a+c_ai)-2)->v.ex=ex;
						}
						if (KlAst_get_op_count(ex->t)>1){
							c_ai++;
							c_a=KlMem_realloc(c_a,c_ai+1);
							c_am=KlMem_realloc(c_am,c_ai+1);
							c_p=KlMem_realloc(c_p,(c_ai+1)*sizeof(struct ASTExpression));
							*(c_a+c_ai)=1;
							*(c_am+c_ai)=KlAst_get_op_count(ex->t);
							*(c_p+c_ai)=c=ex;
						}
					}
					else{
						struct ASTExpression* tmp=c;
						KlMem_ret(tmp);
						c=KlMem_malloc(sizeof(struct ASTExpression));
						c->t=i->v.op;
						c->a.t=AST_EXPRESSION_ARG_TYPE_EXPRESSION;
						c->a.v.ex=tmp;
						c->b=NULL;
						c->bl=0;
						*(c_a+c_ai)=1;
						*(c_p+c_ai)=c;
					}
					break;
				}
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_CHAR:
				{
					if (*(c_a+c_ai)==0){
						c->t=AST_EXPRESSION_TYPE_CONST;
						c->a.t=AST_EXPRESSION_ARG_TYPE_CHAR;
						c->a.v.c=i->v.c;
					}
					else{
						c->bl++;
						c->b=KlMem_realloc(c->b,c->bl*sizeof(struct ASTExpressionArg));
						KlMem_ret(c->b);
						(c->b+c->bl-1)->t=AST_EXPRESSION_ARG_TYPE_CHAR;
						(c->b+c->bl-1)->v.c=i->v.c;
					}
					(*(c_a+c_ai))++;
					while (*(c_a+c_ai)==*(c_am+c_ai)){
						assert(c_ai>0);
						c_a=KlMem_realloc(c_a,c_ai);
						c_am=KlMem_realloc(c_am,c_ai);
						c_p=KlMem_realloc(c_p,c_ai*sizeof(struct ASTExpression*));
						c_ai--;
						c=*(c_p+c_ai);
					}
					break;
				}
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_STRING:
				{
					if (*(c_a+c_ai)==0){
						c->t=AST_EXPRESSION_TYPE_CONST;
						c->a.t=AST_EXPRESSION_ARG_TYPE_STRING;
						c->a.v.s=str_clone(i->v.s);
					}
					else{
						c->bl++;
						c->b=KlMem_realloc(c->b,c->bl*sizeof(struct ASTExpressionArg));
						KlMem_ret(c->b);
						(c->b+c->bl-1)->t=AST_EXPRESSION_ARG_TYPE_STRING;
						(c->b+c->bl-1)->v.s=str_clone(i->v.s);
					}
					(*(c_a+c_ai))++;
					while (*(c_a+c_ai)==*(c_am+c_ai)){
						assert(c_ai>0);
						c_a=KlMem_realloc(c_a,c_ai);
						c_am=KlMem_realloc(c_am,c_ai);
						c_p=KlMem_realloc(c_p,c_ai*sizeof(struct ASTExpression*));
						c_ai--;
						c=*(c_p+c_ai);
					}
					break;
				}
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_INT:
				{
					struct Number* nc=NULL;
					if (i->v.n!=NULL){
						nc=KlNum_assign(NULL,i->v.n);
					}
					if (*(c_a+c_ai)==0){
						c->t=AST_EXPRESSION_TYPE_CONST;
						c->a.t=AST_EXPRESSION_ARG_TYPE_INT;
						c->a.v.n=nc;
					}
					else{
						c->bl++;
						c->b=KlMem_realloc(c->b,c->bl*sizeof(struct ASTExpressionArg));
						KlMem_ret(c->b);
						(c->b+c->bl-1)->t=AST_EXPRESSION_ARG_TYPE_INT;
						(c->b+c->bl-1)->v.n=nc;
					}
					(*(c_a+c_ai))++;
					while (*(c_a+c_ai)==*(c_am+c_ai)){
						assert(c_ai>0);
						c_a=KlMem_realloc(c_a,c_ai);
						c_am=KlMem_realloc(c_am,c_ai);
						c_p=KlMem_realloc(c_p,c_ai*sizeof(struct ASTExpression*));
						c_ai--;
						c=*(c_p+c_ai);
					}
					break;
				}
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_FLOAT:
				{
					KlError_unimplemented_code();
					return(NULL);
				}
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_NATIVE_FUNCTION:
				{
					KlError_unimplemented_code();
					return(NULL);
				}
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_IDENTIFIER:
				{
					if (*(c_a+c_ai)==0){
						c->t=AST_EXPRESSION_TYPE_CONST;
						c->a.t=AST_EXPRESSION_ARG_TYPE_IDENTIFIER;
						c->a.v.i=str_clone(i->v.i);
					}
					else{
						c->bl++;
						c->b=KlMem_realloc(c->b,c->bl*sizeof(struct ASTExpressionArg));
						KlMem_ret(c->b);
						(c->b+c->bl-1)->t=AST_EXPRESSION_ARG_TYPE_IDENTIFIER;
						(c->b+c->bl-1)->v.i=str_clone(i->v.i);
					}
					(*(c_a+c_ai))++;
					while (*(c_a+c_ai)==*(c_am+c_ai)){
						assert(c_ai>0);
						c_a=KlMem_realloc(c_a,c_ai);
						c_am=KlMem_realloc(c_am,c_ai);
						c_p=KlMem_realloc(c_p,c_ai*sizeof(struct ASTExpression*));
						c_ai--;
						c=*(c_p+c_ai);
					}
					break;
				}
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_MODIFIERS:
				{
					KlError_unimplemented_code();
					return(NULL);
				}
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_OBJECT:
				{
					KlError_unimplemented_code();
					return(NULL);
				}
		}
	}
	assert(c_ai==0);
	KlMem_free(c_a);
	KlMem_free(c_am);
	KlMem_free(c_p);
	KlMem_ret(c);
	return(c);
}



struct ASTExpression* KlAst_gen_expression(const char* f,...){
	KlMem_enter_func();
	va_list a;
	va_start(a,f);
	struct ASTExpression* o=KlMem_malloc(sizeof(struct ASTExpression));
	o->t=AST_EXPRESSION_TYPE_EMPTY;
	o->a.t=AST_EXPRESSION_ARG_TYPE_UNKNOWN;
	o->a.v.c=0;
	o->b=NULL;
	o->bl=0;
	size_t ol=0;
	size_t i=0;
	size_t ln=str_len((char*)f);
	if (ln==0){
		va_end(a);
		return(KlMem_const(&o,sizeof(struct ASTExpression)));
	}
	unsigned char b=0;
	while (i<ln){
		if (ol==0&&o->t==AST_EXPRESSION_TYPE_CONST){
			KlError_unimplemented_error();
			return(NULL);
		}
		switch (*(f+i)){
			case '(':
				b++;
				enum AST_EXPRESSION_TYPE t=va_arg(a,enum AST_EXPRESSION_TYPE);
				if (ol==0){
					o->t=t;
					ol=1;
				}
				else{
					ol++;
					o=KlMem_realloc(o,ol*sizeof(struct ASTExpression));
					(o+ol-1)->t=t;
					(o+ol-1)->a.t=AST_EXPRESSION_ARG_TYPE_UNKNOWN;
					(o+ol-1)->a.v.c=0;
					(o+ol-1)->b=NULL;
					(o+ol-1)->bl=0;
				}
				break;
			case ')':
				if (b==0){
					KlError_unimplemented_error();
					return(NULL);
				}
				ol--;
				if (ol!=0){
					if ((o+ol-1)->a.t==AST_EXPRESSION_ARG_TYPE_UNKNOWN){
						(o+ol-1)->a.t=AST_EXPRESSION_ARG_TYPE_EXPRESSION;
						(o+ol-1)->a.v.ex=KlMem_memcpy(KlMem_malloc(sizeof(struct ASTExpression)),o+ol,sizeof(struct ASTExpression));
						KlMem_ret((o+ol-1)->a.v.ex);
					}
					else{
						struct ASTExpressionArg* ea=KlMem_malloc(sizeof(struct ASTExpressionArg));
						ea->t=AST_EXPRESSION_ARG_TYPE_EXPRESSION;
						ea->v.ex=KlMem_memcpy(KlMem_malloc(sizeof(struct ASTExpression)),o+ol,sizeof(struct ASTExpression));
						KlMem_ret(ea->v.ex);
						(o+ol-1)->bl++;
						(o+ol-1)->b=KlMem_realloc((o+ol-1)->b,(o+ol-1)->bl*sizeof(struct ASTExpressionArg));
						KlMem_ret((o+ol-1)->b);
						KlMem_memcpy((o+ol-1)->b+(o+ol-1)->bl-1,ea,sizeof(struct ASTExpressionArg));
						KlMem_free(ea);
					}
					o=KlMem_realloc(o,ol*sizeof(struct ASTExpression));
				}
				b--;
				break;
			case 'c':
				char c_=va_arg(a,char);
				if (ol==0){
					o->t=AST_EXPRESSION_TYPE_CONST;
					o->a.t=AST_EXPRESSION_ARG_TYPE_CHAR;
					o->a.v.c=c_;
				}
				else{
					if ((o+ol-1)->a.t==AST_EXPRESSION_ARG_TYPE_UNKNOWN){
						(o+ol-1)->a.t=AST_EXPRESSION_ARG_TYPE_CHAR;
						(o+ol-1)->a.v.c=c_;
					}
					else{
						struct ASTExpressionArg* ea=KlMem_malloc(sizeof(struct ASTExpressionArg));
						ea->t=AST_EXPRESSION_ARG_TYPE_CHAR;
						ea->v.c=c_;
						(o+ol-1)->bl++;
						(o+ol-1)->b=KlMem_realloc((o+ol-1)->b,(o+ol-1)->bl*sizeof(struct ASTExpressionArg));
						KlMem_ret((o+ol-1)->b);
						KlMem_memcpy((o+ol-1)->b+(o+ol-1)->bl-1,ea,sizeof(struct ASTExpressionArg));
						KlMem_free(ea);
					}
				}
				break;
			case 's':
				char* s=va_arg(a,char*);
				if (ol==0){
					o->t=AST_EXPRESSION_TYPE_CONST;
					o->a.t=AST_EXPRESSION_ARG_TYPE_STRING;
					o->a.v.s=str_clone(s);
				}
				else{
					if ((o+ol-1)->a.t==AST_EXPRESSION_ARG_TYPE_UNKNOWN){
						(o+ol-1)->a.t=AST_EXPRESSION_ARG_TYPE_STRING;
						(o+ol-1)->a.v.s=str_clone(s);
					}
					else{
						struct ASTExpressionArg* ea=KlMem_malloc(sizeof(struct ASTExpressionArg));
						ea->t=AST_EXPRESSION_ARG_TYPE_STRING;
						ea->v.s=str_clone(s);
						(o+ol-1)->bl++;
						(o+ol-1)->b=KlMem_realloc((o+ol-1)->b,(o+ol-1)->bl*sizeof(struct ASTExpressionArg));
						KlMem_ret((o+ol-1)->b);
						KlMem_memcpy((o+ol-1)->b+(o+ol-1)->bl-1,ea,sizeof(struct ASTExpressionArg));
						KlMem_free(ea);
					}
				}
				break;
			case 'n':
				struct Number* n=va_arg(a,struct Number*);
				struct Number* nc=NULL;
				if (n!=NULL){
					nc=KlNum_assign(NULL,n);
				}
				if (ol==0){
					o->t=AST_EXPRESSION_TYPE_CONST;
					o->a.t=AST_EXPRESSION_ARG_TYPE_INT;
					o->a.v.n=nc;
				}
				else{
					if ((o+ol-1)->a.t==AST_EXPRESSION_ARG_TYPE_UNKNOWN){
						(o+ol-1)->a.t=AST_EXPRESSION_ARG_TYPE_INT;
						(o+ol-1)->a.v.n=nc;
					}
					else{
						struct ASTExpressionArg* ea=KlMem_malloc(sizeof(struct ASTExpressionArg));
						ea->t=AST_EXPRESSION_ARG_TYPE_INT;
						ea->v.n=nc;
						(o+ol-1)->bl++;
						(o+ol-1)->b=KlMem_realloc((o+ol-1)->b,(o+ol-1)->bl*sizeof(struct ASTExpressionArg));
						KlMem_memcpy((o+ol-1)->b+(o+ol-1)->bl-1,ea,sizeof(struct ASTExpressionArg));
						KlMem_free(ea);
					}
				}
				break;
			case 'd':
				KlError_unimplemented_error();
				return NULL;
				struct Decimal* d=va_arg(a,struct Decimal*);
				if (ol==0){
					o->t=AST_EXPRESSION_TYPE_CONST;
					o->a.t=AST_EXPRESSION_ARG_TYPE_FLOAT;
					o->a.v.d=d;//////
				}
				else{
					if ((o+ol-1)->a.t==AST_EXPRESSION_ARG_TYPE_UNKNOWN){
						(o+ol-1)->a.t=AST_EXPRESSION_ARG_TYPE_FLOAT;
						(o+ol-1)->a.v.d=d;//////
					}
					else{
						struct ASTExpressionArg* ea=KlMem_malloc(sizeof(struct ASTExpressionArg));
						ea->t=AST_EXPRESSION_ARG_TYPE_FLOAT;
						ea->v.d=d;///////
						(o+ol-1)->bl++;
						(o+ol-1)->b=KlMem_realloc((o+ol-1)->b,(o+ol-1)->bl*sizeof(struct ASTExpressionArg));
						KlMem_memcpy((o+ol-1)->b+(o+ol-1)->bl-1,ea,sizeof(struct ASTExpressionArg));
						KlMem_free(ea);
					}
				}
				break;
			case 'F':
				KlError_unimplemented_error();
				return NULL;
				struct Function* f_=va_arg(a,struct Function*);
				if (ol==0){
					o->t=AST_EXPRESSION_TYPE_CONST;
					o->a.t=AST_EXPRESSION_ARG_TYPE_FUNCTION;
					o->a.v.f=f_;////////////
				}
				else{
					if ((o+ol-1)->a.t==AST_EXPRESSION_ARG_TYPE_UNKNOWN){
						(o+ol-1)->a.t=AST_EXPRESSION_ARG_TYPE_FUNCTION;
						(o+ol-1)->a.v.f=f_;///////////////////////
					}
					else{
						struct ASTExpressionArg* ea=KlMem_malloc(sizeof(struct ASTExpressionArg));
						ea->t=AST_EXPRESSION_ARG_TYPE_FUNCTION;
						ea->v.f=f_;/////////////
						(o+ol-1)->bl++;
						(o+ol-1)->b=KlMem_realloc((o+ol-1)->b,(o+ol-1)->bl*sizeof(struct ASTExpressionArg));
						KlMem_memcpy((o+ol-1)->b+(o+ol-1)->bl-1,ea,sizeof(struct ASTExpressionArg));
						KlMem_free(ea);
					}
				}
				break;
			case 'N':
				i++;
				if (i>=ln){
					KlError_unimplemented_error();
					return(NULL);
				}
				struct NativeFunction nf;
				switch (*(f+i)){
					case '0':
						nf.t=NATIVE_FUNCITON_TYPE_0_ARGS;
						nf.f._0a=va_arg(a,NativeFunctionZeroArg);
						break;
					case '1':
						nf.t=NATIVE_FUNCITON_TYPE_1_ARG;
						nf.f._1a=va_arg(a,NativeFunctionOneArg);
						break;
					case 'a':
						nf.t=NATIVE_FUNCITON_TYPE_ARGS;
						nf.f.a=va_arg(a,NativeFunctionArgs);
						break;
					case 'v':
						nf.t=NATIVE_FUNCITON_TYPE_VARARG;
						nf.f.va=va_arg(a,NativeFunctionVarArg);
						break;
					default:
						KlError_unimplemented_error();
						return(NULL);
				}
				if (ol==0){
					o->t=AST_EXPRESSION_TYPE_CONST;
					o->a.t=AST_EXPRESSION_ARG_TYPE_NATIVE_FUNCTION;
					o->a.v.nf=nf;
				}
				else{
					if ((o+ol-1)->a.t==AST_EXPRESSION_ARG_TYPE_UNKNOWN){
						(o+ol-1)->a.t=AST_EXPRESSION_ARG_TYPE_NATIVE_FUNCTION;
						(o+ol-1)->a.v.nf=nf;
					}
					else{
						struct ASTExpressionArg* ea=KlMem_malloc(sizeof(struct ASTExpressionArg));
						ea->t=AST_EXPRESSION_ARG_TYPE_NATIVE_FUNCTION;
						ea->v.nf=nf;
						(o+ol-1)->bl++;
						(o+ol-1)->b=KlMem_realloc((o+ol-1)->b,(o+ol-1)->bl*sizeof(struct ASTExpressionArg));
						KlMem_memcpy((o+ol-1)->b+(o+ol-1)->bl-1,ea,sizeof(struct ASTExpressionArg));
					}
				}
				break;
			case 'i':
				char* i_=va_arg(a,char*);
				if (ol==0){
					o->t=AST_EXPRESSION_TYPE_CONST;
					o->a.t=AST_EXPRESSION_ARG_TYPE_IDENTIFIER;
					o->a.v.i=str_clone(i_);
				}
				else{
					if ((o+ol-1)->a.t==AST_EXPRESSION_ARG_TYPE_UNKNOWN){
						(o+ol-1)->a.t=AST_EXPRESSION_ARG_TYPE_IDENTIFIER;
						(o+ol-1)->a.v.i=str_clone(i_);
					}
					else{
						struct ASTExpressionArg* ea=KlMem_malloc(sizeof(struct ASTExpressionArg));
						ea->t=AST_EXPRESSION_ARG_TYPE_IDENTIFIER;
						ea->v.i=str_clone(i_);
						(o+ol-1)->bl++;
						(o+ol-1)->b=KlMem_realloc((o+ol-1)->b,(o+ol-1)->bl*sizeof(struct ASTExpressionArg));
						KlMem_ret((o+ol-1)->b);
						KlMem_memcpy((o+ol-1)->b+(o+ol-1)->bl-1,ea,sizeof(struct ASTExpressionArg));
						KlMem_free(ea);
					}
				}
				break;
			case 'm':
				unsigned char m=va_arg(a,unsigned char);
				if (ol==0){
					o->t=AST_EXPRESSION_TYPE_CONST;
					o->a.t=AST_EXPRESSION_ARG_TYPE_MODIFIERS;
					o->a.v.m=m;
				}
				else{
					if ((o+ol-1)->a.t==AST_EXPRESSION_ARG_TYPE_UNKNOWN){
						(o+ol-1)->a.t=AST_EXPRESSION_ARG_TYPE_MODIFIERS;
						(o+ol-1)->a.v.m=m;
					}
					else{
						struct ASTExpressionArg* ea=KlMem_malloc(sizeof(struct ASTExpressionArg));
						ea->t=AST_EXPRESSION_ARG_TYPE_MODIFIERS;
						ea->v.m=m;
						(o+ol-1)->bl++;
						(o+ol-1)->b=KlMem_realloc((o+ol-1)->b,(o+ol-1)->bl*sizeof(struct ASTExpressionArg));
						KlMem_memcpy((o+ol-1)->b+(o+ol-1)->bl-1,ea,sizeof(struct ASTExpressionArg));
						KlMem_free(ea);
					}
				}
				break;
			case 'o':
				KlError_unimplemented_error();
				return NULL;
				struct Object* o_=va_arg(a,struct Object*);
				if (ol==0){
					o->t=AST_EXPRESSION_TYPE_CONST;
					o->a.t=AST_EXPRESSION_ARG_TYPE_OBJECT;
					o->a.v.o=o_;/////////////////
				}
				else{
					if ((o+ol-1)->a.t==AST_EXPRESSION_ARG_TYPE_UNKNOWN){
						(o+ol-1)->a.t=AST_EXPRESSION_ARG_TYPE_OBJECT;
						(o+ol-1)->a.v.o=o_;///////////////
					}
					else{
						struct ASTExpressionArg* ea=KlMem_malloc(sizeof(struct ASTExpressionArg));
						ea->t=AST_EXPRESSION_ARG_TYPE_OBJECT;
						ea->v.o=o_;/////////////////
						(o+ol-1)->bl++;
						(o+ol-1)->b=KlMem_realloc((o+ol-1)->b,(o+ol-1)->bl*sizeof(struct ASTExpressionArg));
						KlMem_memcpy((o+ol-1)->b+(o+ol-1)->bl-1,ea,sizeof(struct ASTExpressionArg));
						KlMem_free(ea);
					}
				}
				break;
			default:
				assert_msg(0,str_format("Unknown Character '%c'",*(f+i)));
				KlError_unimplemented_error();
				break;
		}
		i++;
	}
	if (b!=0){
		KlError_unimplemented_error();
		return(NULL);
	}
	va_end(a);
	KlMem_ret(o);
	return(o);
}



struct ASTExpression KlAst_clone_expression(struct ASTExpression ex){
	KlMem_enter_func();
	struct ASTExpression o;
	o.t=ex.t;
	o.a.t=ex.a.t;
	switch (o.a.t){
		default:
		case AST_EXPRESSION_ARG_TYPE_UNKNOWN:
			KlError_unimplemented_warning();
			break;
		case AST_EXPRESSION_ARG_TYPE_EXPRESSION:
			if (ex.a.v.ex==NULL){
				o.a.v.ex=NULL;
			}
			else{
				struct ASTExpression e=KlAst_clone_expression(*ex.a.v.ex);
				o.a.v.ex=KlMem_const(&e,sizeof(struct ASTExpression));
				KlMem_ret(o.a.v.ex);
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_CHAR:
			o.a.v.c=ex.a.v.c;
			break;
		case AST_EXPRESSION_ARG_TYPE_STRING:
			o.a.v.s=str_clone(ex.a.v.s);
			if (o.a.v.s!=NULL){
				KlMem_ret(o.a.v.s);
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_INT:
			if (ex.a.v.n==NULL){
				o.a.v.n=NULL;
			}
			else{
				o.a.v.n=KlNum_assign(NULL,ex.a.v.n);
				KlMem_ret(o.a.v.n);
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_FLOAT:///////
			if (ex.a.v.d==NULL){
				o.a.v.d=NULL;
			}
			else{
				o.a.v.d=NULL;//
				///////
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_FUNCTION:///////
			if (ex.a.v.f==NULL){
				o.a.v.f=NULL;
			}
			else{
				o.a.v.f=NULL;//
				///////
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_NATIVE_FUNCTION:
			o.a.v.nf=ex.a.v.nf;
			break;
		case AST_EXPRESSION_ARG_TYPE_IDENTIFIER:
			o.a.v.i=str_clone(ex.a.v.i);
			if (o.a.v.i!=NULL){
				KlMem_ret(o.a.v.i);
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_MODIFIERS:
			o.a.v.m=ex.a.v.m;
			break;
		case AST_EXPRESSION_ARG_TYPE_OBJECT:///////
			if (ex.a.v.f==NULL){
				o.a.v.f=NULL;
			}
			else{
				o.a.v.f=NULL;//
				///////
			}
			break;
	}
	o.bl=ex.bl;
	if (ex.b==NULL){
		o.b=NULL;
	}
	else{
		o.b=KlMem_malloc(o.bl*sizeof(struct ASTExpressionArg));
		KlMem_ret(o.b);
		for (unsigned long i=0;i<o.bl;i++){
			(o.b+i)->t=(ex.b+i)->t;
			switch ((o.b+i)->t){
				default:
				case AST_EXPRESSION_ARG_TYPE_UNKNOWN:
					KlError_unimplemented_warning();
					break;
				case AST_EXPRESSION_ARG_TYPE_EXPRESSION:
					if ((ex.b+i)->v.ex==NULL){
						(o.b+i)->v.ex=NULL;
					}
					else{
						struct ASTExpression e=KlAst_clone_expression(*(ex.b+i)->v.ex);
						(o.b+i)->v.ex=KlMem_const(&e,sizeof(struct ASTExpression));
						KlMem_ret((o.b+i)->v.ex);
					}
					break;
				case AST_EXPRESSION_ARG_TYPE_CHAR:
					(o.b+i)->v.c=(ex.b+i)->v.c;
					break;
				case AST_EXPRESSION_ARG_TYPE_STRING:
					if ((ex.b+i)->v.s==NULL){
						(o.b+i)->v.s=NULL;
					}
					else{
						size_t ln=str_len((ex.b+i)->v.s)+1;
						(o.b+i)->v.s=KlMem_memcpy(KlMem_malloc(ln),(ex.b+i)->v.s,ln);
						KlMem_ret((o.b+i)->v.s);
					}
					break;
				case AST_EXPRESSION_ARG_TYPE_INT:
					if ((ex.b+i)->v.n==NULL){
						(o.b+i)->v.n=NULL;
					}
					else{
						(o.b+i)->v.n=KlNum_assign(NULL,(ex.b+i)->v.n);
						KlMem_ret((o.b+i)->v.n);
					}
					break;
				case AST_EXPRESSION_ARG_TYPE_FLOAT:///////
					if ((ex.b+i)->v.d==NULL){
						(o.b+i)->v.d=NULL;
					}
					else{
						(o.b+i)->v.d=NULL;//
						///////
					}
					break;
				case AST_EXPRESSION_ARG_TYPE_FUNCTION:///////
					if ((ex.b+i)->v.f==NULL){
						(o.b+i)->v.f=NULL;
					}
					else{
						(o.b+i)->v.f=NULL;//
						///////
					}
					break;
				case AST_EXPRESSION_ARG_TYPE_NATIVE_FUNCTION:
					(o.b+i)->v.nf=(ex.b+i)->v.nf;
					break;
				case AST_EXPRESSION_ARG_TYPE_IDENTIFIER:
					if ((ex.b+i)->v.i==NULL){
						(o.b+i)->v.i=NULL;
					}
					else{
						size_t ln=str_len((ex.b+i)->v.i)+1;
						(o.b+i)->v.i=KlMem_memcpy(KlMem_malloc(ln),(ex.b+i)->v.i,ln);
						KlMem_ret((o.b+i)->v.i);
					}
					break;
				case AST_EXPRESSION_ARG_TYPE_MODIFIERS:
					(o.b+i)->v.m=(ex.b+i)->v.m;
					break;
				case AST_EXPRESSION_ARG_TYPE_OBJECT:///////
					if ((ex.b+i)->v.f==NULL){
						(o.b+i)->v.f=NULL;
					}
					else{
						(o.b+i)->v.f=NULL;//
						///////
					}
					break;
			}
		}
	}
	return(o);
}



struct ASTToken KlAst_next_token(struct CodeFileObject* fo,size_t i,struct CallStack* cs){
	KlMem_enter_func();
	struct ASTToken o={
		AST_TOKEN_TYPE_END_OF_DATA,
		NULL,
		0
	};
	size_t ln=str_len(fo->dt);
	if (i>=ln){
		print_token(o);
		return(o);
	}
	bool ws=false;
	while (*(fo->dt+i)==' '||*(fo->dt+i)=='\t'||*(fo->dt+i)=='\v'||*(fo->dt+i)=='\r'||*(fo->dt+i)=='\n'){
		i++;
		ws=true;
	}
	if (ws==true){
		o.t=AST_TOKEN_TYPE_WHITESPACE;
		o.i=i;
		print_token(o);
		return(o);
	}
	if (*(fo->dt+i)=='\''){
		o.t=AST_TOKEN_TYPE_CHAR;
		i++;
		if (*(fo->dt+i)=='\''){
			o.t=AST_TOKEN_TYPE_ERROR;
			KlError_set_error("EmptyCharacterStringError","Empty Character String.",KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i-1),ULONG_MAX,i-1,i,"<char>"));
			print_token(o);
			return(o);
		}
		unsigned char e;
		char c=KlAst_parse_string_char(fo->dt,&i,&e);
		o.v=(void*)(uintptr_t)c;
		if (e!=0){
			o.t=AST_TOKEN_TYPE_ERROR;
			switch (e){
				case 1:
					KlError_set_error("InvalidEscapeSequenceError",str_format("Expected a Any of 'b e f n o r t v x', found '%c'",*(fo->dt+i)),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i-1),ULONG_MAX,i-1,i,"<char>"));
					print_token(o);
					return(o);
				case 2:
					KlError_set_error("InvalidEscapeSequenceError",str_format("Expected a Decimal Digit, found '%c'.",*(fo->dt+i)),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i-1),ULONG_MAX,i,ULONG_MAX,"<char>"));
					print_token(o);
					return(o);
				case 3:
					KlError_set_error("InvalidEscapeSequenceError",str_format("Expected an Octal Digit, found '%c'.",*(fo->dt+i)),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i-1),ULONG_MAX,i,ULONG_MAX,"<char>"));
					print_token(o);
					return(o);
				case 4:
					KlError_set_error("InvalidEscapeSequenceError",str_format("Expected a Hexadecimal Character, found '%c'.",*(fo->dt+i)),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i-1),ULONG_MAX,i,ULONG_MAX,"<char>"));
					print_token(o);
					return(o);
				default:
					KlError_unimplemented_error();
					print_token(o);
					return(o);
			}
		}
		if (*(fo->dt+i)!='\''){
			o.t=AST_TOKEN_TYPE_ERROR;
			KlError_set_error("UnmachedQuoteError",str_format("Expected String Quote ('\\''), found '%c'.",*(fo->dt+i)),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i),ULONG_MAX,i,ULONG_MAX,"<char>"));
			print_token(o);
			return(o);
		}
		o.i=i+1;
		print_token(o);
		return(o);
	}
	if (*(fo->dt+i)=='\"'){
		o.t=AST_TOKEN_TYPE_STRING;
		i++;
		size_t sz=0;
		size_t sl=KlError_offset_to_line(fo->dt,i-1);
		size_t su=i-1;
		while (*(fo->dt+i)!='\"'||*(fo->dt+i-1)=='\\'){
			if ((*(fo->dt+i)=='\r'&&*(fo->dt+i)=='\n')||*(fo->dt+i)=='\n'){
				i+=(*(fo->dt+i)=='\r'?2:1);
				continue;
			}
			sz++;
			o.v=KlMem_realloc(o.v,sz+1);
			unsigned char e;
			*((char*)o.v+sz-1)=KlAst_parse_string_char(fo->dt,&i,&e);
			if (e!=0){
				KlMem_free(o.v);
				o.t=AST_TOKEN_TYPE_ERROR;
				switch (e){
					case 1:
						KlError_set_error("InvalidEscapeSequenceError",str_format("Expected a Any of 'b e f n o r t v x', found '%c'",*(fo->dt+i)),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i-1),ULONG_MAX,i=1,i,"<string>"));
						print_token(o);
						return(o);
					case 2:
						KlError_set_error("InvalidEscapeSequenceError",str_format("Expected a Decimal Digit, found '%c'.",*(fo->dt+i)),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i-1),ULONG_MAX,i,ULONG_MAX,"<string>"));
						print_token(o);
						return(o);
					case 3:
						KlError_set_error("InvalidEscapeSequenceError",str_format("Expected an Octal Digit, found '%c'.",*(fo->dt+i)),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i-1),ULONG_MAX,i,ULONG_MAX,"<string>"));
						print_token(o);
						return(o);
					case 4:
						KlError_set_error("InvalidEscapeSequenceError",str_format("Expected a Hexadecimal Character, found '%c'.",*(fo->dt+i)),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i-1),ULONG_MAX,i,ULONG_MAX,"<string>"));
						return(o);
					default:
						KlError_unimplemented_error();
						print_token(o);
						return(o);
				}
			}
			if (i>=ln){
				o.t=AST_TOKEN_TYPE_ERROR;
				KlError_set_error("UnmachedQuoteError","Expected String Quote ('\"'), reached End of Data.",KlError_extend_call_stack(cs,fo,sl,KlError_offset_to_line(fo->dt,i-1),su,i-1,"<string>"));
				print_token(o);
				return(o);
			}
		}
		*((char*)o.v+sz)=0;
		o.i=i+1;
		KlMem_ret(o.v);
		print_token(o);
		return(o);
	}
	if (*(fo->dt+i)=='0'){
		switch (*(fo->dt+i+1)){
			default:
				break;
			case 'b':
				i+=2;
				o.t=AST_TOKEN_TYPE_INT;
				o.v=KlNum_from_long(0);
				while (*(fo->dt+i)=='0'||*(fo->dt+i)=='1'){
					KlNum_add_bin_digit(o.v,*(fo->dt+i)-48);
					i++;
				}
				print_token(o);
				return(o);
			case 'o':
				i+=2;
				o.t=AST_TOKEN_TYPE_INT;
				o.v=KlNum_from_long(0);
				while (*(fo->dt+i)>=48&&*(fo->dt+i)<=55){
					KlNum_add_oct_digit(o.v,*(fo->dt+i)-48);
					i++;
				}
				print_token(o);
				return(o);
			case 'x':
				i+=2;
				o.t=AST_TOKEN_TYPE_INT;
				o.v=KlNum_from_long(0);
				while ((*(fo->dt+i)>=48&&*(fo->dt+i)<=57)||(*(fo->dt+i)>=65&&*(fo->dt+i)<=70)||(*(fo->dt+i)>=97&&*(fo->dt+i)<=102)){
					if (*(fo->dt+i)<=57){
						KlNum_add_hex_digit(o.v,*(fo->dt+i)-48);
					}
					else if (*(fo->dt+i)<=70){
						KlNum_add_hex_digit(o.v,*(fo->dt+i)-55);
					}
					else{
						KlNum_add_hex_digit(o.v,*(fo->dt+i)-87);
					}
					i++;
				}
				print_token(o);
				return(o);
		}
	}
	if (*(fo->dt+i)>=48&&*(fo->dt+i)<=57){
		o.t=AST_TOKEN_TYPE_INT;
		o.v=KlNum_from_long(0);
		while (true){
			if (*(fo->dt+i)=='_'){
				i++;
				continue;
			}
			if (*(fo->dt+i)=='.'){
				if (o.t==AST_TOKEN_TYPE_FLOAT){
					o.t=AST_TOKEN_TYPE_UNKNOWN;
					o.v=(void*)(uintptr_t)'.';
					o.i=i;
					print_token(o);
					return(o);
				}
				o.v=KlDec_from_num(o.v);
				i++;
				continue;
			}
			if (*(fo->dt+i)>=48&&*(fo->dt+i)<=57){
				if (o.t==AST_TOKEN_TYPE_INT){
					KlNum_add_digit(o.v,*(fo->dt+i)-48);
				}
				else{
					KlDec_add_frac_digit(o.v,*(fo->dt+i)-48);
				}
				i++;
				continue;
			}
			break;
		}
		o.i=i;
		print_token(o);
		return(o);
	}
	CMP_STR_RET("from",4,AST_TOKEN_TYPE_KEYWORD,AST_TOKEN_KEYWORD_FROM)
	CMP_STR_RET("use",3,AST_TOKEN_TYPE_KEYWORD,AST_TOKEN_KEYWORD_USE)
	CMP_STR_RET("as",2,AST_TOKEN_TYPE_KEYWORD,AST_TOKEN_KEYWORD_AS)
	CMP_STR_RET("def",3,AST_TOKEN_TYPE_KEYWORD,AST_TOKEN_KEYWORD_DEF)
	CMP_STR_RET("class",5,AST_TOKEN_TYPE_KEYWORD,AST_TOKEN_KEYWORD_CLASS)
	CMP_STR_RET("if",2,AST_TOKEN_TYPE_KEYWORD,AST_TOKEN_KEYWORD_IF)
	CMP_STR_RET("elif",4,AST_TOKEN_TYPE_KEYWORD,AST_TOKEN_KEYWORD_ELIF)
	CMP_STR_RET("else",4,AST_TOKEN_TYPE_KEYWORD,AST_TOKEN_KEYWORD_ELSE)
	CMP_STR_RET("for",3,AST_TOKEN_TYPE_KEYWORD,AST_TOKEN_KEYWORD_FOR)
	CMP_STR_RET("while",5,AST_TOKEN_TYPE_KEYWORD,AST_TOKEN_KEYWORD_WHILE)
	CMP_STR_RET("raise",5,AST_TOKEN_TYPE_KEYWORD,AST_TOKEN_KEYWORD_RAISE)
	CMP_STR_RET("assert",6,AST_TOKEN_TYPE_KEYWORD,AST_TOKEN_KEYWORD_ASSERT)
	CMP_STR_RET("delete",6,AST_TOKEN_TYPE_KEYWORD,AST_TOKEN_KEYWORD_DELETE)
	CMP_STR_RET("return",6,AST_TOKEN_TYPE_KEYWORD,AST_TOKEN_KEYWORD_RETURN)
	CMP_STR_RET("debugger",8,AST_TOKEN_TYPE_KEYWORD,AST_TOKEN_KEYWORD_DEBUGGER)
	CMP_STR_RET("public",6,AST_TOKEN_TYPE_MODIFIER,AST_TOKEN_MODIFIER_PUBLIC)
	CMP_STR_RET("private",7,AST_TOKEN_TYPE_MODIFIER,AST_TOKEN_MODIFIER_PRIVATE)
	CMP_STR_RET("static",6,AST_TOKEN_TYPE_MODIFIER,AST_TOKEN_MODIFIER_STATIC)
	CMP_STR_RET("export",6,AST_TOKEN_TYPE_MODIFIER,AST_TOKEN_MODIFIER_EXPORT)
	CMP_STR_RET("frozentype",10,AST_TOKEN_TYPE_MODIFIER,AST_TOKEN_MODIFIER_FROZENTYPE)
	CMP_STR_RET("frozen",6,AST_TOKEN_TYPE_MODIFIER,AST_TOKEN_MODIFIER_FROZEN)
	else if (*(fo->dt+i)=='&'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_AMP;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)=='@'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_AT;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)=='^'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_CIRCUMFLEX;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)==']'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_CLOSE_BRACKET;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)==':'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_COLON;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)==','){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_COMMA;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)=='='){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_EQUALS;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)=='!'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_EXCLAMATION_MARK;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)=='>'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_GRATER;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)=='('){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_LEFT_PARENTHESIS;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)=='<'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_LESS;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)=='-'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_MINUS;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)=='%'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_MODULO;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)=='['){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_OPEN_BRACKET;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)=='.'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_PERIOD;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)=='+'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_PLUS;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)=='?'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_QUESTION_MARK;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)==')'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_RIGHT_PARENTHESIS;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)==';'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_SEMICOLON;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)=='/'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_SLASH;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)=='*'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_STAR;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)=='~'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_TILDA;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	else if (*(fo->dt+i)=='|'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_VERTICAL_BAR;
		o.i=i+1;
		print_token(o);
		return(o);
	}
	unsigned long sz=0;
	while ((*(fo->dt+i)>47&&*(fo->dt+i)<58)||(*(fo->dt+i)>64&&*(fo->dt+i)<91)||*(fo->dt+i)==95||(*(fo->dt+i)>96&&*(fo->dt+i)<123)||*(fo->dt+i)>126){
		sz++;
		o.v=KlMem_realloc(o.v,sz+1);
		*((char*)o.v+sz-1)=*(fo->dt+i);
		i++;
	}
	if (sz>0){
		if (*(char*)o.v>=48&&*(char*)o.v<=57){
			KlMem_free(o.v);
			o.t=AST_TOKEN_TYPE_UNKNOWN;
			o.v=(void*)(uintptr_t)*(fo->dt+i-sz);
			o.i=i-sz;
			print_token(o);
			return(o);
		}
		o.t=AST_TOKEN_TYPE_IDENTIFIER;
		*((char*)o.v+sz)=0;
		o.i=i;
		print_token(o);
		KlMem_ret(o.v);
		return(o);
	}
	o.t=AST_TOKEN_TYPE_UNKNOWN;
	o.v=(void*)(uintptr_t)*(fo->dt+i);
	o.i=i;
	print_token(o);
	return(o);
}



char KlAst_parse_string_char(char* s,size_t* i,unsigned char* e){
	KlMem_enter_func();
	*e=0;
	if (*(s+*i)=='\\'){
		(*i)+=2;
		switch (*(s+*i-1)){
			case 'b':
				return(0x08);
			case 'd':
				{
					char o=0;
					bool f=false;
					while (48<=*(s+*i)&&*(s+*i)<=57){
						if (o<255){
							f=true;
							if ((unsigned short)o*10+*(s+*i)-48>255){
								o=255;
								break;
							}
							o=o*10+*(s+*i)-48;
						}
						(*i)++;
					}
					if (f==false){
						*e=2;
						return(0);
					}
					return(o);
				}
			case 'e':
				return(0x1b);
			case 'f':
				return(0x0c);
			case 'n':
				return(0x0a);
			case 'o':
				{
					char o=0;
					bool f=false;
					while (48<=*(s+*i)&&*(s+*i)<=55){
						if (o<255){
							f=true;
							if ((unsigned short)o*8+*(s+*i)-48>255){
								o=255;
								break;
							}
							o=o*8+*(s+*i)-48;
						}
						(*i)++;
					}
					if (f==false){
						*e=3;
						return(0);
					}
					return(o);
				}
			case 'r':
				return(0x0d);
			case 't':
				return(0x09);
			case 'v':
				return(0x0b);
			case 'x':
				{
					char o=0;
					bool f=false;
					while ((48<=*(s+*i)&&*(s+*i)<=57)||(97<=*(s+*i)&&*(s+*i)<=102)){
						if (o<255){
							f=true;
							if ((unsigned short)o*16+(*(s+*i)<=57?*(s+*i)-48:*(s+*i)-87)>255){
								o=255;
								break;
							}
							o=o*16+(*(s+*i)<=57?*(s+*i)-48:*(s+*i)-87);
						}
						(*i)++;
					}
					if (f==false){
						*e=4;
						return(0);
					}
					return(o);
				}
			case '\'':
				return(0x27);
			case '\"':
				return(0x22);
			default:
				(*i)--;
				*e=1;
				return(0);
		}
	}
	char o=*(s+*i);
	(*i)++;
	return(o);
}



char* KlAst_token_to_string(struct ASTToken t){
	KlMem_enter_func();
	switch (t.t){
		default:
		case AST_TOKEN_TYPE_UNKNOWN:
			return("<UNKNOWN>");
		case AST_TOKEN_TYPE_STRING:
		case AST_TOKEN_TYPE_IDENTIFIER:
			return(KlMem_const(t.v,str_len(t.v)+1));
		case AST_TOKEN_TYPE_CHAR:
			{
				char* o=KlMem_malloc(2);
				*o=(char)(uintptr_t)t.v;
				*(o+1)=0;
				return(o);
			}
		case AST_TOKEN_TYPE_INT:
			return("AST_TOKEN_TYPE_INT");
		case AST_TOKEN_TYPE_KEYWORD:
			switch ((unsigned char)(uintptr_t)t.v){
				default:
				case AST_TOKEN_KEYWORD_UNKNOWN:
					return("AST_TOKEN_KEYWORD_UNKNOWN");
				case AST_TOKEN_KEYWORD_FROM:
					return("from");
				case AST_TOKEN_KEYWORD_USE:
					return("use");
				case AST_TOKEN_KEYWORD_AS:
					return("as");
				case AST_TOKEN_KEYWORD_DEF:
					return("def");
				case AST_TOKEN_KEYWORD_CLASS:
					return("class");
				case AST_TOKEN_KEYWORD_IF:
					return("if");
				case AST_TOKEN_KEYWORD_ELIF:
					return("elif");
				case AST_TOKEN_KEYWORD_FOR:
					return("for");
				case AST_TOKEN_KEYWORD_WHILE:
					return("while");
				case AST_TOKEN_KEYWORD_RAISE:
					return("raise");
				case AST_TOKEN_KEYWORD_ASSERT:
					return("assert");
				case AST_TOKEN_KEYWORD_DELETE:
					return("delete");
				case AST_TOKEN_KEYWORD_RETURN:
					return("return");
				case AST_TOKEN_KEYWORD_DEBUGGER:
					return("debugger");
			}
		case AST_TOKEN_TYPE_MODIFIER:
			switch ((unsigned char)(uintptr_t)t.v){
				default:
				case AST_TOKEN_MODIFIER_UNKNOWN:
					return("AST_TOKEN_MODIFIER_UNKNOWN");
				case AST_TOKEN_MODIFIER_PUBLIC:
					return("public");
				case AST_TOKEN_MODIFIER_PRIVATE:
					return("private");
				case AST_TOKEN_MODIFIER_STATIC:
					return("static");
				case AST_TOKEN_MODIFIER_EXPORT:
					return("export");
				case AST_TOKEN_MODIFIER_FROZEN:
					return("frozen");
				case AST_TOKEN_MODIFIER_FROZENTYPE:
					return("frozentype");
			}
		case AST_TOKEN_TYPE_OPERATOR:
			{
				if ((unsigned char)(uintptr_t)t.v==AST_TOKEN_MODIFIER_UNKNOWN){
					return("AST_TOKEN_MODIFIER_UNKNOWN");
				}
				char* o=KlMem_malloc(2);
				switch ((unsigned char)(uintptr_t)t.v){
					case AST_TOKEN_OPERATOR_UNKNOWN:
						return("AST_TOKEN_MODIFIER_UNKNOWN");
					case AST_TOKEN_OPERATOR_AMP:
						*o='&';
						break;
					case AST_TOKEN_OPERATOR_AT:
						*o='@';
						break;
					case AST_TOKEN_OPERATOR_CIRCUMFLEX:
						*o='^';
						break;
					case AST_TOKEN_OPERATOR_CLOSE_BRACKET:
						*o=']';
						break;
					case AST_TOKEN_OPERATOR_COLON:
						*o=':';
						break;
					case AST_TOKEN_OPERATOR_COMMA:
						*o=',';
						break;
					case AST_TOKEN_OPERATOR_EQUALS:
						*o='=';
						break;
					case AST_TOKEN_OPERATOR_EXCLAMATION_MARK:
						*o='!';
						break;
					case AST_TOKEN_OPERATOR_GRATER:
						*o='>';
						break;
					case AST_TOKEN_OPERATOR_LEFT_PARENTHESIS:
						*o='(';
						break;
					case AST_TOKEN_OPERATOR_LESS:
						*o='<';
						break;
					case AST_TOKEN_OPERATOR_MINUS:
						*o='-';
						break;
					case AST_TOKEN_OPERATOR_MODULO:
						*o='%';
						break;
					case AST_TOKEN_OPERATOR_OPEN_BRACKET:
						*o='[';
						break;
					case AST_TOKEN_OPERATOR_PERIOD:
						*o='.';
						break;
					case AST_TOKEN_OPERATOR_PLUS:
						*o='+';
						break;
					case AST_TOKEN_OPERATOR_QUESTION_MARK:
						*o='?';
						break;
					case AST_TOKEN_OPERATOR_RIGHT_PARENTHESIS:
						*o=')';
						break;
					case AST_TOKEN_OPERATOR_SEMICOLON:
						*o=';';
						break;
					case AST_TOKEN_OPERATOR_SLASH:
						*o='/';
						break;
					case AST_TOKEN_OPERATOR_STAR:
						*o='*';
						break;
					case AST_TOKEN_OPERATOR_TILDA:
						*o='~';
						break;
					case AST_TOKEN_OPERATOR_VERTICAL_BAR:
						*o='|';
						break;
				}
				*(o+1)=0;
				return(o);
			}
		case AST_TOKEN_TYPE_WHITESPACE:
			return("<WHITESPACE>");
		case AST_TOKEN_TYPE_ERROR:
			return("<ERROR>");
		case AST_TOKEN_TYPE_END_OF_DATA:
			return("<EOF>");
	}
}



unsigned char KlAst_get_precedence(unsigned char op){
	KlMem_enter_func();
	switch (op){
		default:
		case AST_EXPRESSION_TYPE_UNKNOWN:
			KlError_unimplemented_error();
			return(UCHAR_MAX);
		case AST_EXPRESSION_TYPE_INC:
		case AST_EXPRESSION_TYPE_DEC:
		case AST_EXPRESSION_TYPE_CALL:
		case AST_EXPRESSION_TYPE_SLC:
		case AST_EXPRESSION_TYPE_ACS:
			return(1);
		case AST_EXPRESSION_TYPE_PLUS:
		case AST_EXPRESSION_TYPE_MINUS:
		case AST_EXPRESSION_TYPE_NOT:
		case AST_EXPRESSION_TYPE_BNOT:
			return(2);
		case AST_EXPRESSION_TYPE_POW:
		case AST_EXPRESSION_TYPE_ROOT:
		case AST_EXPRESSION_TYPE_IROOT:
			return(3);
		case AST_EXPRESSION_TYPE_MLT:
		case AST_EXPRESSION_TYPE_DIV:
		case AST_EXPRESSION_TYPE_FDIV:
		case AST_EXPRESSION_TYPE_MOD:
		case AST_EXPRESSION_TYPE_MMLT:
			return(4);
		case AST_EXPRESSION_TYPE_ADD:
		case AST_EXPRESSION_TYPE_SUB:
			return(5);
		case AST_EXPRESSION_TYPE_LSH:
		case AST_EXPRESSION_TYPE_RSH:
		case AST_EXPRESSION_TYPE_BAND:
		case AST_EXPRESSION_TYPE_BXOR:
		case AST_EXPRESSION_TYPE_BOR:
			return(6);
		case AST_EXPRESSION_TYPE_LT:
		case AST_EXPRESSION_TYPE_LE:
		case AST_EXPRESSION_TYPE_GT:
		case AST_EXPRESSION_TYPE_GE:
		case AST_EXPRESSION_TYPE_EQ:
		case AST_EXPRESSION_TYPE_NE:
			return(7);
		case AST_EXPRESSION_TYPE_AND:
		case AST_EXPRESSION_TYPE_OR:
			return(8);
		case AST_EXPRESSION_TYPE_TCOND:
			return(9);
		case AST_EXPRESSION_TYPE_EQU:
			return(10);
		case AST_EXPRESSION_TYPE_ADD_EQU:
		case AST_EXPRESSION_TYPE_SUB_EQU:
		case AST_EXPRESSION_TYPE_MLT_EQU:
		case AST_EXPRESSION_TYPE_DIV_EQU:
		case AST_EXPRESSION_TYPE_FDIV_EQU:
		case AST_EXPRESSION_TYPE_MOD_EQU:
		case AST_EXPRESSION_TYPE_MMLT_EQU:
		case AST_EXPRESSION_TYPE_POW_EQU:
		case AST_EXPRESSION_TYPE_ROOT_EQU:
		case AST_EXPRESSION_TYPE_IROOT_EQU:
		case AST_EXPRESSION_TYPE_LSH_EQU:
		case AST_EXPRESSION_TYPE_RSH_EQU:
		case AST_EXPRESSION_TYPE_BAND_EQU:
		case AST_EXPRESSION_TYPE_BXOR_EQU:
		case AST_EXPRESSION_TYPE_BOR_EQU:
		case AST_EXPRESSION_TYPE_BNOT_EQU:
			return(11);
		case AST_EXPRESSION_TYPE_SEP:
			return(12);
	}
}



unsigned char KlAst_get_op_count(unsigned char op){
	KlMem_enter_func();
	switch (op){
		default:
		case AST_EXPRESSION_TYPE_UNKNOWN:
			KlError_unimplemented_error();
			return(UCHAR_MAX);
		case AST_EXPRESSION_TYPE_INC:
		case AST_EXPRESSION_TYPE_DEC:
		case AST_EXPRESSION_TYPE_PLUS:
		case AST_EXPRESSION_TYPE_MINUS:
		case AST_EXPRESSION_TYPE_NOT:
		case AST_EXPRESSION_TYPE_BNOT:
			return(1);
		case AST_EXPRESSION_TYPE_CALL:
		case AST_EXPRESSION_TYPE_SLC:
		case AST_EXPRESSION_TYPE_ACS:
			KlError_unimplemented_code();
			return(UCHAR_MAX);
		case AST_EXPRESSION_TYPE_POW:
		case AST_EXPRESSION_TYPE_ROOT:
		case AST_EXPRESSION_TYPE_IROOT:
		case AST_EXPRESSION_TYPE_MLT:
		case AST_EXPRESSION_TYPE_DIV:
		case AST_EXPRESSION_TYPE_FDIV:
		case AST_EXPRESSION_TYPE_MOD:
		case AST_EXPRESSION_TYPE_MMLT:
		case AST_EXPRESSION_TYPE_ADD:
		case AST_EXPRESSION_TYPE_SUB:
		case AST_EXPRESSION_TYPE_LSH:
		case AST_EXPRESSION_TYPE_RSH:
		case AST_EXPRESSION_TYPE_BAND:
		case AST_EXPRESSION_TYPE_BXOR:
		case AST_EXPRESSION_TYPE_BOR:
		case AST_EXPRESSION_TYPE_LT:
		case AST_EXPRESSION_TYPE_LE:
		case AST_EXPRESSION_TYPE_GT:
		case AST_EXPRESSION_TYPE_GE:
		case AST_EXPRESSION_TYPE_EQ:
		case AST_EXPRESSION_TYPE_NE:
		case AST_EXPRESSION_TYPE_AND:
		case AST_EXPRESSION_TYPE_OR:
		case AST_EXPRESSION_TYPE_EQU:
		case AST_EXPRESSION_TYPE_ADD_EQU:
		case AST_EXPRESSION_TYPE_SUB_EQU:
		case AST_EXPRESSION_TYPE_MLT_EQU:
		case AST_EXPRESSION_TYPE_DIV_EQU:
		case AST_EXPRESSION_TYPE_FDIV_EQU:
		case AST_EXPRESSION_TYPE_MOD_EQU:
		case AST_EXPRESSION_TYPE_MMLT_EQU:
		case AST_EXPRESSION_TYPE_POW_EQU:
		case AST_EXPRESSION_TYPE_ROOT_EQU:
		case AST_EXPRESSION_TYPE_IROOT_EQU:
		case AST_EXPRESSION_TYPE_LSH_EQU:
		case AST_EXPRESSION_TYPE_RSH_EQU:
		case AST_EXPRESSION_TYPE_BAND_EQU:
		case AST_EXPRESSION_TYPE_BXOR_EQU:
		case AST_EXPRESSION_TYPE_BOR_EQU:
		case AST_EXPRESSION_TYPE_BNOT_EQU:
			return(2);
		case AST_EXPRESSION_TYPE_TCOND:
			return(3);
		case AST_EXPRESSION_TYPE_SEP:
			KlError_unimplemented_code();
			return(UCHAR_MAX);
	}
}



unsigned char KlAst_get_decl(char* nm,struct Scope* sc){
	KlMem_enter_func();
	size_t ln=str_len(nm);
	while (true){
		if (sc->k!=NULL){
			for (size_t i=0;i<sc->l;i++){
				if (str_cmp(*(sc->k+i),nm,0,ln)==true){
					return(*(sc->m+i));
				}
			}
		}
		if (sc->p==NULL){
			return(UCHAR_MAX);
		}
		sc=sc->p;
	}
}



void KlAst_add_expression(struct UnoptimisedASTObject* o,struct ASTExpression e){
	KlMem_enter_func();
	struct UnoptimisedASTObjectElem el;
	el.t=AST_OBJECT_ELEM_TYPE_EXPRESSION;
	el.v.e=e;
	o->l++;
	o->e=KlMem_realloc(o->e,sizeof(struct UnoptimisedASTObjectElem)*o->l);
	KlMem_ret(o->e);
	*(o->e+o->l-1)=el;
	return();
}



void KlAst_add_expression_unparsed(struct UnparsedASTExpression* o,struct UnparsedASTExpressionElem e){
	KlMem_enter_func();
	o->l++;
	o->e=KlMem_realloc(o->e,sizeof(struct UnparsedASTExpressionElem)*o->l);
	KlMem_ret(o->e);
	*(o->e+o->l-1)=e;
	return();
}

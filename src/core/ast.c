#include <ast.h>
#include <decimal.h>
#include <error.h>
#include <import.h>
#include <bigint.h>
#include <object.h>
#include <memory.h>
#include <shared.h>
#include <free.h>
#include <io.h>
#include <string_utils.h>
#include <stdarg.h>
#include <limits.h>



#define CMP_STR_RET(s,l,g,v_) \
	else if (str_cmp_sub(fo->dt,s,i,l)==true&&(*(fo->dt+i+l)<=47||(*(fo->dt+i+l)>=58&&*(fo->dt+i+l)<=64)||(*(fo->dt+i+l)>=91&&*(fo->dt+i+l)!=95&&*(fo->dt+i+l)<=96)||(*(fo->dt+i+l)>=123&&*(fo->dt+i+l)<=126))){ \
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
#define print_ast_expr(x) KlDebug_print_ast_expr(x,0,NULL)
#define print_ast_scope(x)
// #define print_ast_scope(x) KlDebug_print_ast_scope(x,0,NULL)
#define print_unparsed_ast_expr(x) KlDebug_print_unparsed_ast_expr(x,0,NULL)
#else
#include <debug_utils.h>
#define print_token(x)
#define print_ast_expr(x)
#define print_ast_scope(x) KlDebug_print_ast_scope(x,0,NULL)
#define print_unparsed_ast_expr(x)
#endif



struct ASTScope* KlAst_parse_ast_all(struct CodeFileObject* fo,struct CallStack* cs){
	KlMem_enter_func();
	size_t z=0;
	struct ASTScope o={
		AST_SCOPE_TYPE_DEFAULT,
		NULL,
		NULL,
		NULL,
		0,
		NULL,
		0,
		NULL,
		0,
		NULL,
		NULL,
		NULL,
		0,
		0,
		NULL,
		0
	};
	struct ASTScope* op=KlMem_const(&o,sizeof(struct ASTScope));
	if (KlAst_parse_ast(fo,&z,cs,op,false)==true){
		return(NULL);
	}
	KlMem_ret(op);
	return(op);
}



bool KlAst_parse_ast(struct CodeFileObject* fo,size_t* off,struct CallStack* cs,struct ASTScope* o,bool be){
	KlMem_enter_func();
	struct ASTToken t=KlAst_next_token(fo,*off,cs);
	struct ModifierData md={
		0,
		0,
		0
	};
	while (t.t!=AST_TOKEN_TYPE_END_OF_DATA&&t.t!=AST_TOKEN_TYPE_ERROR){
		if ((t.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)t.v==AST_TOKEN_OPERATOR_SEMICOLON)||t.t==AST_TOKEN_TYPE_WHITESPACE){
			t=KlAst_next_token(fo,KlFree_free_token(t),cs);
			continue;
		}
		if (t.t==AST_TOKEN_TYPE_UNKNOWN){
			KlError_set_error("UnexpectedCharacterError",str_format("Unexpected Character '%c'.",t.v),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t.i),SIZE_MAX,t.i,SIZE_MAX,NULL));
			return(true);
		}
		if (t.t==AST_TOKEN_TYPE_MODIFIER){
			md=KlAst_parse_modifiers(fo,&t,cs);
			if (md.m==UCHAR_MAX){
				return(true);
			}
			continue;
		}
		if (be==true&&t.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)t.v==AST_TOKEN_OPERATOR_RIGHT_BRACES){
			*off=KlFree_free_token(t);
			print_ast_scope(o);
			return(false);
		}
		if (t.t==AST_TOKEN_TYPE_KEYWORD){
			switch ((unsigned char)(uintptr_t)t.v){
				default:
				case AST_TOKEN_KEYWORD_UNKNOWN:
					KlError_unimplemented_error();
					return(true);
				case AST_TOKEN_KEYWORD_FROM:
					{
						struct ASTToken m_nm=KlAst_next_token(fo,KlFree_free_token(t),cs);
						if (m_nm.t==AST_TOKEN_TYPE_WHITESPACE){
							m_nm=KlAst_next_token(fo,KlFree_free_token(m_nm),cs);
						}
						if (m_nm.t!=AST_TOKEN_TYPE_IDENTIFIER){
							KlError_unimplemented_error();
							return(true);
						}
						char* m_fp=KlImport_find_module(fo,m_nm.v,cs);
						if (m_fp==NULL){
							return(true);
						}
						struct ASTModule* m=KlImport_load_module(fo,m_fp,KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t.i),SIZE_MAX,t.i,SIZE_MAX,"<import>"));
						KlMem_free(m->v_nm);
						m->v_nm=NULL;
						KlMem_free(m_fp);
						if (m==NULL){
							return(true);
						}
						struct ASTToken nt=KlAst_next_token(fo,KlFree_free_token(m_nm),cs);
						struct ASTToken m_e_nm;
						while (true){
							if (nt.t==AST_TOKEN_TYPE_WHITESPACE){
								nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
							}
							if (nt.t!=AST_TOKEN_TYPE_KEYWORD||(unsigned char)(uintptr_t)nt.v!=AST_TOKEN_KEYWORD_USE){
								KlError_unimplemented_error();
								return(true);
							}
							m_e_nm=KlAst_next_token(fo,nt.i,cs);
							if (m_e_nm.t==AST_TOKEN_TYPE_WHITESPACE){
								m_e_nm=KlAst_next_token(fo,KlFree_free_token(m_e_nm),cs);
							}
							if (m_e_nm.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)m_e_nm.v==AST_TOKEN_OPERATOR_STAR){
								m_e_nm=KlAst_next_token(fo,KlFree_free_token(m_e_nm),cs);
								if (m_e_nm.t==AST_TOKEN_TYPE_WHITESPACE){
									m_e_nm=KlAst_next_token(fo,KlFree_free_token(m_e_nm),cs);
								}
								if (m_e_nm.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)m_e_nm.v==AST_TOKEN_OPERATOR_COMMA){
									KlError_unimplemented_error();
									return(true);
								}
								else if (m_e_nm.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)m_e_nm.v==AST_TOKEN_OPERATOR_SEMICOLON){
									m->mf|=OBJECT_MODIFIER_EXPORT_ALL;
									KlFree_free_token(nt);
									nt=KlAst_next_token(fo,KlFree_free_token(m_e_nm),cs);
									break;
								}
								KlError_unimplemented_error();
								return(true);
							}
							else if (m_e_nm.t==AST_TOKEN_TYPE_IDENTIFIER){
								KlFree_free_token(nt);
								nt=KlAst_next_token(fo,m_e_nm.i,cs);
								if (nt.t==AST_TOKEN_TYPE_WHITESPACE){
									nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
								}
								char* m_v_nm;
								if (nt.t==AST_TOKEN_TYPE_KEYWORD&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_KEYWORD_AS){
									nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
									if (nt.t==AST_TOKEN_TYPE_WHITESPACE){
										nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
									}
									if (nt.t!=AST_TOKEN_TYPE_IDENTIFIER){
										KlError_unimplemented_error();
										return(true);
									}
									char* v_nm=str_clone(nt.v);
									nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
									if (nt.t==AST_TOKEN_TYPE_WHITESPACE){
										nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
									}
									m_v_nm=v_nm;
								}
								else{
									m_v_nm=str_clone(m_e_nm.v);
								}
								size_t sln=str_len(m_e_nm.v);
								bool f=false;
								for (size_t i=0;i<m->fl;i++){
									if (str_cmp_sub(m_e_nm.v,*(m->f+i),0,sln)==true){
										*(m->fnm+i)=m_v_nm;
										KlMem_ret(m_v_nm);
										f=true;
										break;
									}
								}
								if (f==false){
									for (size_t i=0;i<m->vl;i++){
										if (str_cmp_sub(m_e_nm.v,*(m->v+i),0,sln)==true){
											*(m->vnm+i)=m_v_nm;
											KlMem_ret(m_v_nm);
											f=true;
											break;
										}
									}
								}
								if (f==false){
									assert_msg(0,str_format("NOT_FOUND: %s\n",m_e_nm.v));
									KlError_unimplemented_error();
									return(true);
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
								return(true);
							}
							KlError_unimplemented_error();
							return(true);
						}
						o->ml++;
						o->m=KlMem_realloc(o->m,o->ml*sizeof(struct ASTScope*));
						KlMem_ret(o->m);
						*(o->m+o->ml-1)=m;
						t.i=nt.i;
						KlFree_free_token(nt);
						break;
					}
				case AST_TOKEN_KEYWORD_USE:
					{
						struct ASTToken m_nm=KlAst_next_token(fo,KlFree_free_token(t),cs);
						if (m_nm.t!=AST_TOKEN_TYPE_WHITESPACE){
							KlError_unimplemented_error();
							return(true);
						}
						m_nm=KlAst_next_token(fo,KlFree_free_token(m_nm),cs);
						if (m_nm.t!=AST_TOKEN_TYPE_IDENTIFIER){
							KlError_unimplemented_error();
							return(true);
						}
						struct ASTToken nt=KlAst_next_token(fo,m_nm.i,cs);
						if (nt.t==AST_TOKEN_TYPE_WHITESPACE){
							nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
						}
						char* m_v_nm;
						if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_SEMICOLON){
							m_v_nm=m_nm.v;
						}
						else if (nt.t==AST_TOKEN_TYPE_KEYWORD&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_KEYWORD_AS){
							nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
							if (nt.t==AST_TOKEN_TYPE_WHITESPACE){
								nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
							}
							if (nt.t!=AST_TOKEN_TYPE_IDENTIFIER){
								KlError_unimplemented_error();
								return(true);
							}
							m_v_nm=nt.v;
						}
						else{
							KlError_unimplemented_error();
							return(true);
						}
						char* m_fp=KlImport_find_module(fo,m_nm.v,cs);
						if (m_fp==NULL){
							return(true);
						}
						struct ASTModule* m=KlImport_load_module(fo,m_fp,KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t.i),KlError_offset_to_line(fo->dt,nt.i),t.i,nt.i,"<import>"));
						KlMem_free(m_fp);
						if (m==NULL){
							return(true);
						}
						if (m->v_nm!=NULL){
							KlMem_free(m->v_nm);
						}
						m->v_nm=str_clone(m_v_nm);
						o->ml++;
						o->m=KlMem_realloc(o->m,o->ml*sizeof(struct ASTModule*));
						KlMem_ret(o->m);
						*(o->m+o->ml-1)=m;
						t.i=nt.i;
						KlFree_free_token(m_nm);
						KlFree_free_token(nt);
						break;
					}
				case AST_TOKEN_KEYWORD_AS:
					if (md.m!=0){
						KlError_set_error("UnexpectedKeywordError",str_format("Expected a func/class/var declaration, found 'as'."),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t.i),SIZE_MAX,t.i-2,t.i-1,NULL));
						return(true);
					}
					else{
						KlError_set_error("UnexpectedKeywordError","Unexpected Keyword 'as'.",KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t.i),SIZE_MAX,t.i-2,t.i-1,NULL));
					}
					return(true);
				case AST_TOKEN_KEYWORD_DEF:
					KlError_unimplemented_code();
					return(true);
				case AST_TOKEN_KEYWORD_CLASS:
					t=KlAst_next_token(fo,KlFree_free_token(t),cs);
					if (t.t!=AST_TOKEN_TYPE_WHITESPACE){
						KlError_unimplemented_error();
						return(true);
					}
					t=KlAst_next_token(fo,KlFree_free_token(t),cs);
					if (t.t!=AST_TOKEN_TYPE_IDENTIFIER){
						KlError_unimplemented_error();
						return(true);
					}
					char* nm=str_clone(t.v);
					t=KlAst_next_token(fo,KlFree_free_token(t),cs);
					if (t.t==AST_TOKEN_TYPE_WHITESPACE){
						t=KlAst_next_token(fo,KlFree_free_token(t),cs);
					}
					if (t.t!=AST_TOKEN_TYPE_OPERATOR||(unsigned char)(uintptr_t)t.v!=AST_TOKEN_OPERATOR_LEFT_BRACES){
						KlError_unimplemented_error();
						return(true);
					}
					t=KlAst_next_token(fo,KlFree_free_token(t),cs);
					struct ModifierData fm={
						0,
						0,
						0
					};
					struct ASTScope c_sc={
						AST_SCOPE_TYPE_CLASS,
						o,
						nm,
						NULL,
						0,
						NULL,
						0,
						NULL,
						0,
						NULL,
						NULL,
						NULL,
						0,
						md.m,
						NULL,
						0
					};
					struct ASTScope* c_scp=KlMem_const(&c_sc,sizeof(struct ASTScope));
					while (t.t!=AST_TOKEN_TYPE_OPERATOR||(unsigned char)(uintptr_t)t.v!=AST_TOKEN_OPERATOR_RIGHT_BRACES){
						if (t.t==AST_TOKEN_TYPE_WHITESPACE){
							t=KlAst_next_token(fo,KlFree_free_token(t),cs);
							continue;
						}
						if (t.t==AST_TOKEN_TYPE_MODIFIER){
							fm=KlAst_parse_modifiers(fo,&t,cs);
							if (fm.m==UINT16_MAX){
								return(true);
							}
							if ((fm.m&OBJECT_MODIFIER_PRIVATE)==0){
								fm.m|=OBJECT_MODIFIER_PUBLIC;
							}
							if ((fm.m&OBJECT_MODIFIER_EXPORT)!=0){
								KlError_unimplemented_error();
								return(true);
							}
							continue;
						}
						if (t.t==AST_TOKEN_TYPE_IDENTIFIER){
							char* i_nm=str_clone(t.v);
							t=KlAst_next_token(fo,KlFree_free_token(t),cs);
							if (t.t==AST_TOKEN_TYPE_WHITESPACE){
								t=KlAst_next_token(fo,KlFree_free_token(t),cs);
							}
							if (t.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)t.v==AST_TOKEN_OPERATOR_SEMICOLON){
								if ((fm.m&OBJECT_MODIFIER_OPERATOR)!=0){
									KlError_unimplemented_error();
									return(true);
								}
								c_scp->vl++;
								c_scp->vnm=KlMem_realloc(c_scp->vnm,c_scp->vl*sizeof(char*));
								c_scp->vm=KlMem_realloc(c_scp->vm,c_scp->vl*sizeof(uint16_t));
								c_scp->vrc=KlMem_realloc(c_scp->vrc,c_scp->vl*sizeof(size_t));
								*(c_scp->vnm+c_scp->vl-1)=i_nm;
								*(c_scp->vm+c_scp->vl-1)=fm.m;
								*(c_scp->vrc+c_scp->vl-1)=0;
							}
							else if (t.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)t.v==AST_TOKEN_OPERATOR_LEFT_PARENTHESIS){
								if ((fm.m&OBJECT_MODIFIER_CONST)!=0){
									KlError_unimplemented_error();
									return(true);
								}
								char** nm=NULL;
								bool v=false;
								size_t l=0;
								t=KlAst_next_token(fo,KlFree_free_token(t),cs);
								while (true){
									if (t.t==AST_TOKEN_TYPE_WHITESPACE){
										t=KlAst_next_token(fo,KlFree_free_token(t),cs);
									}
									if (t.t==AST_TOKEN_TYPE_IDENTIFIER){
										l++;
										nm=KlMem_realloc(nm,l*sizeof(char*));
										*(nm+l-1)=str_clone(t.v);
										t=KlAst_next_token(fo,KlFree_free_token(t),cs);
										if (t.t==AST_TOKEN_TYPE_WHITESPACE){
											t=KlAst_next_token(fo,KlFree_free_token(t),cs);
										}
										if (t.t!=AST_TOKEN_TYPE_OPERATOR){
											KlError_unimplemented_error();
											return(true);
										}
										if ((unsigned char)(uintptr_t)t.v==AST_TOKEN_OPERATOR_COMMA){
											t=KlAst_next_token(fo,KlFree_free_token(t),cs);
											continue;
										}
										if ((unsigned char)(uintptr_t)t.v==AST_TOKEN_OPERATOR_RIGHT_PARENTHESIS){
											break;
										}
										KlError_unimplemented_error();
										return(true);
									}
									if (t.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)t.v==AST_TOKEN_OPERATOR_PERIOD){
										struct ASTToken nt=KlAst_next_token(fo,t.i,cs);
										if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_PERIOD){
											nt=KlAst_next_token(fo,KlFree_free_token(nt),cs);
											if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_PERIOD){
												v=true;
												KlFree_free_token(t);
												t=KlAst_next_token(fo,KlFree_free_token(nt),cs);
												continue;
											}
											else{
												KlFree_free_token(nt);
											}
										}
										else{
											KlFree_free_token(nt);
										}
									}
									KlError_unimplemented_error();
									return(true);
								}
								t=KlAst_next_token(fo,KlFree_free_token(t),cs);
								if (t.t==AST_TOKEN_TYPE_WHITESPACE){
									t=KlAst_next_token(fo,KlFree_free_token(t),cs);
								}
								if (t.t!=AST_TOKEN_TYPE_OPERATOR||(unsigned char)(uintptr_t)t.v!=AST_TOKEN_OPERATOR_LEFT_BRACES){
									KlError_unimplemented_error();
									return(true);
								}
								size_t ti=KlFree_free_token(t);
								struct ASTScope fb={
									AST_SCOPE_TYPE_FUNCTION,
									c_scp,
									i_nm,
									NULL,
									0,
									NULL,
									0,
									NULL,
									0,
									NULL,
									NULL,
									NULL,
									0,
									fm.m|(v==true?OBJECT_MODIFIER_VARARG:0),
									nm,
									l
								};
								struct ASTScope* fbp=KlMem_const(&fb,sizeof(struct ASTScope));
								if (KlAst_parse_ast(fo,&ti,cs,fbp,true)==true){
									return(true);
								}
								t.i=ti;
								c_scp->fl++;
								c_scp->f=KlMem_realloc(c_scp->f,c_scp->fl*sizeof(struct ASTScopeFunc*));
								*(c_scp->f+c_scp->fl-1)=fbp;
								KlMem_ret(c_scp->f);
								KlMem_ret(fbp);
							}
							else{
								KlError_unimplemented_error();
								return(true);
							}
							fm.m=0;
							fm.s=0;
							fm.e=0;
							t=KlAst_next_token(fo,KlFree_free_token(t),cs);
							continue;
						}
						KlError_unimplemented_error();
						return(true);
					}
					print_ast_scope(c_scp);
					o->cl++;
					o->c=KlMem_realloc(o->c,o->cl*sizeof(struct ASTExpression*));
					KlMem_ret(o->c);
					*(o->c+o->cl-1)=KlAst_gen_expression("(iS)",AST_EXPRESSION_TYPE_EQU,c_scp->nm,c_scp);
					o->vl++;
					o->vnm=KlMem_realloc(o->vnm,o->vl*sizeof(char*));
					o->vm=KlMem_realloc(o->vm,o->vl*sizeof(uint16_t));
					o->vrc=KlMem_realloc(o->vrc,o->vl*sizeof(size_t));
					KlMem_ret(o->vnm);
					KlMem_ret(o->vm);
					KlMem_ret(o->vrc);
					*(o->vnm+o->vl-1)=str_clone(c_scp->nm);
					*(o->vm+o->vl-1)=md.m|OBJECT_MODIFIER_ASSIGNED;
					*(o->vrc+o->vl-1)=1;
					print_ast_scope(o);
					break;
				case AST_TOKEN_KEYWORD_IF:
					KlError_unimplemented_code();
					return(true);
				case AST_TOKEN_KEYWORD_ELIF:
					KlError_unimplemented_code();
					return(true);
				case AST_TOKEN_KEYWORD_ELSE:
					KlError_unimplemented_code();
					return(true);
				case AST_TOKEN_KEYWORD_FOR:
					KlError_unimplemented_code();
					return(true);
				case AST_TOKEN_KEYWORD_WHILE:
					KlError_unimplemented_code();
					return(true);
				case AST_TOKEN_KEYWORD_RAISE:
					KlError_unimplemented_code();
					return(true);
				case AST_TOKEN_KEYWORD_ASSERT:
					KlError_unimplemented_code();
					return(true);
				case AST_TOKEN_KEYWORD_DELETE:
					KlError_unimplemented_code();
					return(true);
				case AST_TOKEN_KEYWORD_RETURN:
					KlError_unimplemented_code();
					return(true);
				case AST_TOKEN_KEYWORD_DEBUGGER:
					KlError_unimplemented_code();
					return(true);
			}
			t=KlAst_next_token(fo,KlFree_free_token(t),cs);
			continue;
		}
		struct UnparsedASTExpression* uex=KlAst_read_expression(fo,&t,cs,true,AST_TOKEN_END_SEMICOLON);
		if (uex==NULL){
			return(true);
		}
		struct ASTExpression* ex=KlAst_parse_unparsed_expression(fo,uex,cs,o);
		KlFree_free_unparsed_expression(*uex);
		KlMem_free(uex);
		if (ex==NULL){
			return(true);
		}
		if (KlAst_process_expression(fo,ex,cs,o,md)==true){
			return(true);
		}
		md.m=0;
		md.s=0;
		md.e=0;
		if (ex->t!=AST_EXPRESSION_TYPE_EMPTY){
			o->cl++;
			o->c=KlMem_realloc(o->c,o->cl*sizeof(struct ASTExpression*));
			KlMem_ret(o->c);
			*(o->c+o->cl-1)=ex;
			continue;
		}
		KlMem_free(ex);
		char* s=KlAst_token_to_string(t);
		KlError_set_error("UnexpectedTokenError",str_format("Unexpected Token '%s'.",s),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t.i),SIZE_MAX,t.i-str_len(s),t.i-1,NULL));
		return(true);
	}
	if (t.t==AST_TOKEN_TYPE_ERROR){
		KlFree_free_token(t);
		return(true);
	}
	*off=KlFree_free_token(t);
	print_ast_scope(o);
	return(false);
}



struct ModifierData KlAst_parse_modifiers(struct CodeFileObject* fo,struct ASTToken* t,struct CallStack* cs){
	KlMem_enter_func();
	struct ModifierData o;
	o.m=0;
	o.s=0;
	o.e=0;
	while (t->t==AST_TOKEN_TYPE_MODIFIER){
		if (t->v!=0){
			if (o.m==0){
				o.s=t->i-((unsigned char)(uintptr_t)t->v==OBJECT_MODIFIER_PRIVATE?7:((unsigned char)(uintptr_t)t->v==OBJECT_MODIFIER_FROZENTYPE?10:((unsigned char)(uintptr_t)t->v==OBJECT_MODIFIER_CONST?5:((unsigned char)(uintptr_t)t->v==OBJECT_MODIFIER_OPERATOR?8:6))));
			}
			o.e=t->i-1;
			o.m|=(uint16_t)(uintptr_t)t->v;
		}
		*t=KlAst_next_token(fo,KlFree_free_token_p(t),cs);
		if (t->t!=AST_TOKEN_TYPE_WHITESPACE){
			KlError_unimplemented_error();
			o.m=UINT16_MAX;
			return(o);
		}
		*t=KlAst_next_token(fo,KlFree_free_token_p(t),cs);
	}
	if ((o.m&OBJECT_MODIFIER_PUBLIC)!=0&&(o.m&OBJECT_MODIFIER_PRIVATE)!=0){
		KlError_set_error("UncompatibleModifierError","Modifier 'public' isn't Compatible with Modifier 'private'.",KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t->i),SIZE_MAX,o.s,o.e,NULL));
		o.m=UINT16_MAX;
		return(o);
	}
	if ((o.m&OBJECT_MODIFIER_CONST)!=0&&(o.m&OBJECT_MODIFIER_FROZEN)!=0){
		KlError_set_error("UncompatibleModifierError","Modifier 'const' already Implies Modifier 'frozen'.",KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t->i),SIZE_MAX,o.s,o.e,NULL));
		o.m=UINT16_MAX;
		return(o);
	}
	if ((o.m&OBJECT_MODIFIER_CONST)!=0&&(o.m&OBJECT_MODIFIER_FROZENTYPE)!=0){
		KlError_set_error("UncompatibleModifierError","Modifier 'const' already Implies Modifier 'frozentype'.",KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t->i),SIZE_MAX,o.s,o.e,NULL));
		o.m=UINT16_MAX;
		return(o);
	}
	if ((o.m&OBJECT_MODIFIER_FROZEN)!=0&&(o.m&OBJECT_MODIFIER_FROZENTYPE)!=0){
		KlError_set_error("UncompatibleModifierError","Modifier 'frozen' already Implies Modifier 'frozentype'.",KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t->i),SIZE_MAX,o.s,o.e,NULL));
		o.m=UINT16_MAX;
		return(o);
	}
	return(o);
}



bool KlAst_process_expression(struct CodeFileObject* fo,struct ASTExpression* o,struct CallStack* cs,struct ASTScope* sc,struct ModifierData md){
	KlMem_enter_func();
	if (o->t==AST_EXPRESSION_TYPE_EQU){
		uint16_t m=KlAst_get_decl(o->a.v.i,sc);
		if (m!=UCHAR_MAX&&md.m!=0){
			KlError_unimplemented_error();
			return(true);
		}
		if (o->a.t==AST_EXPRESSION_ARG_TYPE_IDENTIFIER){
			print_ast_expr(o);
			sc->vl++;
			sc->vnm=KlMem_realloc(sc->vnm,sc->vl*sizeof(char*));
			sc->vm=KlMem_realloc(sc->vm,sc->vl*sizeof(uint16_t));
			sc->vrc=KlMem_realloc(sc->vrc,sc->vl*sizeof(size_t));
			KlMem_ret(sc->vnm);
			KlMem_ret(sc->vm);
			KlMem_ret(sc->vrc);
			*(sc->vnm+sc->vl-1)=str_clone(o->a.v.i);
			*(sc->vm+sc->vl-1)=md.m|OBJECT_MODIFIER_ASSIGNED;
			*(sc->vrc+sc->vl-1)=1;
		}
		else{
			KlIo_printf("INC REFCNT OF OBJECT ATTR!\n");
		}
	}
	else if (o->t==AST_EXPRESSION_TYPE_CONST&&o->a.t==AST_EXPRESSION_ARG_TYPE_IDENTIFIER){
		uint16_t m=KlAst_get_decl(o->a.v.i,sc);
		if (m!=UCHAR_MAX&&md.m!=0){
			KlError_unimplemented_error();
			return(true);
		}
		sc->vl++;
		sc->vnm=KlMem_realloc(sc->vnm,sc->vl*sizeof(char*));
		sc->vm=KlMem_realloc(sc->vm,sc->vl*sizeof(uint16_t));
		sc->vrc=KlMem_realloc(sc->vrc,sc->vl*sizeof(size_t));
		KlMem_ret(sc->vnm);
		KlMem_ret(sc->vm);
		KlMem_ret(sc->vrc);
		assert(*o->a.v.i!='\n');
		*(sc->vnm+sc->vl-1)=str_clone(o->a.v.i);
		*(sc->vm+sc->vl-1)=md.m;
		*(sc->vrc+sc->vl-1)=0;
	}
	else if (md.m!=0){
		print_ast_expr(o);
		print_ast_scope(sc);
		KlError_unimplemented_error();
		return(true);
	}
	if (KlAst_optimize_check_ast_expr(fo,o,cs,sc)==true){
		return(true);
	}
	return(false);
}



struct UnparsedASTExpression* KlAst_read_expression(struct CodeFileObject* fo,struct ASTToken* t,struct CallStack* cs,bool e,unsigned char et){
	KlMem_enter_func();
	struct UnparsedASTExpression o={
		NULL,
		0
	};
	assert(et==AST_TOKEN_END_SEMICOLON||et==AST_TOKEN_END_COMMA_OR_PARENTHESIS||et==AST_TOKEN_END_SEMICOLON_OR_PARENTHESIS);
	if (e==false&&t->t==AST_TOKEN_TYPE_OPERATOR&&((et==AST_TOKEN_END_SEMICOLON&&(unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_SEMICOLON)||(et==AST_TOKEN_END_COMMA_OR_PARENTHESIS&&((unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_COMMA||(unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_RIGHT_PARENTHESIS))||(et==AST_TOKEN_END_SEMICOLON_OR_PARENTHESIS&&((unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_SEMICOLON||(unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_RIGHT_PARENTHESIS)))){
		KlError_unimplemented_error();
		return(NULL);
	}
	while (true){
		switch (t->t){
			default:
			case AST_TOKEN_TYPE_UNKNOWN:
				KlError_set_error("UnexpectedCharacterError",str_format("Unexpected Character '%c'.",t->v),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t->i),SIZE_MAX,t->i,SIZE_MAX,NULL));
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
					el.v.n=KlBigInt_assign(NULL,t->v);
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
					print_unparsed_ast_expr(&o);
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
				if ((unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_EQUALS){
					struct ASTToken nt=KlAst_next_token(fo,t->i,cs);
					struct UnparsedASTExpressionElem el;
					el.t=UNPARSED_AST_EXPRESSION_ELEM_TYPE_OPERATOR;
					el.v.op=AST_EXPRESSION_TYPE_EQU;
					if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_EQUALS){
						KlAst_add_expression_unparsed(&o,el);
						*t=nt;
						break;
					}
					KlAst_add_expression_unparsed(&o,el);
					struct UnparsedASTExpressionElem el_p;
					el_p.t=UNPARSED_AST_EXPRESSION_ELEM_TYPE_EXPRESSION;
					el_p.v.ex=KlAst_read_expression(fo,&nt,cs,false,AST_TOKEN_END_SEMICOLON_OR_PARENTHESIS);
					KlAst_add_expression_unparsed(&o,el_p);
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
							struct ASTToken nt=KlAst_next_token(fo,t->i,cs);
							el.v.op=AST_EXPRESSION_TYPE_ACS;
							if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_PERIOD){
								nt=KlAst_next_token(fo,nt.i,cs);
								if (nt.t==AST_TOKEN_TYPE_OPERATOR&&(unsigned char)(uintptr_t)nt.v==AST_TOKEN_OPERATOR_PERIOD){
									el.v.op=AST_EXPRESSION_TYPE_EXPAND;
									KlFree_free_token_p(t);
									*t=nt;
								}
								else{
									KlFree_free_token(nt);
								}
							}
							else{
								KlFree_free_token(nt);
							}
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
							el.v.op=AST_EXPRESSION_TYPE_BNOT;
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
				if (o.l==0&&(el.v.op!=AST_EXPRESSION_TYPE_MINUS&&el.v.op!=AST_EXPRESSION_TYPE_PLUS&&el.v.op!=AST_EXPRESSION_TYPE_EXPAND)){
					KlError_unimplemented_error();
				}
				KlAst_add_expression_unparsed(&o,el);
				break;
			case AST_TOKEN_TYPE_WHITESPACE:
				break;
			case AST_TOKEN_TYPE_ERROR:
				return(NULL);
		}
		if (t->t==AST_TOKEN_TYPE_OPERATOR&&((et==AST_TOKEN_END_SEMICOLON&&(unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_SEMICOLON)||(et==AST_TOKEN_END_COMMA_OR_PARENTHESIS&&((unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_COMMA||(unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_RIGHT_PARENTHESIS))||(et==AST_TOKEN_END_SEMICOLON_OR_PARENTHESIS&&((unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_SEMICOLON||(unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_RIGHT_PARENTHESIS)))){
			break;
		}
		*t=KlAst_next_token(fo,KlFree_free_token_p(t),cs);
		if (t->t==AST_TOKEN_TYPE_UNKNOWN){
			KlError_set_error("UnexpectedCharacterError",str_format("Unexpected Character '%c'.",t->v),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,t->i),SIZE_MAX,t->i,SIZE_MAX,NULL));
			return(NULL);
		}
		if (t->t==AST_TOKEN_TYPE_ERROR){
			return(NULL);
		}
		if (t->t==AST_TOKEN_TYPE_OPERATOR&&((et==AST_TOKEN_END_SEMICOLON&&(unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_SEMICOLON)||(et==AST_TOKEN_END_COMMA_OR_PARENTHESIS&&((unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_COMMA||(unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_RIGHT_PARENTHESIS))||(et==AST_TOKEN_END_SEMICOLON_OR_PARENTHESIS&&((unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_SEMICOLON||(unsigned char)(uintptr_t)t->v==AST_TOKEN_OPERATOR_RIGHT_PARENTHESIS)))){
			break;
		}
	}
	struct UnparsedASTExpression* o_=KlMem_const(&o,sizeof(struct UnparsedASTExpression));
	KlMem_ret(o_);
	return(o_);
}



struct ASTExpression* KlAst_parse_unparsed_expression(struct CodeFileObject* fo,struct UnparsedASTExpression* e,struct CallStack* cs,struct ASTScope* sc){
	KlMem_enter_func();
	print_unparsed_ast_expr(e);
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
						if (i->v.op==AST_EXPRESSION_TYPE_PLUS||i->v.op==AST_EXPRESSION_TYPE_MINUS||i->v.op==AST_EXPRESSION_TYPE_EXPAND){
							c->t=i->v.op;
							break;
						}
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
						if (c->t==AST_EXPRESSION_TYPE_EMPTY){
							c->t=AST_EXPRESSION_TYPE_CONST;
						}
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
						if (c->t==AST_EXPRESSION_TYPE_EMPTY){
							c->t=AST_EXPRESSION_TYPE_CONST;
						}
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
					struct BigInt* nc=NULL;
					if (i->v.n!=NULL){
						nc=KlBigInt_assign(NULL,i->v.n);
					}
					if (*(c_a+c_ai)==0){
						if (c->t==AST_EXPRESSION_TYPE_EMPTY){
							c->t=AST_EXPRESSION_TYPE_CONST;
						}
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
						if (c->t==AST_EXPRESSION_TYPE_EMPTY){
							c->t=AST_EXPRESSION_TYPE_CONST;
						}
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
			case 'S':
				struct ASTScope* sc=va_arg(a,struct ASTScope*);
				if (ol==0){
					o->t=AST_EXPRESSION_TYPE_CONST;
					o->a.t=AST_EXPRESSION_ARG_TYPE_SCOPE;
					o->a.v.sc=sc;
				}
				else{
					if ((o+ol-1)->a.t==AST_EXPRESSION_ARG_TYPE_UNKNOWN){
						(o+ol-1)->a.t=AST_EXPRESSION_ARG_TYPE_SCOPE;
						(o+ol-1)->a.v.sc=sc;
					}
					else{
						struct ASTExpressionArg* ea=KlMem_malloc(sizeof(struct ASTExpressionArg));
						ea->t=AST_EXPRESSION_ARG_TYPE_SCOPE;
						ea->v.sc=sc;
						(o+ol-1)->bl++;
						(o+ol-1)->b=KlMem_realloc((o+ol-1)->b,(o+ol-1)->bl*sizeof(struct ASTExpressionArg));
						KlMem_ret((o+ol-1)->b);
						KlMem_memcpy((o+ol-1)->b+(o+ol-1)->bl-1,ea,sizeof(struct ASTExpressionArg));
						KlMem_free(ea);
					}
				}
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
				struct BigInt* n=va_arg(a,struct BigInt*);
				struct BigInt* nc=NULL;
				if (n!=NULL){
					nc=KlBigInt_assign(NULL,n);
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
				o.a.v.n=KlBigInt_assign(NULL,ex.a.v.n);
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
				case AST_EXPRESSION_ARG_TYPE_SCOPE:
					if ((ex.b+i)->v.sc==NULL){
						(o.b+i)->v.sc=NULL;
					}
					else{
						(o.b+i)->v.sc=KlAst_clone_scope((ex.b+i)->v.sc);
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
						(o.b+i)->v.n=KlBigInt_assign(NULL,(ex.b+i)->v.n);
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
					if ((ex.b+i)->v.o==NULL){
						(o.b+i)->v.o=NULL;
					}
					else{
						(o.b+i)->v.o=NULL;//
						///////
					}
					break;
			}
		}
	}
	return(o);
}



struct ASTScope* KlAst_clone_scope(struct ASTScope* sc){
	KlMem_enter_func();
	if (sc==NULL){
		return(NULL);
	}
	struct ASTScope* o=KlMem_malloc(sizeof(struct ASTScope));
	o->t=sc->t;
	o->p=sc->p;
	o->nm=str_clone(sc->nm);
	o->c=(sc->cl==0?NULL:KlMem_malloc(sc->cl*sizeof(struct ASTExpression*)));
	for (size_t i=0;i<sc->cl;i++){
		KlMem_ret(o->c);
		struct ASTExpression ex=KlAst_clone_expression(**(sc->c+i));
		*(o->c+i)=KlMem_const(&ex,sizeof(struct ASTExpression));
		KlMem_ret(*(o->c+i));
	}
	o->cl=sc->cl;
	o->f=(sc->fl==0?NULL:KlMem_malloc(sc->fl*sizeof(struct ASTExpression*)));
	for (size_t i=0;i<sc->fl;i++){
		KlMem_ret(o->f);
		*(o->f+i)=KlAst_clone_scope(*(sc->f+i));
	}
	o->fl=sc->fl;
	o->m=(sc->ml==0?NULL:KlMem_malloc(sc->ml*sizeof(struct ASTModule*)));
	for (size_t i=0;i<sc->ml;i++){
		KlMem_ret(o->m);
		struct ASTModule* m=KlMem_malloc(sizeof(struct ASTModule));
		m->nm=str_clone((*(sc->m+i))->nm);
		m->v_nm=str_clone((*(sc->m+i))->v_nm);
		m->fp=str_clone((*(sc->m+i))->fp);
		m->f=((*(sc->m+i))->fl==0?NULL:KlMem_malloc((*(sc->m+i))->fl*sizeof(char*)));
		m->fnm=((*(sc->m+i))->fl==0?NULL:KlMem_malloc((*(sc->m+i))->fl*sizeof(char*)));
		for (size_t j=0;j<(*(sc->m+i))->fl;j++){
			KlMem_ret(m->f);
			KlMem_ret(m->fnm);
			*(m->f+j)=str_clone(*((*(sc->m+i))->f+j));
			*(m->fnm+j)=str_clone(*((*(sc->m+i))->fnm+j));
		}
		m->fl=(*(sc->m+i))->fl;
		m->v=((*(sc->m+i))->vl==0?NULL:KlMem_malloc((*(sc->m+i))->vl*sizeof(char*)));
		m->vnm=((*(sc->m+i))->vl==0?NULL:KlMem_malloc((*(sc->m+i))->vl*sizeof(char*)));
		for (size_t j=0;j<(*(sc->m+i))->vl;j++){
			KlMem_ret(m->v);
			KlMem_ret(m->vnm);
			*(m->v+j)=str_clone(*((*(sc->m+i))->v+j));
			*(m->vnm+j)=str_clone(*((*(sc->m+i))->vnm+j));
		}
		m->vl=(*(sc->m+i))->vl;
		m->src=KlAst_clone_scope((*(sc->m+i))->src);
		m->mf=(*(sc->m+i))->mf;
		KlMem_ret(m);
		*(o->m+i)=m;
	}
	o->ml=sc->ml;
	o->vnm=(sc->vl==0?NULL:KlMem_malloc(sc->vl*sizeof(char*)));
	for (size_t i=0;i<sc->vl;i++){
		*(o->vnm+i)=str_clone(*(sc->vnm+i));
	}
	o->vm=(sc->vl==0?NULL:KlMem_memcpy(KlMem_malloc(sc->vl*sizeof(char*)),sc->vm,sc->vl*sizeof(char*)));
	o->vrc=(sc->vl==0?NULL:KlMem_memcpy(KlMem_malloc(sc->vl*sizeof(char*)),sc->vrc,sc->vl*sizeof(char*)));
	if (sc->vl!=0){
		KlMem_ret(o->vnm);
		KlMem_ret(o->vm);
		KlMem_ret(o->vrc);
	}
	o->vl=sc->vl;
	o->md=sc->md;
	o->anm=(sc->al==0?NULL:KlMem_malloc(sc->al*sizeof(char*)));
	for (size_t i=0;i<sc->al;i++){
		KlMem_ret(o->anm);
		*(o->anm+i)=str_clone(*(sc->anm+i));
	}
	o->al=sc->al;
	KlMem_ret(o);
	return(o);
}



bool KlAst_optimize_check_ast_expr(struct CodeFileObject* fo,struct ASTExpression* ex,struct CallStack* cs,struct ASTScope* sc){
	KlMem_enter_func();
	assert(ex!=NULL);
	if (KlAst_optimize_check_ast_expr_arg(fo,&ex->a,cs,sc,true)==true){
		return(true);
	}
	for (size_t i=0;i<ex->bl;i++){
		if (KlAst_optimize_check_ast_expr_arg(fo,ex->b+i,cs,sc,(i==0&&ex->t==AST_EXPRESSION_TYPE_ACS?false:true))==true){
			return(true);
		}
	}
	if (ex->t!=AST_EXPRESSION_TYPE_EMPTY&&ex->t!=AST_EXPRESSION_TYPE_CONST){
		unsigned char op=(ex->t==AST_EXPRESSION_TYPE_TCOND?1:KlAst_get_op_count(ex->t));
		if (op!=UCHAR_MAX){
			assert(ex->bl==op-1);
			if (ex->a.t==AST_EXPRESSION_ARG_TYPE_CHAR||ex->a.t==AST_EXPRESSION_ARG_TYPE_STRING||ex->a.t==AST_EXPRESSION_ARG_TYPE_INT||ex->a.t==AST_EXPRESSION_ARG_TYPE_FLOAT){
				for (size_t i=0;i<ex->bl;i++){
					if (ex->a.t!=AST_EXPRESSION_ARG_TYPE_CHAR&&ex->a.t!=AST_EXPRESSION_ARG_TYPE_STRING&&ex->a.t!=AST_EXPRESSION_ARG_TYPE_INT&&ex->a.t!=AST_EXPRESSION_ARG_TYPE_FLOAT){
						return(false);
					}
				}
				assert(ex->t!=AST_EXPRESSION_TYPE_TCOND);// UNIMPLEMENTED
				if (op==1){
					struct ASTExpressionArg ea=KlObject_eval_unary(ex->t,ex->a);
					KlFree_free_expression(*ex);
					ex->t=AST_EXPRESSION_TYPE_CONST;
					ex->a=ea;
				}
				else if (op==2){
					struct ASTExpressionArg ea=KlObject_eval_binary(ex->t,ex->a,*ex->b);
					KlFree_free_expression(*ex);
					ex->t=AST_EXPRESSION_TYPE_CONST;
					ex->a=ea;
				}
				else{
					KlError_unimplemented_error();
					return(false);
				}
			}
		}
	}
	return(false);
}



bool KlAst_optimize_check_ast_expr_arg(struct CodeFileObject* fo,struct ASTExpressionArg* a,struct CallStack* cs,struct ASTScope* sc,bool ic){
	KlMem_enter_func();
	assert(a!=NULL);
	switch (a->t){
		default:
		case AST_EXPRESSION_ARG_TYPE_UNKNOWN:
			KlError_unimplemented_error();
			return(true);
		case AST_EXPRESSION_ARG_TYPE_EXPRESSION:
			if (KlAst_optimize_check_ast_expr(fo,a->v.ex,cs,sc)==true){
				return(true);
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_SCOPE:
			for (size_t i=0;i<a->v.sc->cl;i++){
				if (KlAst_optimize_check_ast_expr(fo,*(a->v.sc->c+i),cs,a->v.sc)==true){
					return(true);
				}
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_CHAR:
		case AST_EXPRESSION_ARG_TYPE_STRING:
		case AST_EXPRESSION_ARG_TYPE_INT:
		case AST_EXPRESSION_ARG_TYPE_FLOAT:
		case AST_EXPRESSION_ARG_TYPE_FUNCTION:
		case AST_EXPRESSION_ARG_TYPE_NATIVE_FUNCTION:
			break;
		case AST_EXPRESSION_ARG_TYPE_IDENTIFIER:
			if (ic==true){
				size_t rc=KlAst_get_decl_refc(a->v.i,sc);
				if (rc==SIZE_MAX){
					KlError_unimplemented_error();
					return(true);
				}
				if (rc==0){
					KlError_unimplemented_error();
					return(true);
				}
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_MODIFIERS:
			break;
		case AST_EXPRESSION_ARG_TYPE_OBJECT:
			KlError_unimplemented_code();
			break;
	}
	if (a->t==AST_EXPRESSION_ARG_TYPE_EXPRESSION&&a->v.ex->t==AST_EXPRESSION_TYPE_CONST){
		struct ASTExpression* tmp=a->v.ex;
		a->t=tmp->a.t;
		a->v=tmp->a.v;
		KlMem_free(tmp);
	}
	return(false);
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
			KlError_set_error("EmptyCharacterStringError","Empty Character String.",KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i-1),SIZE_MAX,i-1,i,"<char>"));
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
					KlError_set_error("InvalidEscapeSequenceError",str_format("Expected a Any of 'b e f n o r t v x', found '%c'",*(fo->dt+i)),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i-1),SIZE_MAX,i-1,i,"<char>"));
					print_token(o);
					return(o);
				case 2:
					KlError_set_error("InvalidEscapeSequenceError",str_format("Expected a Decimal Digit, found '%c'.",*(fo->dt+i)),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i-1),SIZE_MAX,i,SIZE_MAX,"<char>"));
					print_token(o);
					return(o);
				case 3:
					KlError_set_error("InvalidEscapeSequenceError",str_format("Expected an Octal Digit, found '%c'.",*(fo->dt+i)),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i-1),SIZE_MAX,i,SIZE_MAX,"<char>"));
					print_token(o);
					return(o);
				case 4:
					KlError_set_error("InvalidEscapeSequenceError",str_format("Expected a Hexadecimal Character, found '%c'.",*(fo->dt+i)),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i-1),SIZE_MAX,i,SIZE_MAX,"<char>"));
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
			KlError_set_error("UnmachedQuoteError",str_format("Expected String Quote ('\\''), found '%c'.",*(fo->dt+i)),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i),SIZE_MAX,i,SIZE_MAX,"<char>"));
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
						KlError_set_error("InvalidEscapeSequenceError",str_format("Expected a Any of 'b e f n o r t v x', found '%c'",*(fo->dt+i)),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i-1),SIZE_MAX,i=1,i,"<string>"));
						print_token(o);
						return(o);
					case 2:
						KlError_set_error("InvalidEscapeSequenceError",str_format("Expected a Decimal Digit, found '%c'.",*(fo->dt+i)),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i-1),SIZE_MAX,i,SIZE_MAX,"<string>"));
						print_token(o);
						return(o);
					case 3:
						KlError_set_error("InvalidEscapeSequenceError",str_format("Expected an Octal Digit, found '%c'.",*(fo->dt+i)),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i-1),SIZE_MAX,i,SIZE_MAX,"<string>"));
						print_token(o);
						return(o);
					case 4:
						KlError_set_error("InvalidEscapeSequenceError",str_format("Expected a Hexadecimal Character, found '%c'.",*(fo->dt+i)),KlError_extend_call_stack(cs,fo,KlError_offset_to_line(fo->dt,i-1),SIZE_MAX,i,SIZE_MAX,"<string>"));
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
				o.v=KlBigInt_from_long(0);
				while (*(fo->dt+i)=='0'||*(fo->dt+i)=='1'){
					KlBigInt_add_bin_digit(o.v,*(fo->dt+i)-48);
					i++;
				}
				print_token(o);
				return(o);
			case 'o':
				i+=2;
				o.t=AST_TOKEN_TYPE_INT;
				o.v=KlBigInt_from_long(0);
				while (*(fo->dt+i)>=48&&*(fo->dt+i)<=55){
					KlBigInt_add_oct_digit(o.v,*(fo->dt+i)-48);
					i++;
				}
				print_token(o);
				return(o);
			case 'x':
				i+=2;
				o.t=AST_TOKEN_TYPE_INT;
				o.v=KlBigInt_from_long(0);
				while ((*(fo->dt+i)>=48&&*(fo->dt+i)<=57)||(*(fo->dt+i)>=65&&*(fo->dt+i)<=70)||(*(fo->dt+i)>=97&&*(fo->dt+i)<=102)){
					if (*(fo->dt+i)<=57){
						KlBigInt_add_hex_digit(o.v,*(fo->dt+i)-48);
					}
					else if (*(fo->dt+i)<=70){
						KlBigInt_add_hex_digit(o.v,*(fo->dt+i)-55);
					}
					else{
						KlBigInt_add_hex_digit(o.v,*(fo->dt+i)-87);
					}
					i++;
				}
				print_token(o);
				return(o);
		}
	}
	if (*(fo->dt+i)>=48&&*(fo->dt+i)<=57){
		o.t=AST_TOKEN_TYPE_INT;
		o.v=KlBigInt_from_long(0);
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
					KlBigInt_add_digit(o.v,*(fo->dt+i)-48);
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
	CMP_STR_RET("public",6,AST_TOKEN_TYPE_MODIFIER,OBJECT_MODIFIER_PUBLIC)
	CMP_STR_RET("private",7,AST_TOKEN_TYPE_MODIFIER,OBJECT_MODIFIER_PRIVATE)
	CMP_STR_RET("static",6,AST_TOKEN_TYPE_MODIFIER,OBJECT_MODIFIER_STATIC)
	CMP_STR_RET("export",6,AST_TOKEN_TYPE_MODIFIER,OBJECT_MODIFIER_EXPORT)
	CMP_STR_RET("frozen",6,AST_TOKEN_TYPE_MODIFIER,OBJECT_MODIFIER_FROZEN)
	CMP_STR_RET("frozentype",10,AST_TOKEN_TYPE_MODIFIER,OBJECT_MODIFIER_FROZENTYPE)
	CMP_STR_RET("const",5,AST_TOKEN_TYPE_MODIFIER,OBJECT_MODIFIER_CONST)
	CMP_STR_RET("operator",8,AST_TOKEN_TYPE_MODIFIER,OBJECT_MODIFIER_OPERATOR)
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
	else if (*(fo->dt+i)=='{'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_LEFT_BRACES;
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
	else if (*(fo->dt+i)=='}'){
		o.t=AST_TOKEN_TYPE_OPERATOR;
		o.v=(void*)AST_TOKEN_OPERATOR_RIGHT_BRACES;
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
	size_t sz=0;
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
				case OBJECT_MODIFIER_UNKNOWN:
					return("OBJECT_MODIFIER_UNKNOWN");
				case OBJECT_MODIFIER_PUBLIC:
					return("public");
				case OBJECT_MODIFIER_PRIVATE:
					return("private");
				case OBJECT_MODIFIER_STATIC:
					return("static");
				case OBJECT_MODIFIER_EXPORT:
					return("export");
				case OBJECT_MODIFIER_FROZEN:
					return("frozen");
				case OBJECT_MODIFIER_FROZENTYPE:
					return("frozentype");
				case OBJECT_MODIFIER_CONST:
					return("const");
				case OBJECT_MODIFIER_OPERATOR:
					return("operator");
			}
		case AST_TOKEN_TYPE_OPERATOR:
			{
				if ((unsigned char)(uintptr_t)t.v==OBJECT_MODIFIER_UNKNOWN){
					return("OBJECT_MODIFIER_UNKNOWN");
				}
				char* o=KlMem_malloc(2);
				switch ((unsigned char)(uintptr_t)t.v){
					case AST_TOKEN_OPERATOR_UNKNOWN:
						return("OBJECT_MODIFIER_UNKNOWN");
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
					case AST_TOKEN_OPERATOR_LEFT_BRACES:
						*o='{';
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
					case AST_TOKEN_OPERATOR_RIGHT_BRACES:
						*o='}';
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
		case AST_EXPRESSION_TYPE_ACS:
			return(0);
		case AST_EXPRESSION_TYPE_INC:
		case AST_EXPRESSION_TYPE_DEC:
		case AST_EXPRESSION_TYPE_CALL:
		case AST_EXPRESSION_TYPE_SLC:
			return(1);
		case AST_EXPRESSION_TYPE_EXPAND:
			return(2);
		case AST_EXPRESSION_TYPE_PLUS:
		case AST_EXPRESSION_TYPE_MINUS:
		case AST_EXPRESSION_TYPE_NOT:
		case AST_EXPRESSION_TYPE_BNOT:
			return(3);
		case AST_EXPRESSION_TYPE_POW:
		case AST_EXPRESSION_TYPE_ROOT:
		case AST_EXPRESSION_TYPE_IROOT:
			return(4);
		case AST_EXPRESSION_TYPE_MLT:
		case AST_EXPRESSION_TYPE_DIV:
		case AST_EXPRESSION_TYPE_FDIV:
		case AST_EXPRESSION_TYPE_MOD:
		case AST_EXPRESSION_TYPE_MMLT:
			return(5);
		case AST_EXPRESSION_TYPE_ADD:
		case AST_EXPRESSION_TYPE_SUB:
			return(6);
		case AST_EXPRESSION_TYPE_LSH:
		case AST_EXPRESSION_TYPE_RSH:
		case AST_EXPRESSION_TYPE_BAND:
		case AST_EXPRESSION_TYPE_BXOR:
		case AST_EXPRESSION_TYPE_BOR:
			return(7);
		case AST_EXPRESSION_TYPE_LT:
		case AST_EXPRESSION_TYPE_LE:
		case AST_EXPRESSION_TYPE_GT:
		case AST_EXPRESSION_TYPE_GE:
		case AST_EXPRESSION_TYPE_EQ:
		case AST_EXPRESSION_TYPE_NE:
			return(8);
		case AST_EXPRESSION_TYPE_AND:
		case AST_EXPRESSION_TYPE_OR:
			return(9);
		case AST_EXPRESSION_TYPE_TCOND:
			return(10);
		case AST_EXPRESSION_TYPE_EQU:
			return(11);
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
			return(12);
		case AST_EXPRESSION_TYPE_SEP:
			return(13);
	}
}



unsigned char KlAst_get_op_count(unsigned char op){
	KlMem_enter_func();
	switch (op){
		default:
		case AST_EXPRESSION_TYPE_UNKNOWN:
			KlError_unimplemented_error();
			return(UCHAR_MAX);
		case AST_EXPRESSION_TYPE_EXPAND:
		case AST_EXPRESSION_TYPE_INC:
		case AST_EXPRESSION_TYPE_DEC:
		case AST_EXPRESSION_TYPE_PLUS:
		case AST_EXPRESSION_TYPE_MINUS:
		case AST_EXPRESSION_TYPE_NOT:
		case AST_EXPRESSION_TYPE_BNOT:
			return(1);
		case AST_EXPRESSION_TYPE_CALL:
		case AST_EXPRESSION_TYPE_SLC:
			return(UCHAR_MAX);
		case AST_EXPRESSION_TYPE_ACS:
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
			return(2);
		case AST_EXPRESSION_TYPE_TCOND:
			return(3);
		case AST_EXPRESSION_TYPE_SEP:
			KlError_unimplemented_code();
			return(UCHAR_MAX);
	}
}



uint16_t KlAst_get_decl(char* nm,struct ASTScope* sc){
	KlMem_enter_func();
	size_t ln=str_len(nm);
	bool nm_ths=(str_cmp_sub(nm,"this",0,ln)==true?true:false);
	while (true){
		if (sc->t==AST_SCOPE_TYPE_DEFAULT){
			for (size_t i=0;i<sc->vl;i++){
				if (str_cmp_sub(nm,*(sc->vnm+i),0,ln)==true){
					return(*(sc->vm+i));
				}
			}
		}
		if (sc->t==AST_SCOPE_TYPE_FUNCTION){
			for (size_t i=0;i<sc->al;i++){
				if (str_cmp_sub(nm,*(sc->anm+i),0,ln)==true){
					return(0);
				}
			}
		}
		if (sc->t==AST_SCOPE_TYPE_CLASS&&nm_ths==true){
			return(0);
		}
		for (size_t i=0;i<sc->ml;i++){
			if ((*(sc->m+i))->v_nm!=NULL){
				if (str_cmp_sub(nm,(*(sc->m+i))->v_nm,0,ln)==true){
					return(0);
				}
			}
			else{
				for (size_t j=0;j<(*(sc->m+i))->fl;j++){
					if ((*((*(sc->m+i))->fnm+j)!=NULL||((*(sc->m+i))->mf&OBJECT_MODIFIER_EXPORT_ALL)!=0)&&str_cmp_sub(nm,(*((*(sc->m+i))->fnm+j)==NULL?*((*(sc->m+i))->f+j):*((*(sc->m+i))->fnm+j)),0,ln)==true){
						ln=str_len(*((*(sc->m+i))->f+j));
						for (size_t k=0;k<(*(sc->m+i))->src->fl;k++){
							if (str_cmp_sub(*((*(sc->m+i))->f+j),(*((*(sc->m+i))->src->f+k))->nm,0,ln)==true){
								return((*((*(sc->m+i))->src->f+k))->md);
							}
						}
						assert(0);
					}
				}
				for (size_t j=0;j<(*(sc->m+i))->vl;j++){
					if ((*((*(sc->m+i))->vnm+j)!=NULL||((*(sc->m+i))->mf&OBJECT_MODIFIER_EXPORT_ALL)!=0)&&str_cmp_sub(nm,(*((*(sc->m+i))->vnm+j)==NULL?*((*(sc->m+i))->v+j):*((*(sc->m+i))->vnm+j)),0,ln)==true){
						ln=str_len(*((*(sc->m+i))->v+j));
						for (size_t k=0;k<(*(sc->m+i))->src->vl;k++){
							if (str_cmp_sub(*((*(sc->m+i))->v+j),*((*(sc->m+i))->src->vnm+k),0,ln)==true){
								return(*((*(sc->m+i))->src->vm+k));
							}
						}
						assert(0);
					}
				}
			}
		}
		if (sc->p==NULL){
			return(UCHAR_MAX);
		}
		sc=sc->p;
	}
}



size_t KlAst_get_decl_refc(char* nm,struct ASTScope* sc){
	KlMem_enter_func();
	print_ast_scope(sc);
	size_t ln=str_len(nm);
	bool nm_ths=(str_cmp_sub(nm,"this",0,ln)==true?true:false);
	while (true){
		if (sc->t==AST_SCOPE_TYPE_DEFAULT){
			for (size_t i=0;i<sc->vl;i++){
				if (str_cmp_sub(nm,*(sc->vnm+i),0,ln)==true){
					return(*(sc->vrc+i));
				}
			}
		}
		if (sc->t==AST_SCOPE_TYPE_FUNCTION){
			for (size_t i=0;i<sc->al;i++){
				if (str_cmp_sub(nm,*(sc->anm+i),0,ln)==true){
					return(1);
				}
			}
		}
		if (sc->t==AST_SCOPE_TYPE_CLASS&&nm_ths==true){
			return(1);
		}
		for (size_t i=0;i<sc->ml;i++){
			if ((*(sc->m+i))->v_nm!=NULL){
				if (str_cmp_sub(nm,(*(sc->m+i))->v_nm,0,ln)==true){
					return(1);
				}
			}
			else{
				for (size_t j=0;j<(*(sc->m+i))->fl;j++){
					if ((*((*(sc->m+i))->fnm+j)!=NULL||((*(sc->m+i))->mf&OBJECT_MODIFIER_EXPORT_ALL)!=0)&&str_cmp_sub(nm,(*((*(sc->m+i))->fnm+j)==NULL?*((*(sc->m+i))->f+j):*((*(sc->m+i))->fnm+j)),0,ln)==true){
						ln=str_len(*((*(sc->m+i))->f+j));
						for (size_t k=0;k<(*(sc->m+i))->src->fl;k++){
							if (str_cmp_sub(*((*(sc->m+i))->f+j),(*((*(sc->m+i))->src->f+k))->nm,0,ln)==true){
								return((*((*(sc->m+i))->src->f+k))->md);
							}
						}
						assert(0);
					}
				}
				for (size_t j=0;j<(*(sc->m+i))->vl;j++){
					if ((*((*(sc->m+i))->vnm+j)!=NULL||((*(sc->m+i))->mf&OBJECT_MODIFIER_EXPORT_ALL)!=0)&&str_cmp_sub(nm,(*((*(sc->m+i))->vnm+j)==NULL?*((*(sc->m+i))->v+j):*((*(sc->m+i))->vnm+j)),0,ln)==true){
						ln=str_len(*((*(sc->m+i))->v+j));
						for (size_t k=0;k<(*(sc->m+i))->src->vl;k++){
							if (str_cmp_sub(*((*(sc->m+i))->v+j),*((*(sc->m+i))->src->vnm+k),0,ln)==true){
								return(*((*(sc->m+i))->src->vrc+k));
							}
						}
						assert(0);
					}
				}
			}
		}
		if (sc->p==NULL){
			return(SIZE_MAX);
		}
		sc=sc->p;
	}
}



void KlAst_add_expression_unparsed(struct UnparsedASTExpression* o,struct UnparsedASTExpressionElem e){
	KlMem_enter_func();
	o->l++;
	o->e=KlMem_realloc(o->e,sizeof(struct UnparsedASTExpressionElem)*o->l);
	KlMem_ret(o->e);
	*(o->e+o->l-1)=e;
	return();
}

#ifndef AST_H
#define AST_H
#include <shared.h>



struct UnoptimisedASTObject* KlAst_parse_ast(struct CodeFileObject* fo,struct CallStack* cs);



struct ModifierData KlAst_parse_modifiers(struct CodeFileObject* fo,struct ASTToken* t,struct CallStack* cs);



struct ASTExpression* KlAst_parse_expression(struct CodeFileObject* fo,struct ASTToken* t,struct CallStack* cs,struct Scope* sc,struct ModifierData md,bool e,unsigned char et);



struct UnparsedASTExpression* KlAst_read_expression(struct CodeFileObject* fo,struct ASTToken* t,struct CallStack* cs,bool e,unsigned char et);



struct ASTExpression* KlAst_parse_unparsed_expression(struct CodeFileObject* fo,struct UnparsedASTExpression* e,struct CallStack* cs,struct Scope* sc);



struct ASTExpression* KlAst_gen_expression(const char* f,...);



struct ASTExpression KlAst_clone_expression(struct ASTExpression ex);



struct ASTToken KlAst_next_token(struct CodeFileObject* fo,size_t i,struct CallStack* cs);



char KlAst_parse_string_char(char* s,size_t* i,unsigned char* e);



char* KlAst_token_to_string(struct ASTToken t);



unsigned char KlAst_get_precedence(unsigned char op);



unsigned char KlAst_get_op_count(unsigned char op);



unsigned char KlAst_get_decl(char* nm,struct Scope* sc);



void KlAst_add_expression(struct UnoptimisedASTObject* o,struct ASTExpression e);



void KlAst_add_expression_unparsed(struct UnparsedASTExpression* o,struct UnparsedASTExpressionElem e);



#endif

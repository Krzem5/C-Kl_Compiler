#ifndef AST_H
#define AST_H
#include <shared.h>



struct ASTScope* KlAst_parse_ast_all(struct CodeFileObject* fo,struct CallStack* cs);



bool KlAst_parse_ast(struct CodeFileObject* fo,size_t* off,struct CallStack* cs,struct ASTScope* o,bool be);



struct ModifierData KlAst_parse_modifiers(struct CodeFileObject* fo,struct ASTToken* t,struct CallStack* cs);



bool KlAst_process_expression(struct CodeFileObject* fo,struct ASTExpression* t,struct CallStack* cs,struct ASTScope* sc,struct ModifierData md);



struct UnparsedASTExpression* KlAst_read_expression(struct CodeFileObject* fo,struct ASTToken* t,struct CallStack* cs,bool e,unsigned char et);



struct ASTExpression* KlAst_parse_unparsed_expression(struct CodeFileObject* fo,struct UnparsedASTExpression* e,struct CallStack* cs,struct ASTScope* sc);



struct ASTExpression* KlAst_gen_expression(const char* f,...);



struct ASTExpression KlAst_clone_expression(struct ASTExpression ex);



struct ASTScope* KlAst_clone_scope(struct ASTScope* sc);



bool KlAst_optimize_check_ast_expr(struct CodeFileObject* fo,struct ASTExpression* ex,struct CallStack* cs,struct ASTScope* sc);



bool KlAst_optimize_check_ast_expr_arg(struct CodeFileObject* fo,struct ASTExpressionArg* a,struct CallStack* cs,struct ASTScope* sc,bool ic);



struct ASTToken KlAst_next_token(struct CodeFileObject* fo,size_t i,struct CallStack* cs);



char KlAst_parse_string_char(char* s,size_t* i,unsigned char* e);



char* KlAst_token_to_string(struct ASTToken t);



unsigned char KlAst_get_precedence(unsigned char op);



unsigned char KlAst_get_op_count(unsigned char op);



uint16_t KlAst_get_decl(char* nm,struct ASTScope* sc);



size_t KlAst_get_decl_refc(char* nm,struct ASTScope* sc);



void KlAst_add_expression_unparsed(struct UnparsedASTExpression* o,struct UnparsedASTExpressionElem e);



#endif

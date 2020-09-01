#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H
#include <types.h>



void debug_print_ast_token(struct ASTToken t);



void debug_print_unopt_ast_object(struct UnoptimisedASTObject* o);



void debug_print_ast_expr(struct ASTExpression* e,unsigned char i);



void debug_print_ast_expr_arg(struct ASTExpressionArg* ea,unsigned char i);



void debug_print_unparsed_ast_expr(struct UnparsedASTExpression* e,unsigned char i);



char* debug_print_string(char* s);



#endif

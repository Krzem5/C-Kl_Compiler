#ifndef FREE_H
#define FREE_H
#include <types.h>



void KlFree_free_code_file_object(struct CodeFileObject o);



unsigned long KlFree_free_token(struct ASTToken t);



unsigned long KlFree_free_token_p(struct ASTToken* t);



void KlFree_free_unopt_ast_object(struct UnoptimisedASTObject o);



void KlFree_free_expression(struct ASTExpression ex);



void KlFree_free_unparsed_expression(struct UnparsedASTExpression e);



void KlFree_free_scope(struct Scope sc);



#endif

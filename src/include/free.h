#ifndef FREE_H
#define FREE_H
#include <shared.h>



void KlFree_free_code_file_object(struct CodeFileObject o);



size_t KlFree_free_token(struct ASTToken t);



size_t KlFree_free_token_p(struct ASTToken* t);



void KlFree_free_unopt_ast_object(struct UnoptimisedASTObject o);



void KlFree_free_expression(struct ASTExpression ex);



void KlFree_free_unparsed_expression(struct UnparsedASTExpression e);



void KlFree_free_scope(struct Scope sc);



#endif

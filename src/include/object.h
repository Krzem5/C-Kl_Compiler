#ifndef OBJECT_H
#define OBJECT_H
#include <stdarg.h>
#include <shared.h>



#define KlObject_get_op(k,a,b) KlObject_get_op_(OFFSETOF(struct Type,k),(a),(b))



struct Object* KlObject_new(struct Type t);



struct Object* KlObject_new_args(struct Type t,struct Object** a,size_t al);



size_t KlObject_get_size(struct Object* o);



void* KlObject_get_op_(size_t off,struct Type* a,struct Type* b);



struct Object* KlObject_expr_arg_to_object(struct ASTExpressionArg a);



struct ASTExpressionArg KlObject_to_ast_expr_arg(struct Object* a);



struct BigIntObject* KlObject_to_bigint(struct Object* a);



struct ASTExpressionArg KlObject_eval_unary(unsigned char t,struct ASTExpressionArg a);



struct ASTExpressionArg KlObject_eval_binary(unsigned char t,struct ASTExpressionArg a,struct ASTExpressionArg b);



size_t KlObject_op_to_offset(unsigned char op);



struct Type* KlObject_get_type_ptr(struct Type t);



void KlObject_free_all_types(void);



#endif

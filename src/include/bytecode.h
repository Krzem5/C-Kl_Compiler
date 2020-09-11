#ifndef OPCODE_H
#define OPCODE_H
#include <shared.h>



struct BytecodeData* KlBytecode_from_ast_scope(struct CodeFileObject* fo,struct ASTScope* ast);



uint32_t KlBytecode_get_string(struct BytecodeData* bdt,char* s);



uint32_t KlBytecode_get_var(struct BytecodeStackData* sdt,char* nm);



void KlBytecode_add_opcode_arg(struct BytecodeData* bdt,unsigned char op,uint32_t v);



uint32_t KlBytecode_read_opcode_arg(unsigned char* c,bool ex,uint64_t* i);



void KlBytecode_compile_expr(struct ASTExpression* e,uint32_t i,struct BytecodeData* bdt,struct BytecodeStackData* sdt);



void KlBytecode_compile_expr_arg(struct ASTExpressionArg* ea,uint32_t i,struct BytecodeData* bdt,struct BytecodeStackData* sdt);



struct BytecodeData* KlBytecode_read_from_file(const char* fp);



void KlBytecode_write_to_file(const char* fp,struct BytecodeData* b);



char* KlBytecode_decode_string(char* s,size_t* ln);



char* KlBytecode_encode_string(char* s,size_t ln);



#endif

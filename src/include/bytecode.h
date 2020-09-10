#ifndef OPCODE_H
#define OPCODE_H
#include <shared.h>



struct BytecodeData* KlBytecode_from_ast_scope(struct CodeFileObject* fo,struct ASTScope* ast);



struct BytecodeData* KlBytecode_read_from_file(const char* fp);



void KlBytecode_write_to_file(const char* fp,struct BytecodeData* b);



char* KlBytecode_decode_string(char* s,size_t* ln);



char* KlBytecode_encode_string(char* s,size_t ln);



#endif

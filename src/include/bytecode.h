#ifndef OPCODE_H
#define OPCODE_H
#include <shared.h>



struct BytecodeData* KlBytecode_from_ast_object(struct ASTObject* o);



struct BytecodeData* KlBytecode_read_from_file(const char* fp);



void KlBytecode_write_to_file(const char* fp,struct BytecodeData* b);



#endif

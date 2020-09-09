#ifndef OPCODE_H
#define OPCODE_H
#include <shared.h>



struct COFFFile* KlBytecode_from_ast_object(struct ASTObject* o);



struct COFFFile* KlBytecode_read_from_file(const char* fp);



void KlBytecode_write_to_file(const char* fp,struct COFFFile* b);



char* KlBytecode_demangle_string(char* nm);



size_t KlBytecode_demangle_string_length(char* nm,size_t* i);



#endif

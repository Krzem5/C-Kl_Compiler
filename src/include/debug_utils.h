#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H
#include <shared.h>



#define COLOR_DEBUG_UTILS_PUNCTUATION 0x15b5b5b000000 // #5b5b5b;#000000
#define COLOR_DEBUG_UTILS_TYPE 0x1344a34000000 // #344a34;#000000
#define COLOR_DEBUG_UTILS_KEY 0x11ec0c6000000 // #1ec0c6;#000000
#define COLOR_DEBUG_UTILS_OPERATOR 0x1ecd40c000000 // #ecd40c;#000000
#define COLOR_DEBUG_UTILS_KEYWORD 0x1e09a1e000000 // #e09a1e;#000000
#define COLOR_DEBUG_UTILS_POINTER 0x19143ed000000 // #9143ed;#000000
#define COLOR_DEBUG_UTILS_IDENTIFIER 0x1efefef000000 // #efefef;#000000
#define COLOR_DEBUG_UTILS_STRING 0x16cc332000000 // #6cc332;#000000
#define COLOR_DEBUG_UTILS_STRING_BYTE 0x16c4978000000 // #6C4978;#000000
#define COLOR_DEBUG_UTILS_MODIFIER 0x1f07000000000 // #f07000;#000000
#define COLOR_DEBUG_UTILS_NULL 0x1693ad7000000 // #693ad7;#000000
#define COLOR_DEBUG_UTILS_INT 0x1144bea000000 // #144bea;#000000
#define COLOR_DEBUG_UTILS_INSTRUCTION 0x1e14f46000000 // #e14f46;#000000
#define COLOR_DEBUG_UTILS_INSTRUCTION_TYPE 0x1aa30ce000000 // #aa30ce;#000000
#define COLOR_DEBUG_UTILS_INSTRUCTION_VALUE 0x1347eca000000 // #347eca;#000000



void KlDebug_print_ast_token(struct ASTToken t);



void KlDebug_print_ast_module(struct ASTModule* o,unsigned char i,void** rp);



void KlDebug_print_ast_scope(struct ASTScope* o,unsigned char i,void** rp);



void KlDebug_print_ast_expr(struct ASTExpression* e,unsigned char i,void** rp);



void KlDebug_print_ast_expr_arg(struct ASTExpressionArg* ea,unsigned char i,void** rp);



void KlDebug_print_unparsed_ast_expr(struct UnparsedASTExpression* e,unsigned char i,void** rp);



void KlDebug_print_bytecode(struct BytecodeData* bdt);



char* KlDebug_print_char(char c);



char* KlDebug_print_string(char* s);



char* KlDebug_print_string_len(char* s,size_t s_ln);



#endif

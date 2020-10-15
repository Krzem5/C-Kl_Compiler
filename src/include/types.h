#ifndef TYPES_H
#define TYPES_H
#include <stdint.h>



#ifdef NULL
#undef NULL
#endif
#define NULL 0
#define NULL_OBJECT (struct Object){OBJECT_TYPE_NONE,0,NULL,0}
#define bool _Bool
#define false 0
#define true 1
#ifdef WIN32
#define PTR_TYPE unsigned long
#else
#define PTR_TYPE unsigned long long
#endif
#define AST_TOKEN_ALL_OPERATORS "&@^]:=!>(<-%[.+?)/*~|"



struct Object;
struct Type;
struct UnparsedASTExpression;
struct Expression;
struct UnoptimisedASTObject;



typedef void (*ModuleInitFunc)(void);
typedef void (*ModuleDeinitFunc)(void);
typedef struct Expression (*UnaryFunc)(struct Expression a);
typedef struct Expression (*BinaryFunc)(struct Expression a,struct Expression b);
typedef struct Expression (*TernaryFunc)(struct Expression a,struct Expression b,struct Expression c);
typedef struct Object (*TypeCreateFunc)(struct Object* a);
typedef void (*TypeDeleteFunc)(struct Object o);
typedef char* (*FileReadFunc)(void* p,unsigned char m,unsigned long l,unsigned long* r);
typedef unsigned long (*FileWriteFunc)(void* p,unsigned char m,char* s);
typedef bool (*FileFlushFunc)(void* p,unsigned char m);
typedef struct Object (*NativeFunctionZeroArg)(void);
typedef struct Object (*NativeFunctionOneArg)(struct Object a);
typedef struct Object (*NativeFunctionArgs)(struct Object* a);
typedef struct Object (*NativeFunctionVarArg)(struct Object* a,struct Object** va);



enum AST_TOKEN_TYPE{
	AST_TOKEN_TYPE_UNKNOWN=0x00,
	AST_TOKEN_TYPE_STRING=0x01,
	AST_TOKEN_TYPE_CHAR=0x02,
	AST_TOKEN_TYPE_NUMBER=0x03,
	AST_TOKEN_TYPE_DECIMAL=0x04,
	AST_TOKEN_TYPE_KEYWORD=0x05,
	AST_TOKEN_TYPE_MODIFIER=0x06,
	AST_TOKEN_TYPE_IDENTIFIER=0x07,
	AST_TOKEN_TYPE_OPERATOR=0x08,
	AST_TOKEN_TYPE_COMMA=0x09,
	AST_TOKEN_TYPE_SEMICOLON=0x0a,
	AST_TOKEN_TYPE_WHITESPACE=0x0b,
	AST_TOKEN_TYPE_ERROR=0x0c,
	AST_TOKEN_TYPE_END_OF_DATA=0x0d
};



enum AST_TOKEN_KEYWORD{
	AST_TOKEN_KEYWORD_UNKNOWN=0x00,
	AST_TOKEN_KEYWORD_FROM=0x01,
	AST_TOKEN_KEYWORD_USE=0x02,
	AST_TOKEN_KEYWORD_AS=0x03,
	AST_TOKEN_KEYWORD_DEF=0x04,
	AST_TOKEN_KEYWORD_CLASS=0x05,
	AST_TOKEN_KEYWORD_IF=0x06,
	AST_TOKEN_KEYWORD_ELIF=0x07,
	AST_TOKEN_KEYWORD_ELSE=0x08,
	AST_TOKEN_KEYWORD_FOR=0x09,
	AST_TOKEN_KEYWORD_WHILE=0x0a,
	AST_TOKEN_KEYWORD_RAISE=0x0b,
	AST_TOKEN_KEYWORD_ASSERT=0x0c,
	AST_TOKEN_KEYWORD_DELETE=0x0d,
	AST_TOKEN_KEYWORD_RETURN=0x0e,
	AST_TOKEN_KEYWORD_DEBUGGER=0x0f
};



enum AST_TOKEN_MODIFIER{
	AST_TOKEN_MODIFIER_UNKNOWN=0x00,
	AST_TOKEN_MODIFIER_PUBLIC=0x01,
	AST_TOKEN_MODIFIER_PRIVATE=0x02,
	AST_TOKEN_MODIFIER_STATIC=0x04,
	AST_TOKEN_MODIFIER_EXPORT=0x08,
	AST_TOKEN_MODIFIER_FROZEN=0x10,
	AST_TOKEN_MODIFIER_FROZENTYPE=0x20
};



enum AST_TOKEN_OPERATOR{
	AST_TOKEN_OPERATOR_UNKNOWN=0x00,
	AST_TOKEN_OPERATOR_AMP=0x01,
	AST_TOKEN_OPERATOR_AT=0x02,
	AST_TOKEN_OPERATOR_CIRCUMFLEX=0x03,
	AST_TOKEN_OPERATOR_CLOSE_BRACKET=0x04,
	AST_TOKEN_OPERATOR_COLON=0x05,
	AST_TOKEN_OPERATOR_EQUALS=0x06,
	AST_TOKEN_OPERATOR_EXCLAMATION_MARK=0x07,
	AST_TOKEN_OPERATOR_GRATER=0x08,
	AST_TOKEN_OPERATOR_LEFT_PARENTHESIS=0x09,
	AST_TOKEN_OPERATOR_LESS=0x0a,
	AST_TOKEN_OPERATOR_MINUS=0x0b,
	AST_TOKEN_OPERATOR_MODULO=0x0c,
	AST_TOKEN_OPERATOR_OPEN_BRACKET=0x0d,
	AST_TOKEN_OPERATOR_PERIOD=0x0e,
	AST_TOKEN_OPERATOR_PLUS=0x0f,
	AST_TOKEN_OPERATOR_QUESTION_MARK=0x10,
	AST_TOKEN_OPERATOR_RIGHT_PARENTHESIS=0x11,
	AST_TOKEN_OPERATOR_SLASH=0x12,
	AST_TOKEN_OPERATOR_STAR=0x13,
	AST_TOKEN_OPERATOR_TILDA=0x14,
	AST_TOKEN_OPERATOR_VERTICAL_BAR=0x15
};



enum AST_TOKEN_OPERATOR_CMP{
	AST_TOKEN_OPERATOR_CMP_UNKNOWN=0x00,
	AST_TOKEN_OPERATOR_CMP_EQ=0x01,
	AST_TOKEN_OPERATOR_CMP_NE=0x02,
	AST_TOKEN_OPERATOR_CMP_GE=0x03,
	AST_TOKEN_OPERATOR_CMP_GT=0x04,
	AST_TOKEN_OPERATOR_CMP_LE=0x05,
	AST_TOKEN_OPERATOR_CMP_LT=0x06,
	AST_TOKEN_OPERATOR_CMP_DF=0x07
};



enum AST_TOKEN_BRACKET{
	AST_TOKEN_BRACKET_UNKNOWN=0x00,
	AST_TOKEN_BRACKET_NORMAL=0x01,
	AST_TOKEN_BRACKET_SQUARE=0x02,
	AST_TOKEN_BRACKET_CURLY=0x03
};



enum UNPARSED_AST_EXPRESSION_ELEM_TYPE{
	UNPARSED_AST_EXPRESSION_ELEM_TYPE_UNKNOWN=0x00,
	UNPARSED_AST_EXPRESSION_ELEM_TYPE_EXPRESSION=0x01,
	UNPARSED_AST_EXPRESSION_ELEM_TYPE_OPERATOR=0x02,
	UNPARSED_AST_EXPRESSION_ELEM_TYPE_CHAR=0x03,
	UNPARSED_AST_EXPRESSION_ELEM_TYPE_STRING=0x04,
	UNPARSED_AST_EXPRESSION_ELEM_TYPE_NUMBER=0x05,
	UNPARSED_AST_EXPRESSION_ELEM_TYPE_DECIMAL=0x06,
	UNPARSED_AST_EXPRESSION_ELEM_TYPE_NATIVE_FUNCTION=0x07,
	UNPARSED_AST_EXPRESSION_ELEM_TYPE_IDENTIFIER=0x08,
	UNPARSED_AST_EXPRESSION_ELEM_TYPE_MODIFIERS=0x09,
	UNPARSED_AST_EXPRESSION_ELEM_TYPE_OBJECT=0x0a
};



enum AST_EXPRESSION_TYPE{
	AST_EXPRESSION_TYPE_UNKNOWN=0x00,
	AST_EXPRESSION_TYPE_EMPTY=0x01,
	AST_EXPRESSION_TYPE_CONST=0x02,
	AST_EXPRESSION_TYPE_INC=0x03,
	AST_EXPRESSION_TYPE_DEC=0x04,
	AST_EXPRESSION_TYPE_CALL=0x05,
	AST_EXPRESSION_TYPE_SLICE=0x06,
	AST_EXPRESSION_TYPE_ACCESS=0x07,
	AST_EXPRESSION_TYPE_PLUS=0x08,
	AST_EXPRESSION_TYPE_MINUS=0x09,
	AST_EXPRESSION_TYPE_NOT=0x0a,
	AST_EXPRESSION_TYPE_BNOT=0x0b,
	AST_EXPRESSION_TYPE_POW=0x0c,
	AST_EXPRESSION_TYPE_ROOT=0x0d,
	AST_EXPRESSION_TYPE_MLT=0x0e,
	AST_EXPRESSION_TYPE_DIV=0x0f,
	AST_EXPRESSION_TYPE_FDIV=0x10,
	AST_EXPRESSION_TYPE_MOD=0x11,
	AST_EXPRESSION_TYPE_MMLT=0x12,
	AST_EXPRESSION_TYPE_ADD=0x13,
	AST_EXPRESSION_TYPE_SUB=0x14,
	AST_EXPRESSION_TYPE_LSH=0x15,
	AST_EXPRESSION_TYPE_RSH=0x16,
	AST_EXPRESSION_TYPE_LT=0x17,
	AST_EXPRESSION_TYPE_LE=0x18,
	AST_EXPRESSION_TYPE_GT=0x19,
	AST_EXPRESSION_TYPE_GE=0x1a,
	AST_EXPRESSION_TYPE_EQ=0x1b,
	AST_EXPRESSION_TYPE_NE=0x1c,
	AST_EXPRESSION_TYPE_BAND=0x1d,
	AST_EXPRESSION_TYPE_BXOR=0x1e,
	AST_EXPRESSION_TYPE_BOR=0x1f,
	AST_EXPRESSION_TYPE_AND=0x20,
	AST_EXPRESSION_TYPE_OR=0x21,
	AST_EXPRESSION_TYPE_TCOND=0x22,
	AST_EXPRESSION_TYPE_EQU=0x23,
	AST_EXPRESSION_TYPE_ADD_EQU=0x24,
	AST_EXPRESSION_TYPE_SUB_EQU=0x25,
	AST_EXPRESSION_TYPE_MLT_EQU=0x26,
	AST_EXPRESSION_TYPE_DIV_EQU=0x27,
	AST_EXPRESSION_TYPE_FDIV_EQU=0x28,
	AST_EXPRESSION_TYPE_MOD_EQU=0x29,
	AST_EXPRESSION_TYPE_MMLT_EQU=0x2a,
	AST_EXPRESSION_TYPE_POW_EQU=0x2b,
	AST_EXPRESSION_TYPE_ROOT_EQU=0x2c,
	AST_EXPRESSION_TYPE_LSH_EQU=0x2d,
	AST_EXPRESSION_TYPE_RSH_EQU=0x2e,
	AST_EXPRESSION_TYPE_BAND_EQU=0x2f,
	AST_EXPRESSION_TYPE_BXOR_EQU=0x30,
	AST_EXPRESSION_TYPE_BOR_EQU=0x31,
	AST_EXPRESSION_TYPE_BNOT_EQU=0x32
};



enum AST_EXPRESSION_ARG_TYPE{
	AST_EXPRESSION_ARG_TYPE_UNKNOWN=0x00,
	AST_EXPRESSION_ARG_TYPE_EXPRESSION=0x01,
	AST_EXPRESSION_ARG_TYPE_CHAR=0x02,
	AST_EXPRESSION_ARG_TYPE_STRING=0x03,
	AST_EXPRESSION_ARG_TYPE_NUMBER=0x04,
	AST_EXPRESSION_ARG_TYPE_DECIMAL=0x05,
	AST_EXPRESSION_ARG_TYPE_FUNCTION=0x06,
	AST_EXPRESSION_ARG_TYPE_NATIVE_FUNCTION=0x07,
	AST_EXPRESSION_ARG_TYPE_IDENTIFIER=0x08,
	AST_EXPRESSION_ARG_TYPE_MODIFIERS=0x09,
	AST_EXPRESSION_ARG_TYPE_OBJECT=0x0a
};



enum AST_OBJECT_ELEM_TYPE{
	AST_OBJECT_ELEM_TYPE_UNKNOWN=0x00,
	AST_OBJECT_ELEM_TYPE_EXPRESSION=0x01,
	AST_OBJECT_ELEM_TYPE_FUNC_DECLARATION=0x02,
	AST_OBJECT_ELEM_TYPE_CLASS_DECLARATION=0x03
};



enum OBJECT_TYPE{
	OBJECT_TYPE_NONE=0x00,
	OBJECT_TYPE_NODE_LIST=0x01,
	OBJECT_TYPE_CHAR=0x02,
	OBJECT_TYPE_STRING=0x03,
	OBJECT_TYPE_NUM=0x04,
	OBJECT_TYPE_DECIMAL=0x05,
	OBJECT_TYPE_FUNCTION=0x06,
	OBJECT_TYPE_NATIVE_FUNCTION=0x07,
	OBJECT_TYPE_OTHER=0x0c
};



enum NATIVE_FUNCITON_TYPE{
	NATIVE_FUNCITON_TYPE_UNKNOWN=0x00,
	NATIVE_FUNCITON_TYPE_0_ARGS=0x01,
	NATIVE_FUNCITON_TYPE_1_ARG=0x02,
	NATIVE_FUNCITON_TYPE_ARGS=0x03,
	NATIVE_FUNCITON_TYPE_VARARG=0x04
};



enum OBJECT_MODIFIER{
	OBJECT_MODIFIER_NONE=0x00,
	OBJECT_MODIFIER_PUBLIC=0x01,
	OBJECT_MODIFIER_PRIVATE=0x02,
	OBJECT_MODIFIER_STATIC=0x04,
	OBJECT_MODIFIER_EXPORT=0x08,
	OBJECT_MODIFIER_FROZEN=0x10,
	OBJECT_MODIFIER_FROZENTYPE=0x20
};



enum FILE_MODIFIER{
	FILE_MODIFIER_NONE=0x00,
	FILE_MODIFIER_EXISTS=0x01,
	FILE_MODIFIER_REAL=0x02,
	FILE_MODIFIER_VIRTUAL=0x04,
	FILE_MODIFIER_READ=0x08,
	FILE_MODIFIER_WRITE=0x10,
	FILE_MODIFIER_TEXT=0x20,
	FILE_MODIFIER_BINARY=0x40
};



struct NativeFunction{
	enum NATIVE_FUNCITON_TYPE t;
	union{
		NativeFunctionZeroArg _0a;
		NativeFunctionOneArg _1a;
		NativeFunctionArgs a;
		NativeFunctionVarArg va;
	} f;
};



struct NativeModule{
	const char* nm;
	ModuleInitFunc if_;
	ModuleDeinitFunc df;
};



struct LoadedModule{
	bool n;
	void* m;
};



struct LoadedModuleList{
	struct LoadedModule* e;
	unsigned long l;
};



struct File{
	void* p;
	unsigned char m;
	FileReadFunc rf;
	FileWriteFunc wf;
	FileFlushFunc ff;
};



struct Number{
	long l;
	uint32_t* v;
};



struct Decimal{
	long l;
	unsigned long fl;
	uint32_t* v;
	uint32_t* fv;
};



struct Type{
	const char* nm;
	TypeCreateFunc cf;
	TypeDeleteFunc df;
};



struct CallStackElem{
	struct CodeFileObject* c;
	unsigned long sl;
	unsigned long el;
	unsigned long su;
	unsigned long eu;
	const char* f;
};



struct CallStack{
	struct CallStackElem* e;
	unsigned int l;
};



struct Error{
	char* nm;
	char* msg;
	struct CallStack* cs;
};



struct Object{
	enum OBJECT_TYPE t;
	enum OBJECT_MODIFIER m;
	struct Type* ot;
	union{
		char c;
		char* s;
		struct Number* n;
		struct Decimal* d;
		struct Function* f;
		struct NativeFunction nf;
		struct Module* m;
		struct Class* cl;
		struct Error* e;
		struct Type* t;
		void* o;
	} v;
};



struct CodeFileObject{
	const char* fp;
	char* dt;
	unsigned long l;
};



struct ASTToken{
	enum AST_TOKEN_TYPE t;
	void* v;
	unsigned long i;
};



struct ModifierData{
	unsigned char m;
	unsigned long s;
	unsigned long e;
};



struct UnparsedASTExpressionElem{
	enum UNPARSED_AST_EXPRESSION_ELEM_TYPE t;
	union{
		struct UnparsedASTExpression* ex;
		enum AST_EXPRESSION_TYPE op;
		char c;
		char* s;
		struct Number* n;
		struct Decimal* d;
		struct NativeFunction nf;
		char* i;
		unsigned char m;
		void* o;
	} v;
};



struct UnparsedASTExpression{
	struct UnparsedASTExpressionElem* e;
	unsigned long l;
};



struct ASTExpressionArg{
	enum AST_EXPRESSION_ARG_TYPE t;
	union{
		struct ASTExpression* ex;
		char c;
		char* s;
		struct Number* n;
		struct Decimal* d;
		struct Function* f;
		struct NativeFunction nf;
		char* i;
		unsigned char m;
		void* o;
	} v;
};



struct ASTExpression{
	enum AST_EXPRESSION_TYPE t;
	struct ASTExpressionArg a;
	struct ASTExpressionArg* b;
	unsigned long bl;
	union{
		UnaryFunc u;
		BinaryFunc b;
		TernaryFunc t;
	} f;
};



struct ASTFuncDeclaration{
	char* nm;
	unsigned char m;
	unsigned long a_l;
	char** a_nm;
	struct UnoptimisedASTObject* c;
};



struct ASTClassDeclaration{
	char* nm;
	unsigned char m;
	unsigned long ml_l;
	struct ASTFuncDeclaration** ml;
};



struct UnoptimisedASTObjectElem{
	enum AST_OBJECT_ELEM_TYPE t;
	union{
		struct ASTExpression e;
		struct ASTFuncDeclaration fd;
		struct ASTClassDeclaration cd;
	} v;
};



struct UnoptimisedASTObject{
	struct UnoptimisedASTObjectElem* e;
	unsigned long l;
};



struct Scope{
	struct Scope* p;
	char** k;
	struct Object* v;
	unsigned long l;
};



#endif

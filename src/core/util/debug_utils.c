#include <bigint.h>
#include <debug_utils.h>
#include <string_utils.h>
#include <memory.h>
#include <io.h>
#include <shared.h>
#include <bytecode.h>
#include <limits.h>



void KlDebug_print_ast_token(struct ASTToken t){
	KlMem_enter_func();
	KlIo_printf("%fASTToken %f= {\n  %fType%f: %f",COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_TYPE);
	switch (t.t){
		default:
			assert(0);
		case AST_TOKEN_TYPE_UNKNOWN:
			KlIo_printf("Unknown%f,\n  %fCharacter%f: %f'%c'%f,\n  %fFile Offset%f: %f%S%f,\n};%f\n",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_STRING,(char)(uintptr_t)t.v,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_RESET);
			break;
		case AST_TOKEN_TYPE_STRING:
			char* ss=KlDebug_print_string(t.v);
			KlIo_printf("String%f,\n  %fValue%f: %s%f,\n  %fNext File Offset%f: %f%S%f,\n};%f\n",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,ss,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,t.i,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_RESET);
			KlMem_free(ss);
			break;
		case AST_TOKEN_TYPE_CHAR:
			char* cs=KlDebug_print_char((char)(uintptr_t)t.v);
			KlIo_printf("Char%f,\n  %fValue%f: %s%f,\n  %fNext File Offset%f: %f%S%f,\n};%f\n",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,cs,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,t.i,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_RESET);
			KlMem_free(cs);
			break;
		case AST_TOKEN_TYPE_INT:
			char* ns=KlBigInt_print((struct BigInt*)t.v);
			KlIo_printf("Number%f,\n  %fValue%f: %f%s%f,\n  %fNext File Offset%f: %f%S%f,\n};%f\n",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,ns,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,t.i,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_RESET);
			KlMem_free(ns);
			break;
		case AST_TOKEN_TYPE_FLOAT:
			KlIo_printf("Decimal%f,\n  %fValue%f: %f%p%f,\n  %fNext File Offset%f: %f%S%f,\n};%f\n",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,t.v,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,t.i,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_RESET);
			break;
		case AST_TOKEN_TYPE_KEYWORD:
			KlIo_printf("Keyword%f,\n  %fValue%f: %f",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEYWORD);
			switch ((unsigned char)(uintptr_t)t.v){
				default:
				case AST_TOKEN_KEYWORD_UNKNOWN:
					KlIo_printf("<unknown>");
					break;
				case AST_TOKEN_KEYWORD_FROM:
					KlIo_printf("from");
					break;
				case AST_TOKEN_KEYWORD_USE:
					KlIo_printf("use");
					break;
				case AST_TOKEN_KEYWORD_AS:
					KlIo_printf("as");
					break;
				case AST_TOKEN_KEYWORD_DEF:
					KlIo_printf("def");
					break;
				case AST_TOKEN_KEYWORD_CLASS:
					KlIo_printf("class");
					break;
				case AST_TOKEN_KEYWORD_IF:
					KlIo_printf("if");
					break;
				case AST_TOKEN_KEYWORD_ELIF:
					KlIo_printf("elif");
					break;
				case AST_TOKEN_KEYWORD_ELSE:
					KlIo_printf("else");
					break;
				case AST_TOKEN_KEYWORD_FOR:
					KlIo_printf("for");
					break;
				case AST_TOKEN_KEYWORD_WHILE:
					KlIo_printf("while");
					break;
				case AST_TOKEN_KEYWORD_RAISE:
					KlIo_printf("raise");
					break;
				case AST_TOKEN_KEYWORD_ASSERT:
					KlIo_printf("assert");
					break;
				case AST_TOKEN_KEYWORD_DELETE:
					KlIo_printf("delete");
					break;
				case AST_TOKEN_KEYWORD_RETURN:
					KlIo_printf("return");
					break;
				case AST_TOKEN_KEYWORD_DEBUGGER:
					KlIo_printf("debugger");
					break;
			}
			KlIo_printf("%f,\n  %fNext File Offset%f: %f%S%f,\n};%f\n",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,(size_t)t.i,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_RESET);
			break;
		case AST_TOKEN_TYPE_MODIFIER:
			KlIo_printf("Modifier%f,\n  %fValue%f: %f",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_MODIFIER);
			switch ((unsigned char)(uintptr_t)t.v){
				default:
				case OBJECT_MODIFIER_UNKNOWN:
					KlIo_printf("<unknown>");
					break;
				case OBJECT_MODIFIER_PUBLIC:
					KlIo_printf("public");
					break;
				case OBJECT_MODIFIER_PRIVATE:
					KlIo_printf("private");
					break;
				case OBJECT_MODIFIER_STATIC:
					KlIo_printf("static");
					break;
				case OBJECT_MODIFIER_EXPORT:
					KlIo_printf("export");
					break;
				case OBJECT_MODIFIER_FROZEN:
					KlIo_printf("frozen");
					break;
				case OBJECT_MODIFIER_FROZENTYPE:
					KlIo_printf("frozentype");
					break;
				case OBJECT_MODIFIER_CONST:
					KlIo_printf("const");
					break;
				case OBJECT_MODIFIER_OPERATOR:
					KlIo_printf("operator");
					break;
			}
			KlIo_printf("%f,\n  %fNext File Offset%f: %f%S%f,\n};%f\n",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,t.i,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_RESET);
			break;
		case AST_TOKEN_TYPE_IDENTIFIER:
			KlIo_printf("Identifier%f,\n  %fValue%f: %f%s%f,\n  %fNext File Offset%f: %f%S%f,\n};%f\n",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_IDENTIFIER,(char*)t.v,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,t.i,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_RESET);
			break;
		case AST_TOKEN_TYPE_OPERATOR:
			KlIo_printf("Operator%f,\n  %fValue%f: %f",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR);
			switch ((unsigned char)(uintptr_t)t.v){
				default:
				case AST_TOKEN_OPERATOR_UNKNOWN:
					KlIo_printf("%f(Unknown)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_AMP:
					KlIo_printf("& %f(Amp)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_AT:
					KlIo_printf("@ %f(At)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_CIRCUMFLEX:
					KlIo_printf("^ %f(Circumflex)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_CLOSE_BRACKET:
					KlIo_printf("] %f(Close Bracket)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_COLON:
					KlIo_printf(": %f(Colon)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_COMMA:
					KlIo_printf(", %f(Comma)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_EQUALS:
					KlIo_printf("= %f(Equals)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_EXCLAMATION_MARK:
					KlIo_printf("! %f(Exclamation Mark)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_GRATER:
					KlIo_printf("> %f(Greater)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_LEFT_BRACES:
					KlIo_printf("{ %f(Left Brace)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_LEFT_PARENTHESIS:
					KlIo_printf("( %f(Left Parenthesis)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_LESS:
					KlIo_printf("< %f(Less)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_MINUS:
					KlIo_printf("- %f(Minus)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_MODULO:
					KlIo_printf("%% %f(Modulo)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_OPEN_BRACKET:
					KlIo_printf("[ %f(Open Bracket)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_PERIOD:
					KlIo_printf(". %f(Period)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_PLUS:
					KlIo_printf("+ %f(Plus)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_QUESTION_MARK:
					KlIo_printf("? %f(Question Mark)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_RIGHT_BRACES:
					KlIo_printf("} %f(Right Brace)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_RIGHT_PARENTHESIS:
					KlIo_printf(") %f(Right Parenthesis)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_SEMICOLON:
					KlIo_printf("; %f(Semicolon)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_SLASH:
					KlIo_printf("/ %f(Slash)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_STAR:
					KlIo_printf("* %f(Star)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_TILDA:
					KlIo_printf("~ %f(Tilda)",COLOR_DEBUG_UTILS_TYPE);
					break;
				case AST_TOKEN_OPERATOR_VERTICAL_BAR:
					KlIo_printf("| %f(Vertical Bar)",COLOR_DEBUG_UTILS_TYPE);
					break;
			}
			KlIo_printf("%f,\n  %fNext File Offset%f: %f%S%f,\n};%f\n",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,t.i,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_RESET);
			break;
		case AST_TOKEN_TYPE_WHITESPACE:
			KlIo_printf("Whitespace%f,\n  %fNext File Offset%f: %f%S%f,\n};%f\n",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,t.i,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_RESET);
			break;
		case AST_TOKEN_TYPE_ERROR:
			KlIo_printf("Error\n%f};%f\n",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_RESET);
			break;
		case AST_TOKEN_TYPE_END_OF_DATA:
			KlIo_printf("End-Of-Data\n%f};%f\n",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_RESET);
			break;
	}
	return();
}



void KlDebug_print_ast_module(struct ASTModule* o,unsigned char i,void** rp){
	KlMem_enter_func();
	size_t ln=2;
	if (rp!=NULL){
		for (size_t j=0;true;j++){
			if (*(rp+j)==NULL){
				break;
			}
			ln++;
		}
	}
	void** rpo=KlMem_malloc(ln*sizeof(void*));
	for (size_t j=0;j<ln-2;j++){
		*(rpo+j)=*(rp+j);
	}
	*(rpo+ln-2)=o;
	*(rpo+ln-1)=NULL;
	char* is=KlMem_malloc(i+1);
	for (unsigned char j=0;j<i;j++){
		*(is+j)=' ';
	}
	*(is+i)=0;
	char* fps=KlDebug_print_string(o->fp);
	if (i==0){
		KlIo_printf("%fASTModule %f= {\n%s  %fName%f: %f%s%f,\n%s  %fFile Path%f: %f%s%f,\n%s  %fImport Name%f: ",COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_IDENTIFIER,o->nm,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_IDENTIFIER,fps,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	else{
		KlIo_printf("{\n%s  %fName%f: %f%s%f,\n%s  %fFile Path%f: %f%s%f,\n%s  %fImport Name%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_IDENTIFIER,o->nm,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_IDENTIFIER,fps,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	KlMem_free(fps);
	if (o->v_nm==NULL){
		KlIo_printf("%f(None)%f,\n%s  %fExported Variables%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	else{
		KlIo_printf("%f%s%f,\n%s  %fExported Variables%f: ",COLOR_DEBUG_UTILS_IDENTIFIER,o->v_nm,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	if (o->vl==0){
		KlIo_printf("%f(None)%f,\n%s  %fExported Functions%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	else{
		bool f=false;
		for (size_t j=0;j<o->vl;j++){
			if ((o->mf&OBJECT_MODIFIER_EXPORT_ALL)!=0||*(o->vnm+j)!=NULL){
				continue;
			}
			if (f==false){
				KlIo_printf("{\n");
			}
			f=true;
			KlIo_printf("%s    {\n%s      %fExport Name%f: %f%s%f,\n%s      %fImport Name%f: %f%s%f,\n%s    },\n",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_IDENTIFIER,*(o->v+j),COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_IDENTIFIER,(*(o->vnm+j)==NULL?*(o->v+j):*(o->vnm+j)),COLOR_DEBUG_UTILS_PUNCTUATION,is);
		}
		if (f==true){
			KlIo_printf("%s  },\n%s  %fExported Functions%f: ",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		else{
			KlIo_printf("%f(None)%f,\n%s  %fExported Functions%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
	}
	if (o->fl==0){
		KlIo_printf("%f(None)%f,\n%s  %fCode%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	else{
		bool f=false;
		for (size_t j=0;j<o->fl;j++){
			if ((o->mf&OBJECT_MODIFIER_EXPORT_ALL)!=0||*(o->fnm+j)!=NULL){
				continue;
			}
			if (f==false){
				KlIo_printf("{\n");
			}
			f=true;
			KlIo_printf("%s    {\n%s      %fExport Name%f: %f%s%f,\n%s      %fImport Name%f: %f%s%f,\n%s    },\n",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_IDENTIFIER,*(o->f+j),COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_IDENTIFIER,(*(o->fnm+j)==NULL?*(o->f+j):*(o->fnm+j)),COLOR_DEBUG_UTILS_PUNCTUATION,is);
		}
		if (f==true){
			KlIo_printf("%s  },\n%s  %fCode%f: ",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		else{
			KlIo_printf("%f(None)%f,\n%s  %fCode%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
	}
	char ec=(i==0?';':',');
	if (o->src==NULL){
		KlIo_printf("%f(None)%f,\n%s}%c\n",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,ec);
	}
	else{
		KlDebug_print_ast_scope(o->src,i+2,rpo);
		KlIo_printf("%s}%c\n",is,ec);
	}
	KlMem_free(is);
	KlMem_free(rpo);
	return();
}



void KlDebug_print_ast_scope(struct ASTScope* o,unsigned char i,void** rp){
	KlMem_enter_func();
	size_t ln=2;
	if (rp!=NULL){
		for (size_t j=0;true;j++){
			if (*(rp+j)==NULL){
				break;
			}
			ln++;
		}
	}
	void** rpo=KlMem_malloc(ln*sizeof(void*));
	for (size_t j=0;j<ln-2;j++){
		*(rpo+j)=*(rp+j);
	}
	*(rpo+ln-2)=o;
	*(rpo+ln-1)=NULL;
	char* is=KlMem_malloc(i+1);
	for (unsigned char j=0;j<i;j++){
		*(is+j)=' ';
	}
	*(is+i)=0;
	char* ts;
	switch (o->t){
		default:
		case AST_SCOPE_TYPE_UNKNOWN:
			ts="Unknown";
			break;
		case AST_SCOPE_TYPE_DEFAULT:
			ts="Default";
			break;
		case AST_SCOPE_TYPE_FUNCTION:
			ts="Function";
			break;
		case AST_SCOPE_TYPE_CLASS:
			ts="Class";
			break;
	}
	if (i==0){
		KlIo_printf("%fASTScope %f= %f(%s) %f{\n  %fName%f:%f ",COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_TYPE,ts,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	else{
		KlIo_printf("%f(Scope -> %s)%f {\n%s  %fName%f:%f ",COLOR_DEBUG_UTILS_TYPE,ts,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	if (o->t!=AST_SCOPE_TYPE_UNKNOWN&&o->t!=AST_SCOPE_TYPE_DEFAULT){
		if (o->nm==NULL){
			KlIo_printf("%f(Anonymous)%f,\n%s  %fModifiers%f:",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		else{
			KlIo_printf("%f%s%f,\n%s  %fModifiers%f:%f",COLOR_DEBUG_UTILS_IDENTIFIER,o->nm,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_MODIFIER);
		}
		if (o->m==0){
			KlIo_printf(" %f(None)%f,\n%s  %fParent Scope%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		else{
			if ((o->md&OBJECT_MODIFIER_PUBLIC)!=0){
				KlIo_printf(" public");
			}
			if ((o->md&OBJECT_MODIFIER_PRIVATE)!=0){
				KlIo_printf(" private");
			}
			if ((o->md&OBJECT_MODIFIER_STATIC)!=0){
				KlIo_printf(" static");
			}
			if ((o->md&OBJECT_MODIFIER_EXPORT)!=0){
				KlIo_printf(" export");
			}
			KlIo_printf("%f,\n%s  %fParent Scope%f: ",COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
	}
	else{
		if (o->nm==NULL){
			KlIo_printf("%f(Anonymous)%f,\n%s  %fParent Scope%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		else{
			KlIo_printf("%f%s%f,\n%s  %fParent Scope%f: ",COLOR_DEBUG_UTILS_IDENTIFIER,o->nm,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
	}
	if (o->p==NULL){
		KlIo_printf("%f(None)%f,\n%s  %fModules%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	else{
		bool f=false;
		for (size_t j=0;j<ln-1;j++){
			if (*(rpo+j)==o->p){
				f=true;
				break;
			}
		}
		if (f==false){
			KlDebug_print_ast_scope(o->p,i+2,rpo);
			KlIo_printf("\n%s  %fModules%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		else{
			KlIo_printf("%f(Recurive)%f,\n%s  %fModules%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
	}
	if (o->ml==0){
		KlIo_printf("%f(None)%f,\n",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	else{
		KlIo_printf("{\n");
		for (size_t j=0;j<o->ml;j++){
			KlIo_printf("%s    {\n%s      %fName%f: %f%s%f,\n%s      %fModule%f: ",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_IDENTIFIER,(*(o->m+j))->nm,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			bool f=false;
			for (size_t k=0;k<ln-1;k++){
				if (*(rpo+k)==*(o->m+j)){
					f=true;
					break;
				}
			}
			if (f==false){
				KlDebug_print_ast_module(*(o->m+j),i+6,rpo);
			}
			else{
				KlIo_printf("%f(Recurive)%f,\n",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
			KlIo_printf("%s    },\n",is);
		}
		KlIo_printf("%s  },\n",is);
	}
	if (o->t==AST_SCOPE_TYPE_DEFAULT){
		KlIo_printf("%s  %fVariables%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		if (o->vl==0){
			KlIo_printf("%f(None)%f,\n%s  %fFunctions%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		else{
			KlIo_printf("{\n");
			for (size_t j=0;j<o->vl;j++){
				KlIo_printf("%s    {\n%s      %fName%f: %f%s%f,\n%s      %fModifiers%f:%f",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_IDENTIFIER,*(o->vnm+j),COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_MODIFIER);
				if (*(o->vm+j)==0){
					KlIo_printf(" %f(None)",COLOR_DEBUG_UTILS_TYPE);
				}
				else{
					if ((*(o->vm+j)&OBJECT_MODIFIER_ASSIGNED)!=0){
						KlIo_printf(" <assigned>");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_PUBLIC)!=0){
						KlIo_printf(" public");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_PRIVATE)!=0){
						KlIo_printf(" private");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_STATIC)!=0){
						KlIo_printf(" static");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_EXPORT)!=0){
						KlIo_printf(" export");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_FROZEN)!=0){
						KlIo_printf(" frozen");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_FROZENTYPE)!=0){
						KlIo_printf(" frozentype");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_CONST)!=0){
						KlIo_printf(" const");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_OPERATOR)!=0){
						KlIo_printf(" operator");
					}
				}
				KlIo_printf("%f,\n%s      %fReferences%f: %f%S%f,\n%s    },\n",COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,*(o->vrc+j),COLOR_DEBUG_UTILS_PUNCTUATION,is);
			}
			KlIo_printf("%s  },\n%s  %fFunctions%f: ",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		if (o->fl==0){
			KlIo_printf("%f(None)%f,\n%s  %fCode%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		else{
			KlIo_printf("{\n");
			for (size_t j=0;j<o->fl;j++){
				KlIo_printf("%s    {\n%s      %fName%f: %f%s%f,\n%s      %fScope%f: ",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_IDENTIFIER,(*(o->f+j))->nm,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
				bool f=false;
				for (size_t k=0;k<ln-1;k++){
					if (*(rpo+k)==*(o->f+j)){
						f=true;
						break;
					}
				}
				if (f==false){
					KlDebug_print_ast_scope(*(o->f+j),i+6,rpo);
				}
				else{
					KlIo_printf("%f(Recurive)%f,\n",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
				}
				KlIo_printf("%s    },\n",is);
			}
			KlIo_printf("%s  },\n",is);
		}
		if (o->cl==0){
			KlIo_printf("%f(None)%f,\n",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		else{
			KlIo_printf("{\n");
			for (size_t j=0;j<o->cl;j++){
				KlIo_printf("%s    ",is);
				KlDebug_print_ast_expr(*(o->c+j),i+4,rpo);
			}
			KlIo_printf("%s  },\n",is);
		}
	}
	else if (o->t==AST_SCOPE_TYPE_FUNCTION){
		KlIo_printf("%s  %fArguments%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		if (o->al==0){
			KlIo_printf("%f(None)%f,\n%s  %fVariables%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		else{
			KlIo_printf("{\n");
			for (size_t i=0;i<o->al;i++){
				if (i==o->al-1&&(o->md&OBJECT_MODIFIER_VARARG)!=0){
					KlIo_printf("%s    %f...%f%s %f(Vararg)%f,\n",is,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_IDENTIFIER,*(o->anm+i),COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
				}
				else{
					KlIo_printf("%s    %f%s%f,\n",is,COLOR_DEBUG_UTILS_IDENTIFIER,*(o->anm+i),COLOR_DEBUG_UTILS_PUNCTUATION);
				}
			}
			KlIo_printf("%s  },\n%s  %fVariables%f: ",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		if (o->vl==0){
			KlIo_printf("%f(None)%f,\n%s  %fFunctions%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		else{
			KlIo_printf("{\n");
			for (size_t j=0;j<o->vl;j++){
				assert(**(o->vnm+j)!='\n');
				KlIo_printf("%s    {\n%s      %fName%f: %f%s%f,\n%s      %fModifiers%f:%f",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_IDENTIFIER,*(o->vnm+j),COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_MODIFIER);
				if (*(o->vm+j)==0){
					KlIo_printf(" %f(None)",COLOR_DEBUG_UTILS_TYPE);
				}
				else{
					if ((*(o->vm+j)&OBJECT_MODIFIER_ASSIGNED)!=0){
						KlIo_printf(" <assigned>");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_PUBLIC)!=0){
						KlIo_printf(" public");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_PRIVATE)!=0){
						KlIo_printf(" private");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_STATIC)!=0){
						KlIo_printf(" static");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_EXPORT)!=0){
						KlIo_printf(" export");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_FROZEN)!=0){
						KlIo_printf(" frozen");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_FROZENTYPE)!=0){
						KlIo_printf(" frozentype");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_CONST)!=0){
						KlIo_printf(" const");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_OPERATOR)!=0){
						KlIo_printf(" operator");
					}
				}
				KlIo_printf("%f,\n%s      %fReferences%f: %f%S%f,\n%s    },\n",COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,*(o->vrc+j),COLOR_DEBUG_UTILS_PUNCTUATION,is);
			}
			KlIo_printf("%s  },\n%s  %fFunctions%f: ",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		if (o->fl==0){
			KlIo_printf("%f(None)%f,\n%s  %fCode%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		else{
			KlIo_printf("{\n");
			for (size_t j=0;j<o->fl;j++){
				KlIo_printf("%s    {\n%s      %fName%f: %f%s%f,\n%s      %fScope%f: ",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_IDENTIFIER,(*(o->f+j))->nm,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
				bool f=false;
				for (size_t k=0;k<ln-1;k++){
					if (*(rpo+k)==*(o->f+j)){
						f=true;
						break;
					}
				}
				if (f==false){
					KlDebug_print_ast_scope(*(o->f+j),i+6,rpo);
				}
				else{
					KlIo_printf("%f(Recurive)%f,\n",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
				}
				KlIo_printf("%s    },\n",is);
			}
			KlIo_printf("%s  },\n",is);
		}
		if (o->cl==0){
			KlIo_printf("%f(None)%f,\n",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		else{
			KlIo_printf("{\n");
			for (size_t j=0;j<o->cl;j++){
				KlIo_printf("%s    ",is);
				KlDebug_print_ast_expr(*(o->c+j),i+4,rpo);
			}
			KlIo_printf("%s  },\n",is);
		}
	}
	else if (o->t==AST_SCOPE_TYPE_CLASS){
		KlIo_printf("%s  %fClass Variables%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		if (o->vl==0){
			KlIo_printf("%f(None)%f,\n%s  %fClass Functions%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		else{
			bool f=false;
			for (size_t j=0;j<o->vl;j++){
				if ((*(o->vm+j)&OBJECT_MODIFIER_STATIC)==0){
					continue;
				}
				if (f==false){
					KlIo_printf("{\n");
				}
				f=true;
				KlIo_printf("%s    {\n%s      %fName%f: %f%s%f,\n%s      %fModifiers%f:%f",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_IDENTIFIER,*(o->vnm+j),COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_MODIFIER);
				if (*(o->vm+j)==0){
					KlIo_printf(" %f(None)",COLOR_DEBUG_UTILS_TYPE);
				}
				else{
					if ((*(o->vm+j)&OBJECT_MODIFIER_ASSIGNED)!=0){
						KlIo_printf(" <assigned>");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_PUBLIC)!=0){
						KlIo_printf(" public");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_PRIVATE)!=0){
						KlIo_printf(" private");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_STATIC)!=0){
						KlIo_printf(" static");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_EXPORT)!=0){
						KlIo_printf(" export");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_FROZEN)!=0){
						KlIo_printf(" frozen");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_FROZENTYPE)!=0){
						KlIo_printf(" frozentype");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_CONST)!=0){
						KlIo_printf(" const");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_OPERATOR)!=0){
						KlIo_printf(" operator");
					}
				}
				KlIo_printf("%f,\n%s      %fReferences%f: %f%S%f,\n%s    },\n",COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,*(o->vrc+j),COLOR_DEBUG_UTILS_PUNCTUATION,is);
			}
			if (f==true){
				KlIo_printf("%s  },\n%s  %fClass Functions%f: ",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
			else{
				KlIo_printf("%f(None)%f,\n%s  %fClass Functions%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
		}
		if (o->fl==0){
			KlIo_printf("%f(None)%f,\n%s  %fInstance Variables%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		else{
			bool f=false;
			for (size_t j=0;j<o->fl;j++){
				if (((*(o->f+j))->md&OBJECT_MODIFIER_STATIC)==0){
					continue;
				}
				if (f==false){
					KlIo_printf("{\n");
				}
				f=true;
				KlIo_printf("%s    {\n%s      %fName%f: %f%s%f,\n%s      %fScope%f: ",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_IDENTIFIER,(*(o->f+j))->nm,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
				bool fp=false;
				for (size_t k=0;k<ln-1;k++){
					if (*(rpo+k)==*(o->f+j)){
						fp=true;
						break;
					}
				}
				if (fp==false){
					KlDebug_print_ast_scope(*(o->f+j),i+6,rpo);
				}
				else{
					KlIo_printf("%f(Recurive)%f,\n",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
				}
				KlIo_printf("%s    },\n",is);
			}
			if (f==true){
				KlIo_printf("%s  },\n%s  %fInstance Variables%f: ",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
			else{
				KlIo_printf("%f(None)%f,\n%s  %fInstance Variables%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
		}
		if (o->vl==0){
			KlIo_printf("%f(None)%f,\n%s  %fInstance Functions%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		else{
			bool f=false;
			for (size_t j=0;j<o->vl;j++){
				if ((*(o->vm+j)&OBJECT_MODIFIER_STATIC)!=0){
					continue;
				}
				if (f==false){
					KlIo_printf("{\n");
				}
				f=true;
				KlIo_printf("%s    {\n%s      %fName%f: %f%s%f,\n%s      %fModifiers%f:%f",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_IDENTIFIER,*(o->vnm+j),COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_MODIFIER);
				if (*(o->vm+j)==0){
					KlIo_printf(" %f(None)",COLOR_DEBUG_UTILS_TYPE);
				}
				else{
					if ((*(o->vm+j)&OBJECT_MODIFIER_ASSIGNED)!=0){
						KlIo_printf(" <assigned>");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_PUBLIC)!=0){
						KlIo_printf(" public");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_PRIVATE)!=0){
						KlIo_printf(" private");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_STATIC)!=0){
						KlIo_printf(" static");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_EXPORT)!=0){
						KlIo_printf(" export");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_FROZEN)!=0){
						KlIo_printf(" frozen");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_FROZENTYPE)!=0){
						KlIo_printf(" frozentype");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_CONST)!=0){
						KlIo_printf(" const");
					}
					if ((*(o->vm+j)&OBJECT_MODIFIER_OPERATOR)!=0){
						KlIo_printf(" operator");
					}
				}
				KlIo_printf("%f,\n%s      %fReferences%f: %f%S%f,\n%s    },\n",COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,*(o->vrc+j),COLOR_DEBUG_UTILS_PUNCTUATION,is);
			}
			if (f==true){
				KlIo_printf("%s  },\n%s  %fInstance Functions%f: ",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
			else{
				KlIo_printf("%f(None)%f,\n%s  %fInstance Functions%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
		}
		if (o->fl==0){
			KlIo_printf("%f(None)%f,\n",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
		}
		else{
			bool f=false;
			for (size_t j=0;j<o->fl;j++){
				if (((*(o->f+j))->md&OBJECT_MODIFIER_STATIC)!=0){
					continue;
				}
				if (f==false){
					KlIo_printf("{\n");
				}
				f=true;
				KlIo_printf("%s    {\n%s      %fName%f: %f%s%f,\n%s      %fScope%f: ",is,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_IDENTIFIER,(*(o->f+j))->nm,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
				bool fp=false;
				for (size_t k=0;k<ln-1;k++){
					if (*(rpo+k)==*(o->f+j)){
						fp=true;
						break;
					}
				}
				if (fp==false){
					KlDebug_print_ast_scope(*(o->f+j),i+6,rpo);
				}
				else{
					KlIo_printf("%f(Recurive)%f,\n",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
				}
				KlIo_printf("%s    },\n",is);
			}
			if (f==true){
				KlIo_printf("%s  },\n",is);
			}
			else{
				KlIo_printf("%f(None)%f,\n",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
		}
	}
	else{
		assert(0);
	}
	if (i==0){
		KlIo_printf("%f};%f\n",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_RESET);
	}
	else{
		KlIo_printf("%s%f},\n",is,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	KlMem_free(is);
	KlMem_free(rpo);
	return();
}



void KlDebug_print_ast_expr(struct ASTExpression* e,unsigned char i,void** rp){
	KlMem_enter_func();
	size_t ln=2;
	if (rp!=NULL){
		for (size_t j=0;true;j++){
			if (*(rp+j)==NULL){
				break;
			}
			ln++;
		}
	}
	void** rpo=KlMem_malloc(ln*sizeof(void*));
	for (size_t j=0;j<ln-2;j++){
		*(rpo+j)=*(rp+j);
	}
	*(rpo+ln-2)=e;
	*(rpo+ln-1)=NULL;
	char* is=KlMem_malloc(i+1);
	for (unsigned long j=0;j<i;j++){
		*(is+j)=' ';
	}
	*(is+i)=0;
	if (i==0){
		KlIo_printf("%fASTExpression %f= {\n",COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	else{
		KlIo_printf("%f{\n",COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	switch (e->t){
		default:
		case AST_EXPRESSION_TYPE_UNKNOWN:
			KlIo_printf("%s  %fType%f: %f<unknown> %f(Unknown)%f,\n",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			break;
		case AST_EXPRESSION_TYPE_EMPTY:
			KlIo_printf("%s  %fType%f: %f<empty> %f(Empty)%f,\n",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			break;
		case AST_EXPRESSION_TYPE_CONST:
			KlIo_printf("%s  %fType%f: %f<const> %f(Const)%f,\n%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_LST:
			KlIo_printf("%s  %fType%f: %f(...) %f(Tuple)%f,\n",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			assert(0);
			break;
		case AST_EXPRESSION_TYPE_ARR:
			KlIo_printf("%s  %fType%f: %f[...] %f(Array)%f,\n",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			assert(0);
			break;
		case AST_EXPRESSION_TYPE_EXPAND:
			KlIo_printf("%s  %fType%f: %f... %f(Expand)%f,\n%s  %fVariable%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_INC:
			KlIo_printf("%s  %fType%f: %f++ %f(Increment)%f,\n%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_DEC:
			KlIo_printf("%s  %fType%f: %f-- %f(Decrement)%f,\n%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_CALL:
			KlIo_printf("%s  %fType%f: %f() %f(Function Call)%f,\n%s  %fFunc%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			if (e->bl==0){
				KlIo_printf("%s  %fArguments%f: %f(None)%f,\n",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
			else if (e->b==NULL){
				KlIo_printf("%s  %fArguments%f: %f(Null)%f,\n",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
			else{
				for (unsigned long k=0;k<e->bl;k++){
					KlIo_printf("%s  %fArgument#%i%f: ",is,COLOR_DEBUG_UTILS_KEY,k+1,COLOR_DEBUG_UTILS_PUNCTUATION);
					KlDebug_print_ast_expr_arg(e->b+k,i+2,rpo);
				}
			}
			break;
		case AST_EXPRESSION_TYPE_SLC:
			KlIo_printf("%s  %fType%f: %f[::] %f(Array Slice)%f,\n",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			break;
		case AST_EXPRESSION_TYPE_ACS:
			KlIo_printf("%s  %fType%f: %f. %f(Attribute Access)%f,\n%s  %fObject%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fAttribute%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_PLUS:
			KlIo_printf("%s  %fType%f: %f+ %f(Unary Plus)%f,\n%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_MINUS:
			KlIo_printf("%s  %fType%f: %f- %f(Unary Minus)%f,\n%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_NOT:
			KlIo_printf("%s  %fType%f: %f~ %f(Bitwise Not)%f,\n%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_BNOT:
			KlIo_printf("%s  %fType%f: %f! %f(Logical Not)%f,\n%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_POW:
			KlIo_printf("%s  %fType%f: %f** %f(Exponentiation)%f,\n%s  %fBase%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fExponent%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_ROOT:
			KlIo_printf("%s  %fType%f: %f*/ %f(Nth Root)%f,\n%s  %fRadicant%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fDegree%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_IROOT:
			KlIo_printf("%s  %fType%f: %f*// %f(Nth Integer Root)%f,\n%s  %fRadicant%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fDegree%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_MLT:
			KlIo_printf("%s  %fType%f: %f* %f(Multiplication)%f,\n%s  %fMultiplier%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fMultiplicand%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_DIV:
			KlIo_printf("%s  %fType%f: %f/ %f(Division)%f,\n%s  %fDividend%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fDivisor%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_FDIV:
			KlIo_printf("%s  %fType%f: %f// %f(Floor Division)%f,\n%s  %fDividend%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fDivisor%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_MOD:
			KlIo_printf("%s  %fType%f: %f%% %f(Modulo)%f,\n%s  %fDividend%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fDivisor%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_MMLT:
			KlIo_printf("%s  %fType%f: %f@ %f(Matrix Multiplication)%f,\n%s  %fMultiplier Matrix%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fMultiplicand Matrix%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_ADD:
			KlIo_printf("%s  %fType%f: %f+ %f(Addition)%f,\n%s  %fLeft Term%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fRight Term%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_SUB:
			KlIo_printf("%s  %fType%f: %f- %f(Subtraction)%f,\n%s  %fLeft Term%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fRight Term%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_LSH:
			KlIo_printf("%s  %fType%f: %f<< %f(Bitwise Left Shift)%f,\n%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fOffset%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_RSH:
			KlIo_printf("%s  %fType%f: %f>> %f(Bitwise Right Shift)%f,\n%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fOffset%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_LT:
			KlIo_printf("%s  %fType%f: %f< %f(Less Than)%f,\n%s  %fLeft Expression%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fRight Expression%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_LE:
			KlIo_printf("%s  %fType%f: %f<= %f(Less Or Equal)%f,\n%s  %fLeft Expression%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fRight Expression%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_GT:
			KlIo_printf("%s  %fType%f: %f> %f(Greather Than)%f,\n%s  %fLeft Expression%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fRight Expression%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_GE:
			KlIo_printf("%s  %fType%f: %f>= %f(Greather Or Equal)%f,\n%s  %fLeft Expression%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fRight Expression%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_EQ:
			KlIo_printf("%s  %fType%f: %f== %f(Equal)%f,\n%s  %fLeft Expression%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fRight Expression%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_NE:
			KlIo_printf("%s  %fType%f: %f!= %f(Not Equal)%f,\n%s  %fLeft Expression%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fRight Expression%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_BAND:
			KlIo_printf("%s  %fType%f: %f& %f(Bitwise And)%f,\n%s  %fLeft Value%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fRight Value%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_BXOR:
			KlIo_printf("%s  %fType%f: %f^ %f(Bitwise Xor)%f,\n%s  %fLeft Value%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fRight Value%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_BOR:
			KlIo_printf("%s  %fType%f: %f| %f(Bitwise Or)%f,\n%s  %fLeft Value%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fRight Value%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_AND:
			KlIo_printf("%s  %fType%f: %f&& %f(Logical And)%f,\n%s  %fLeft Expression%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fRight Expression%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_OR:
			KlIo_printf("%s  %fType%f: %f|| %f(Logical Or)%f,\n%s  %fLeft Expression%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fRight Expression%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_TCOND:
			KlIo_printf("%s  %fType%f: %f(?:) %f(Ternary Conditional)%f,\n%s  %fCondition%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fExpression - True%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			KlIo_printf("%s  %fExpression - False%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b+1,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_EQU:
			KlIo_printf("%s  %fType%f: %f= %f(Assignment)%f,\n%s  %fVariable%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_ADD_EQU:
			KlIo_printf("%s  %fType%f: %f+= %f(Assignment by Sum)%f,\n%s  %fVariable%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_SUB_EQU:
			KlIo_printf("%s  %fType%f: %f-= %f(Assignment by Diffrence)%f,\n%s  %fVariable%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_MLT_EQU:
			KlIo_printf("%s  %fType%f: %f*= %f(Assignment by Product)%f,\n%s  %fVariable%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_DIV_EQU:
			KlIo_printf("%s  %fType%f: %f/= %f(Assignment by Quotient)%f,\n%s  %fVariable%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_FDIV_EQU:
			KlIo_printf("%s  %fType%f: %f//= %f(Assignment by Floored Quotient)%f,\n%s  %fVariable%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_MOD_EQU:
			KlIo_printf("%s  %fType%f: %f%= %f(Assignment by Remainder)%f,\n%s  %fVariable%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_MMLT_EQU:
			KlIo_printf("%s  %fType%f: %f@= %f(Assignment by Matrix Product)%f,\n%s  %fVariable%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_LSH_EQU:
			KlIo_printf("%s  %fType%f: %f<<= %f(Assignment by Bitwise Left Shift)%f,\n%s  %fVariable%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_RSH_EQU:
			KlIo_printf("%s  %fType%f: %f>>= %f(Assignment by Bitwise Right Shift)%f,\n%s  %fVariable%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_BAND_EQU:
			KlIo_printf("%s  %fType%f: %f&= %f(Assignment by Bitwise And)%f,\n%s  %fVariable%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_BXOR_EQU:
			KlIo_printf("%s  %fType%f: %f^= %f(Assignment by Bitwise Xor)%f,\n%s  %fVariable%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
		case AST_EXPRESSION_TYPE_BOR_EQU:
			KlIo_printf("%s  %fType%f: %f|= %f(Assignment by Bitwise Or)%f,\n%s  %fVariable%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_OPERATOR,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(&e->a,i+2,rpo);
			KlIo_printf("%s  %fValue%f: ",is,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlDebug_print_ast_expr_arg(e->b,i+2,rpo);
			break;
	}
	if (i==0){
		KlIo_printf("%f};%f\n",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_RESET);
	}
	else{
		KlIo_printf("%s%f},\n",is,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	KlMem_free(is);
	KlMem_free(rpo);
	return();
}



void KlDebug_print_ast_expr_arg(struct ASTExpressionArg* ea,unsigned char i,void** rp){
	KlMem_enter_func();
	size_t ln=2;
	if (rp!=NULL){
		for (size_t j=0;true;j++){
			if (*(rp+j)==NULL){
				break;
			}
			ln++;
		}
	}
	void** rpo=KlMem_malloc(ln*sizeof(void*));
	for (size_t j=0;j<ln-2;j++){
		*(rpo+j)=*(rp+j);
	}
	*(rpo+ln-2)=ea;
	*(rpo+ln-1)=NULL;
	if (ea==NULL){
		KlIo_printf("%fNULL%f,\n",COLOR_DEBUG_UTILS_NULL,COLOR_DEBUG_UTILS_PUNCTUATION);
		return();
	}
	switch (ea->t){
		default:
		case AST_EXPRESSION_ARG_TYPE_UNKNOWN:
			KlIo_printf("%f(Unknown)%f,\n",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			break;
		case AST_EXPRESSION_ARG_TYPE_EXPRESSION:
			if (ea->v.ex==NULL){
				KlIo_printf("%fNULL %f(Expression)%f,\n",COLOR_DEBUG_UTILS_NULL,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
			else{
				bool f=false;
				for (size_t i=0;i<ln-1;i++){
					if (*(rpo+i)==ea->v.ex){
						f=true;
						break;
					}
				}
				if (f==false){
					KlDebug_print_ast_expr(ea->v.ex,i,rpo);
				}
				else{
					KlIo_printf("%f(Recursion)%f,\n",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
				}
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_SCOPE:
			bool f=false;
			for (size_t i=0;i<ln-1;i++){
				if (*(rpo+i)==ea->v.sc){
					f=true;
					break;
				}
			}
			if (f==false){
				KlDebug_print_ast_scope(ea->v.sc,i,rpo);
			}
			else{
				KlIo_printf("%f(Recursion)%f,\n",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_CHAR:
			char* cs=KlDebug_print_char(ea->v.c);
			KlIo_printf("%s %f(Char)%f,\n",cs,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlMem_free(cs);
			break;
		case AST_EXPRESSION_ARG_TYPE_STRING:
			if (ea->v.s==NULL){
				KlIo_printf("%fNULL %f(String)%f,\n",COLOR_DEBUG_UTILS_NULL,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
			else{
				char* ss=KlDebug_print_string(ea->v.s);
				KlIo_printf("%s %f(String)%f,\n",ss,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
				KlMem_free(ss);
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_INT:
			if (ea->v.n==NULL){
				KlIo_printf("%fNULL %f(Number)%f,\n",COLOR_DEBUG_UTILS_NULL,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
			else{
				char* ns=KlBigInt_print(ea->v.n);
				KlIo_printf("%f%s %f(Number)%f,\n",COLOR_DEBUG_UTILS_INT,ns,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
				KlMem_free(ns);
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_FLOAT:
			if (ea->v.d==NULL){
				KlIo_printf("%fNULL %f(Decimal)%f,\n",COLOR_DEBUG_UTILS_NULL,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
			else{
				KlIo_printf("%f%p %f(Decimal)%f,\n",COLOR_DEBUG_UTILS_POINTER,ea->v.d,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_FUNCTION:
			if (ea->v.f==NULL){
				KlIo_printf("%fNULL %f(Function)%f,\n",COLOR_DEBUG_UTILS_NULL,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
			else{
				KlIo_printf("%f%p %f(Function)%f,\n",COLOR_DEBUG_UTILS_POINTER,ea->v.f,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_NATIVE_FUNCTION:
			switch (ea->v.nf.t){
				default:
				case NATIVE_FUNCITON_TYPE_UNKNOWN:
					KlIo_printf("%fNULL %f(Native Function -> Unknown)%f,\n",COLOR_DEBUG_UTILS_NULL,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
					break;
				case NATIVE_FUNCITON_TYPE_0_ARGS:
					KlIo_printf("%f%p %f(Native Function -> 0 Args)%f,\n",COLOR_DEBUG_UTILS_POINTER,ea->v.nf.f._0a,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
					break;
				case NATIVE_FUNCITON_TYPE_1_ARG:
					KlIo_printf("%f%p %f(Native Function -> 1 Arg)%f,\n",COLOR_DEBUG_UTILS_POINTER,ea->v.nf.f._1a,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
					break;
				case NATIVE_FUNCITON_TYPE_ARGS:
					KlIo_printf("%f%p %f(Native Function -> Args)%f,\n",COLOR_DEBUG_UTILS_POINTER,ea->v.nf.f.a,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
					break;
				case NATIVE_FUNCITON_TYPE_VARARG:
					KlIo_printf("%f%p %f(Native Function -> Var Args)%f,\n",COLOR_DEBUG_UTILS_POINTER,ea->v.nf.f.va,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
					break;
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_IDENTIFIER:
			if (ea->v.i==NULL){
				KlIo_printf("%fNULL %f(Identifier)%f,\n",COLOR_DEBUG_UTILS_NULL,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
			else{
				KlIo_printf("%f%s %f(Identifier)%f,\n",COLOR_DEBUG_UTILS_IDENTIFIER,ea->v.i,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
			break;
		case AST_EXPRESSION_ARG_TYPE_MODIFIERS:
			KlIo_printf("%f",COLOR_DEBUG_UTILS_MODIFIER);
			if (ea->v.m==0){
				KlIo_printf("%f(None) ",COLOR_DEBUG_UTILS_TYPE);
			}
			else{
				if ((ea->v.m&OBJECT_MODIFIER_ASSIGNED)!=0){
					KlIo_printf("<assigned> ");
				}
				if ((ea->v.m&OBJECT_MODIFIER_PUBLIC)!=0){
					KlIo_printf("public ");
				}
				if ((ea->v.m&OBJECT_MODIFIER_PRIVATE)!=0){
					KlIo_printf("private ");
				}
				if ((ea->v.m&OBJECT_MODIFIER_STATIC)!=0){
					KlIo_printf("static ");
				}
				if ((ea->v.m&OBJECT_MODIFIER_EXPORT)!=0){
					KlIo_printf("export ");
				}
				if ((ea->v.m&OBJECT_MODIFIER_FROZEN)!=0){
					KlIo_printf("frozen ");
				}
				if ((ea->v.m&OBJECT_MODIFIER_FROZENTYPE)!=0){
					KlIo_printf("frozentype ");
				}
				if ((ea->v.m&OBJECT_MODIFIER_CONST)!=0){
					KlIo_printf("const ");
				}
				if ((ea->v.m&OBJECT_MODIFIER_OPERATOR)!=0){
					KlIo_printf("operator ");
				}
			}
			KlIo_printf("%f(Modifiers)%f,\n",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			break;
		case AST_EXPRESSION_ARG_TYPE_OBJECT:
			if (ea->v.o==NULL){
				KlIo_printf("%fNULL %f(Object)%f,\n",COLOR_DEBUG_UTILS_NULL,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
			else{
				KlIo_printf("%f%p %f(Object)%f,\n",COLOR_DEBUG_UTILS_POINTER,ea->v.o,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
			}
			break;
	}
	KlMem_free(rpo);
	return();
}



void KlDebug_print_unparsed_ast_expr(struct UnparsedASTExpression* e,unsigned char i,void** rp){
	KlMem_enter_func();
	size_t ln=2;
	if (rp!=NULL){
		for (size_t j=0;true;j++){
			if (*(rp+j)==NULL){
				break;
			}
			ln++;
		}
	}
	void** rpo=KlMem_malloc(ln*sizeof(void*));
	for (size_t j=0;j<ln-2;j++){
		*(rpo+j)=*(rp+j);
	}
	*(rpo+ln-2)=e;
	*(rpo+ln-1)=NULL;
	char* is=KlMem_malloc(i+1);
	for (unsigned long j=0;j<i;j++){
		*(is+j)=' ';
	}
	*(is+i)=0;
	if (i==0){
		KlIo_printf("%fUnparsedASTExpression %f= {\n",COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	else{
		KlIo_printf("%s%f{\n",is,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	for (struct UnparsedASTExpressionElem* j=e->e;j<e->e+e->l;j++){
		switch (j->t){
			default:
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_UNKNOWN:
				KlIo_printf("%s  %f(Undefined)%f,\n",is,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_EXPRESSION:
				bool f=false;
				for (size_t k=0;k<ln-1;k++){
					if (*(rpo+k)==j->v.ex){
						f=true;
						break;
					}
				}
				if (f==false){
					KlDebug_print_unparsed_ast_expr(j->v.ex,i+2,rpo);
				}
				else{
					KlIo_printf("%f(Recursion)%f,\n",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
				}
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_OPERATOR:
				KlIo_printf("%s  %f",is,COLOR_DEBUG_UTILS_OPERATOR);
				switch (j->v.op){
					default:
					case AST_EXPRESSION_TYPE_UNKNOWN:
						KlIo_printf("<unknown>");
						assert(0);
						break;
					case AST_EXPRESSION_TYPE_LST:
						KlIo_printf("(...)");
						break;
					case AST_EXPRESSION_TYPE_ARR:
						KlIo_printf("[...]");
						break;
					case AST_EXPRESSION_TYPE_EXPAND:
						KlIo_printf("...");
						break;
					case AST_EXPRESSION_TYPE_INC:
						KlIo_printf("++");
						break;
					case AST_EXPRESSION_TYPE_DEC:
						KlIo_printf("--");
						break;
					case AST_EXPRESSION_TYPE_CALL:
						KlIo_printf("()");
						break;
					case AST_EXPRESSION_TYPE_SLC:
						KlIo_printf("[::]");
						break;
					case AST_EXPRESSION_TYPE_ACS:
						KlIo_printf(".");
						break;
					case AST_EXPRESSION_TYPE_PLUS:
						KlIo_printf("+ (U)");
						break;
					case AST_EXPRESSION_TYPE_MINUS:
						KlIo_printf("- (U)");
						break;
					case AST_EXPRESSION_TYPE_NOT:
						KlIo_printf("!");
						break;
					case AST_EXPRESSION_TYPE_BNOT:
						KlIo_printf("~");
						break;
					case AST_EXPRESSION_TYPE_POW:
						KlIo_printf("**");
						break;
					case AST_EXPRESSION_TYPE_ROOT:
						KlIo_printf("*/");
						break;
					case AST_EXPRESSION_TYPE_IROOT:
						KlIo_printf("*//");
						break;
					case AST_EXPRESSION_TYPE_MLT:
						KlIo_printf("*");
						break;
					case AST_EXPRESSION_TYPE_DIV:
						KlIo_printf("/");
						break;
					case AST_EXPRESSION_TYPE_FDIV:
						KlIo_printf("//");
						break;
					case AST_EXPRESSION_TYPE_MOD:
						KlIo_printf("%%");
						break;
					case AST_EXPRESSION_TYPE_MMLT:
						KlIo_printf("@");
						break;
					case AST_EXPRESSION_TYPE_ADD:
						KlIo_printf("+");
						break;
					case AST_EXPRESSION_TYPE_SUB:
						KlIo_printf("-");
						break;
					case AST_EXPRESSION_TYPE_LSH:
						KlIo_printf("<<");
						break;
					case AST_EXPRESSION_TYPE_RSH:
						KlIo_printf(">>");
						break;
					case AST_EXPRESSION_TYPE_LT:
						KlIo_printf("<");
						break;
					case AST_EXPRESSION_TYPE_LE:
						KlIo_printf("<=");
						break;
					case AST_EXPRESSION_TYPE_GT:
						KlIo_printf(">");
						break;
					case AST_EXPRESSION_TYPE_GE:
						KlIo_printf(">=");
						break;
					case AST_EXPRESSION_TYPE_EQ:
						KlIo_printf("==");
						break;
					case AST_EXPRESSION_TYPE_NE:
						KlIo_printf("!=");
						break;
					case AST_EXPRESSION_TYPE_BAND:
						KlIo_printf("&");
						break;
					case AST_EXPRESSION_TYPE_BXOR:
						KlIo_printf("^");
						break;
					case AST_EXPRESSION_TYPE_BOR:
						KlIo_printf("|");
						break;
					case AST_EXPRESSION_TYPE_AND:
						KlIo_printf("&&");
						break;
					case AST_EXPRESSION_TYPE_OR:
						KlIo_printf("||");
						break;
					case AST_EXPRESSION_TYPE_TCOND:
						KlIo_printf("?:");
						break;
					case AST_EXPRESSION_TYPE_EQU:
						KlIo_printf("=");
						break;
					case AST_EXPRESSION_TYPE_ADD_EQU:
						KlIo_printf("+=");
						break;
					case AST_EXPRESSION_TYPE_SUB_EQU:
						KlIo_printf("-=");
						break;
					case AST_EXPRESSION_TYPE_MLT_EQU:
						KlIo_printf("*=");
						break;
					case AST_EXPRESSION_TYPE_DIV_EQU:
						KlIo_printf("/=");
						break;
					case AST_EXPRESSION_TYPE_FDIV_EQU:
						KlIo_printf("//=");
						break;
					case AST_EXPRESSION_TYPE_MOD_EQU:
						KlIo_printf("%%=");
						break;
					case AST_EXPRESSION_TYPE_MMLT_EQU:
						KlIo_printf("@=");
						break;
					case AST_EXPRESSION_TYPE_POW_EQU:
						KlIo_printf("**=");
						break;
					case AST_EXPRESSION_TYPE_ROOT_EQU:
						KlIo_printf("*/=");
						break;
					case AST_EXPRESSION_TYPE_IROOT_EQU:
						KlIo_printf("*//=");
						break;
					case AST_EXPRESSION_TYPE_LSH_EQU:
						KlIo_printf("<<=");
						break;
					case AST_EXPRESSION_TYPE_RSH_EQU:
						KlIo_printf(">>=");
						break;
					case AST_EXPRESSION_TYPE_BAND_EQU:
						KlIo_printf("&=");
						break;
					case AST_EXPRESSION_TYPE_BXOR_EQU:
						KlIo_printf("^=");
						break;
					case AST_EXPRESSION_TYPE_BOR_EQU:
						KlIo_printf("|=");
						break;
					case AST_EXPRESSION_TYPE_SEP:
						KlIo_printf(",");
						break;
				}
				KlIo_printf(" %f(Operator)%f,\n",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_CHAR:
				char* cs=KlDebug_print_char(j->v.c);
				KlIo_printf("%s  %s %f(Char)%f,\n",is,cs,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
				KlMem_free(cs);
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_STRING:
				char* ss=KlDebug_print_string(j->v.s);
				KlIo_printf("%s  %s %f(String)%f,\n",is,ss,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
				KlMem_free(ss);
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_INT:
				char* ns=KlBigInt_print(j->v.n);
				KlIo_printf("%s  %f%s %f(Number)%f,\n",is,COLOR_DEBUG_UTILS_INT,ns,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
				KlMem_free(ns);
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_FLOAT:
				KlIo_printf("%s  %f%p %f(Decimal)%f,\n",is,COLOR_DEBUG_UTILS_POINTER,j->v.d,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_NATIVE_FUNCTION:
				switch (j->v.nf.t){
					default:
					case NATIVE_FUNCITON_TYPE_UNKNOWN:
						KlIo_printf("%s  %f(Native Function -> Unknown)%f,\n",is,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
						break;
					case NATIVE_FUNCITON_TYPE_0_ARGS:
						KlIo_printf("%s  %f%p %f(Native Function -> 0 Args)%f,\n",is,COLOR_DEBUG_UTILS_POINTER,j->v.nf.f._0a,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
						break;
					case NATIVE_FUNCITON_TYPE_1_ARG:
						KlIo_printf("%s  %f%p %f(Native Function -> 1 Arg)%f,\n",is,COLOR_DEBUG_UTILS_POINTER,j->v.nf.f._1a,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
						break;
					case NATIVE_FUNCITON_TYPE_ARGS:
						KlIo_printf("%s  %f%p %f(Native Function -> Args)%f,\n",is,COLOR_DEBUG_UTILS_POINTER,j->v.nf.f.a,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
						break;
					case NATIVE_FUNCITON_TYPE_VARARG:
						KlIo_printf("%s  %f%p %f(Native Function -> Var Args)%f,\n",is,COLOR_DEBUG_UTILS_POINTER,j->v.nf.f.va,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
						break;
				}
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_IDENTIFIER:
				KlIo_printf("%s  %f%s %f(Identifier)%f,\n",is,COLOR_DEBUG_UTILS_IDENTIFIER,j->v.i,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_MODIFIERS:
				KlIo_printf("%s  %f%d %f(Modifiers)%f,\n",is,COLOR_DEBUG_UTILS_MODIFIER,j->v.m,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
				break;
			case UNPARSED_AST_EXPRESSION_ELEM_TYPE_OBJECT:
				KlIo_printf("%s  %f%p %f(Object)%f,\n",is,COLOR_DEBUG_UTILS_POINTER,j->v.o,COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION);
				break;
		}
	}
	if (i==0){
		KlIo_printf("};%f\n",COLOR_RESET);
	}
	else{
		KlIo_printf("%s%f},%f\n",is,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_RESET);
	}
	KlMem_free(is);
	KlMem_free(rpo);
	return();
}



void KlDebug_print_bytecode(struct BytecodeData* bdt){
	KlMem_enter_func();
	char* vs;
	switch (bdt->v){
		default:
		case 0x0000:
			vs="Unknown";
			break;
		case 0x0001:
			vs="Alpha v1.0";
			break;
	}
	KlIo_printf("%fBytecodeData %f= {\n  %fFile Magic%f: %f%x %f(%c%c%c%c)%f,\n  %fVersion%f: %f%Z %f(%s)%f,\n  %fString Table Length%f: %f%d%f,\n  %fFunction Table Length%f: %f%w%f,\n  %fResource Table Length%f: %f%b%f,\n  %fImport Table Length%f: %f%w%f,\n  %fExport Table Length%f: %f%w%f,\n  %fFlags%f:%f",COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,bdt->m,COLOR_DEBUG_UTILS_TYPE,(char)((bdt->m>>24)&0xff),(char)((bdt->m>>16)&0xff),(char)((bdt->m>>8)&0xff),(char)(bdt->m&0xff),COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,bdt->v,COLOR_DEBUG_UTILS_TYPE,vs,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,bdt->stl,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,bdt->rtl,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,bdt->rtl,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,bdt->itl,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,bdt->etl,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_MODIFIER);
	if (bdt->f==0){
		KlIo_printf("%f(None)%f,\n  %fString Table%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	else{
		if ((bdt->f&BYTECODE_DATA_FLAG_ENTRY)!=0){
			KlIo_printf(" entry");
		}
		if ((bdt->f&BYTECODE_DATA_FLAG_DEBUG)!=0){
			KlIo_printf(" debug");
		}
		if ((bdt->f&BYTECODE_DATA_FLAG_EMBEDDED)!=0){
			KlIo_printf(" embedded-resource");
		}
		if ((bdt->f&BYTECODE_DATA_FLAG_EMBEDDED_SRC)!=0){
			KlIo_printf(" embedded-src");
		}
		KlIo_printf("%f,\n  %fString Table%f: ",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	if (bdt->stl>0){
		KlIo_printf("{\n");
		for (uint32_t i=0,j=0;i<bdt->stl-1;j++){
			size_t dsl=0;
			char* ds=KlBytecode_decode_string(bdt->st+i,&dsl);
			char* ps=KlDebug_print_string_len(ds,dsl);
			KlMem_free(ds);
			KlIo_printf("    %f#%d %f(%d)%f: %s%f,\n",COLOR_DEBUG_UTILS_KEY,j,COLOR_DEBUG_UTILS_TYPE,i,COLOR_DEBUG_UTILS_PUNCTUATION,ps,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlMem_free(ps);
			i+=(uint32_t)str_len(bdt->st+i)+1;
		}
		KlIo_printf("  },\n  %fFunction Table%f: ",COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	else{
		KlIo_printf("%f(Empty)%f,\n  %fFunction Table%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	if (bdt->ftl>0){
		KlIo_printf("{\n");
		assert(0);
		KlIo_printf("  },\n  %fResource Table%f: ",COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	else{
		KlIo_printf("%f(Empty)%f,\n  %fResource Table%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	if (bdt->rtl>0){
		KlIo_printf("{\n");
		assert(0);
		KlIo_printf("  },\n  %fImport Table%f: ",COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	else{
		KlIo_printf("%f(Empty)%f,\n  %fImport Table%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	if (bdt->itl>0){
		KlIo_printf("{\n");
		for (uint32_t i=0;i<bdt->itl;i++){
			size_t dsl=0;
			char* ds=KlBytecode_decode_string(bdt->st+*(bdt->it+i),&dsl);
			char* ps=KlDebug_print_string_len(ds,dsl);
			KlMem_free(ds);
			KlIo_printf("    {\n      %fString Offset%f: %f%d%f,\n      %fValue%f: %s%f,\n    },\n",COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_INT,*(bdt->it+i),COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION,ps,COLOR_DEBUG_UTILS_PUNCTUATION);
			KlMem_free(ps);
		}
		KlIo_printf("  },\n  %fExport Table%f: ",COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	else{
		KlIo_printf("%f(Empty)%f,\n  %fExport Table%f: ",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	if (bdt->etl>0){
		KlIo_printf("{\n");
		assert(0);
		KlIo_printf("  },\n  %fCode%f: ",COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	else{
		KlIo_printf("%f(Empty)%f,\n  %fCode%f: {\n",COLOR_DEBUG_UTILS_TYPE,COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_DEBUG_UTILS_KEY,COLOR_DEBUG_UTILS_PUNCTUATION);
	}
	if (bdt->cl==0){
		KlIo_printf("    %fnop\n",COLOR_DEBUG_UTILS_INSTRUCTION);
	}
	KlIo_printf("  %f}\n};%f\n",COLOR_DEBUG_UTILS_PUNCTUATION,COLOR_RESET);
	return();
}



char* KlDebug_print_char(char c){
	KlMem_enter_func();
	char* sf=str_escape_ansi(COLOR_DEBUG_UTILS_STRING);
	char* bf=str_escape_ansi(COLOR_DEBUG_UTILS_STRING_BYTE);
	size_t sf_ln=str_len(sf);
	size_t bf_ln=str_len(bf);
	size_t ln=sf_ln+4;
	char* o=KlMem_malloc(ln);
	KlMem_memcpy(o,sf,sf_ln);
	*(o+sf_ln)='\'';
	size_t i=sf_ln+1;
	bool cbf=false;
	if (c<=0x1f||c==0x22||c==0x27||c==0x5c||c==0x7f){
		if (cbf==false){
			ln+=bf_ln;
			o=KlMem_realloc(o,ln);
			KlMem_memcpy(o+i,bf,bf_ln);
			i+=bf_ln;
			cbf=true;
		}
		if (c==0x08||c==0x09||c==0x0a||c==0x0b||c==0x0c||c==0x0d||c==0x1b||c==0x22||c==0x27||c==0x5c){
			ln++;
			o=KlMem_realloc(o,ln);
			*(o+i)='\\';
			i++;
			switch (c){
				case 0x08:
					*(o+i)='b';
					break;
				case 0x09:
					*(o+i)='t';
					break;
				case 0x0a:
					*(o+i)='n';
					break;
				case 0x0b:
					*(o+i)='v';
					break;
				case 0x0c:
					*(o+i)='f';
					break;
				case 0x0d:
					*(o+i)='r';
					break;
				case 0x1b:
					*(o+i)='e';
					break;
				default:
					*(o+i)=c;
					break;
			}
		}
		else{
			ln+=5;
			o=KlMem_realloc(o,ln);
			*(o+i)='<';
			*(o+i+1)='0';
			*(o+i+2)='x';
			*(o+i+3)=c/16+(c/16>=10?87:48);
			*(o+i+4)=c%16+(c%16>=10?87:48);
			*(o+i+5)='>';
			i+=5;
		}
	}
	else{
		if (cbf==true){
			ln+=sf_ln;
			o=KlMem_realloc(o,ln);
			KlMem_memcpy(o+i,sf,sf_ln);
			i+=sf_ln;
			cbf=false;
		}
		*(o+i)=c;
	}
	i++;
	if (cbf==true){
		ln+=sf_ln;
		o=KlMem_realloc(o,ln);
		KlMem_memcpy(o+i,sf,sf_ln);
		i+=sf_ln;
	}
	*(o+i)='\'';
	*(o+i+1)=0;
	assert(i+2==ln);
	KlMem_free(sf);
	KlMem_free(bf);
	return o;
}



char* KlDebug_print_string(char* s){
	KlMem_enter_func();
	return(KlDebug_print_string_len(s,str_len(s)));
}



char* KlDebug_print_string_len(char* s,size_t s_ln){
	KlMem_enter_func();
	char* sf=str_escape_ansi(COLOR_DEBUG_UTILS_STRING);
	char* bf=str_escape_ansi(COLOR_DEBUG_UTILS_STRING_BYTE);
	size_t sf_ln=str_len(sf);
	size_t bf_ln=str_len(bf);
	size_t ln=s_ln+sf_ln+3;
	char* o=KlMem_malloc(ln);
	KlMem_memcpy(o,sf,sf_ln);
	*(o+sf_ln)='"';
	size_t i=sf_ln+1;
	bool cbf=false;
	for (unsigned char* c=s;c<s+s_ln;c++){
		if (*c<=0x1f||*c==0x22||*c==0x27||*c==0x5c||*c==0x7f){
			if (cbf==false){
				ln+=bf_ln;
				o=KlMem_realloc(o,ln);
				KlMem_memcpy(o+i,bf,bf_ln);
				i+=bf_ln;
				cbf=true;
			}
			if (*c==0x08||*c==0x09||*c==0x0a||*c==0x0b||*c==0x0c||*c==0x0d||*c==0x1b||*c==0x22||*c==0x27||*c==0x5c){
				ln++;
				o=KlMem_realloc(o,ln);
				*(o+i)='\\';
				i++;
				switch (*c){
					case 0x08:
						*(o+i)='b';
						break;
					case 0x09:
						*(o+i)='t';
						break;
					case 0x0a:
						*(o+i)='n';
						break;
					case 0x0b:
						*(o+i)='v';
						break;
					case 0x0c:
						*(o+i)='f';
						break;
					case 0x0d:
						*(o+i)='r';
						break;
					case 0x1b:
						*(o+i)='e';
						break;
					default:
						*(o+i)=*c;
						break;
				}
			}
			else{
				ln+=5;
				o=KlMem_realloc(o,ln);
				*(o+i)='<';
				*(o+i+1)='0';
				*(o+i+2)='x';
				*(o+i+3)=*c/16+(*c/16>=10?87:48);
				*(o+i+4)=*c%16+(*c%16>=10?87:48);
				*(o+i+5)='>';
				i+=5;
			}
		}
		else{
			if (cbf==true){
				ln+=sf_ln;
				o=KlMem_realloc(o,ln);
				KlMem_memcpy(o+i,sf,sf_ln);
				i+=sf_ln;
				cbf=false;
			}
			*(o+i)=*c;
		}
		i++;
	}
	if (cbf==true){
		ln+=sf_ln;
		o=KlMem_realloc(o,ln);
		KlMem_memcpy(o+i,sf,sf_ln);
		i+=sf_ln;
	}
	*(o+i)='"';
	*(o+i+1)=0;
	assert(i+2==ln);
	KlMem_free(sf);
	KlMem_free(bf);
	KlMem_ret(o);
	return(o);
}

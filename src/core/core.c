#include <ast.h>
#include <core.h>
#include <error.h>
#include <platform.h>
#include <sys.h>
#include <free.h>
#include <import.h>
#include <shared.h>
#include <memory.h>
#include <string_utils.h>
#ifndef NDEBUG
#include <io.h>
#include <bytecode.h>
#endif



int KlCore_run_all(int argc,const char** argv){
	(void)argc;
	KlMem_enter_func();
	KlPlatform_setup_console();
	struct COFFFile* cf=KlBytecode_read_from_file("ast.obj");
	// struct CodeFileObject* fo=KlCore_read_file(argv[1],NULL);
	// if (fo==NULL){
	// 	KlError_raise();
	// 	KlPlatform_restore_console();
	// 	return(1);
	// }
	// struct ASTScope* ast=KlAst_parse_ast_all(fo,NULL);
	// if (ast==NULL){
	// 	KlError_raise();
	// 	KlFree_free_code_file_object(*fo);
	// 	KlMem_free(fo);
	// 	KlFree_free_scope(*ast);
	// 	KlMem_free(ast);
	// 	KlPlatform_restore_console();
	// 	return(1);
	// }
	// KlFree_free_code_file_object(*fo);
	// KlMem_free(fo);
	// KlFree_free_scope(*ast);
	// KlMem_free(ast);
	KlPlatform_restore_console();
	return(0);
}



struct CodeFileObject* KlCore_read_file(const char* fp,struct CallStack* cs){
	KlMem_enter_func();
	struct CodeFileObject o;
	o.fp=KlPlatform_get_full_path(fp);
	o.dt=NULL;
	o.l=0;
	if (KlPlatform_file_exists(o.fp)==false){
		KlError_set_error("FileNotFoundError",str_format("File '%s' not Found.",o.fp),cs);
		return(NULL);
	}
	o.dt=KlPlatform_read_file_content(fp,&o.l);
	assert(o.dt!=NULL);
	return(KlMem_const(&o,sizeof(struct CodeFileObject)));
}

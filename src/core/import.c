#include <import.h>
#include <memory.h>
#include <platform.h>
#include <sys.h>
#include <shared.h>
#include <error.h>
#include <string_utils.h>
#include <object.h>
#include <type.h>
#include <ast.h>
#include <core.h>
#include <io.h>



char** _ckl=NULL;
struct ASTScope** _cvl=NULL;
size_t _cll=0;



char* KlImport_find_module(struct CodeFileObject* fo,const char* nm,struct CallStack* cs){
	KlMem_enter_func();
	size_t i=0;
	size_t j=0;
	size_t sz=0;
	size_t ln=str_len(nm);
	for (i=0;i<_cll;i++){
		if (str_cmp_sub(nm,*(_ckl+i),0,ln)==true){
			return str_clone(nm);
		}
	}
	size_t pln=str_len(KlSys_import_path);
	char* bf=NULL;
	assert(ln!=0);
	i=0;
	while (i<=pln){
		if (*(KlSys_import_path+i)==';'){
			if (sz>0&&*(bf+j-1)!='/'&&*(bf+j-1)!='\\'){
				sz++;
				bf=KlMem_realloc(bf,sz+4);
				*(bf+j)='/';
				j++;
			}
			for (size_t k=0;k<ln;k++){
				sz++;
				bf=KlMem_realloc(bf,sz+4);
				*(bf+j)=*(nm+k);
				j++;
			}
			*(bf+j)='.';
			*(bf+j+1)='k';
			*(bf+j+2)='l';
			*(bf+j+3)=0;
			if (KlPlatform_file_exists(bf)==true){
				KlMem_ret(bf);
				return(bf);
			}
			i++;
			j=0;
			sz=0;
			continue;
		}
		sz++;
		bf=KlMem_realloc(bf,sz+4);
		*(bf+j)=*(KlSys_import_path+i);
		j++;
		i++;
	}
	if (bf!=NULL){
		KlMem_free(bf);
	}
	KlError_unimplemented_error();
	return(NULL);
}



struct ASTScope* KlImport_load_module(struct CodeFileObject* fo,const char* fp,struct CallStack* cs){
	KlMem_enter_func();
	size_t ln=str_len(fp);
	for (size_t i=0;i<_cll;i++){
		if (str_cmp_sub(fp,*(_ckl+i),0,ln)==true){
			return(*(_cvl+i));
		}
	}
	struct CodeFileObject* nfo=KlCore_read_file(fp,cs);
	struct ASTScope* o=KlAst_parse_ast_all(nfo,cs);
	size_t nm_l=0;
	char* nm=NULL;
	size_t i=ln-1;
	while (*(fp+i)!='/'&&*(fp+i)!='\\'){
		i--;
		if (i==0){
			break;
		}
	}
	if (*(fp+i)=='/'||*(fp+i)=='\\'){
		i++;
	}
	while (*(fp+i)!='.'){
		nm_l++;
		nm=KlMem_realloc(nm,nm_l+1);
		*(nm+nm_l-1)=*(fp+i);
		i++;
	}
	*(nm+nm_l)=0;
	KlMem_ret(nm);
	KlImport_define_native_module(nm,o);
	return(o);
}



void KlImport_define_native_module(char* nm,struct ASTScope* m){
	KlMem_enter_func();
	_cll++;
	_ckl=KlMem_realloc(_ckl,_cll*sizeof(char*));
	_cvl=KlMem_realloc(_cvl,_cll*sizeof(struct ASTScope*));
	KlMem_ret(_ckl);
	KlMem_ret(_cvl);
	*(_ckl+_cll-1)=nm;
	*(_cvl+_cll-1)=m;
	return();
}

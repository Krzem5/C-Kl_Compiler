#include <import.h>
#include <memory.h>
#include <platform.h>
#include <sys.h>
#include <shared.h>
#include <error.h>
#include <string_utils.h>
#include <object.h>
#include <type.h>
#include <strtype.h>
#include <io.h>



char* KlImport_find_module(struct CodeFileObject* fo,const char* nm,struct CallStack* cs){
	KlMem_enter_func();
	size_t i=0;
	size_t j=0;
	size_t sz=0;
	size_t ln=str_len(nm);
	size_t pln=str_len(KlSys_import_path);
	char* bf=NULL;
	assert(ln!=0);
	while (i<=pln){
		if (*(KlSys_import_path+i)==';'){
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
				KlIo_printf("Found File: '%s'\n",bf);
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
	return(KlMem_const("AAA",4));/*****/
	// KlError_unimplemented_error();
	// return(NULL);
}



struct Module* KlImport_load_module(struct CodeFileObject* fo,const char* fp,struct CallStack* cs){
	KlMem_enter_func();
	return((struct Module*)1);
}

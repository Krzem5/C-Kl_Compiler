#include <import.h>
#include <memory.h>
#include <platform.h>
#include <sys.h>
#include <shared.h>
#include <string_utils.h>
#include <object.h>
#include <types.h>



#include <stdio.h>/////////////



struct Object* KlImport_import_module(const char* nm){
	KlMem_enter_func();
	struct StrObject* o=(struct StrObject*)KlObject_new(StrType);
	size_t i=0;
	size_t j=0;
	size_t sz=0;
	size_t ln=str_len(nm);
	char* bf=NULL;
	while (true){
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
				printf("Found File: '%s'\n",bf);
				o->sz=sz+4;
				o->s=KlMem_const(bf,sz+4);
				KlMem_free(bf);
				return(OBJ(o));
			}
			i=0;
			KlMem_free(bf);
			bf=NULL;
			continue;
		}
		sz++;
		bf=KlMem_realloc(bf,sz+4);
		*(bf+j)=*(KlSys_import_path+i);
		j++;
	}
	KlMem_free(o);
	return(NULL);
}

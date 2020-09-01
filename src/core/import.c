#include <import.h>
#include <memory.h>
#include <platform.h>
#include <sys.h>
#include <types.h>
#include <string_utils.h>



#include <stdio.h>/////////////



struct Object KlImport_import_module(struct Object a){
	KlMem_enter_func();
	struct Object o=NULL_OBJECT;
	if (a.t!=OBJECT_TYPE_STRING){
		return(o);
	}
	unsigned long i=0;
	unsigned long j=0;
	unsigned long sz=0;
	unsigned long ln=str_len(a.v.s);
	char* bf=NULL;
	while (true){
		if (*(KlSys_import_path+i)==';'){
			for (unsigned int k=0;k<ln;k++){
				sz++;
				bf=KlMem_realloc(bf,sz+4);
				*(bf+j)=*(a.v.s+k);
				j++;
			}
			*(bf+j)='.';
			*(bf+j+1)='k';
			*(bf+j+2)='l';
			*(bf+j+3)=0;
			if (KlPlatform_file_exists((const char*)bf)==true){
				o.t=OBJECT_TYPE_STRING;
				printf("Found File: '%s'\n",bf);
				o.v.s=KlMem_const(bf,sz+4);
				KlMem_free(bf);
				return(o);
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
	return(o);
}

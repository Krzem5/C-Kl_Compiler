#include <import.h>
#include <memory.h>
#include <platform.h>
#include <sys.h>
#include <shared.h>
#include <error.h>
#include <string_utils.h>
#include <type.h>
#include <free.h>
#include <ast.h>
#include <core.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>



struct ASTModule** _cm=NULL;
size_t _cml=0;



char* KlImport_find_module(struct CodeFileObject* fo,const char* nm,struct CallStack* cs){
	KlMem_enter_func();
	size_t i=0;
	size_t j=0;
	size_t sz=0;
	size_t ln=str_len(nm);
	for (i=0;i<_cml;i++){
		if (str_cmp_sub(nm,(*(_cm+i))->nm,0,ln)==true){
			return str_clone((*(_cm+i))->fp);
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



struct ASTModule* KlImport_load_module(struct CodeFileObject* fo,const char* fp,struct CallStack* cs){
	KlMem_enter_func();
	size_t ln=str_len(fp);
	for (size_t i=0;i<_cml;i++){
		if (str_cmp_sub(fp,(*(_cm+i))->fp,0,ln)==true){
			struct ASTModule* o=KlMem_malloc(sizeof(struct ASTModule));
			o->nm=str_clone((*(_cm+i))->nm);
			o->v_nm=str_clone((*(_cm+i))->v_nm);
			o->fp=str_clone((*(_cm+i))->fp);
			o->f=((*(_cm+i))->fl==0?NULL:KlMem_malloc((*(_cm+i))->fl*sizeof(char*)));
			o->fnm=((*(_cm+i))->fl==0?NULL:KlMem_malloc((*(_cm+i))->fl*sizeof(char*)));
			for (size_t j=0;j<(*(_cm+i))->fl;j++){
				KlMem_ret(o->f);
				KlMem_ret(o->fnm);
				*(o->f+j)=str_clone(*((*(_cm+i))->f+j));
				*(o->fnm+j)=str_clone(*((*(_cm+i))->fnm+j));
			}
			o->fl=(*(_cm+i))->fl;
			o->v=((*(_cm+i))->vl==0?NULL:KlMem_malloc((*(_cm+i))->vl*sizeof(char*)));
			o->vnm=((*(_cm+i))->vl==0?NULL:KlMem_malloc((*(_cm+i))->vl*sizeof(char*)));
			for (size_t j=0;j<(*(_cm+i))->vl;j++){
				KlMem_ret(o->v);
				KlMem_ret(o->vnm);
				*(o->v+j)=str_clone(*((*(_cm+i))->v+j));
				*(o->vnm+j)=str_clone(*((*(_cm+i))->vnm+j));
			}
			o->vl=(*(_cm+i))->vl;
			o->src=KlAst_clone_scope((*(_cm+i))->src);
			o->n=(*(_cm+i))->n;
			KlMem_ret(o);
			return(o);
		}
	}
	struct CodeFileObject* nfo=KlCore_read_file(fp,cs);
	struct ASTScope* oa=KlAst_parse_ast_all(nfo,cs);
	KlFree_free_code_file_object(*nfo);
	KlMem_free(nfo);
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
	struct ASTModule* o=KlMem_malloc(sizeof(struct ASTModule));
	o->nm=str_clone(nm);
	o->v_nm=str_clone(nm);
	o->fp=str_clone(fp);
	o->f=NULL;
	o->fnm=NULL;
	o->fl=0;
	for (size_t i=0;i<oa->fl;i++){
		if (((*(oa->vm+i))&OBJECT_MODIFIER_EXPORT)!=0){
			o->fl++;
			o->f=KlMem_realloc(o->f,o->fl*sizeof(char*));
			o->fnm=KlMem_realloc(o->fnm,o->fl*sizeof(char*));
			*(o->f+o->fl-1)=str_clone((*(oa->f+i))->nm);
			*(o->fnm+o->fl-1)=NULL;
			KlMem_ret(o->f);
		}
	}
	o->v=NULL;
	o->vnm=NULL;
	o->vl=0;
	for (size_t i=0;i<oa->vl;i++){
		if (((*(oa->vm+i))&OBJECT_MODIFIER_EXPORT)!=0){
			o->vl++;
			o->v=KlMem_realloc(o->v,o->vl*sizeof(char*));
			o->vnm=KlMem_realloc(o->vnm,o->vl*sizeof(char*));
			KlMem_ret(o->v);
			KlMem_ret(o->vnm);
			*(o->v+o->vl-1)=str_clone(*(oa->vnm+i));
			*(o->vnm+o->vl-1)=NULL;
		}
	}
	o->src=oa;
	o->n=false;
	KlMem_ret(o);
	KlImport_define_module(o);
	return(o);
}



void KlImport_define_module(struct ASTModule* m){
	KlMem_enter_func();
	_cml++;
	_cm=KlMem_realloc(_cm,_cml*sizeof(struct ASTModule*));
	KlMem_ret(_cm);
	*(_cm+_cml-1)=m;
	return();
}



void KlImport_free_modules(void){
	if (_cml>0){
		for (size_t i=0;i<_cml;i++){
			if ((*(_cm+i))->n==false){
				KlFree_free_module(**(_cm+i));
				KlMem_free(*(_cm+i));
			}
		}
		KlMem_free(_cm);
		_cm=NULL;
		_cml=0;
	}
}

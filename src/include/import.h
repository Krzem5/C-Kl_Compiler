#ifndef IMPORT_H
#define IMPORT_H
#include <shared.h>



char* KlImport_find_module(struct CodeFileObject* fo,const char* nm,struct CallStack* cs);



struct ASTModule* KlImport_load_module(struct CodeFileObject* fo,const char* fp,struct CallStack* cs);



void KlImport_define_module(struct ASTModule* m);



void KlImport_free_modules(void);



#endif

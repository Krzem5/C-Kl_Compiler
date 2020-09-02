#ifndef IMPORT_H
#define IMPORT_H
#include <shared.h>



char* KlImport_find_module(struct CodeFileObject* fo,const char* nm,struct CallStack* cs);



struct Module* KlImport_load_module(struct CodeFileObject* fo,const char* fp,struct CallStack* cs);



#endif

#ifndef CORE_H
#define CORE_H
#include <types.h>



int KlCore_run_all(int argc,const char** argv);



struct CodeFileObject* KlCore_read_file(const char* fp,struct CallStack* cs);



#endif

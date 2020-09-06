#ifndef SYS_H
#define SYS_H
#include <shared.h>



extern char* KlSys_import_path;



extern struct File KlSys_stdout;



extern struct File KlSys_stderr;



extern struct File KlSys_stdin;



extern const struct ASTModule KlSys_Module;



void KlSys_init(void);



#endif

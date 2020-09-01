#ifndef SYS_H
#define SYS_H
#include <io.h>
#include <memory.h>
#include <types.h>



extern char* KlSys_import_path;



extern struct File KlSys_stdout;



extern struct File KlSys_stderr;



extern struct File KlSys_stdin;



void KlSys_init(void);



#endif

#include <io.h>
#include <memory.h>
#include <sys.h>
#include <stdio.h>
#include <shared.h>



char* KlSys_import_path="this/is/the/default;sys/import/path;";



struct File KlSys_stdout={0};



struct File KlSys_stderr={0};



struct File KlSys_stdin={0};



void KlSys_init(void){
	KlMem_enter_func();
	static bool c=false;
	if (c==true){
		return();
	}
	c=true;
	KlSys_stdout.p=stdout;
	KlSys_stdout.m=FILE_MODIFIER_EXISTS|FILE_MODIFIER_VIRTUAL|FILE_MODIFIER_WRITE;
	KlSys_stdout.wf=KlIo_default_write_func;
	KlSys_stdout.ff=KlIo_default_flush_func;
	KlSys_stderr.p=stderr;
	KlSys_stderr.m=FILE_MODIFIER_EXISTS|FILE_MODIFIER_VIRTUAL|FILE_MODIFIER_WRITE;
	KlSys_stderr.wf=KlIo_default_write_func;
	KlSys_stderr.ff=KlIo_default_flush_func;
	KlSys_stdin.p=stdin;
	KlSys_stdin.m=FILE_MODIFIER_EXISTS|FILE_MODIFIER_VIRTUAL|FILE_MODIFIER_READ;
	KlSys_stdin.rf=KlIo_default_read_func;
	return();
}

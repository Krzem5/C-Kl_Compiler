#include <io.h>
#include <memory.h>
#include <sys.h>
#include <import.h>
#include <stdio.h>
#include <shared.h>



char* KlSys_import_path="lib;";



struct File KlSys_stdout={0};



struct File KlSys_stderr={0};



struct File KlSys_stdin={0};



static const struct ASTModule KlSys_Module={
	"_sys",
	"_sys",
	"<builtin>/_sys",
	NULL,
	NULL,
	0,
	NULL,
	NULL,
	0,
	NULL,//CRASH
	true
};



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
	KlImport_define_module((struct ASTModule*)&KlSys_Module);
	return();
}

#include <windows.h>
#include <platform.h>
#include <memory.h>
#include <string_utils.h>
#include <shared.h>



unsigned long _cm_in;
unsigned long _cm_out;
unsigned long _cm_err;



bool KlPlatform_file_exists(const char* fp){
	KlMem_enter_func();
	WIN32_FIND_DATAA f;
	HANDLE h=FindFirstFileA(fp,&f);
	if (h!=INVALID_HANDLE_VALUE){
		FindClose(h);
	}
	return((h==INVALID_HANDLE_VALUE?false:true));
}



char* KlPlatform_get_cwd(){
	KlMem_enter_func();
	char bf[MAX_PATH];
	unsigned int l=GetCurrentDirectoryA(MAX_PATH,bf);
	char* o=KlMem_malloc(l+1);
	for (unsigned int i=0;i<l;i++){
		*(o+i)=*(bf+i);
	}
	*(o+l)=0;
	KlMem_ret(o);
	return(o);
}



char* KlPlatform_get_full_path(const char* fp){
	KlMem_enter_func();
	char* t;
	char bf[MAX_PATH];
	unsigned int l=GetFullPathNameA(fp,MAX_PATH,bf,&t);
	char* o=KlMem_malloc(l+1);
	for (unsigned int i=0;i<l;i++){
		*(o+i)=*(bf+i);
	}
	*(o+l)=0;
	KlMem_ret(o);
	return(o);
}



char* KlPlatform_read_file_content(const char* fp,size_t* l){
	KlMem_enter_func();
	HANDLE fh=CreateFileA(fp,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	*l=GetFileSize(fh,NULL);
	char* o=KlMem_malloc(*l+1);
	unsigned long t;
	ReadFile(fh,o,(unsigned long)*l,&t,NULL);
	CloseHandle(fh);
	*(o+*l)=0;
	KlMem_ret(o);
	return(o);
}



void KlPlatform_setup_console(void){
	KlMem_enter_func();
	HANDLE h=GetStdHandle(STD_INPUT_HANDLE);
	GetConsoleMode(h,&_cm_in);
	SetConsoleMode(h,_cm_in|(~ENABLE_PROCESSED_INPUT));
	h=GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleMode(h,&_cm_out);
	SetConsoleMode(h,_cm_out|ENABLE_PROCESSED_OUTPUT|ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	h=GetStdHandle(STD_ERROR_HANDLE);
	GetConsoleMode(h,&_cm_err);
	SetConsoleMode(h,_cm_err|ENABLE_PROCESSED_OUTPUT|ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	return();
}



void KlPlatform_restore_console(void){
	KlMem_enter_func();
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE),_cm_in);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE),_cm_out);
	SetConsoleMode(GetStdHandle(STD_ERROR_HANDLE),_cm_err);
	return();
}



// HMODULE dll=LoadLibraryExA(fp.c_str(),NULL,DONT_RESOLVE_DLL_REFERENCES);
// _LOADED_IMAGE dll_i;
// MapAndLoad(fp.c_str(),NULL,&dll_i,true,true);
// unsigned long ds;
// _IMAGE_EXPORT_DIRECTORY* ex_d=(_IMAGE_EXPORT_DIRECTORY*)
// ImageDirectoryEntryToData(dll_i.MappedAddress,
// false,IMAGE_DIRECTORY_ENTRY_EXPORT,&ds);
// unsigned long* nm_rva=(unsigned long*)ImageRvaToVa(dll_i.FileHeader,dll_i.MappedAddress,ex_d->AddressOfNames,NULL);
// for (unsigned int i=0;i<ex_d->NumberOfNames;i++){
// 	std::string f_nm=(char*)ImageRvaToVa(dll_i.FileHeader,dll_i.MappedAddress,nm_rva[i],NULL);
// 	// GetProcAddress(dll,f_nm.c_str());
// 	std::cout<<"Function: "<<f_nm<<"\n";
// }
// UnMapAndLoad(&dll_i);
// FreeLibrary(dll);
// return {};

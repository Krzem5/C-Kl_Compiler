#include <windows.h>
#include <sys.h>



int WINAPI DllMain(void* dll,unsigned long r,void* rs){
	(void)dll;
	(void)rs;
	switch (r){
		case DLL_PROCESS_ATTACH:
			KlSys_init();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
	}
	return 1;
}

#ifndef PLATFORM_API_H
#define PLATFORM_API_H
#include <shared.h>



bool KlPlatform_file_exists(const char* nm);



char* KlPlatform_get_cwd();



char* KlPlatform_get_full_path(const char* fp);



char* KlPlatform_read_file_content(const char* fp,size_t* l);



void KlPlatform_setup_console(void);



void KlPlatform_restore_console(void);



#endif

#ifndef ERROR_H
#define ERROR_H
#include <shared.h>



#ifndef NDEBUG
#define KlError_unimplemented_code() assert_msg(0,"Unimplemented Code")
#define KlError_unimplemented_error() assert_msg(0,"Unimplemented Error")
#define KlError_unimplemented_warning() assert_msg(0,"Unimplemented Warning")
#else
#define KlError_unimplemented_code() KlError_unimplemented("Code",__LINE__)
#define KlError_unimplemented_error() KlError_unimplemented("Error",__LINE__)
#define KlError_unimplemented_warning() KlError_unimplemented("Warning",__LINE__)
#endif
#define COLOR_ERROR_TXT 0x1d23c1e000000 // #d23c1e;#000000
#define COLOR_ERROR_SRC_HIGHLIGHT 0x3fefefee932a2 // #fefefe;#e932a2



void KlError_raise(void);



void KlError_cleanup(void);



void KlError_set_error(char* nm,char* msg,struct CallStack* cs);



struct CallStack* KlError_extend_call_stack(struct CallStack* cs,struct CodeFileObject* c,size_t sl,size_t el,size_t su,size_t eu,char* f);



size_t KlError_offset_to_line(char* dt,size_t i);



void KlError_unimplemented(char* t,unsigned int ln);



#endif

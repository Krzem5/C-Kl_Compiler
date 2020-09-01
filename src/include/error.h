#ifndef ERROR_H
#define ERROR_H
#include <types.h>



#ifndef NDEBUG
#define KlError_unimplemented_code() assert_msg(0,"Unimplemented Code")
#define KlError_unimplemented_error() assert_msg(0,"Unimplemented Error")
#define KlError_unimplemented_warning() assert_msg(0,"Unimplemented Warning")
#else
#define KlError_unimplemented_code() KlError_unimplemented("Code",__LINE__)
#define KlError_unimplemented_error() KlError_unimplemented("Error",__LINE__)
#define KlError_unimplemented_warning() KlError_unimplemented("Warning",__LINE__)
#endif



void KlError_raise(void);



void KlError_set_error(char* nm,char* msg,struct CallStack* cs);



struct CallStack* KlError_extend_call_stack(struct CallStack* cs,struct CodeFileObject* c,unsigned long sl,unsigned long el,unsigned long su,unsigned long eu,char* f);



unsigned long KlError_offset_to_line(char* dt,unsigned long i);



void KlError_unimplemented(char* t,unsigned long ln);



#endif

#ifndef NUMBER_H
#define NUMBER_H
#include <types.h>



#define NUM_SHIFT 30
#define NUM_BASE ((uint32_t)1<<NUM_SHIFT)
#define NUM_MASK (uint32_t)(NUM_BASE-1)
#define NUM_MAX (uint32_t)(NUM_BASE-1)
#define DEC_SHIFT 9
#define DEC_BASE ((uint32_t)1000000000)



#define KlNum_add_base_digit(o,b,d) \
	do{ \
		struct Number* tmp=KlNum_mult_long(o,b); \
		struct Number* tmp2=KlNum_add_long(tmp,d); \
		KlNum_free(tmp); \
		o=KlNum_assign(o,tmp2); \
		KlNum_free(tmp2); \
	} while(0)



#define KlNum_add_bin_digit(o,d) KlNum_add_base_digit(o,2,d)



#define KlNum_add_oct_digit(o,d) KlNum_add_base_digit(o,8,d)



#define KlNum_add_digit(o,d) KlNum_add_base_digit(o,10,d)



#define KlNum_add_hex_digit(o,d) KlNum_add_base_digit(o,16,d)



struct Number* KlNum_from_long(long long ll);



struct Number* KlNum_assign(struct Number* o,struct Number* v);



struct Number* KlNum_assign_long(struct Number* o,long v);



void KlNum_free(struct Number* n);



struct Number* KlNum_trunc(struct Number* o);



bool KlNum_is_zero(struct Number* n);



bool KlNum_is_one(struct Number* n);



struct Number* KlNum_abs(struct Number* a);



struct Number* KlNum_neg(struct Number* a);



struct Number* KlNum_add(struct Number* a,struct Number* b);



struct Number* KlNum_add_long(struct Number* a,long b);



struct Number* KlNum_add_abs(struct Number* a,struct Number* b);



struct Number* KlNum_add_long_abs(struct Number* a,unsigned long b);



struct Number* KlNum_sub(struct Number* a,struct Number* b);



struct Number* KlNum_sub_long(struct Number* a,long b);



struct Number* KlNum_sub_abs(struct Number* a,struct Number* b);



struct Number* KlNum_sub_long_abs(struct Number* a,unsigned long b);



struct Number* KlNum_mult(struct Number* a,struct Number* b);



struct Number* KlNum_mult_long(struct Number* a,long b);



struct Number* KlNum_mult_abs(struct Number* a,struct Number* b);



struct Number* KlNum_mult_long_abs(struct Number* a,unsigned long b);



struct Number* KlNum_div(struct Number* a,struct Number* b);



struct Number* KlNum_fdiv(struct Number* a,struct Number* b);



struct Number* KlNum_fdiv_long(struct Number* a,long b);



struct Number* KlNum_mod(struct Number* a,struct Number* b);



struct Number* KlNum_mod_long(struct Number* a,long b);



unsigned char KlNum_divrem(struct Number* a,struct Number* b,struct Number** div,struct Number** rem);



unsigned char KlNum_divrem_long(struct Number* a,long b,struct Number** div,struct Number** rem);



unsigned char KlNum_divrem_abs(struct Number* a,struct Number* b,struct Number** div,struct Number** rem);



unsigned char KlNum_divmod(struct Number* a,struct Number* b,struct Number** div,struct Number** mod);



unsigned char KlNum_divmod_long(struct Number* a,long b,struct Number** div,struct Number** mod);



struct Number* KlNum_pow(struct Number* a,struct Number* b,struct Number* c);



struct Number* KlNum_pow_long(struct Number* a,long b,struct Number* c);



struct Number* KlNum_invmod(struct Number* a,struct Number* n);



struct Number* KlNum_iroot(struct Number* r,struct Number* dg);



struct Number* KlNum_iroot_long(struct Number* r,unsigned long dg);



// struct Number* KlNum_root(struct Number* n,struct Number* b);



bool KlNum_eq(struct Number* a,struct Number* b);



bool KlNum_neq(struct Number* a,struct Number* b);



signed char KlNum_cmp(struct Number* a,struct Number* b);



signed char KlNum_cmp_long(struct Number* a,long b);



signed char KlNum_cmp_abs(struct Number* a,struct Number* b);



signed char KlNum_cmp_long_abs(struct Number* a,unsigned long b);



char* KlNum_print(struct Number* n);



#endif

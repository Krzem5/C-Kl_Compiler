#ifndef INT_H
#define INT_H
#include <shared.h>



#define NUM_SHIFT 30
#define NUM_BASE ((uint32_t)1<<NUM_SHIFT)
#define NUM_MASK (uint32_t)(NUM_BASE-1)
#define NUM_MAX (uint32_t)(NUM_BASE-1)
#define DEC_SHIFT 9
#define DEC_BASE ((uint32_t)1000000000)



#define KLBigInt_add_base_digit(o,b,d) \
	do{ \
		struct BigInt* __tmp=KLBigInt_mult_long(o,b); \
		struct BigInt* __tmp2=KLBigInt_add_long(__tmp,d); \
		KLBigInt_free(__tmp); \
		o=KLBigInt_assign(o,__tmp2); \
		KLBigInt_free(__tmp2); \
	} while(0)



#define KLBigInt_add_bin_digit(o,d) KLBigInt_add_base_digit(o,2,d)



#define KLBigInt_add_oct_digit(o,d) KLBigInt_add_base_digit(o,8,d)



#define KLBigInt_add_digit(o,d) KLBigInt_add_base_digit(o,10,d)



#define KLBigInt_add_hex_digit(o,d) KLBigInt_add_base_digit(o,16,d)



struct BigInt* KLBigInt_from_long(long long ll);



struct BigInt* KLBigInt_assign(struct BigInt* o,struct BigInt* v);



struct BigInt* KLBigInt_assign_long(struct BigInt* o,long v);



void KLBigInt_free(struct BigInt* n);



struct BigInt* KLBigInt_trunc(struct BigInt* o);



bool KLBigInt_is_zero(struct BigInt* n);



bool KLBigInt_is_one(struct BigInt* n);



struct BigInt* KLBigInt_abs(struct BigInt* a);



struct BigInt* KLBigInt_neg(struct BigInt* a);



struct BigInt* KLBigInt_add(struct BigInt* a,struct BigInt* b);



struct BigInt* KLBigInt_add_long(struct BigInt* a,long b);



struct BigInt* KLBigInt_add_abs(struct BigInt* a,struct BigInt* b);



struct BigInt* KLBigInt_add_long_abs(struct BigInt* a,unsigned long b);



struct BigInt* KLBigInt_sub(struct BigInt* a,struct BigInt* b);



struct BigInt* KLBigInt_sub_long(struct BigInt* a,long b);



struct BigInt* KLBigInt_sub_abs(struct BigInt* a,struct BigInt* b);



struct BigInt* KLBigInt_sub_long_abs(struct BigInt* a,unsigned long b);



struct BigInt* KLBigInt_mult(struct BigInt* a,struct BigInt* b);



struct BigInt* KLBigInt_mult_long(struct BigInt* a,long b);



struct BigInt* KLBigInt_mult_abs(struct BigInt* a,struct BigInt* b);



struct BigInt* KLBigInt_mult_long_abs(struct BigInt* a,unsigned long b);



struct BigInt* KLBigInt_div(struct BigInt* a,struct BigInt* b);



struct BigInt* KLBigInt_fdiv(struct BigInt* a,struct BigInt* b);



struct BigInt* KLBigInt_fdiv_long(struct BigInt* a,long b);



struct BigInt* KLBigInt_mod(struct BigInt* a,struct BigInt* b);



struct BigInt* KLBigInt_mod_long(struct BigInt* a,long b);



unsigned char KLBigInt_divrem(struct BigInt* a,struct BigInt* b,struct BigInt** div,struct BigInt** rem);



unsigned char KLBigInt_divrem_long(struct BigInt* a,long b,struct BigInt** div,struct BigInt** rem);



unsigned char KLBigInt_divrem_abs(struct BigInt* a,struct BigInt* b,struct BigInt** div,struct BigInt** rem);



unsigned char KLBigInt_divmod(struct BigInt* a,struct BigInt* b,struct BigInt** div,struct BigInt** mod);



unsigned char KLBigInt_divmod_long(struct BigInt* a,long b,struct BigInt** div,struct BigInt** mod);



struct BigInt* KLBigInt_pow(struct BigInt* a,struct BigInt* b,struct BigInt* c);



struct BigInt* KLBigInt_pow_long(struct BigInt* a,long b,struct BigInt* c);



struct BigInt* KLBigInt_invmod(struct BigInt* a,struct BigInt* n);



struct BigInt* KLBigInt_iroot(struct BigInt* r,struct BigInt* dg);



struct BigInt* KLBigInt_iroot_long(struct BigInt* r,unsigned long dg);



// struct BigInt* KLBigInt_root(struct BigInt* n,struct BigInt* b);



bool KLBigInt_eq(struct BigInt* a,struct BigInt* b);



bool KLBigInt_neq(struct BigInt* a,struct BigInt* b);



signed char KLBigInt_cmp(struct BigInt* a,struct BigInt* b);



signed char KLBigInt_cmp_long(struct BigInt* a,long b);



signed char KLBigInt_cmp_abs(struct BigInt* a,struct BigInt* b);



signed char KLBigInt_cmp_long_abs(struct BigInt* a,unsigned long b);



char* KLBigInt_print(struct BigInt* n);



#endif

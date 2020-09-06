#ifndef INT_H
#define INT_H
#include <shared.h>



#define NUM_SHIFT 30
#define NUM_BASE ((uint32_t)1<<NUM_SHIFT)
#define NUM_MASK (uint32_t)(NUM_BASE-1)
#define NUM_MAX (uint32_t)(NUM_BASE-1)
#define DEC_SHIFT 9
#define DEC_BASE ((uint32_t)1000000000)



#define KlBigInt_add_base_digit(o,b,d) \
	do{ \
		struct BigInt* __tmp=KlBigInt_mult_long(o,b); \
		struct BigInt* __tmp2=KlBigInt_add_long(__tmp,d); \
		KlBigInt_free(__tmp); \
		o=KlBigInt_assign(o,__tmp2); \
		KlBigInt_free(__tmp2); \
	} while(0)



#define KlBigInt_add_bin_digit(o,d) KlBigInt_add_base_digit(o,2,d)



#define KlBigInt_add_oct_digit(o,d) KlBigInt_add_base_digit(o,8,d)



#define KlBigInt_add_digit(o,d) KlBigInt_add_base_digit(o,10,d)



#define KlBigInt_add_hex_digit(o,d) KlBigInt_add_base_digit(o,16,d)



struct BigInt* KlBigInt_from_long(long long ll);



struct BigInt* KlBigInt_assign(struct BigInt* o,struct BigInt* v);



struct BigInt* KlBigInt_assign_long(struct BigInt* o,long v);



void KlBigInt_free(struct BigInt* n);



struct BigInt* KlBigInt_trunc(struct BigInt* o);



bool KlBigInt_is_zero(struct BigInt* n);



bool KlBigInt_is_one(struct BigInt* n);



struct BigInt* KlBigInt_abs(struct BigInt* a);



struct BigInt* KlBigInt_neg(struct BigInt* a);



struct BigInt* KlBigInt_add(struct BigInt* a,struct BigInt* b);



struct BigInt* KlBigInt_add_long(struct BigInt* a,long b);



struct BigInt* KlBigInt_add_abs(struct BigInt* a,struct BigInt* b);



struct BigInt* KlBigInt_add_long_abs(struct BigInt* a,unsigned long b);



struct BigInt* KlBigInt_sub(struct BigInt* a,struct BigInt* b);



struct BigInt* KlBigInt_sub_long(struct BigInt* a,long b);



struct BigInt* KlBigInt_sub_abs(struct BigInt* a,struct BigInt* b);



struct BigInt* KlBigInt_sub_long_abs(struct BigInt* a,unsigned long b);



struct BigInt* KlBigInt_mult(struct BigInt* a,struct BigInt* b);



struct BigInt* KlBigInt_mult_long(struct BigInt* a,long b);



struct BigInt* KlBigInt_mult_abs(struct BigInt* a,struct BigInt* b);



struct BigInt* KlBigInt_mult_long_abs(struct BigInt* a,unsigned long b);



struct BigInt* KlBigInt_div(struct BigInt* a,struct BigInt* b);



struct BigInt* KlBigInt_fdiv(struct BigInt* a,struct BigInt* b);



struct BigInt* KlBigInt_fdiv_long(struct BigInt* a,long b);



struct BigInt* KlBigInt_mod(struct BigInt* a,struct BigInt* b);



struct BigInt* KlBigInt_mod_long(struct BigInt* a,long b);



unsigned char KlBigInt_divrem(struct BigInt* a,struct BigInt* b,struct BigInt** div,struct BigInt** rem);



unsigned char KlBigInt_divrem_long(struct BigInt* a,long b,struct BigInt** div,struct BigInt** rem);



unsigned char KlBigInt_divrem_abs(struct BigInt* a,struct BigInt* b,struct BigInt** div,struct BigInt** rem);



unsigned char KlBigInt_divmod(struct BigInt* a,struct BigInt* b,struct BigInt** div,struct BigInt** mod);



unsigned char KlBigInt_divmod_long(struct BigInt* a,long b,struct BigInt** div,struct BigInt** mod);



struct BigInt* KlBigInt_pow(struct BigInt* a,struct BigInt* b,struct BigInt* c);



struct BigInt* KlBigInt_pow_long(struct BigInt* a,long b,struct BigInt* c);



struct BigInt* KlBigInt_invmod(struct BigInt* a,struct BigInt* n);



struct BigInt* KlBigInt_iroot(struct BigInt* r,struct BigInt* dg);



struct BigInt* KlBigInt_iroot_long(struct BigInt* r,unsigned long dg);



// struct BigInt* KlBigInt_root(struct BigInt* n,struct BigInt* b);



bool KlBigInt_eq(struct BigInt* a,struct BigInt* b);



bool KlBigInt_neq(struct BigInt* a,struct BigInt* b);



signed char KlBigInt_cmp(struct BigInt* a,struct BigInt* b);



signed char KlBigInt_cmp_long(struct BigInt* a,long b);



signed char KlBigInt_cmp_abs(struct BigInt* a,struct BigInt* b);



signed char KlBigInt_cmp_long_abs(struct BigInt* a,unsigned long b);



char* KlBigInt_print(struct BigInt* n);



#endif

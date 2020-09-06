#ifndef FLOAT_H
#define FLOAT_H
#include <shared.h>



struct Decimal* KlDec_create_dec(bool s,unsigned int n,unsigned int f);



struct Decimal* KlDec_from_num(struct BigInt* n);



void KlDec_add_frac_digit(struct Decimal* o,unsigned char d);



#endif

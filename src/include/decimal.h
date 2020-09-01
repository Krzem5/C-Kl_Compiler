#ifndef DECIMAL_H
#define DECIMAL_H
#include <types.h>



struct Decimal* KlDec_create_dec(bool s,unsigned int n,unsigned int f);



struct Decimal* KlDec_from_num(struct Number* n);



void KlDec_add_frac_digit(struct Decimal* o,unsigned char d);



#endif

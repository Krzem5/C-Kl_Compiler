#ifndef CRYPT_H
#define CRYPT_H
#include <shared.h>



struct SHA256 KlCrypt_init_sha256(void);



void KlCrypt_update_sha256(struct SHA256 sha,unsigned char* s,size_t ln);



uint256_t KlCrypt_finalize_sha256(struct SHA256 sha);



uint256_t KlCrypt_quick_sha256(unsigned char* s,size_t ln);



#endif

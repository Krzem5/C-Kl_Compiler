#include <crypt.h>
#include <memory.h>
#include <free.h>
#include <shared.h>



static const unsigned int SHA256_T_CONST[64]={
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};



struct SHA256 KlCrypt_init_sha256(void){
	KlMem_enter_func();
	struct SHA256 o={
		0,
		0,
		KlMem_calloc(64,sizeof(unsigned char)),
		KlMem_malloc(8*sizeof(unsigned long))
	};
	*o.st=0x6a09e667;
	*(o.st+1)=0xbb67ae85;
	*(o.st+2)=0x3c6ef372;
	*(o.st+3)=0xa54ff53a;
	*(o.st+4)=0x510e527f;
	*(o.st+5)=0x9b05688c;
	*(o.st+6)=0x1f83d9ab;
	*(o.st+7)=0x5be0cd19;
	return(o);
}



void KlCrypt_update_sha256(struct SHA256 sha,unsigned char* s,size_t ln){
	KlMem_enter_func();
	for (size_t i=0;i<sha.bln;i++){
		*(sha.dt+sha.dln)=*((char*)s+i);
		sha.dln++;
		if (sha.dln==64){
			sha.bln+=512;
			sha.dln=0;
		}
	}
	return();
}



void KlCrypt_transform_sha256(struct SHA256 sha){
	KlMem_enter_func();
	unsigned int a,b,c,d,e,f,g,h,i,j,t1,t2;
	unsigned int* m=KlMem_malloc(64);
	for (i=0,j=0;i<16;i++,j+=4){
		*(m+i)=(*(sha.dt+j)<<24)|(*(sha.dt+j+1)<<16)|(*(sha.dt+j+2)<<8)|(*(sha.dt+j+3));
	}
	for (;i<64;i++){
		*(m+i)=(((*(m+i-2)>>17)|(*(m+i-2)<<(32-17)))^((*(m+i-2)>>19)|(*(m+i-2)<<(32-19)))^((*(m+i-2))>>10))+*(m+i-7)+(((*(m+i-15)>>7)|(*(m+i-15)<<(32-7)))^((*(m+i-15)>>18)|(*(m+i-15)<<(32-18)))^((*(m+i-15))>>3))+*(m+i-16);
	}
	a=*sha.st;
	b=*(sha.st+1);
	c=*(sha.st+2);
	d=*(sha.st+3);
	e=*(sha.st+4);
	f=*(sha.st+5);
	g=*(sha.st+6);
	h=*(sha.st+7);
	for (i=0;i<64;i++){
		t1=h+(((e>>6)|(e<<(32-6)))^((e>>11)|(e<<(32-11)))^((e>>25)|(e<<(32-25))))+((e&f)^(~e&g))+*(SHA256_T_CONST+i)+*(m+i);
		t2=(((a>>2)|(a<<(32-2)))^((a>>13)|(a<<(32-13)))^((a>>22)|(a<<(32-22))))+((a&b)^(a&c)^(b&c));
		h=g;
		g=f;
		f=e;
		e=d+t1;
		d=c;
		c=b;
		b=a;
		a=t1+t2;
	}
	*sha.st+=a;
	*(sha.st+1)+=b;
	*(sha.st+2)+=c;
	*(sha.st+3)+=d;
	*(sha.st+4)+=e;
	*(sha.st+5)+=f;
	*(sha.st+6)+=g;
	*(sha.st+7)+=h;
}



uint256_t KlCrypt_finalize_sha256(struct SHA256 sha){
	KlMem_enter_func();
	unsigned char i=sha.dln;
	if (sha.dln<56){
		i++;
		*(sha.dt+i)=0x80;
		while (i<56){
			i++;
			*(sha.dt+i)=0x00;
		}
	}
	else{
		i++;
		*(sha.dt+i)=0x80;
		while (i<64){
			i++;
			*(sha.dt+i)=0x00;
		}
		KlCrypt_transform_sha256(sha);
		for (i=0;i<56;i++){
			*(sha.dt+i)=0;
		}
	}
	sha.bln+=sha.dln*8;
	*(sha.dt+63)=sha.bln&0xff;
	*(sha.dt+62)=(sha.bln>>8)&0xff;
	*(sha.dt+61)=(sha.bln>>16)&0xff;
	*(sha.dt+60)=(sha.bln>>24)&0xff;
	*(sha.dt+59)=(sha.bln>>32)&0xff;
	*(sha.dt+58)=(sha.bln>>40)&0xff;
	*(sha.dt+57)=(sha.bln>>48)&0xff;
	*(sha.dt+56)=(sha.bln>>56)&0xff;
	KlCrypt_transform_sha256(sha);
	uint256_t o;
	o.a=((uint64_t)((*sha.st>>24)&0xff)<<56)|((uint64_t)((*(sha.st+1)>>24)&0xff)<<48)|((uint64_t)((*(sha.st+2)>>24)&0xff)<<40)|((uint64_t)((*(sha.st+3)>>24)&0xff)<<32)|((uint64_t)((*(sha.st+4)>>24)&0xff)<<24)|((uint64_t)((*(sha.st+5)>>24)&0xff)<<16)|((uint64_t)((*(sha.st+6)>>24)&0xff)<<8)|((*(sha.st+7)>>24)&0xff);
	o.b=((uint64_t)((*sha.st>>16)&0xff)<<56)|((uint64_t)((*(sha.st+1)>>16)&0xff)<<48)|((uint64_t)((*(sha.st+2)>>16)&0xff)<<40)|((uint64_t)((*(sha.st+3)>>16)&0xff)<<32)|((uint64_t)((*(sha.st+4)>>16)&0xff)<<24)|((uint64_t)((*(sha.st+5)>>16)&0xff)<<16)|((uint64_t)((*(sha.st+6)>>16)&0xff)<<8)|((*(sha.st+7)>>16)&0xff);
	o.c=((uint64_t)((*sha.st>>8)&0xff)<<56)|((uint64_t)((*(sha.st+1)>>8)&0xff)<<48)|((uint64_t)((*(sha.st+2)>>8)&0xff)<<40)|((uint64_t)((*(sha.st+3)>>8)&0xff)<<32)|((uint64_t)((*(sha.st+4)>>8)&0xff)<<24)|((uint64_t)((*(sha.st+5)>>8)&0xff)<<16)|((uint64_t)((*(sha.st+6)>>8)&0xff)<<8)|((*(sha.st+7)>>8)&0xff);
	o.d=((uint64_t)((*sha.st)&0xff)<<56)|((uint64_t)(*(sha.st+1)&0xff)<<48)|((uint64_t)(*(sha.st+2)&0xff)<<40)|((uint64_t)(*(sha.st+3)&0xff)<<32)|((uint64_t)(*(sha.st+4)&0xff)<<24)|((uint64_t)(*(sha.st+5)&0xff)<<16)|((uint64_t)(*(sha.st+6)&0xff)<<8)|(*(sha.st+7)&0xff);
	return(o);
}



uint256_t KlCrypt_quick_sha256(unsigned char* s,size_t ln){
	KlMem_enter_func();
	struct SHA256 sha=KlCrypt_init_sha256();
	KlCrypt_update_sha256(sha,s,ln);
	uint256_t o=KlCrypt_finalize_sha256(sha);
	KlFree_free_sha256(sha);
	return(o);
}

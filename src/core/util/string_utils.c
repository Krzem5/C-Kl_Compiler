#include <string_utils.h>
#include <memory.h>
#include <shared.h>
#include <stdarg.h>
#include <limits.h>
#include <shared.h>



size_t str_len(const char* s){
	KlMem_enter_func();
	size_t i=0;
	while (*(s+i)!=0){
		i++;
	}
	return(i);
}



char* str_clone(const char* s){
	KlMem_enter_func();
	if (s==NULL){
		return(NULL);
	}
	size_t ln=str_len(s)+1;
	return(KlMem_memcpy(KlMem_malloc(ln),(char*)s,ln));
}



char* str_append(const char* s,char c){
	KlMem_enter_func();
	size_t sz=str_len(s);
	char* o=KlMem_realloc((char*)s,sz+2);
	o[sz]=c;
	o[sz+1]=0;
	KlMem_ret(o);
	return(o);
}



char* str_concat(const char* a,char* b){
	KlMem_enter_func();
	char* o=KlMem_malloc(str_len(a)+str_len(b)+1);
	KlMem_memcpy(o,(char*)a,str_len(a));
	KlMem_memcpy(o+str_len(a),b,str_len(b));
	*(o+str_len(a)+str_len(b))=0;
	KlMem_ret(o);
	return(o);
}



char* str_substr(const char* s,size_t i,size_t j){
	KlMem_enter_func();
	char* o=KlMem_malloc(j-i+1);
	for (size_t k=i;k<j;k++){
		*(o+k-i)=*(s+k);
	}
	o[j-i]=0;
	KlMem_ret(o);
	return(o);
}



size_t str_find(const char* s,char c,size_t i){
	KlMem_enter_func();
	size_t j=i;
	while (*(s+j)!=0){
		if (*(s+j)==c){
			return(j-i);
		}
		j++;
	}
	return(ULONG_MAX);
}



size_t str_rfind(const char* s,char c,size_t i){
	KlMem_enter_func();
	while (true){
		if (*(s+i)==c){
			return(i);
		}
		if (i==0){
			return(ULONG_MAX);
		}
		i--;
	}
}



bool str_cmp(const char* a,const char* b){
	KlMem_enter_func();
	size_t ln=str_len(a)+1;
	if (str_len(b)+1!=ln){
		return(false);
	}
	for (size_t i=0;i<ln;i++){
		if (*(a+i)!=*(b+i)){
			return(false);
		}
	}
	return(true);
}



bool str_cmp_sub(const char* a,char* b,size_t s,size_t l){
	KlMem_enter_func();
	if (l!=str_len(b)){
		return(false);
	}
	for (size_t i=0;i<l;i++){
		if (*(a+s+i)!=*(b+i)){
			return(false);
		}
	}
	return(true);
}



char* str_format(const char* t,...){
	KlMem_enter_func();
	va_list a;
	va_start(a,t);
	char* o=str_format_va(t,a);
	va_end(a);
	return(o);
}



char* str_format_va(const char* t,va_list a){
	KlMem_enter_func();
	size_t ln=str_len(t)+1;
	char* o=KlMem_calloc(ln,1);
	size_t i=0;
	while (*t!=0){
		if (*t=='%'){
			t++;
			ln--;
			if (*t==0||*t=='%'){
				*(o+i)='%';
				i++;
			}
			else{
				ln--;
				switch (*t){
					case 'b':
						*(o+i)='b';
						i++;
						// va_arg(a,uint)%256
						break;
					case 'c':
						*(o+i)=va_arg(a,unsigned int)%256;
						i++;
						ln++;
						break;
					case 'i':
						{
							int v=va_arg(a,int);
							if (v<0){
								*(o+i)='-';
								i++;
								v=abs(v);
								ln++;
							}
							unsigned char sz=1;
							unsigned int pw=10;
							while (pw<(unsigned int)v){
								sz++;
								pw*=10;
							}
							ln+=sz;
							o=KlMem_realloc(o,ln);
							pw/=10;
							while (sz>0){
								*(o+i)=48+(v/pw)%10;
								i++;
								sz--;
								pw/=10;
							}
							break;
						}
					case 'S':
						{
							size_t v=va_arg(a,size_t);
							if (v==0){
								ln++;
								o=KlMem_realloc(o,ln);
								*(o+i)='0';
								i++;
							}
							else{
								unsigned char sz=1;
								size_t pw=10;
								while (pw<v){
									sz++;
									if (sz==20){
										break;
									}
									pw*=10;
								}
								ln+=sz;
								o=KlMem_realloc(o,ln);
								if (sz!=20){
									pw/=10;
								}
								while (sz>0){
									*(o+i)=48+(v/pw)%10;
									i++;
									sz--;
									pw/=10;
								}
							}
							break;
						}
					case 'p':
						{
							ln+=sizeof(uintptr_t)*2+2;
							o=KlMem_realloc(o,ln);
							*(o+i)='0';
							*(o+i+1)='x';
							i+=2;
							uintptr_t p=(uintptr_t)va_arg(a,void*);
							for (signed char j=sizeof(uintptr_t)*8-4;j>=0;j-=4){
								unsigned char v=(p>>j)%16;
								if (v<=9){
									*(o+i)=48+v;
								}
								else{
									*(o+i)=87+v;
								}
								i++;
							}
							break;
						}
					case 's':
						{
							char* s=va_arg(a,char*);
							if (s==NULL){
								ln+=6;
								o=KlMem_realloc(o,ln);
								*(o+i)='(';
								*(o+i+1)='N';
								*(o+i+2)='U';
								*(o+i+3)='L';
								*(o+i+4)='L';
								*(o+i+5)=')';
								i+=6;
							}
							else{
								ln+=str_len(s);
								o=KlMem_realloc(o,ln);
								while (*s!=0){
									*(o+i)=*s;
									i++;
									s++;
								}
							}
							break;
						}
					case 'f':
						{
							uint64_t f=va_arg(a,uint64_t);
							unsigned char m=(f>>48)&255;
							if (m==0){
								ln+=4;
								o=KlMem_realloc(o,ln);
								*(o+i)=27;
								*(o+i+1)='[';
								*(o+i+2)='0';
								*(o+i+3)='m';
								i+=4;
							}
							else{
								f%=281474976710656;
								if ((m&1)==1){
									ln+=10;
									o=KlMem_realloc(o,ln);
									*(o+i)=27;
									*(o+i+1)='[';
									*(o+i+2)='3';
									*(o+i+3)='8';
									*(o+i+4)=';';
									*(o+i+5)='2';
									*(o+i+6)=';';
									i+=7;
									unsigned char v=(f>>40)&255;
									ln+=(v>=100?3:(v>=10?2:1));
									o=KlMem_realloc(o,ln);
									if (v>=100){
										*(o+i)=48+v/100;
										i++;
									}
									if (v>=10){
										*(o+i)=48+(v/10)%10;
										i++;
									}
									*(o+i)=48+v%10;
									*(o+i+1)=';';
									i+=2;
									v=(f>>32)&255;
									ln+=(v>=100?3:(v>=10?2:1));
									o=KlMem_realloc(o,ln);
									if (v>=100){
										*(o+i)=48+v/100;
										i++;
									}
									if (v>=10){
										*(o+i)=48+(v/10)%10;
										i++;
									}
									*(o+i)=48+v%10;
									*(o+i+1)=';';
									i+=2;
									v=(f>>24)&255;
									ln+=(v>=100?3:(v>=10?2:1));
									o=KlMem_realloc(o,ln);
									if (v>=100){
										*(o+i)=48+v/100;
										i++;
									}
									if (v>=10){
										*(o+i)=48+(v/10)%10;
										i++;
									}
									*(o+i)=48+v%10;
									*(o+i+1)='m';
									i+=2;
								}
								if ((m&2)==2){
									ln+=10;
									o=KlMem_realloc(o,ln);
									*(o+i)=27;
									*(o+i+1)='[';
									*(o+i+2)='4';
									*(o+i+3)='8';
									*(o+i+4)=';';
									*(o+i+5)='2';
									*(o+i+6)=';';
									i+=7;
									unsigned char v=(f>>16)&255;
									ln+=(v>=100?3:(v>=10?2:1));
									o=KlMem_realloc(o,ln);
									if (v>=100){
										*(o+i)=48+v/100;
										i++;
									}
									if (v>=10){
										*(o+i)=48+(v/10)%10;
										i++;
									}
									*(o+i)=48+v%10;
									*(o+i+1)=';';
									i+=2;
									v=(f>>8)&255;
									ln+=(v>=100?3:(v>=10?2:1));
									o=KlMem_realloc(o,ln);
									if (v>=100){
										*(o+i)=48+v/100;
										i++;
									}
									if (v>=10){
										*(o+i)=48+(v/10)%10;
										i++;
									}
									*(o+i)=48+v%10;
									*(o+i+1)=';';
									i+=2;
									v=f&255;
									ln+=(v>=100?3:(v>=10?2:1));
									o=KlMem_realloc(o,ln);
									if (v>=100){
										*(o+i)=48+v/100;
										i++;
									}
									if (v>=10){
										*(o+i)=48+(v/10)%10;
										i++;
									}
									*(o+i)=48+v%10;
									*(o+i+1)='m';
									i+=2;
								}
							}
							break;
						}
					default:
						ln+=2;
						o=KlMem_realloc(o,ln);
						*(o+i)='%';
						*(o+i+1)=*t;
						i+=2;
						break;
				}
			}
		}
		else{
			*(o+i)=*t;
			i++;
		}
		t++;
	}
	o[ln-1]=0;
	KlMem_ret(o);
	return(o);
}



char* str_escape_ansi(uint64_t f){
	KlMem_enter_func();
	unsigned char m=(f>>48)&255;
	if (m==0){
		return("\x1b[0m");
	}
	else{
		f%=281474976710656;
		char* o=NULL;
		unsigned long ln=0;
		unsigned long i=0;
		if ((m&1)==1){
			ln+=10;
			o=KlMem_realloc(o,ln+1);
			*(o+i)=27;
			*(o+i+1)='[';
			*(o+i+2)='3';
			*(o+i+3)='8';
			*(o+i+4)=';';
			*(o+i+5)='2';
			*(o+i+6)=';';
			i+=7;
			unsigned char v=(f>>40)&255;
			ln+=(v>=100?3:(v>=10?2:1));
			o=KlMem_realloc(o,ln+1);
			if (v>=100){
				*(o+i)=48+v/100;
				i++;
			}
			if (v>=10){
				*(o+i)=48+(v/10)%10;
				i++;
			}
			*(o+i)=48+v%10;
			*(o+i+1)=';';
			i+=2;
			v=(f>>32)&255;
			ln+=(v>=100?3:(v>=10?2:1));
			o=KlMem_realloc(o,ln+1);
			if (v>=100){
				*(o+i)=48+v/100;
				i++;
			}
			if (v>=10){
				*(o+i)=48+(v/10)%10;
				i++;
			}
			*(o+i)=48+v%10;
			*(o+i+1)=';';
			i+=2;
			v=(f>>24)&255;
			ln+=(v>=100?3:(v>=10?2:1));
			o=KlMem_realloc(o,ln+1);
			if (v>=100){
				*(o+i)=48+v/100;
				i++;
			}
			if (v>=10){
				*(o+i)=48+(v/10)%10;
				i++;
			}
			*(o+i)=48+v%10;
			*(o+i+1)='m';
			i+=2;
		}
		if ((m&2)==2){
			ln+=10;
			o=KlMem_realloc(o,ln+1);
			*(o+i)=27;
			*(o+i+1)='[';
			*(o+i+2)='4';
			*(o+i+3)='8';
			*(o+i+4)=';';
			*(o+i+5)='2';
			*(o+i+6)=';';
			i+=7;
			unsigned char v=(f>>16)&255;
			ln+=(v>=100?3:(v>=10?2:1));
			o=KlMem_realloc(o,ln+1);
			if (v>=100){
				*(o+i)=48+v/100;
				i++;
			}
			if (v>=10){
				*(o+i)=48+(v/10)%10;
				i++;
			}
			*(o+i)=48+v%10;
			*(o+i+1)=';';
			i+=2;
			v=(f>>8)&255;
			ln+=(v>=100?3:(v>=10?2:1));
			o=KlMem_realloc(o,ln+1);
			if (v>=100){
				*(o+i)=48+v/100;
				i++;
			}
			if (v>=10){
				*(o+i)=48+(v/10)%10;
				i++;
			}
			*(o+i)=48+v%10;
			*(o+i+1)=';';
			i+=2;
			v=f&255;
			ln+=(v>=100?3:(v>=10?2:1));
			o=KlMem_realloc(o,ln+1);
			if (v>=100){
				*(o+i)=48+v/100;
				i++;
			}
			if (v>=10){
				*(o+i)=48+(v/10)%10;
				i++;
			}
			*(o+i)=48+v%10;
			*(o+i+1)='m';
			i+=2;
		}
		*(o+i)=0;
		KlMem_ret(o);
		return(o);
	}
}

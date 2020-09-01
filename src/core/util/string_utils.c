#include <string_utils.h>
#include <memory.h>
#include <types.h>
#include <stdarg.h>
#include <limits.h>



unsigned long str_len(char* s){
	KlMem_enter_func();
	unsigned long i=0;
	while (*(s+i)!=0){
		i++;
	}
	return(i);
}



char* str_clone(char* s){
	KlMem_enter_func();
	if (s==NULL){
		return(NULL);
	}
	unsigned long ln=str_len(s)+1;
	return(KlMem_memcpy(KlMem_malloc(ln),s,ln));
}



char* str_append(char* s,char c){
	KlMem_enter_func();
	unsigned long sz=str_len(s);
	char* o=KlMem_realloc(s,sz+2);
	o[sz]=c;
	o[sz+1]=0;
	KlMem_ret(o);
	return(o);
}



char* str_concat(char* a,char* b){
	KlMem_enter_func();
	char* o=KlMem_malloc(str_len(a)+str_len(b)+1);
	KlMem_memcpy(o,a,str_len(a));
	KlMem_memcpy(o+str_len(a),b,str_len(b));
	*(o+str_len(a)+str_len(b))=0;
	KlMem_ret(o);
	return(o);
}



char* str_substr(char* s,unsigned long i,unsigned long j){
	KlMem_enter_func();
	char* o=KlMem_malloc(j-i+1);
	for (unsigned long k=i;k<j;k++){
		*(o+k-i)=*(s+k);
	}
	o[j-i]=0;
	KlMem_ret(o);
	return(o);
}



unsigned long str_find(char* s,char c,unsigned long i){
	KlMem_enter_func();
	unsigned long j=i;
	while (*(s+j)!=0){
		if (*(s+j)==c){
			return(j-i);
		}
		j++;
	}
	return(ULONG_MAX);
}



unsigned long str_rfind(char* s,char c,unsigned long i){
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



bool str_cmp(char* a,char* b,unsigned long s,unsigned long l){
	KlMem_enter_func();
	for (unsigned long i=0;i<l;i++){
		if (*(a+s+i)!=*(b+i)){
			return(false);
		}
	}
	return(true);
}



char* str_format(char* t,...){
	KlMem_enter_func();
	va_list a;
	va_start(a,t);
	char* o=str_format_va(t,a);
	va_end(a);
	return(o);
}



char* str_format_va(char* t,va_list a){
	KlMem_enter_func();
	unsigned long ln=str_len(t)+1;
	char* o=KlMem_calloc(ln,1);
	unsigned long i=0;
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
					case 'h':
						*(o+i)='h';
						i++;
						// printH(va_arg(a,unsigned long));
						break;
					case 'l':
						*(o+i)='l';
						i++;
						// printL(va_arg(a,long));
						break;
					case 'L':
						*(o+i)='L';
						i++;
						// printLL(va_arg(a,long long int));
						break;
					case 'd':
						*(o+i)='d';
						i++;
						// printD(va_arg(a,double));
						break;
					case 'D':
						*(o+i)='D';
						i++;
						// printLD(va_arg(a,long double));
						break;
					case 'p':
						{
#ifdef WIN32
							ln+=10;
#else
							ln+=18;
#endif
							o=KlMem_realloc(o,ln);
							*(o+i)='0';
							*(o+i+1)='x';
							i+=2;
							PTR_TYPE p=(PTR_TYPE)va_arg(a,void*);
#ifdef WIN32
							for (signed char j=28;j>=0;j-=4){
#else
							for (signed char j=60;j>=0;j-=4){
#endif
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
							ln+=str_len(s);
							o=KlMem_realloc(o,ln);
							while (*s!=0){
								*(o+i)=*s;
								i++;
								s++;
							}
						}
						break;
					case 'f':
						{
							unsigned long long int f=va_arg(a,unsigned long long int);
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



char* str_escape_ansi(unsigned long long int f){
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

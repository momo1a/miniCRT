#include "minicrt.h"

#ifdef WIN32
#include <windows.h>
#endif

extern int main(int argc,char **argv);

void exit(int);

static void crt_fatal_error(const char* msg)
{
	printf("fatal error %s",msg);
	exit(1);
}

void mini_crt_entry(void)
{
	int ret;
#ifdef WIN32
	/* WINDOWS */
	int flag = 0;
	int argc = 0;
	char* argv[16];
	char* cl = GetCommandLineA();
	argv[0] = cl;
	argc++;
	while(*cl){
		if(*cl == '\"'){
			if(flag == 0 ) flag =1;
			else flag =0;
		}else if(*cl == ' ' && flag == 0){
			if(*(cl+1)){
				argv[argc] = cl+1;
				argc++;
			}
			*cl = '\0';
		}
		cl++;
	}
#else
	/* LIUNX */
	int argc;
	char** argv;
	char *ebp_reg = 0; /* %ebp 寄存器*/
	asm("movl %%esp,%0 \n":"=r"(ebp_reg));
	argc = *(int*)(ebp_reg + 4);
	argv = (char**)(ebp_reg + 8);
#endif

	if(!mini_crt_heap_init())  // 初始化堆
			crt_fatal_error("heap initialize fatal!");
		if(!mini_crt_io_init())  // 初始化IO
			crt_fatal_error("IO initialize fatal!");

	ret = main(argc,argv);
	exit(ret);
}

void exit(int exitCode)
{
	//mini_crt_call_exit_routine();
#ifdef WIN32
	ExitProcess(exitCode);
#else
	asm( "movl %0,%%ebx \n\t"
		 "movl $1,%%eax \n\t"
		 "int $0x80 \n\t"
		 "hlt \n\t"::"m" (exitCode));
#endif
}


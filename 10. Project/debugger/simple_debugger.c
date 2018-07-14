// Build : gcc -o simple_debugger simple_debugger.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <elf.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/mman.h>


typedef struct handle
{
	Elf64_Ehdr *ehdr;
	Elf64_Phdr *phdr;
	Elf64_Shdr *shdr;

	uint8_t *mem;
	char *symname;
	Elf64_Addr symaddr;
	struct user_regs_struct pt_reg;
	char * exec;

} handle_t;


Elf64_Addr lookup_symbol(handle_t *, const char *);

int main(int argc, char * argv[], char **envp)
{
	int fd;
	handle_t h;
	struct stat st;
	long trap, orig;
	int status, pid;
	char * args[2];

	if( argc < 3 )
	{
		printf("Usage : %s [Program] [Function] \n", argv[0]);
		exit(0);
	}
	
	if( (h.exec = strdup( argv[1])) == NULL )
	{
		perror("strdup error! \n");
		exit(-1);
	}
	
	args[0] = h.exec;
	args[1] = NULL;

	if( (h.symname = strdup(argv[2])) == NULL )
	{
		perror("strdup error! \n");
		exit(-1);
	}

	if( (fd = open(argv[1], O_RDONLY)) < 0 )
	{
		perror("open error! \n");
		exit(-1);
	}
	
	if( fstat( fd, &st ) < 0)
	{
		perror("fstat error! \n");
		exit(-1);
	}

	h.mem = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	if( h.mem == MAP_FAILED )
	{
		perror("mmap error! \n");
		exit(-1);
	}
	
	h.ehdr = (Elf64_Ehdr *) h.mem;
	h.phdr = (Elf64_Phdr *) (h.mem + h.ehdr->e_phoff);
	h.shdr = (Elf64_Shdr *) (h.mem + h.ehdr->e_shoff);
	
	if( h.mem[0] != 0x7f || strncmp((char *)&h.mem[1], "ELF", 3) ) 
	{
		printf("%s is not an ELF file \n", h.exec);
		exit(-1);
	}
	
	if( h.ehdr->e_type != ET_EXEC )
	{
		printf("%s is not an ELF executable \n", h.exec);
		exit(-1);
	}

	if( h.ehdr->e_shstrndx == 0 || h.ehdr->e_shoff == 0 || h.ehdr->e_shnum == 0 ) 
	{
		printf("Sectioin Header Table Not Found \n");
		exit(-1);
	}
	
	if( (h.symaddr = lookup_symbol(&h, h.symname)) == 0 )
	{
		printf("Unable to find Symbol : %s not found in executable \n", h.symname);
		exit(-1);
	}
	
	close(fd);
	
	if( (pid = fork()) < 0 )
	{
		perror("fork error! \n");
		exit(-1);
	}	

	if (pid == 0)
	{
		if( ptrace(PTRACE_TRACEME, pid, NULL, NULL ) < 0 )
		{
			perror("ptrace error! \n");
			exit(-1);
		}

		execve(h.exec, args, envp);

		exit(0);
	}

	wait(&status);

	printf("Beginning analysis of pid : %d at %lx \n", pid, h.symaddr);

	if( (orig = ptrace( PTRACE_PEEKTEXT, pid, h.symaddr, NULL)) < 0 )
	{
		perror("PTRACE_PEEKTEXT Error! \n");
		exit(-1);
	}
		
	trap = (orig & ~0xff) | 0xcc;

	if( ptrace( PTRACE_POKETEXT, pid, h.symaddr, trap) < 0 )
	{
		perror("PTRACE_POKETEXT");
		exit(-1);
	}
	
trace:

	if( ptrace(PTRACE_CONT, pid, NULL, NULL ) < 0 )
	{
		perror("PTRACE_CONT Error! \n");
		exit( -1 );
	}

	wait(&status);

	if( WIFSTOPPED( status ) && WSTOPSIG( status ) == SIGTRAP )
	{
		if( ptrace( PTRACE_GETREGS, pid, NULL, NULL, &h.pt_reg ) < 0 )
		{
			perror("PTRACE_GETREGS Error! \n");
			exit(-1);
		}
	
		printf("Executable %s (Pid : %d) has hit breakpoint! 0x%lx \n", h.exec, pid, h.symaddr);

		printf("%%rcx: %llx\n%%rdx: %llx\n%%rbx: %llx\n"
				"%%rax: %llx\n%%rdi: %llx\n%%rsi: %llx\n"
				"%%r8: %llx\n%%r9: %llx\nr10: %llx\n"
				"%%r11: %llx\n%%r12: %llx\n%%r13: %llx\n"
				"%%r14: %llx\n%%r15: %llx\n%%rsp: %llx\n",
				h.pt_reg.rcx, h.pt_reg.rdx, h.pt_reg.rbx, 
				h.pt_reg.rax, h.pt_reg.rdi, h.pt_reg.rsi,
				h.pt_reg.r8, h.pt_reg.r9, h.pt_reg.r10,
				h.pt_reg.r11, h.pt_reg.r12, h.pt_reg.r13,
				h.pt_reg.r14, h.pt_reg.r15, h.pt_reg.rsp);

		printf("Please hit any key to continue! \n");
		getchar();

		if( ptrace(PTRACE_POKETEXT, pid, NULL, &h.pt_reg) < 0 )
		{
			perror("PTRACE_SETREGS Error! \n");
			exit(-1);
		}

		if( ptrace( PTRACE_SINGLESTEP, pid, NULL, NULL) < 0)
		{
			perror("PTRACE_SINGLESTEP Error! \n");
			exit(-1);
		}

		wait(NULL);

		if( ptrace( PTRACE_POKETEXT, pid, h.symaddr, trap) < 0 )
		{
			perror("PTRACE_POKETEXT Error! \n");
			exit(-1);
		}

		goto trace;

		if( WIFEXITED(status) )
		{
			printf("Completed tracing pid : %d \n", pid);
			exit(0);
		}	
	}

	return 0;
}


Elf64_Addr lookup_symbol(handle_t *h, const char * symname)
{
	int i, j;

	char * strtab;

	Elf64_Sym *symtab;

	for( i = 0; i < h->ehdr->e_shnum; i++)
	{
		strtab = ( char * ) &h->mem[h->shdr[h->shdr[i].sh_link].sh_offset];

		symtab = (Elf64_Sym *) &h->mem[h->shdr[i].sh_offset];

		for( j = 0; j < h->shdr[i].sh_size / sizeof(Elf64_Sym) ; j++)
		{
			printf("Addr : 0x%x \n", &strtab[symtab->st_name]);
			
			printf("String : %s \n", &strtab[symtab->st_name]);

			if( strcmp(&strtab[symtab->st_name], symname) == 0 )
			{
				return (symtab->st_value);
			}

			symtab++;
		}
	}
	
	return 0;
}




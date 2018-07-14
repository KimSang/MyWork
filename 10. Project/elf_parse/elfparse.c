// Build : gcc -o elf_parse elf_parse.c 
//
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <elf.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char * argv[])
{

	int fd, i; 
	uint8_t *mem;
	struct stat st;
	char *stringTable, *interp;

	Elf32_Ehdr *ehdr;
	Elf32_Phdr *phdr;
	Elf32_Shdr * shdr;

	if( argc < 2 )
	{
		printf("Usage : %s <executable file> \n", argv[0]);
		exit(0);
	}

	if( (fd = open(argv[1], O_RDONLY)) < 0 )
	{
		perror("Open Error! \n");

		exit(-1);
	}
	
	if( fstat(fd, &st) < 0 )
	{
		perror("fstat error! \n");
		exit(-1);
	}

	mem = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	if( mem == MAP_FAILED )
	{
		perror("mmap error! \n");
		exit(-1);
	}
	
	ehdr = (Elf32_Ehdr *) mem;

	phdr = (Elf32_Phdr*) &mem[ehdr->e_phoff];

	shdr = (Elf32_Shdr*) &mem[ehdr->e_shoff];

	if( mem[0] != 0x7f && strncmp(&mem[1], "ELF", 3)) 
	{
		fprintf(stderr, "%s is not an ELF file \n", argv[1]);

		exit(-1);
	}
	
	if( ehdr->e_type != ET_EXEC )
	{
		fprintf(stderr, "%s is not an executable \n", argv[1]);
		exit(-1);
	}

	printf("Program Entry point : 0x%x \n", ehdr->e_entry);

	stringTable = &mem[shdr[ehdr->e_shstrndx].sh_offset];

	printf("[Section Header List] \n");
	
	for( i = 1; i < ehdr->e_shnum; i++)
	{
		printf("%s : 0x%x\n", &stringTable[shdr[i].sh_name], shdr[i].sh_addr);
	}

	printf("[Program Header List]\n");

	for(i = 0; i < ehdr->e_phnum; i++)
	{
		switch( phdr[i].p_type )
		{
			case PT_LOAD:
				if( phdr[i].p_offset == 0)
				{
					printf("Text Segment : 0x%x\n", phdr[i].p_vaddr);
				}
				else
				{
					printf("Data Segment : 0x%x\n", phdr[i].p_vaddr);
				}
				break;

			case PT_INTERP:
				interp = strdup( (char *) &mem[phdr[i].p_offset] );
				printf("Interpreter : %s \n", interp);
				break;

			case PT_NOTE:
				printf("Note Segment : 0x%x\n", phdr[i].p_vaddr);
				break;

			case PT_DYNAMIC:
				printf("Dynamic Segment : 0x%x\n", phdr[i].p_vaddr);
				break;

			case PT_PHDR:
				printf("Phdr Segment : 0x%x\n", phdr[i].p_vaddr);
				break;
		}
	}

	return 0;
}

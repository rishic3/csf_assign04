#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cerrno>
#include <cstdint>
#include <elf.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <fcntl.h>

#include "elf_names.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Not an ELF file");
        return 0;
    }

    int fd = open(argv[1], O_RDONLY);

    if (fd < 0) {
	printf("Not an ELF file");
	return 0;
    }

    struct stat statbuf;
    int rc = fstat(fd, &statbuf);
    if (rc != 0) {
  	printf("ERROR: Invalid number of bytes");
  	return 0;
    }
    size_t file_size = statbuf.st_size;
    void *data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == (void *)-1) {
	printf("ERROR: Invalid region of memory");
    }

    Elf64_Ehdr *elf_header = (Elf64_Ehdr *) data;
    //printf(".shstrtab section index is %u\n", elf_header->e_shstrndx);

    printf("Object file type: %s\n", get_type_name(elf_header->e_type));
    printf("Instruction set: %s\n", get_machine_name(elf_header->e_machine));
    if (elf_header->e_ident[EI_DATA] == 1) {
	printf("Endianness: Little endian\n");
    } else {
	printf("Endianness: Big endian\n");
    }

    int sectionRange = elf_header->e_shnum;	// number of section headers
    Elf64_Shdr *section_header = (Elf64_Shdr *) elf_header->e_shoff;	// pointer to the start of section header table
    int string_table_index = elf_header->e_shstrndx;	// index of section header table entry that contains section names

    for (int i = 0; i < sectionRange; i++) {

	//printf("Section header %u: name=%s, type=%lx, offset=%lx, size=%lx", i, )

    }
}

// Start with elf header
// Find out where the section headers are
// Find out which section header is the section header string table (a field in the elf header will tell us this)
// Find the data associated with the section header string table, allowing us to use the sh_name fields in the section header entries to look up, in the section header string table, the name of each section
// Now we know which section is which, allowing us to focus on finding and iterating over the symbol table

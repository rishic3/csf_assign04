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

#include <iostream>
#include <errno.h>

using namespace std;

int main(int argc, char **argv) {

    if (argc != 2) {
        return -1;
    }

    int fd = open(argv[1], O_RDONLY);

    if (fd < 0) {
	    perror("open failed: ");
	    return -1;
    }

    struct stat statbuf;
    int rc = fstat(fd, &statbuf);

    if (rc != 0) {
        perror("fstat failed: ");
  	    return -1;
    }

    size_t file_size = statbuf.st_size;
    unsigned char *data = (unsigned char *) mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
	    perror("mmap failed: ");
        return -1;
    }

    Elf64_Ehdr *elf_header = (Elf64_Ehdr *) data;
    //printf(".shstrtab section index is %u\n", elf_header->e_shstrndx);

    if (file_size < 4) {
        printf("Not an ELF file\n");
        return 0;
    }

    // first 4 bytes: {0x7f, 'E', 'L', 'F'}

    if (!((elf_header->e_ident[0] == 0x7f) && (elf_header->e_ident[1] == 'E') && (elf_header->e_ident[2] == 'L') && (elf_header->e_ident[3] == 'F'))) {
        printf("Not an ELF file\n");
        return 0;
    }

    printf("Object file type: %s\n", get_type_name(elf_header->e_type));
    printf("Instruction set: %s\n", get_machine_name(elf_header->e_machine));
    if (elf_header->e_ident[EI_DATA] == 1) {
	    printf("Endianness: Little endian\n");
    } else {
	    printf("Endianness: Big endian\n");
    }

    int sectionRange = elf_header->e_shnum;	// number of section headers
    Elf64_Shdr *section_header = (Elf64_Shdr *) (data + elf_header->e_shoff);	// pointer to the start of section header table
    int string_table_index = elf_header->e_shstrndx;	// index of section header table entry that contains section names
    Elf64_Shdr *shstrtab = &(section_header[string_table_index]);
    //Elf32_Shdr *sh_strtab = &shdr[ehdr->e_shstrndx];
    const char *const shstrtab_p = data + shstrtab->sh_offset;

    for (int i = 0; i < sectionRange; i++) {

        char *name = shstrtab_p + section_header[i].sh_name;
	    printf("Section header %u: name=%s, type=%lx, offset=%lx, size=%lx\n", i, name, section_header[i].sh_type, section_header[i].sh_offset, section_header[i].sh_size);
        //name currently not working, everything else is though.

    }
}

// Start with elf header
// Find out where the section headers are
// Find out which section header is the section header string table (a field in the elf header will tell us this)
// Find the data associated with the section header string table, allowing us to use the sh_name fields in the section header entries to look up, in the section header string table, the name of each section
// Now we know which section is which, allowing us to focus on finding and iterating over the symbol table

#ifndef ELOADER_ELF
#define ELOADER_ELF

#include <common/sdk.h>

/*******************/
/* ELF typedefs */
/*******************/
/* Types for ELF file manipulation */
/* Be sure to modify when used on other platform */
typedef u32* Elf32_Addr;
typedef int Elf32_Off;
typedef int Elf32_Sword;
typedef int Elf32_Word;
typedef unsigned short int Elf32_Half;
typedef char BYTE;

/*************/
/* ELF types */
/*************/
#define ELF_STATIC 0x0002     /* Static ELF */
#define ELF_RELOC 0xffa0      /* Relocatable ELF, aka PRX */

/**************/
/* ELF HEADER */
/**************/

#define EI_NIDENT 16 //Size of e_ident[]
typedef struct
{
    BYTE e_ident[EI_NIDENT];  //Magic number
    Elf32_Half e_type;      // Identifies object file type
    Elf32_Half e_machine;   // Architecture build
    Elf32_Word e_version;   // Object file version
    Elf32_Addr e_entry;     // Virtual address of code entry
    Elf32_Off e_phoff;      // Program header table's file offset in bytes
    Elf32_Off e_shoff;      // Section header table's file offset in bytes
    Elf32_Word e_flags;     // Processor specific flags
    Elf32_Half e_ehsize;    // ELF header size in bytes
    Elf32_Half e_phentsize; // Program header size (all the same size)
    Elf32_Half e_phnum;     // Number of program headers
    Elf32_Half e_shentsize; // Section header size (all the same size)
    Elf32_Half e_shnum;     // Number of section headers
    Elf32_Half e_shstrndx;  // Section header table index of the entry associated with the
                            // section name string table.
} Elf32_Ehdr;

/* e_ident */
#define EI_MAG0 0    //File identification
#define EI_MAG1 1    //File identification
#define EI_MAG2 2    //File identification
#define EI_MAG3 3    //File identification
#define EI_CLASS 4   //File class
#define EI_DATA 5    //Data encoding
#define EI_VERSION 6 //File version
#define EI_PAD 7     //Start of padding bytes in header (should be set to zero)

/* Magic number for ELF */
#define ELFMAG0 0x7f //e_ident[EI_MAG0]
#define ELFMAG1 'E'  //e_ident[EI_MAG1]
#define ELFMAG2 'L'  //e_ident[EI_MAG2]
#define ELFMAG3 'F'  //e_ident[EI_MAG3]

/* File class */
#define ELFCLASSNONE 0 //Invalid class
#define ELFCLASS32 1   //32-bit objects
#define ELFCLASS64 2   //64-bit objects

/* Data encoding */
#define ELFDATANONE 0 //Invalid data encoding
#define ELFDATA2LSB 1 //Little-endian
#define ELFDATA2MSB 2 //Big-endian

/* e_type */
#define ET_NONE 0        //No file type
#define ET_REL 1         //Relocatable file
#define ET_EXEC 2        //Executable file
#define ET_DYN 3         //Shared object file
#define ET_CORE 4        //Core file
#define ET_LOPROC 0xff00 //Processor-specific
#define ET_HIPROC 0xffff //Processor-specific

/* e_machine */
#define ET_NONE 0         //No machine
#define EM_M32 1          //AT&T WE 32100
#define EM_SPARC 2        //SPARC
#define EM_386 3          //Intel Architecture
#define EM_68K 4          //Motorola 68000
#define EM_88K 5          //Motorola 88000
#define EM_860 7          //Intel 80860
#define EM_MIPS 8         //MIPS RS3000
#define EM_MIPS_RS4_BE 10 //MIPS RS4000 Big-endian

/* e_version */
#define EV_NONE 0    //Invalid version
#define EV_CURRENT 1 //Current version

/******************/
/* SECTION HEADER */
/******************/

typedef struct
{
    Elf32_Word sh_name;       //Name of section (value is index to string table)
    Elf32_Word sh_type;       //Type of section
    Elf32_Word sh_flags;      //Flags :P
    Elf32_Addr sh_addr;       //Address in process image (0 -> not used)
    Elf32_Off  sh_offset;     //Section offset in file
    Elf32_Word sh_size;       //Section size in bytes
    Elf32_Word sh_link;       //Section header table index link
    Elf32_Word sh_info;       //Extra info
    Elf32_Word sh_addralign;  //Alignment
    Elf32_Word sh_entsize;    //Some sections hold a table of fixed-size entries, such as
                              //a symbol table. This member gives the size of each entry.
} Elf32_Shdr;

/******************/
/* PROGRAM HEADER */
/******************/

typedef struct
{
    Elf32_Word p_type;      // Type of segment
    Elf32_Off p_offset;     // Offset for segment's first byte in file
    Elf32_Addr p_vaddr;     // Virtual address for segment
    Elf32_Addr p_paddr;     // Physical address for segment
    Elf32_Word p_filesz;    // Segment image size in file
    Elf32_Word p_memsz;     // Segment image size in memory
    Elf32_Word p_flags;     // Flags :P
    Elf32_Word p_align;     // Alignment
} Elf32_Phdr;

/* p_type */

#define PT_NULL 0
#define PT_LOAD 1               // Loadable segment
#define PT_DYNAMIC 2            // Specifies dynamic linking info
#define PT_INTERP 3             // Location and size of interpreter
#define PT_NOTE 4               // Location and size of additional info
#define PT_SHLIB 5              // Reserved
#define PT_PHDR 6               // Not relevant
#define PT_LOPROC 0x70000000    // Processor-specific semantics
#define PT_HIPROC 0x7fffffff

/*******************/
/* .lib.stub entry */
/*******************/
typedef struct
{
	Elf32_Addr library_name;     // Pointer to library name
	Elf32_Half import_flags;
	Elf32_Half library_version;
	Elf32_Half import_stubs;
	Elf32_Half stub_size;        // Number of stubs imported from library
	Elf32_Addr nid_pointer;      // Pointer to array of NIDs from library
	Elf32_Addr jump_pointer;     // Pointer to array of stubs from library
} tStubEntry;

/******************/
/* PRX relocation */
/******************/

// Relocation section type
#define LOPROC 0x700000a0

// Relocation entry
typedef struct
{
    Elf32_Addr r_offset; // Offset of relocation
    Elf32_Word r_info;   // Packed information about relocation
} tRelEntry;

/* Macros for the r_info field */
/* Determines which program header the current address value in memory should be relocated from */
#define ELF32_R_ADDR_BASE(i) (((i)> >16) & 0xFF)

/* Determines which program header the r_offset field is based from */
#define ELF32_R_OFS_BASE(i) (((i)> >8) & 0xFF)

/* Determines type of relocation needed, see defines below */
#define ELF32_R_TYPE(i) (i&0xFF)

/* MIPS Relocation Entry Types */
#define R_MIPS_NONE 0
#define R_MIPS_16 1
#define R_MIPS_32 2
#define R_MIPS_REL32 3
#define R_MIPS_26 4
#define R_MIPS_HI16 5
#define R_MIPS_LO16 6
#define R_MIPS_GPREL16 7
#define R_MIPS_LITERAL 8
#define R_MIPS_GOT16 9
#define R_MIPS_PC16 10
#define R_MIPS_CALL16 11
#define R_MIPS_GPREL32 12

#define MAX_MODULE_NAME 0x1c

/* .rodata.sceModuleInfo */
typedef struct
{
	Elf32_Half module_attributes;
	Elf32_Half module_version;
	BYTE       module_name[MAX_MODULE_NAME];
	Elf32_Addr gp;
	Elf32_Addr library_entry;
	Elf32_Addr library_entry_end;
	Elf32_Addr library_stubs;
	Elf32_Addr library_stubs_end;
} tModInfoEntry;

// .lib.ent
typedef struct
{
	Elf32_Addr name;
	Elf32_Half version;
	Elf32_Half attributes;
	BYTE       size;  //Size of exports in 64-bit words
	BYTE       num_variables;
	Elf32_Half num_functions;
	Elf32_Addr exports_pointer;
} tExportEntry;

/**************/
/* PROTOTYPES */
/**************/

/* Returns 1 if header is ELF, 0 otherwise */
int elf_check_magic(Elf32_Ehdr* pelf_header);

/* Load static executable in memory using virtual address */
/* Returns total size copied in memory */
unsigned int elf_load_program(SceUID elf_file, SceOff start_offset, Elf32_Ehdr* pelf_header, unsigned int* size);

// Load relocatable executable in memory using fixed address
// and fills pointer to stub with first stub entry
// Returns total size copied in memory
unsigned int prx_load_program(SceUID elf_file, SceOff start_offset, Elf32_Ehdr* pelf_header, tStubEntry** pstub_entry, u32* size, void** addr);

// Get index of section if you know the section name
int elf_get_section_index_by_section_name(SceUID elf_file, SceOff start_offset, Elf32_Ehdr* pelf_header, char* section_name_to_find);

/* Copies the string pointed by table_offset into "buffer" */
/* WARNING: modifies file pointer. This behaviour MUST be changed */
unsigned int elf_read_string(SceUID elf_file, Elf32_Off table_offset, char *buffer);

/* Returns size and address (pstub) of ".lib.stub" section (imports) */
unsigned int elf_find_imports(SceUID elf_file, SceOff start_offset, Elf32_Ehdr* pelf_header, tStubEntry** pstub);

// Extracts ELF from PBP,returns pointer to EBOOT File & fills offset
SceUID elf_eboot_extract_open(const char* eboot_path, SceOff *offset);

// Get ELF GP value
u32 getGP(SceUID elf_file, SceOff start_offset, Elf32_Ehdr* pelf_header);

// Returns !=0 if stub entry is valid, 0 if it's not
int elf_check_stub_entry(tStubEntry* pentry);

#endif


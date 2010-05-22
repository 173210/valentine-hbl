#include "sdk.h"
#include "debug.h"
#include "elf.h"
#include "eloader.h"
#include "malloc.h"

// Relocates based on a MIPS relocation type
// Returns 0 on success, -1 on fail
int relocate_entry(tRelEntry reloc_entry, void* reloc_addr)
{
	u32 buffer = 0, code = 0, offset = 0, offset_target, i;

	// Actual offset
	offset_target = (u32)reloc_entry.r_offset + (u32)reloc_addr;

	// Load word to be relocated into buffer
    u32 misalign = offset_target%4;
    if (misalign) 
	{
        u32 array[2];
        array[0] = *(u32*)(offset_target - misalign);
        array[1] = *(u32*)(offset_target + 4 - misalign);
        u8* array8 = (u8*)&array;
        u8* buffer8 = (u8*)&buffer;
        for (i = 3 + misalign; i >= misalign; --i)
		{
            buffer8[i-misalign] = array8[i];
        }
    } 
	else 
	{
        buffer = *(u32*)offset_target;
    }
	
	//Relocate depending on reloc type
	switch(ELF32_R_TYPE(reloc_entry.r_info))
	{
		// Nothing
		case R_MIPS_NONE:
			return 0;
			
		// 32-bit address
		case R_MIPS_32:
			buffer += (u32)reloc_addr;
			break;
		
		// Jump instruction
		case R_MIPS_26:    
			code = buffer & 0xfc000000;
			offset = (buffer & 0x03ffffff) << 2;
			offset += (u32)reloc_addr;
			buffer = ((offset >> 2) & 0x03ffffff) | code;
			break;
		
		// Low 16 bits relocate as high 16 bits
		case R_MIPS_HI16:     
			offset = (buffer << 16) + (u32)reloc_addr;
			offset = offset >> 16;
			buffer = (buffer & 0xffff0000) | offset;
			break;
		
		// Low 16 bits relocate as low 16 bits
		case R_MIPS_LO16:       
			offset = (buffer & 0x0000ffff) + (u32)reloc_addr;
			buffer = (buffer & 0xffff0000) | (offset & 0x0000ffff);
			break;
		
		default:
			return -1;		
	}
	
	// Restore relocated word
    if (misalign) 
	{
        u32 array[2];
        array[0] = *(u32*)(offset_target - misalign);
        array[1] = *(u32*)(offset_target + 4 - misalign);
        u8* array8 = (u8*)&array;
        u8* buffer8 = (u8*)&buffer;
        for (i = 3 + misalign; i >= misalign; --i)
		{
             array8[i] = buffer8[i-misalign];
        }
        *(u32*)(offset_target - misalign) = array[0];
        *(u32*)(offset_target + 4 - misalign) = array[1];
    } 
	else 
	{ 
        *(u32*)offset_target = buffer;
    }

	return 0;
}

// Relocates PRX sections that need to
// Returns number of relocated entries
unsigned int relocate_sections(SceUID elf_file, SceOff start_offset, Elf32_Ehdr *pelf_header, void* reloc_addr)
{
	Elf32_Half i;
	Elf32_Shdr sec_header;
	Elf32_Off strtab_offset;
	Elf32_Off cur_offset;
	unsigned int j, entries_relocated = 0, num_entries;
	tRelEntry* reloc_entry;

	LOGSTR0("relocate_sections\n");
	
	// Seek string table
	sceIoLseek(elf_file, start_offset + pelf_header->e_shoff + pelf_header->e_shstrndx * sizeof(Elf32_Shdr), PSP_SEEK_SET);
	sceIoRead(elf_file, &sec_header, sizeof(Elf32_Shdr));
	strtab_offset = sec_header.sh_offset;

	//LOGSTR1("String table offset: 0x%08lX\n", strtab_offset);

	// First section header
	cur_offset = pelf_header->e_shoff;
	
	LOGSTR1("Number of sections: %d\n", pelf_header->e_shnum);

	// Browse all section headers
	for(i=0; i<pelf_header->e_shnum; i++)
	{
		// Get section header
		sceIoLseek(elf_file, start_offset + cur_offset, PSP_SEEK_SET);
		sceIoRead(elf_file, &sec_header, sizeof(Elf32_Shdr));

		if(sec_header.sh_type == LOPROC)
		{
			//LOGSTR0("Relocating...\n");
			
			// Allocate memory for section
			num_entries = sec_header.sh_size / sizeof(tRelEntry);
			reloc_entry = malloc(sec_header.sh_size);
			if(!reloc_entry)
			{
				LOGSTR1("\nWARNING: cannot allocate memory (size: %d) for section\n", sec_header.sh_size);
				LOGSTR2("Free memory: %d (max: %d)\n ", sceKernelTotalFreeMemSize(), sceKernelMaxFreeMemSize());
				continue;
			}
			
			// Read section
			sceIoLseek(elf_file, start_offset + sec_header.sh_offset, PSP_SEEK_SET);
			sceIoRead(elf_file, reloc_entry, sec_header.sh_size);

			for(j=0; j<num_entries; j++)
			{
				//DEBUG_PRINT(" RELOC_ENTRY ", &(reloc_entry[j]), sizeof(tRelEntry));	
                relocate_entry(reloc_entry[j], reloc_addr);
				entries_relocated++;
			}
			
			// Free section memory
			free(reloc_entry);

			sceKernelDcacheWritebackInvalidateAll();
		}

		// Next section header
		cur_offset += sizeof(Elf32_Shdr);
	}

	// All relocation section processed
	return entries_relocated;
}

/* NIDTOHUMAN by m0skit0

Transforms a NID table dump into human readable format

NID table should be in the following format:

NID 0x20 SYSCALL 0x20 NID 0x20 SYSCALL 0x20...

0x20 is a space, can be any other value */

#include <stdio.h>
#include <stdlib.h>

char nibble_to_hexchar(char nibble)
{
	if (nibble <= 9)
		return (nibble + 0x30);
	
	else
		return (nibble + 0x41 - 0xA);
}

void hex_to_string(unsigned int hex, char* buffer)
{
	buffer[0] = '0';
	buffer[1] = 'x';
	buffer[2] = nibble_to_hexchar((hex >> 28) & 0x0000000F);
	buffer[3] = nibble_to_hexchar((hex >> 24) & 0x0000000F);
	buffer[4] = nibble_to_hexchar((hex >> 20) & 0x0000000F);
	buffer[5] = nibble_to_hexchar((hex >> 16) & 0x0000000F);
	buffer[6] = nibble_to_hexchar((hex >> 12) & 0x0000000F);
	buffer[7] = nibble_to_hexchar((hex >> 8) & 0x0000000F);
	buffer[8] = nibble_to_hexchar((hex >> 4) & 0x0000000F);
	buffer[9] = nibble_to_hexchar(hex & 0x0000000F);
}

int main(int argc, char* argv[])
{
	FILE* nidtable_fd;
	unsigned int hex1, hex2;
	char hex_chars[11] = {"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"};
	char sep;

	if (argc < 2)
	{
		printf("File path missing\n");
		exit(1);
	}

	nidtable_fd = fopen(argv[1], "r");

	if (nidtable_fd == NULL)
	{
		printf("Cannot open specified file\n");
		exit(1);
	}

	printf("NID\t\tSYSCALL\n");
	printf("---\t\t-------\n");

	while(fread(&hex1, sizeof(hex1), 1, nidtable_fd) > 0)
	{
		fread(&sep, sizeof(sep), 1, nidtable_fd);

		fread(&hex2, sizeof(hex2), 1, nidtable_fd);

		// Only output syscalls
		if (hex2 < 0x3000)
		{
			hex_to_string(hex1, hex_chars);
			printf("%s", hex_chars);
			hex_to_string(hex2, hex_chars);			
			printf("\t%s\n", hex_chars);
		}

		fread(&sep, sizeof(sep), 1, nidtable_fd);
	}

	fclose(nidtable_fd);

	return 0;
}


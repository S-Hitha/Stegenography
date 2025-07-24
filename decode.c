#include <stdio.h>
#include "decode.h"
#include<string.h>
#include"common.h"
#include "types.h"
#include <unistd.h>

Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo)
{
	if(strstr(argv[2], ".bmp") != NULL)
	{
		decInfo -> src_image_fname = argv[2];
	}
	else
		return e_failure;

	if(argv[3] != NULL)
	{
		decInfo -> output_fname = argv[3];
	}	
	else
	{
		decInfo -> output_fname = "output.txt";
	}
	return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
	if(open_file(decInfo) == e_success)
	{
		printf(PURPLE"Files are opened successfully\n"RESET);
		sleep(1);
		if(decode_magic_string(decInfo) == e_success)
		{
			printf(PURPLE"Magic_strings are decoded successfully\n"RESET);
			sleep(1);
			if(decode_secret_file_extn_size(decInfo) == e_success)
			{
				printf(PURPLE"Secret_file_extn_size is decoded successfully\n"RESET);
				sleep(1);
				if(decode_secret_file_extn(decInfo) == e_success)
				{
					printf(PURPLE"secret_file_extn is decoded successfully\n"RESET);
					sleep(1);
					if(decode_secret_file_size(decInfo) == e_success)
					{
						printf(PURPLE"secret_file_size is decoded successfully\n"RESET);
						sleep(1);
						if(decode_secret_file_data(decInfo) == e_success)
						{
							printf(PURPLE"secret_file_data decoded successfully\n"RESET);
							sleep(1);
						}
						else
						{
							printf(YELLOW"secret_file_data function failed\n"RESET);
						}
					}
					else
					{
						printf(YELLOW"secret_file_size function failed\n"RESET);
					}
				}
				else
				{
					printf(YELLOW"secret_file_extn function failed\n"RESET);
				}
			}	
			else
			{
				printf(YELLOW"Secret_file_extn_size function failed\n"RESET);
			}
		}
		else
		{
			printf(YELLOW"Magic_string function failed\n"RESET);
		}
	}
	else
	{
		printf(YELLOW"Error : open_files failed\n"RESET);
	}
}

Status open_file(DecodeInfo *decInfo)
{
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "r");
    if (decInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, YELLOW"ERROR: Unable to open file %s\n"RESET, decInfo->src_image_fname);
    	return e_failure;
    }

    decInfo->fptr_output = fopen(decInfo -> output_fname, "w");
    
    if (decInfo->fptr_output == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, YELLOW"ERROR: Unable to open file %s\n"RESET, decInfo -> output_fname);
    	return e_failure;
    }
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
	char arr[8];
	char magic_string[strlen(MAGIC_STRING)+1];

	fseek(decInfo-> fptr_src_image, 54, SEEK_SET);

	for(int i=0;i<strlen(MAGIC_STRING);i++)
	{
		fread(arr, 8, 1, decInfo -> fptr_src_image);
		decode_byte_from_lsb(arr, &magic_string[i]);
	}
	magic_string[strlen(MAGIC_STRING)] = '\0';

	if(strcmp(magic_string, MAGIC_STRING) == 0)
	{
		return e_success;
	}
	else
		return e_failure;
}

Status decode_byte_from_lsb(char *arr, char *decode_byte)
{
	*decode_byte = 0;
	for(int i=0;i<8;i++)
	{
		*decode_byte |= ((arr[i] & 0x01) << (7-i));
	}

	return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
	char arr[32];
	int extn_size;
	fread(arr, 32, 1, decInfo -> fptr_src_image);
	decode_size_from_lsb(arr, &extn_size);
	decInfo -> extn_size = extn_size;
	return e_success;
}

Status decode_size_from_lsb(char *arr, int *size)
{
	*size = 0;
	for(int i=0;i<32;i++)
	{
		*size |= (arr[i] & 0x01) << (31 - i);
	}
	return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
	char arr[8];
	for(int i=0;i<decInfo -> extn_size;i++)
	{
		fread(arr, 8, 1, decInfo -> fptr_src_image);
		decode_byte_from_lsb(arr, &decInfo -> extn_secret_file[i]);
	}
	decInfo -> extn_secret_file[decInfo -> extn_size] = '\0';

	return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
	char arr[32];
	char file_size;
	fread(arr, 32, 1, decInfo -> fptr_src_image);
	decode_size_from_lsb(arr, (int *) &file_size);
	decInfo -> size_secret_file = file_size;

	return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
	char arr[8];
	char secret_byte;
	for(int i=0;i<decInfo -> size_secret_file;i++)
	{
		fread(arr, 8, 1, decInfo -> fptr_src_image);
		decode_byte_from_lsb(arr, &secret_byte);
		fwrite(&secret_byte, 1, 1, decInfo -> fptr_output);
	}

	return e_success;
}

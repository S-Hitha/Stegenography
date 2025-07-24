#include <stdio.h>
#include <unistd.h>
#include "encode.h"
#include<string.h>
#include"common.h"
#include "types.h"


/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)
{
	if(strstr(argv[2], ".bmp") != NULL)
	{
		encInfo -> src_image_fname = argv[2];
	}
	else
		return e_failure;

	if(strstr(argv[3], ".txt") != NULL)
	{
		encInfo -> secret_fname = argv[3];
		strcpy(encInfo -> extn_secret_file, strstr(argv[3], "."));
	}
	else
		return e_failure;

	if(argv[4] != NULL)
	{
		if(strstr(argv[4], ".bmp") != NULL)
		{
			encInfo -> stego_image_fname = argv[4];
		}
		else
			return e_failure;
	}
	else
	{
		encInfo -> stego_image_fname = "output.bmp";
	}
	return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
	if(open_files(encInfo) == e_success)
	{
		printf(RED"Files are opened successfully\n"RESET);
		sleep(1);
		if(check_capacity(encInfo) == e_success)
		{
			printf(RED"check_capacity is successfully done\n"RESET);
			sleep(1);
			if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
			{
				printf(RED"copy_bmp_header is successfully done\n"RESET);
				sleep(1);
				if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
				{
					printf(RED"Magic_strings are encoded successfully\n"RESET);
					sleep(1);
					if(encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo) == e_success)
					{
						printf(RED"Secret_file_extn_size is encoded successfully\n"RESET);
						sleep(1);
						if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
						{
							printf(RED"Secret_file_extn_size is encoded successfully\n"RESET);
							sleep(1);
							if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
							{
								printf(RED"secret_file_size is encoded successfully\n"RESET);
								sleep(1);
								if(encode_secret_file_data(encInfo) == e_success)
								{
									printf(RED"secret_file_data is encoded successfully\n"RESET);
									sleep(1);
									if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
									{
										sleep(1);
										printf(RED"Remaining data is successfully copied\n"RESET);
									}
									else
									{
										printf(YELLOW"copy_remaining_img_data function failed\n"RESET);
									}
								}
								else
								{
									printf(YELLOW"secret_file_data function failed\n"RESET);
								}
							}
							else
							{
								printf(YELLOW"encode_secret_file_size function failed\n"RESET);
							}
						}
						else
						{
							printf(YELLOW"secret_file_extn_size function failed\n"RESET);
						}
					}
					else
					{
						printf(YELLOW"encode_secret_file_extn_size function failed\n"RESET);
					}
				}
				else
				{
					printf(YELLOW"Magic_string function failed\n"RESET);
				}
			}
			else
			{
				printf(YELLOW"copy_bmp_header function is failed\n"RESET);
			}
		}
		else
		{
			printf(YELLOW"check_capacity function is failed\n"RESET);
		}
	}
	else
	{
		printf(YELLOW"Error : open_files failed\n"RESET);
	}
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
	char arr[8];
	for(int i=0;i<strlen(magic_string);i++)
	{
		fread(arr,8,1,encInfo -> fptr_src_image);
		encode_byte_to_lsb(magic_string[i], arr);
		fwrite(arr, 8, 1, encInfo -> fptr_stego_image);
	}
	return e_success;
}

Status encode_byte_to_lsb(char data, char *arr)
{
	for(int i=0;i<8;i++)
	{
		arr[i]=(arr[i] & 0xFE) | (data & (1<<(7-i))) >> (7-i);

	}
}

Status encode_size_to_lsb(int data, char *arr)
{
	for(int i=0;i<32;i++)
	{
		arr[i]=(arr[i] & (~1)) | (data & (1 << (31-i))) >> (31-i);
	}
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
	char arr[32];
	fread(arr, 32, 1, encInfo -> fptr_src_image);
	encode_size_to_lsb(size, arr);
	fwrite(arr, 32, 1, encInfo -> fptr_stego_image);
	return e_success;
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
	char arr[8];
	for(int i=0;i<strlen(file_extn);i++)
	{
		fread(arr, 8, 1, encInfo -> fptr_src_image);
		encode_byte_to_lsb(file_extn[i], arr);
		fwrite(arr, 8, 1,encInfo -> fptr_stego_image);

	}
	return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
	char arr[32];
	fread(arr, 32, 1, encInfo -> fptr_src_image);
	encode_size_to_lsb(file_size, arr);
	fwrite(arr, 32, 1, encInfo -> fptr_stego_image);
	return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
	char secret_data[encInfo -> size_secret_file];
	rewind(encInfo -> fptr_secret);
	fread(secret_data,encInfo -> size_secret_file, 1, encInfo -> fptr_secret);
	char arr[8];
	for(int i=0;i<encInfo->size_secret_file;i++)
	{
		fread(arr, 8, 1, encInfo -> fptr_src_image);
		encode_byte_to_lsb(secret_data[i], arr);
		fwrite(arr, 8, 1, encInfo -> fptr_stego_image);
	}
	return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_des)
{
	char ch;
	while(fread(&ch, 1, 1, fptr_src) > 0)
		fwrite(&ch, 1, 1, fptr_des);
	return e_success;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
	rewind(fptr_src_image);
	char arr[54];
	fread(arr, 54, 1,fptr_src_image);
	fwrite(arr, 54, 1, fptr_stego_image);
	return e_success;
}	

Status check_capacity(EncodeInfo *encInfo)
{
	encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
	encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);
	if(encInfo -> image_capacity > (54 + (strlen(MAGIC_STRING)*8) + 32 + 32 + 32 + ((encInfo -> size_secret_file) * 8)))
		{
			return e_success;
		}
		else
			return e_failure;
				
}

uint get_file_size(FILE *fptr)
{
	fseek(fptr, 0, SEEK_END);
	return ftell(fptr);
}

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf(BLUE"width = %u\n"RESET, width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf(BLUE"height = %u\n"RESET, height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, YELLOW"ERROR: Unable to open file %s\n"RESET, encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, YELLOW"ERROR: Unable to open file %s\n"RESET, encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr,YELLOW"ERROR: Unable to open file %s\n"RESET, encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

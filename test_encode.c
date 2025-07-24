#include <stdio.h>
#include<string.h>
#include "encode.h"
#include"decode.h"
#include "types.h"
#include "common.h"
#include <unistd.h>

int main(int argc,char *argv[])
{
	int ret=check_operation_type(argv);
	if(ret == e_encode)
	{
		printf(GREEN"Selected encoding\n"RESET);
		sleep(1);
		EncodeInfo encInfo;
		if(read_and_validate_encode_args(argv, &encInfo) == e_success)
		{
			printf(GREEN"read and validation is successfully done\n"RESET);
			sleep(1);
			do_encoding(&encInfo);
		} 
		else
			printf(YELLOW"read and validation is failed\n"RESET);
	}

	else if(ret == e_decode)
	{
		printf(GREEN"Selected decoding\n"RESET);
		sleep(1);
		DecodeInfo decInfo;
		if(read_and_validate_decode_args(argv, &decInfo) == e_success)
		{
			printf(GREEN"read_and_validation is successfully done\n"RESET);
			sleep(1);
			do_decoding(&decInfo);
		}
		else
			printf(YELLOW"read_and_validation is failed\n"RESET);
	}

	else
		printf(YELLOW"Error\n"RESET);
}

OperationType check_operation_type(char *argv[])
{
	if(strcmp(argv[1], "-e") == 0)
		return e_encode;
	else if(strcmp(argv[1], "-d") == 0)
		return e_decode;
	else
		return e_unsupported;
}
	


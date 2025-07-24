#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    char *src_image_fname;
    FILE *fptr_src_image;
    char *output_fname;
    FILE *fptr_output;
    int extn_size;
    char extn_secret_file[4];
    long size_secret_file;

} DecodeInfo;

Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo);

Status open_file(DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status decode_magic_string(DecodeInfo *decInfo);

Status decode_byte_from_lsb(char *arr, char *decode_byte);

Status decode_secret_file_extn_size(DecodeInfo *decInfo);

Status decode_size_from_lsb(char *arr, int *size);

Status decode_secret_file_extn(DecodeInfo *decInfo);

Status decode_secret_file_size(DecodeInfo *decInfo);

Status decode_secret_file_data(DecodeInfo *decInfo);

#endif

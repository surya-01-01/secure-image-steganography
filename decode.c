#include<stdio.h>
#include<string.h>
#include "decode.h"
#include "types.h"


Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    char *ch;
    if ((ch = strstr(argv[2], ".bmp")))
    {
        if (strcmp(ch, ".bmp") == 0)
        {
            decInfo->stego_image_fname = argv[2];
        }
        else
            return e_failure;
    }
    else
    {
        return e_failure;
    }
    if(argv[3]==NULL)
    {
        decInfo->output_fname="output";
    }
    else
    {
        decInfo->output_fname=strtok(argv[3],".");
        //printf("%s",decInfo->output_fname);
    } 
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    printf("## Decoding Procedure Started ##\n");
    printf("INFO: Opening required files\n");
    if(decode_open_files(decInfo)==e_failure)
    {
        return e_failure;
    }
    else
    {
        printf("INFO: Opened %s\n",decInfo->output_fname);
        if(skip_bmp_header(decInfo->fptr_stego_image)==e_failure)
        {
            return e_failure;
        }
        else
        {
            if(decode_magic_string_size(decInfo)==e_failure)
            {
                return e_failure;
            }
            else
            {
                printf("INFO: Decoding Magic String Signature\n");
                if(decode_magic_string(decInfo)==e_failure)
                {
                    return e_failure;
                }
                else
                {
                    printf("INFO :Done\n");
                    if(decode_secret_file_extn_size(decInfo)==e_failure)
                    {
                        return e_failure;
                    }
                    else
                    {
                        if(decode_secret_file_extn(decInfo)==e_failure)
                        {
                            return e_failure;
                        }
                        else
                        {
                            printf("INFO: Decoding Output File extension\n");
                            if(decode_secret_file_size(decInfo)==e_failure)
                            {
                                return e_failure;
                            }
                            else
                            {
                                printf("INFO :Done\n");
                                 printf("INFO: Decoding decoded.File data\n");
                                if(decode_secret_file_data(decInfo)==e_failure)
                                {
                                    return e_failure;
                                }
                                else
                                {
                                    printf("INFO :Done\n");
                                    return e_success;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

Status decode_open_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

    	return e_failure;
    }
    return e_success;
}

Status skip_bmp_header(FILE *fptr_stego_image)
{
    fseek(fptr_stego_image,54l,SEEK_SET);
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    char mag[20];
    char mag1[20];
    int i;
    for( i=0;i<decInfo->magic_string_size;i++)
    {
        fread(decInfo->image_data,8,1,decInfo->fptr_stego_image);
        mag[i]=decode_byte_to_lsb(decInfo->image_data);
    }
    mag[i]='\0';
    printf("Enter the magic string to move further\n");
    scanf(" %s",mag1);
    printf("\n");
    if(strcmp(mag,mag1)==0)
    {
        printf("magic key matching\n");
        return e_success;
    }
    else
    {
        printf("Incorrect magic string\n");
        return e_failure;
    }
}

char decode_byte_to_lsb(char *image_buffer)
{
    char ch =0;
    for(int i=0;i<8;i++)
    {
        ch=ch | (( image_buffer[i] & 1)<<i);
    }
    return ch;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char arr[32];
    fread(arr,32,1,decInfo->fptr_stego_image);
    decInfo->extn_size=decode_size_to_lsb(arr);
    return e_success;
}


int decode_size_to_lsb(char*image_buffer)
{
    int size=0;
    for(int i=0;i<32;i++)
    {
        size=size|((image_buffer[i] & 1)<<i);
    }
    return size;
}

Status decode_secret_file_extn( DecodeInfo *decInfo)
{
    char extension[20],name[20];
    int i=0;
    strcpy(name,decInfo->output_fname);
    for(i=0;i<decInfo->extn_size;i++)
    {
        fread(decInfo->image_data,8,1,decInfo->fptr_stego_image);
        extension[i]=decode_byte_to_lsb(decInfo->image_data);
    }
    extension[i]='\0';
    strcat(name,extension);
    decInfo->fptr_output=fopen(name,"w");
    if (decInfo->fptr_output == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_fname);

    	return e_failure;
    }
    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char arr[32];
    fread(arr,32,1,decInfo->fptr_stego_image);
    decInfo->size_secret_file=decode_size_to_lsb(arr);
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char ch;
    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
        fread(decInfo->image_data,8,1,decInfo->fptr_stego_image);
        ch=decode_byte_to_lsb(decInfo->image_data);
        fwrite(&ch,1,1,decInfo->fptr_output);
    }
    return e_success;
}

Status decode_magic_string_size(DecodeInfo *decInfo)
{
    char arr[32];
    fread(arr,32,1,decInfo->fptr_stego_image);
    decInfo->magic_string_size=decode_size_to_lsb(arr);
    return e_success;
}


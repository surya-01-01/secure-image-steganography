#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"


/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

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
    printf("INFO: Opening required files\n");
    // Src Image file
    printf("INFO: Opening %s\n",encInfo->src_image_fname);
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    printf("INFO: Opening %s\n",encInfo->secret_fname);
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    printf("INFO: Opening %s\n",encInfo->stego_image_fname);
    
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    printf("Done\n");
    // No failure return e_success
    return e_success;
}
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e")==0)
        return e_encode;
    else if(strcmp(argv[1],"-d")==0)
        return e_decode;
    else
        return e_unsupported;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char *ch;
    if ((ch = strstr(argv[2], ".bmp")))
    {
        if (strcmp(ch, ".bmp") == 0)
        {
            encInfo->src_image_fname = argv[2];
        }
        else
            return e_failure;
    }
    else
    {
        return e_failure;
    }
    // if ((ch = strstr(argv[3], ".txt")))
    // {
    //     if (strcmp(ch, ".txt") == 0)
    //     {
    //         encInfo->secret_fname = argv[3];
    //     }
    //     else
    //         return e_failure;
    // }
    if(ch=strstr(argv[3],"."))
    {
        encInfo->secret_fname=argv[3];
    }
    else
    {
        return e_failure;
    }
    if (argv[4] == NULL)
    {
        printf("INFO: Output File not mentioned. Creating stego.bmp as default\n");
        encInfo->stego_image_fname = "stego.bmp";
    }
    else
    {
        if ((ch = strstr(argv[4], ".bmp")))
        {
            if (strcmp(ch, ".bmp") == 0)
            {
                printf("INFO: Output File mentioned. Creating %s\n",argv[4]);
                encInfo->stego_image_fname = argv[4];
            }
            else
                return e_failure;
        }
    }
    return e_success;
}
Status do_encoding(EncodeInfo *encInfo)
{
   if(open_files(encInfo)==e_failure)
   {
        return e_failure;
   }
   else
   {
        printf("## Encoding Procedure Started ##\n");
        if(check_capacity(encInfo)==e_failure)
        {
            return e_failure;
        }
        else
        {
            if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
            {
                return e_failure;
            }
            else
            {
                if(encode_magic_string_size(encInfo)==e_failure)
                {
                    return e_failure;
                }
                else
                {
                    if(encode_magic_string(encInfo)==e_failure)
                    {
                        return e_failure;
                    }
                    else
                    {
                        if(encode_secret_file_extn_size(strlen(strstr(encInfo->secret_fname,".")),encInfo)==e_failure)
                        {
                            return e_failure;
                        }
                        else
                        {
                            if(encode_secret_file_extn(strstr(encInfo->secret_fname,"."),encInfo)==e_failure)
                            {
                                return e_failure;
                            }
                            else
                            {
                                if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_failure)
                                {   
                                    return e_failure;
                                }
                                else
                                {
                                    if(encode_secret_file_data(encInfo)==e_failure)
                                    {
                                        return e_failure;
                                    }
                                    else
                                    {
                                        if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure)
                                        {
                                            return e_failure;
                                        }
                                        else
                                        {
                                            
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
   }
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
    printf("Checking for %s size\n",encInfo->secret_fname);
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
    if(encInfo->size_secret_file)
    {
        printf("Done. Not Empty\n");
    }
    uint sec_extention_size=strlen(strstr(encInfo->secret_fname,"."));
    //printf("%u ",sec_extention_size);
    printf("Checking for %s capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
    if(encInfo->image_capacity<((3*sizeof(int))+sec_extention_size+encInfo->size_secret_file))
    {
        return e_failure;
    }
    else
    {
        printf("Done. Found OK\n");
        return e_success;
    }
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr,0l,SEEK_END);
    uint size=ftell(fptr);
    return size;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("Copying Image Header\n");
    rewind(fptr_src_image);
    rewind(fptr_dest_image);
    char buf[54];
    fread(buf,54,1,fptr_src_image);
    fwrite(buf,54,1,fptr_dest_image);
    printf("Done\n");
    return e_success;
}

Status encode_magic_string( EncodeInfo *encInfo)
{
    char magic_string[encInfo->size_magic_string];
    printf("Enter the magic string:");
    scanf(" %s",magic_string);
    printf("Encoding Magic String Signature\n");
    for(int i=0;i<encInfo->size_magic_string;i++)
    {
        fread(encInfo->image_data,8,1,encInfo->fptr_src_image);
        if(encode_byte_to_lsb(magic_string[i],encInfo->image_data)==e_failure)
        {
            return e_failure;
        }
        else
        {
            fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);
        }
    }
    printf("Done\n");
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    //clearing the lsb bits of image data
    for(int i=0;i<8;i++)
    {
        image_buffer[i]=image_buffer[i] & (-1<<1);
    }  
    for(int i=0;i<8;i++)
    {
        image_buffer[i]=image_buffer[i]|(((unsigned)data>>i)&1);
    } 
    return e_success;
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    printf("Encoding %s File Extension Size\n",encInfo->secret_fname);
    char ch[32];
    fread(ch,32,1,encInfo->fptr_src_image);
    for(int i=0;i<32;i++)
    {
        ch[i]=ch[i] & (-1<<1);
    }
    for(int i=0;i<32;i++)
    {
        ch[i]=ch[i]|(((unsigned)size>>i)&1);
    }
    fwrite(ch,32,1,encInfo->fptr_stego_image);
    printf("Done\n");
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("Encoding %s File Extension\n",encInfo->secret_fname);
    int i=0;
    while(file_extn[i])
    {
        fread(encInfo->image_data,8,1,encInfo->fptr_src_image);
        if(encode_byte_to_lsb(file_extn[i],encInfo->image_data)==e_failure)
        {
            return e_failure;
        }
        else
        {
            fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);
            i++;
        }
    }
    printf("Done\n");
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    //printf("%lu ",sizeof(file_size));
    printf("Encoding %s File Size\n",encInfo->secret_fname);
    char ch[32];
    fread(ch,32,1,encInfo->fptr_src_image);
    for(int i=0;i<32;i++)
    {
        ch[i]=ch[i] & (-1<<1);
    }
    for(int i=0;i<32;i++)
    {
        ch[i]=ch[i]|(((unsigned)file_size>>i)&1);
    }
    fwrite(ch,32,1,encInfo->fptr_stego_image);
    printf("Done\n");
    return e_success;
}


Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("Encoding %s File Data\n",encInfo->secret_fname);
    char ch;
    fseek(encInfo->fptr_secret,0l,SEEK_END);
    long int l=ftell(encInfo->fptr_secret);
    rewind(encInfo->fptr_secret);
    for(long int i=0;i<l;i++)
    {
        fread(&ch,1,1,encInfo->fptr_secret);
        fread(encInfo->image_data,8,1,encInfo->fptr_src_image);
        if(encode_byte_to_lsb(ch,encInfo->image_data)==e_failure)
            return e_failure;
        else
        {
            fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);
        }
    }
    printf("Done\n");
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("Copying Left Over Data\n");
    char ch;
    long int st=ftell(fptr_src);
    fseek(fptr_src,0l,SEEK_END);
    long int end=ftell(fptr_src);
    fseek(fptr_src,st,SEEK_SET);
    for(long int i=st;i<end;i++)
    {
        fread(&ch,1,1,fptr_src);
        fwrite(&ch,1,1,fptr_dest);
    }
    printf("Done\n");

    // char ch;
    // fseek(fptr_src,0l,SEEK_END);
    // long int end=ftell(fptr_src);
    // rewind(fptr_src);
    // rewind(fptr_dest);
    // for(long int i=0;i<end;i++)
    // {
    //     fread(&ch,1,1,fptr_src);
    //     fwrite(&ch,1,1,fptr_dest);
    // }

    return e_success;
}


Status encode_magic_string_size( EncodeInfo *encInfo)
{
    printf("Enter the magic string size:");
    scanf(" %d",&encInfo->size_magic_string);
    printf("Encoding Magic String Signature size\n");
    char ch[32];
    fread(ch,32,1,encInfo->fptr_src_image);
    for(int i=0;i<32;i++)
    {
        ch[i]=ch[i] & (-1<<1);
    }
    for(int i=0;i<32;i++)
    {
        ch[i]=ch[i]|(((unsigned)encInfo->size_magic_string>>i)&1);
    }
    fwrite(ch,32,1,encInfo->fptr_stego_image);
    printf("Done\n");
    return e_success;
    
}
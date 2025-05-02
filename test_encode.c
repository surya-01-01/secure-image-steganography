/*
NAME:SURYA G
DATE:31-01-2025
PROJECT NAME: Steganography
DESCRIPTION:Hiding one form of data in a another form of data
*/


#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    uint img_size;
    if (argc < 3 || argc > 5)
    {
        printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
        printf("%s: Decoding: %s -d <.bmp file> [output file]\n",argv[0],argv[0]);
        return 1;
    }
    else
    {

        if (check_operation_type(argv) == e_unsupported)
        {
            printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
            printf("%s: Decoding: %s -d <.bmp file> [output file]\n",argv[0],argv[0]);
            return 1;
        }
        else if (check_operation_type(argv) == e_encode )
        {
            if(argc>=4 && argc<=5)
            {
                if (read_and_validate_encode_args(argv, &encInfo) == e_failure)
                {
                    printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
                    return 1;
                }
                else
                {
                    if (do_encoding(&encInfo) == e_failure)
                    {
                        printf("error(encode)");
                    }
                    else
                    {
                        printf("## Encoding Done Successfully ##\n");
                    }
                 }
            }
            else
            {
                printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
                printf("%s: Decoding: %s -d <.bmp file> [output file]\n",argv[0],argv[0]);
            }
        }
        else if(check_operation_type(argv)==e_decode)
        {
            if(argc>=3 && argc<=4)
            {
                if(read_and_validate_decode_args(argv,&decInfo)==e_failure)
                {
                    printf("%s: Decoding: %s -d <.bmp file> [output file]\n",argv[0],argv[0]);
                    return 1;
                }
                else
                {
                    if (do_decoding(&decInfo)==e_failure)
                    {
                        printf("error(decode)");
                    }
                    else
                    {
                        printf("## Decoding Done Successfully ##\n");
                    }
                
                }
            }
            else
            {
                printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n",argv[0],argv[0]);
                printf("%s: Decoding: %s -d <.bmp file> [output file]\n",argv[0],argv[0]);
            }
        }
    }
    // call the check operation type function

    // // Fill with sample filenames
    // encInfo.src_image_fname = "beautiful.bmp";
    // encInfo.secret_fname = "secret.txt";
    // encInfo.stego_image_fname = "stego_img.bmp";

    // // Test open_files
    // if (open_files(&encInfo) == e_failure)
    // {
    // 	printf("ERROR: %s function failed\n", "open_files" );
    // 	return 1;
    // }
    // else
    // {
    // 	printf("SUCCESS: %s function completed\n", "open_files" );
    // }

    // // Test get_image_size_for_bmp
    // img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
    // printf("INFO: Image size = %u\n", img_size);

    return 0;
}

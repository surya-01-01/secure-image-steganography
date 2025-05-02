#ifndef DECODE_H
#define DECODE_H

#include "types.h" 
#include<stdio.h>
// Contains user defined types

/* 
 * Structure to store information required for
 * decoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo  //declare in main
{
    /* Stego Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char image_data[8];

    /* output  File Info */
    char *output_fname;
    FILE *fptr_output;
    char extn_secret_file[5];       //size can be anything here 4
    int size_secret_file;
    int extn_size;
    int magic_string_size;

} DecodeInfo;


/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status decode_open_files(DecodeInfo *decInfo);
    //1ly open stego.bmp file 
/* Copy bmp image header */
Status skip_bmp_header(FILE *fptr_stego_image);

    //fseek


/* decode Magic String */
Status decode_magic_string_size(DecodeInfo *decInfo);

Status decode_magic_string(DecodeInfo *decInfo);

    /*decode # and * 
    read 8 b of data 
    decode byte to lsb==return type is char
    pass image buffer
    2 times for magic string
    and store it in char array
    store null at the end of array
    identification mark is *#
    ask user to enter the magic string  compare */



/* Encode secret file extensio size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

int decode_size_to_lsb(char*image_buffer);
/*encode a size to lsb of image 
right for 32 bytes
get the size and store it in structure member
extn_size

*/

/* Encode secret file extenstion */
Status decode_secret_file_extn( DecodeInfo *decInfo);

    /*extension of our data
    read 8 b from stego.bmp
    call decode_byte_to lsb
    store into char extn_secret_file[4]
    run a loop SIZE times
    
    
    concatanate the output file name 
    ->and open the file here*/



/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

    /*int 32 bytes
    char 8 bytes
    
    extension of our data
    read 8 b from stego.bmp
    call decode_byte_to lsb
    store into char extn_secret_file[4]
    run a loop SIZE times
    */


/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);




/* Decode a byte into LSB of image data array */
char decode_byte_to_lsb(char *image_buffer);

int decode_size_to_lsb(char* image_buffer);

  /*>loop for 8 times
    >get lsb of image buffer
    >shit left or msb side
    */




#endif
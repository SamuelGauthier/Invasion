#ifndef FILE_STRUCT_H
#define FILE_STRUCT_H
#include "utils.h"

// BMP File Format
#define BMP_HF_SIGNATURE 0
#define BMP_HF_DATA_OFFSET 0xA
#define BMP_HI_WIDTH 0x12
#define BMP_HI_HEIGHT 0x16
#define BMP_HI_BPP 0x1C
#define BMP_HI_COMPRESSION 0x1E

// TGA File Format
#define TGA_HD_ID_LENGTH 0
#define TGA_HD_COLORMAP_TYPE 1
#define TGA_HD_IMAGE_TYPE 2
#define TGA_HD_COLORMAP_SPECS 3
#define TGA_HD_IMAGE_SPECS 8
#define TGA_IMG_ID_FIELD 0x12

#endif

#pragma once
#include "u8g2.h"
#include <stdio.h>

#define BLOCKCHAR "@"
#define WHITECHAR " "
#define SHOW_BROAD 1

void print_console(u8g2_t* u8g2);

void print_console(u8g2_t* u8g2)
{
	uint8_t* tile_buf = u8g2->tile_buf_ptr;

#ifdef __U8G2_VR
	for(int h=0;h<8*4;h++)
	{
		for(int w=0;w<16;w++)
		{
			for(int b=0;b<8;b++)
			{
				uint8_t p = (*(tile_buf+w));
				printf("%s", p & (0x1<<b)?"+":".");
			}
			printf("-");
		}
		printf("\n");
		tile_buf += 16;
	//			break;
	}
#else

	printf("\n\n\n\n");

#if defined(SHOW_BROAD) && SHOW_BROAD
	for(int c=0;c< 2+ 8 * u8g2->u8x8.display_info->tile_width;c++)
	{
		printf("=");
	}
	printf("\n");
#endif

	for(int zd=0;zd<u8g2->tile_buf_height;zd++)
	{
		for(int h=0;h<8;h++)
		{
#if defined(SHOW_BROAD) && SHOW_BROAD
			printf("=");
#endif
			for(int f=0;f<(8*(u8g2->u8x8.display_info->tile_width));f++)
			{
				uint8_t p = (*(tile_buf+f));
				printf("%s", p & (0x1<<h)?BLOCKCHAR:WHITECHAR);
			}
#if defined(SHOW_BROAD) && SHOW_BROAD
			printf("=\n");
#else
			printf("\n");
#endif
		}

		tile_buf += 8*(u8g2->u8x8.display_info->tile_width);
	}

#if defined(SHOW_BROAD) && SHOW_BROAD
	for(int c=0;c< 2+ 8 * u8g2->u8x8.display_info->tile_width;c++)
	{
		printf("=");
	}
	printf("\n");
#endif

#endif
}



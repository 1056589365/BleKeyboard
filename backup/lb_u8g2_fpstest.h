/*
 * lb_u8g2_fpstest.h
 *
 *  Created on: 2018Äê7ÔÂ7ÈÕ
 *      Author: Administrator
 */
#pragma once

//void draw_set_screen()
//{
//	static uint8_t flip_color;
//	flip_color = !flip_color;
//	u8g2_SetDrawColor(&u8g2, flip_color);
//	u8g2_DrawBox(&u8g2, 0, 0, u8g2_GetDisplayWidth(&u8g2), u8g2_GetDisplayHeight(&u8g2));
//}
//
//const char *convert_FPS(uint16_t fps)
//{
//	static char buf[6];
//	strcpy(buf, u8g2_u8toa( (uint8_t)(fps/5), 3));
//	buf[3] =  '.';
//	buf[4] = (fps % 10) + '0';
//	buf[5] = '\0';
//	return buf;
//}

//
//uint32_t time = 0;
//		uint16_t FPSC = 0;
//
//		time = getCurrentTimeMillis() + 5*1000;
//
//		do{
//			u8g2_ClearBuffer(&u8g2);
//			draw_set_screen();
//			u8g2_SendBuffer(&u8g2);
//			print_console(&u8g2);
//
//			FPSC++;
//		} while(getCurrentTimeMillis() < time);
//
//		printf("%s\n", convert_FPS(FPSC));

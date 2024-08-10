#ifndef OLED_OLED_H_
#define OLED_OLED_H_
#include "main.h"
#ifdef DEBUG
#include "oledfont.h"

void OLED_WR_CMD(unsigned char cmd);
void OLED_WR_DATA(unsigned char pData);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_On(void);
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size2,unsigned char Color_Turn);
void OLED_Showdecimal(unsigned char x,unsigned char y,float num,unsigned char z_len,unsigned char f_len,unsigned char size2, unsigned char Color_Turn);
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr,unsigned char Char_Size,unsigned char Color_Turn);
void OLED_ShowString(unsigned char x,unsigned char y,char*chr,unsigned char Char_Size,unsigned char Color_Turn);
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no,unsigned char Color_Turn);
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char *  BMP,unsigned char Color_Turn);
void OLED_HorizontalShift(unsigned char direction);
void OLED_Some_HorizontalShift(unsigned char direction,unsigned char start,unsigned char end);
void OLED_VerticalAndHorizontalShift(unsigned char direction);
void OLED_DisplayMode(unsigned char mode);
void OLED_IntensityControl(unsigned char intensity);



#endif /* OLED_OLED_H_ */

#endif
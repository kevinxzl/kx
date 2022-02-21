#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"
#include "delay.h"

/**
 *128 x 64 Dot Matrix
 * OLED RAM (Buffer)
 *[0]0 1 2 3 ... 127
 *[1]0 1 2 3 ... 127
 *[2]0 1 2 3 ... 127
 *[3]0 1 2 3 ... 127
 *[4]0 1 2 3 ... 127
 *[5]0 1 2 3 ... 127
 *[6]0 1 2 3 ... 127
 *[7]0 1 2 3 ... 127
 */

uint8_t OLED_GRAM[128][8];

/**
 * @brief initialization OLED
 *
 */
void OLED_Init(void)
{
	RCC->AHB1ENR |= 0X8F; // Enable PORTA~F,PORTH clock
	if (OLED_MODE == OLED_PARALLEL)
	{																													 // using 8080 interface
		GPIO_Set(GPIOA, PIN15, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);							 // config PA15
		GPIO_Set(GPIOB, PIN3 | PIN4 | PIN7 | PIN8 | PIN9, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);	 // config PB3,4,7,8,9
		GPIO_Set(GPIOC, PIN6 | PIN7 | PIN8 | PIN9 | PIN11, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU); // coffig PC6~9,PC11
		GPIO_Set(GPIOD, PIN3, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);								 // config PD3
		GPIO_Set(GPIOH, PIN8, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);								 // config PH8
		OLED_WR = 1;
		OLED_RD = 1;

		OLED_CS = 1;
		OLED_RS = 1;

		OLED_RST = 0;
		delay_ms(100);
		OLED_RST = 1;

		OLED_WR_Byte(0xAE, OLED_CMD); // Send command AEh for display OFF
		OLED_WR_Byte(0xD5, OLED_CMD); // Set Display Clock Divide Ratio/ Oscillator Frequency (D5h)
		OLED_WR_Byte(80, OLED_CMD);	  //[3:0], the division factor;[7:4], frequency
		OLED_WR_Byte(0xA8, OLED_CMD); // Set Multiplex Ratio
		OLED_WR_Byte(0X3F, OLED_CMD); // default 0X3F(1/64)
		OLED_WR_Byte(0xD3, OLED_CMD); // Set Display Offset
		OLED_WR_Byte(0X00, OLED_CMD); // default 0

		OLED_WR_Byte(0x40, OLED_CMD); // Set Display Start Line (X5X4X3X2X1X0 of 40h~7Fh) < B[6:0]

		OLED_WR_Byte(0x8D, OLED_CMD); // The Charge Pump must be enabled by the following command: 8Dh
		OLED_WR_Byte(0x14, OLED_CMD); // Charge Pump Setting 14h ; Enable Charge Pump AFh; Display ON
		OLED_WR_Byte(0x20, OLED_CMD); // Set Memory Addressing Mode
		OLED_WR_Byte(0x02, OLED_CMD);
		OLED_WR_Byte(0xA1, OLED_CMD); // Set Segment Re-map,bit0:0,0->0;1,0->127;
		OLED_WR_Byte(0xC0, OLED_CMD); // Set COM Output Scan Direction
		OLED_WR_Byte(0xDA, OLED_CMD); // Set COM Pins Hardware Configuration
		OLED_WR_Byte(0x12, OLED_CMD);

		OLED_WR_Byte(0x81, OLED_CMD); // Set Contrast Control for BANK0 (81h)
		OLED_WR_Byte(0xEF, OLED_CMD); // 1~255(00h-FFh); default 0X7F
		OLED_WR_Byte(0xD9, OLED_CMD); // Set Pre-charge Period
		OLED_WR_Byte(0xf1, OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
		OLED_WR_Byte(0xDB, OLED_CMD); // Set Vcomh Deselect Level (DBh)
		OLED_WR_Byte(0x30, OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

		OLED_WR_Byte(0xA4, OLED_CMD); // Entire Dispaly ON;bit0:1,ON;0,OFF;(白屏/黑屏)
		OLED_WR_Byte(0xA6, OLED_CMD); // Set Normal/Inverse Dispaly
		OLED_WR_Byte(0xAF, OLED_CMD); // Set Dispaly ON/OFF
		OLED_Clear();
	}
}

//更新显存到LCD

/**
 * @brief write 8 bits to OLED
 *
 * @param
 * data: output data
 *
 */
void OLED_Data_Out(uint8_t data)
{
	uint16_t dat = data & 0X0F;
	GPIOC->ODR &= ~(0XF << 6);		// clear bit6 to  bit9
	GPIOC->ODR |= dat << 6;			// D[3:0]-->PC[9:6]
	PCout(11) = (data >> 4) & 0X01; // D4
	PDout(3) = (data >> 5) & 0X01;	// D5
	PBout(8) = (data >> 6) & 0X01;	// D6
	PBout(9) = (data >> 7) & 0X01;	// D7
}

/**
 * @brief write a byte to OLED
 *
 * @param
 * data: needs to be written data
 * cmd:  data or command
 *
 */
void OLED_WR_Byte(uint8_t data, uint8_t cmd)
{
	OLED_Data_Out(data);

	// commnad
	if (cmd == OLED_CMD)
	{
		OLED_RS = 1;
	}
	else // data
	{
		OLED_RS = 0;
	}

	OLED_CS = 0;
	OLED_WR = 0;
	OLED_WR = 1;
	OLED_CS = 1;
	OLED_RS = 1;
}

/**
 * @brief turn on OLED
 * The Charge Pump must be enabled by the following command:
 * 8Dh : Charge Pump Setting
 * 14h : Enable Charge Pump
 * AFh : Display ON
 */
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);
	OLED_WR_Byte(0X14, OLED_CMD);
	OLED_WR_Byte(0XAF, OLED_CMD);
}

/**
 * @brief turn off OLED
 * The Charge Pump must be enabled by the following command:
 * 8Dh : Charge Pump Setting
 * 10h : Diabled Charge Pump
 * AEh : Display OFF
 *
 */
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD); // Set DCDC Command
	OLED_WR_Byte(0X10, OLED_CMD); // DCDC OFF
	OLED_WR_Byte(0XAE, OLED_CMD); // Display OFF
}

/**
 * @brief clear screen. After clearing the screen, the whole screen is black.
 *
 */
void OLED_Clear(void)
{
	uint8_t i, n;
	for (i = 0; i < 8; i++)
	{
		for (n = 0; n < 128; n++)
		{
			OLED_GRAM[n][i] = 0X00;
		}
	}
	OLED_Refresh_Gram();
}

/**
 * @brief draw a point
 *
 * @param
 * x: X coordinate
 * y: Y coordinate
 * t: 1: filled dot  0: empyt dot
 *
 */
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t)
{
	uint8_t pos, bx, temp = 0;
	if (x < 0 || x > 127 || y < 0 || y > 63)
	{
		return; // out of range
	}
	pos = 7 - y / 8;
	bx = y % 8;
	temp = 1 << (7 - bx);
	if (t)
	{
		OLED_GRAM[x][pos] |= temp;
	}
	else
	{
		OLED_GRAM[x][pos] &= ~temp;
	}
}

/**
 * @brief fill a rectangle
 *
 * @param
 * x1: start X coordinate
 * y1: start Y coordinate
 * x2: end x coordinate
 * y2: end y coordinate
 * t: 1: filled dot  0: empyt dot
 *
 */
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot)
{
	if (x1 < x2 || y1 < y2)
	{
		return;
	}

	uint8_t x, y;
	for (x = x1; x <= x2; x++)
	{
		for (y = y1; y <= y2; y++)
		{
			OLED_DrawPoint(x, y, dot);
		}
	}

	OLED_Refresh_Gram();
}

/**
 * @brief show a char at(x, y)
 *
 * @param
 * x:  X coordinate (0~127)
 * y:  Y coordinate (0~63)
 * size: font size 12/16/24  12*12 ASCII
 * mode: Normal (1) / Inverse Display (0)
 *
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{
	if (x < 0 || x > 127 || y < 0 || y > 63)
	{
		return;
	}
	uint8_t temp, t, t1;
	uint8_t y0 = y;
	// Get the number of bytes occupied by the dot matrix set corresponding to a character of the font
	uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
	chr = chr - ' '; // offset

	for (t = 0; t < csize; t++)
	{
		if (size == 12)
		{
			temp = asc2_1206[chr][t]; // 1206 ASCII font
		}
		else if (size == 16)
		{
			temp = asc2_1608[chr][t]; // 1608 ASCII font
		}
		else if (size == 24)
		{
			temp = asc2_2412[chr][t]; // 2412 ASCII font
		}
		else
		{
			return; // invalid font
		}

		for (t1 = 0; t1 < 8; t1++)
		{
			if (temp & 0x80)
			{
				OLED_DrawPoint(x, y, mode);
			}
			else
			{
				OLED_DrawPoint(x, y, !mode);
			}
			temp <<= 1;
			y++;
			if ((y - y0) == size)
			{
				y = y0;
				x++;
				break;
			}
		}
	}
}

// m^n power
uint32_t _pow(uint8_t m, uint8_t n)
{
	u32 result = 1;
	while (n--)
	{
		result *= m;
	}
	return result;
}

/**
 * @brief show numbers at(x, y)
 *
 * @param
 * x:  X coordinate (0~127)
 * y:  Y coordinate (0~63)
 * num: number (0~4294967295)
 * len: the length of number
 * size: font size 12/16/24  12*12 ASCII
 *
 */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size)
{
	if (x < 0 || x > 127 || y < 0 || y > 63)
	{
		return;
	}
	uint8_t t, temp;
	uint8_t enshow = 0;
	for (t = 0; t < len; t++)
	{
		temp = (num / pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				OLED_ShowChar(x + (size / 2) * t, y, ' ', size, 1);
				continue;
			}
			else
				enshow = 1;
		}
		OLED_ShowChar(x + (size / 2) * t, y, temp + '0', size, 1);
	}
}

/**
 * @brief show a string at(x, y)
 *
 * @param
 * x:  X coordinate (0~127)
 * y:  Y coordinate (0~63)
 * *p: the string start address
 * size: font size 12/16/24  12*12 ASCII
 *
 */
void OLED_ShowString(uint8_t x, uint8_t y, const uint8_t *p, uint8_t size)
{
	if (x < 0 || x > 127 || y < 0 || y > 63)
	{
		return;
	}
	while ((*p <= '~') && (*p >= ' ')) //判断是不是非法字符!
	{
		if (x > (128 - (size / 2)))
		{
			x = 0;
			y += size;
		}
		if (y > (64 - size))
		{
			y = x = 0;
			OLED_Clear();
		}
		OLED_ShowChar(x, y, *p, size, 1);
		x += size / 2;
		p++;
	}
}

/**
 * @brief update RAM to OLED memory
 *
 */
void OLED_Refresh_Gram(void)
{
	uint8_t i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD); // page address（0~7）
		OLED_WR_Byte(0x00, OLED_CMD);	  // column address low
		OLED_WR_Byte(0x10, OLED_CMD);	  // column address high
		for (n = 0; n < 128; n++)
		{
			OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
		}
	}
}

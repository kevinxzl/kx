/*
 * oled.h
 *
 */

#ifndef __OLED_H_
#define __OLED_H_
#include "sys.h"
#include "stdlib.h"

/**
 *  MCU Bus Interface Pin Selection
 *  ________________________________________
 *  |Interface |  8080 | SPI | IIC  | 6800 |
 *  |__________|_______|_____|______|______|
 *  |   BS1    |   1   |  0  | 1    |  0   |
 *  |__________|_______|_____|______|______|
 *  |   BS2    |   1   |  0  | 0    |  1   |
 *  |__________|_______|_____|______|______|
 */
typedef enum _OLED_INTERFACE
{
    OLED_PARALLEL, // 8080
    OLED_SPI,      // SPI
    OLED_IIC       // IIC
} OLED_INTERFACE;

typedef enum _OLED_DC
{
    OLED_CMD, // WR command
    OLED_DATA // WR data
} OLED_DC;

/**
 * CS: OLED CS
 * WR: write data to OLED
 * RD: Read data from OLED
 * D[7:0]: 8 bits parallel data
 * RST(RES):  reset hardware
 * DC: 0: Command, 1: data  for OLED Controller
 *
 */

// OLED ports
#define OLED_CS PBout(7)
#define OLED_RST PAout(15)
#define OLED_RS PBout(4)
#define OLED_WR PHout(8)
#define OLED_RD PBout(3)

/**
 * @brief initialization OLED
 *
 */
void OLED_Init(void);

/**
 * @brief clear screen. After clearing the screen, the whole screen is black.
 *
 */
void OLED_Clear(void);

/**
 * @brief turn on OLED
 * The Charge Pump must be enabled by the following command:
 * 8Dh : Charge Pump Setting
 * 14h : Enable Charge Pump
 * AFh : Display ON
 */
void OLED_Display_On(void);

/**
 * @brief turn off OLED
 * The Charge Pump must be enabled by the following command:
 * 8Dh : Charge Pump Setting
 * 10h : Diabled Charge Pump
 * AEh : Display OFF
 *
 */
void OLED_Display_Off(void);

/**
 * @brief draw a point
 *
 * @param
 * x: X coordinate
 * y: Y coordinate
 * t: 1: filled dot  0: empyt dot
 *
 */
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t);

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
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot);

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
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode);

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
void OLED_ShowNum(uint8_t x, uint8_t y, u32 num, uint8_t len, uint8_t size);

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
void OLED_ShowString(uint8_t x, uint8_t y, const uint8_t *p, uint8_t size);

// OLED Control Functions

/**
 * @brief write a byte to OLED
 *
 * @param
 * data: needs to be written data
 * cmd:  data or command
 *
 */
void OLED_WR_Byte(uint8_t dat, uint8_t cmd);

/**
 * @brief update RAM to OLED memory
 *
 */
void OLED_Refresh_Gram(void);

#endif

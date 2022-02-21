/*
 * w25qxx.h
 *
 */

#ifndef __W25QXX_H_
#define __W25QXX_H_
#include "sys.h"

//W25XX/Q Flash Memory	   
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16	
//W25Q128 ID  0XEF17	
//W25Q256 ID  0XEF18
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17
#define W25Q256 0XEF18

extern uint16_t W25QXX_TYPE;						   

#define	W25QXX_CS 		PFout(6)  		//W25QXX CS 

////////////////////////////////////////////////////
//COMMANDS
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg1		0x05 
#define W25X_ReadStatusReg2		0x35 
#define W25X_ReadStatusReg3		0x15 
#define W25X_WriteStatusReg1    0x01 
#define W25X_WriteStatusReg2    0x31 
#define W25X_WriteStatusReg3    0x11 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 
#define W25X_Enable4ByteAddr    0xB7
#define W25X_Exit4ByteAddr      0xE9

/**
 * @brief initialization W25Q256
 * size: 32M
 * block: 512
 * sector: 8192
 * sector size: 4k
 *
 */
void W25QXX_Init(void);

/**
 * @brief read W25QXX ID
 *
 * @return W25QXX ID
 *
 */
uint16_t  W25QXX_ReadID(void);  

/**
 * @brief read W25QXX status registers (3 status registers)
 * Register 1£º
 * BIT7  6   5   4   3   2   1   0
 * SPR   RV  TB BP2 BP1 BP0 WEL BUSY
 * SPR: default 0, Status register protection bit, used with WP
 * TB,BP2,BP1,BP0:FLASH area write protection settings
 * WEL: write enable lock
 * BUSY: 1: busy; 0: idle
 * default: 0x00
 *
 * Register 2£º
 * BIT7  6   5   4   3   2   1   0
 * SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
 *
 *
 * Register 3£º
 * BIT7      6    5    4   3   2   1   0
 * HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
 *
 * @param
 * regno: status register number. range:1~3
 *
 * @return: register status
 *
 */
uint8_t W25QXX_ReadSR(uint8_t regno);            

/**
 * @brief write W25QXX status registers
 *
 * @param
 * regno: W25QXX status register number. range 1 ~ 3.
 * sr: status register data
 *
 */
void W25QXX_Write_SR(uint8_t regno,uint8_t sr);  

/**
 * @brief enable write W25QXX
 *
 */
void W25QXX_Write_Enable(void);  		

/**
 * @brief disable write W25QXX
 *
 */
void W25QXX_Write_Disable(void);	

/**
 * @brief write data to W25QXX FLASH by SPI (NO CHECK)
 *
 * @param
 * pBuffer: data in buffer
 * WriteAddr: flash start address(24bits)
 * NumByteToWrite: The number of bytes to write (max 256),
 * which should not exceed the number of bytes remaining on the page
 *
 */
void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);

/**
 * @brief read data from  W25QXX FLASH by SPI
 * @param
 * pBuffer: read to buffer
 * ReadAddr: flash start address
 * NumByteToRead: number of bytes to read(MAX: 65535)
 *
 */
void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);   

/**
 * @brief write data to W25QXX FLASH by SPI with erase
 *
 * @param
 * pBuffer: data in buffer
 * WriteAddr: flash start address(24bits)
 * NumByteToWrite: The number of bytes to write (max 65535),
 *
 */
void W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);

/**
 * @brief erase the whole falsh
 */
void W25QXX_Erase_Chip(void);    

/**
 * @brief erase a sector
 *
 * @param
 * Dst_Addr: sector address
 * Minimum time to erase a sector: 150ms
 *
 */
void W25QXX_Erase_Sector(uint32_t Dst_Addr);

/**
 * @brief wait for busy
 *
 */
void W25QXX_Wait_Busy(void);     

/**
 * @brief write protenction using Power-down instruction
 *
 */
void W25QXX_PowerDown(void);        

/**
 * @brief wake up by send release Power-down instruction
 *
 */
void W25QXX_WAKEUP(void);			

#endif

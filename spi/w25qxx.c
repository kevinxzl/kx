#include "w25qxx.h"
#include "spi.h"
#include "delay.h"
#include "usart.h"

uint16_t W25QXX_TYPE = W25Q256; // default W25Q256

/**
 * @brief initialization W25Q256
 * size: 32M
 * block: 512
 * sector: 8192
 * sector size: 4k
 *
 */
void W25QXX_Init(void)
{
	uint8_t temp;
	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOF_CLK_ENABLE(); // Enable GPIO F Clock
	// PF6
	GPIO_Initure.Pin = GPIO_PIN_6; // PF6
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull = GPIO_PULLUP;
	GPIO_Initure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOF, &GPIO_Initure);

	W25QXX_CS = 1;
	SPI5_Init();
	SPI5_SetSpeed(SPI_BAUDRATEPRESCALER_2);
	W25QXX_TYPE = W25QXX_ReadID();
	if (W25QXX_TYPE == W25Q256)
	{
		temp = W25QXX_ReadSR(3);
		if ((temp & 0X01) == 0)
		{
			W25QXX_CS = 0;
			SPI5_ReadWriteByte(W25X_Enable4ByteAddr);
			W25QXX_CS = 1;
		}
	}
}

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
uint8_t W25QXX_ReadSR(uint8_t regno)
{
	uint8_t byte = 0, command = 0;
	switch (regno)
	{
	case 1:
		command = W25X_ReadStatusReg1;
		break;
	case 2:
		command = W25X_ReadStatusReg2;
		break;
	case 3:
		command = W25X_ReadStatusReg3;
		break;
	default:
		command = W25X_ReadStatusReg1;
		break;
	}
	W25QXX_CS = 0;
	SPI5_ReadWriteByte(command);
	byte = SPI5_ReadWriteByte(0Xff);
	W25QXX_CS = 1;
	return byte;
}

/**
 * @brief write W25QXX status registers
 *
 * @param
 * regno: W25QXX status register number. range 1 ~ 3.
 * sr: status register data
 *
 */
void W25QXX_Write_SR(uint8_t regno, uint8_t sr)
{
	uint8_t command = 0;
	switch (regno)
	{
	case 1:
		command = W25X_WriteStatusReg1;
		break;
	case 2:
		command = W25X_WriteStatusReg2;
		break;
	case 3:
		command = W25X_WriteStatusReg3;
		break;
	default:
		command = W25X_WriteStatusReg1;
		break;
	}
	W25QXX_CS = 0;
	SPI5_ReadWriteByte(command);
	SPI5_ReadWriteByte(sr);
	W25QXX_CS = 1;
}

/**
 * @brief enable write W25QXX
 *
 */
void W25QXX_Write_Enable(void)
{
	W25QXX_CS = 0;
	SPI5_ReadWriteByte(W25X_WriteEnable);
	W25QXX_CS = 1;
}

/**
 * @brief disable write W25QXX
 *
 */
void W25QXX_Write_Disable(void)
{
	W25QXX_CS = 0;
	SPI5_ReadWriteByte(W25X_WriteDisable);
	W25QXX_CS = 1;
}

/**
 * @brief read W25QXX ID
 *
 * @return W25QXX ID
 *
 */
uint16_t W25QXX_ReadID(void)
{
	uint16_t Temp = 0;
	W25QXX_CS = 0;
	SPI5_ReadWriteByte(0x90);
	SPI5_ReadWriteByte(0x00);
	SPI5_ReadWriteByte(0x00);
	SPI5_ReadWriteByte(0x00);
	Temp |= SPI5_ReadWriteByte(0xFF) << 8;
	Temp |= SPI5_ReadWriteByte(0xFF);
	W25QXX_CS = 1;
	return Temp;
}

/**
 * @brief read data from  W25QXX FLASH by SPI
 * @param
 * pBuffer: read to buffer
 * ReadAddr: flash start address
 * NumByteToRead: number of bytes to read(MAX: 65535)
 *
 */
void W25QXX_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
	if (NumByteToRead < 0 || NumByteToRead > 65535)
	{
		return;
	}
	uint16_t i;
	W25QXX_CS = 0;
	SPI5_ReadWriteByte(W25X_ReadData);
	if (W25QXX_TYPE == W25Q256)
	{
		SPI5_ReadWriteByte((uint8_t)((ReadAddr) >> 24));
	}
	SPI5_ReadWriteByte((uint8_t)((ReadAddr) >> 16));
	SPI5_ReadWriteByte((uint8_t)((ReadAddr) >> 8));
	SPI5_ReadWriteByte((uint8_t)ReadAddr);
	for (i = 0; i < NumByteToRead; i++)
	{
		pBuffer[i] = SPI5_ReadWriteByte(0XFF);
	}
	W25QXX_CS = 1;
}

/**
 * @brief write data to W25QXX FLASH by SPI
 *
 * @param
 * pBuffer: data in buffer
 * WriteAddr: flash start address(24bits)
 * NumByteToWrite: The number of bytes to write (max 256),
 * which should not exceed the number of bytes remaining on the page
 *
 */
void W25QXX_Write_Page(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	if (NumByteToRead < 0 || NumByteToRead > 256)
	{
		return;
	}
	uint16_t i;
	W25QXX_Write_Enable();
	W25QXX_CS = 0;
	SPI5_ReadWriteByte(W25X_PageProgram);
	if (W25QXX_TYPE == W25Q256)
	{
		SPI5_ReadWriteByte((uint8_t)((WriteAddr) >> 24));
	}
	SPI5_ReadWriteByte((uint8_t)((WriteAddr) >> 16));
	SPI5_ReadWriteByte((uint8_t)((WriteAddr) >> 8));
	SPI5_ReadWriteByte((uint8_t)WriteAddr);
	for (i = 0; i < NumByteToWrite; i++)
	{
		SPI5_ReadWriteByte(pBuffer[i]);
	}
	W25QXX_CS = 1;
	W25QXX_Wait_Busy();
}

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
void W25QXX_Write_NoCheck(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint16_t pageremain;
	pageremain = 256 - WriteAddr % 256;
	if (NumByteToWrite <= pageremain)
	{
		pageremain = NumByteToWrite;
	}
	while (1)
	{
		W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
		if (NumByteToWrite == pageremain)
		{
			break;
		}
		else // NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;

			NumByteToWrite -= pageremain;
			if (NumByteToWrite > 256)
			{
				pageremain = 256;
			}
			else
			{
				pageremain = NumByteToWrite;
			}
		}
	};
}

/**
 * @brief write data to W25QXX FLASH by SPI with erase
 *
 * @param
 * pBuffer: data in buffer
 * WriteAddr: flash start address(24bits)
 * NumByteToWrite: The number of bytes to write (max 65535),
 *
 */
uint8_t W25QXX_BUFFER[4096];
void W25QXX_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;
	uint16_t i;
	uint8_t *W25QXX_BUF;
	W25QXX_BUF = W25QXX_BUFFER;
	secpos = WriteAddr / 4096; // sector addr
	secoff = WriteAddr % 4096; // offset in sector
	secremain = 4096 - secoff; // Sector remaining space size
	// printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);
	if (NumByteToWrite <= secremain)
	{
		secremain = NumByteToWrite;
	}
	while (1)
	{
		W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096);
		for (i = 0; i < secremain; i++)
		{
			if (W25QXX_BUF[secoff + i] != 0XFF)
			{
				break;
			}
		}
		if (i < secremain)
		{
			W25QXX_Erase_Sector(secpos);
			for (i = 0; i < secremain; i++)
			{
				W25QXX_BUF[i + secoff] = pBuffer[i];
			}
			W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096);
		}
		else
		{
			W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain);
		}
		if (NumByteToWrite == secremain)
		{
			break;
		}
		else
		{
			secpos++;	// sector addr increase 1
			secoff = 0; // offset set to 0

			pBuffer += secremain;
			WriteAddr += secremain;
			NumByteToWrite -= secremain;
			if (NumByteToWrite > 4096)
			{
				secremain = 4096;
			}
			else
			{
				secremain = NumByteToWrite;
			}
		}
	};
}

/**
 * @brief erase the whole falsh
 */
void W25QXX_Erase_Chip(void)
{
	W25QXX_Write_Enable(); // SET WEL
	W25QXX_Wait_Busy();
	W25QXX_CS = 0;
	SPI5_ReadWriteByte(W25X_ChipErase);
	W25QXX_CS = 1;
	W25QXX_Wait_Busy();
}

/**
 * @brief erase a sector
 *
 * @param
 * Dst_Addr: sector address
 * Minimum time to erase a sector: 150ms
 *
 */
void W25QXX_Erase_Sector(uint32_t Dst_Addr)
{
	// printf("fe:%x\r\n",Dst_Addr);
	Dst_Addr *= 4096;
	W25QXX_Write_Enable(); // SET WEL
	W25QXX_Wait_Busy();
	W25QXX_CS = 0;
	SPI5_ReadWriteByte(W25X_SectorErase);
	if (W25QXX_TYPE == W25Q256)
	{
		SPI5_ReadWriteByte((uint8_t)((Dst_Addr) >> 24));
	}
	SPI5_ReadWriteByte((uint8_t)((Dst_Addr) >> 16));
	SPI5_ReadWriteByte((uint8_t)((Dst_Addr) >> 8));
	SPI5_ReadWriteByte((uint8_t)Dst_Addr);
	W25QXX_CS = 1;
	W25QXX_Wait_Busy();
}

/**
 * @brief wait for busy
 *
 */
void W25QXX_Wait_Busy(void)
{
	while ((W25QXX_ReadSR(1) & 0x01) == 0x01)
	{
	};
}

/**
 * @brief write protenction using Power-down instruction
 *
 */
void W25QXX_PowerDown(void)
{
	W25QXX_CS = 0;
	SPI5_ReadWriteByte(W25X_PowerDown);
	W25QXX_CS = 1;
	delay_us(3);
}

/**
 * @brief wake up by send release Power-down instruction
 *
 */
void W25QXX_WAKEUP(void)
{
	W25QXX_CS = 0;
	SPI5_ReadWriteByte(W25X_ReleasePowerDown); //  send W25X_PowerDown command 0xAB
	W25QXX_CS = 1;
	delay_us(3);
}


/*
 * spi.h
 *
 */

#ifndef __SPI_H_
#define __SPI_H_

#include "sys.h"

//SPI Handle
extern SPI_HandleTypeDef SPI5_Handler;  

void SPI5_Init(void);
void SPI5_SetSpeed(uint8_t SPI_BaudRatePrescaler);
uint8_t SPI5_ReadWriteByte(uint8_t TxData);
#endif

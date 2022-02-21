/*
 * iic.h
 *
 */

#ifndef _IIC_H_
#define _IIC_H_
#include "sys.h"
	
//IO Direction
#define SDA_IN()  {GPIOH->MODER&=~(3<<(5*2));GPIOH->MODER|=0<<5*2;}	//PH5 input
#define SDA_OUT() {GPIOH->MODER&=~(3<<(5*2));GPIOH->MODER|=1<<5*2;} //PH5 output

//IO
#define IIC_SCL   PHout(4) //SCL
#define IIC_SDA   PHout(5) //SDA
#define READ_SDA  PHin(5)  //input SDA

/**
 * @brief initialization IIC
 *
 */
void IIC_Init(void);                

/**
 * @brief  Data transfer is initiated with a start condition (S) signaled by
 *         SDA being pulled low while SCL stays high.
 *
 */
void IIC_Start(void);				

/**
 * @brief  A stop condition (P) is signaled when SCL rises, followed by
 *         SDA rising.
 *
 */
void IIC_Stop(void);	  

/**
 * @brief Each frame in a message is followed by an acknowledge bit.
 * If an address frame or data frame was successfully received,
 * an ACK bit is returned to the sender from the receiving device.
 *
 */
void IIC_Ack(void);	

/**
 * @brief NACK Bit
 *
 */				
void IIC_NAck(void);

/**
 * @brief Wait ACK Signal
 *
 */
uint8_t IIC_Wait_Ack(void); 

/**
 * @brief  Write a byte to I2C bus. Return 0 if ack by the target.
 *
 * @param
 * txd: the send data
 */
void IIC_Send_Byte(uint8_t txd);		

/**
 * @brief Read a byte from I2C bus
 *
 * @param
 * ack: when ack=1, send ACK, when ack=0, send nACK
 *
 * @return read a btye from I2C bus
 */
uint8_t IIC_Read_Byte(unsigned char ack);
		
/**
 * @brief  Write a byte to device 
 *
 * @param
 * addr: device address
 * data: the data
 */        
void IIC_Write_One_Byte(uint8_t addr,uint8_t data);

/**
 * @brief  read a byte from device 
 *
 * @param
 * addr: device address
 * 
 * @return the data
 *
 */   
uint8_t IIC_Read_One_Byte(uint8_t addr);	 

#endif


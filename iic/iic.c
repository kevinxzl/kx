#include "iic.h"
#include "delay.h"

/**
 * @brief initialization IIC
 *
 */
void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOH_CLK_ENABLE(); // Enable GPIO H Clock

	// config PH4 and PH5
	GPIO_Initure.Pin = GPIO_PIN_4 | GPIO_PIN_5;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; // push pull
	GPIO_Initure.Pull = GPIO_PULLUP;		 // pull up
	GPIO_Initure.Speed = GPIO_SPEED_FAST;	 // fast speed
	HAL_GPIO_Init(GPIOH, &GPIO_Initure);

	IIC_SDA = 1;
	IIC_SCL = 1;
}

/**
 * @brief  Data transfer is initiated with a start condition (S) signaled by
 *         SDA being pulled low while SCL stays high.
 *
 */
void IIC_Start(void)
{
	SDA_OUT();
	IIC_SDA = 1;
	IIC_SCL = 1;
	delay_us(4);
	IIC_SDA = 0; // START:when CLK is high,DATA change form high to low
	delay_us(4);
	IIC_SCL = 0; // lock I2C bus£¬and ready to send or receive data
}

/**
 * @brief  A stop condition (P) is signaled when SCL rises, followed by
 *         SDA rising.
 *
 */
void IIC_Stop(void)
{
	SDA_OUT();
	IIC_SCL = 0;
	IIC_SDA = 0; // STOP:when CLK is high DATA change form low to high
	delay_us(4);
	IIC_SCL = 1;
	delay_us(4);
	IIC_SDA = 1;
}

/**
 * @brief Wait ACK Signal
 *
 */
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime = 0;
	SDA_IN(); 
	IIC_SDA = 1;
	delay_us(1);
	IIC_SCL = 1;
	delay_us(1);
	while (READ_SDA)
	{
		ucErrTime++;
		if (ucErrTime > 250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL = 0;
	return 0;
}

/**
 * @brief Each frame in a message is followed by an acknowledge bit.
 * If an address frame or data frame was successfully received,
 * an ACK bit is returned to the sender from the receiving device.
 *
 */
void IIC_Ack(void)
{
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 0;
	delay_us(2);
	IIC_SCL = 1;
	delay_us(2);
	IIC_SCL = 0;
}

/**
 * @brief NACK Bit
 *
 */
void IIC_NAck(void)
{
	IIC_SCL = 0;
	SDA_OUT();
	IIC_SDA = 1;
	delay_us(2);
	IIC_SCL = 1;
	delay_us(2);
	IIC_SCL = 0;
}

/**
 * @brief  Write a byte to I2C bus. Return 0 if ack by the target.
 *
 * @param
 * txd: the send data
 */
void IIC_Send_Byte(uint8_t txd)
{
	uint8_t t;
	SDA_OUT();
	IIC_SCL = 0;
	for (t = 0; t < 8; t++)
	{
		IIC_SDA = (txd & 0x80) >> 7;
		txd <<= 1;
		delay_us(2);
		IIC_SCL = 1;
		delay_us(2);
		IIC_SCL = 0;
		delay_us(2);
	}
}

/**
 * @brief Read a byte from I2C bus
 *
 * @param
 * ack: when ack=1, send ACK, when ack=0, send nACK
 *
 * @return read a btye from I2C bus
 */
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i, receive = 0;
	SDA_IN();
	for (i = 0; i < 8; i++)
	{
		IIC_SCL = 0;
		delay_us(2);
		IIC_SCL = 1;
		receive <<= 1;
		if (READ_SDA)
		{
			receive++;
		}
		delay_us(1);
	}
	if (!ack)
	{
		IIC_NAck();
	}
	else
	{
		IIC_Ack();
	}
	return receive;
}

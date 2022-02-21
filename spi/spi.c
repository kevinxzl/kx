#include "spi.h"

SPI_HandleTypeDef SPI5_Handler; // SPI Handle

/**
 * @brief initialization SPI 5
 *
 */
void SPI5_Init(void)
{
    SPI5_Handler.Instance = SPI5;             // SP5
    SPI5_Handler.Init.Mode = SPI_MODE_MASTER; // master mode
    SPI5_Handler.Init.Direction = SPI_DIRECTION_2LINES;
    SPI5_Handler.Init.DataSize = SPI_DATASIZE_8BIT; // data size
    SPI5_Handler.Init.CLKPolarity = SPI_POLARITY_HIGH;
    SPI5_Handler.Init.CLKPhase = SPI_PHASE_2EDGE;
    SPI5_Handler.Init.NSS = SPI_NSS_SOFT;
    SPI5_Handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    SPI5_Handler.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SPI5_Handler.Init.TIMode = SPI_TIMODE_DISABLE;
    SPI5_Handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    SPI5_Handler.Init.CRCPolynomial = 7;
    HAL_SPI_Init(&SPI5_Handler);

    __HAL_SPI_ENABLE(&SPI5_Handler);

    SPI5_ReadWriteByte(0Xff);
}

/**
 * @brief low level HAL initialization SPI
 *
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_SPI5_CLK_ENABLE();

    // PF7,8,9
    GPIO_Initure.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_FAST;
    GPIO_Initure.Alternate = GPIO_AF5_SPI5;
    HAL_GPIO_Init(GPIOF, &GPIO_Initure);
}

/**
 * @brief Set SPI 5 speed
 *
 * @param
 * SPI_BaudRatePrescaler: SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
 *  the fAPB1 defuat 45Mhz
 *
 */
void SPI5_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
    __HAL_SPI_DISABLE(&SPI5_Handler);
    SPI5_Handler.Instance->CR1 &= 0XFFC7;
    SPI5_Handler.Instance->CR1 |= SPI_BaudRatePrescaler;
    __HAL_SPI_ENABLE(&SPI5_Handler);
}

/**
 * @brief Transmit a byte
 *
 * @param
 * TxData: master send data to slave
 *
 * @retrun master recieve data from salve
 *
 */
uint8_t SPI5_ReadWriteByte(uint8_t TxData)
{
    uint8_t Rxdata;
    HAL_SPI_TransmitReceive(&SPI5_Handler, &TxData, &Rxdata, 1, 1000);
    return Rxdata;
}

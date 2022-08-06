#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

extern SPI_HandleTypeDef SPI1_Handler;  //SPI���

void SPI1_Init(void);
void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler);
uint8_t SPI1_ReadWriteByte(uint8_t TxData);

#endif

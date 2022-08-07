#ifndef _NRF_H
#define _NRF_H

#include "spi.h"

//命令代码
#define NRF_READ_REG    0x00//读命令
#define NRF_WRITE_REG   0x20//写命令
#define RD_RX_PLOAD     0x61//读RX有效数据
#define WR_TX_PLOAD     0xA0//写TX有效数据
#define FLUSH_TX        0xE1//清除TX FIFO寄存器
#define FLUSH_RX        0xE2//清除RX FIFO寄存器
#define REUSE_TX_PL     0xE3//重新使用上一包发射的有效数据
#define NOP             0xFF//空操作

//寄存器地址
#define CONFIG          0x00
#define EN_AA           0x01
#define EN_RXADDR       0x02
#define SETUP_AW        0x03
#define SETUP_RETR      0x04
#define RF_CH           0x05
#define RF_SETUP        0x06
#define STATUS          0x07
#define OBSERVE_TX      0x08
#define CD              0x09
#define RX_ADDR_P0      0x0A
#define RX_ADDR_P1      0x0B
#define RX_ADDR_P2      0x0C
#define RX_ADDR_P3      0x0D
#define RX_ADDR_P4      0x0E
#define RX_ADDR_P5      0x0F
#define TX_ADDR         0x10
#define RX_PW_P0        0x11
#define RX_PW_P1        0x12
#define RX_PW_P2        0x13
#define RX_PW_P3        0x14
#define RX_PW_P4        0x15
#define RX_PW_P5        0x16
#define FIFO_STATUS     0x17

//24L01发送接收数据宽度定义
#define TX_ADR_WIDTH    5//5字节的地址宽度
#define RX_ADR_WIDTH    5//5字节的地址宽度
#define TX_PLOAD_WIDTH  32//32字节的用户数据宽度
#define RX_PLOAD_WIDTH  32//32字节的用户数据宽度

#define SPI_CE_GPIO_Port    GPIOC
#define SPI_CE_Pin          GPIO_PIN_0
#define SPI_CSN_GPIO_Port   GPIOC
#define SPI_CSN_Pin         GPIO_PIN_1
#define SPI_IRQ_GPIO_Port   GPIOB
#define SPI_IRQ_Pin         GPIO_PIN_0

//CE引脚控制
#define NRF24L01_CE_HIGH         HAL_GPIO_WritePin(SPI_CE_GPIO_Port, SPI_CE_Pin, GPIO_PIN_SET)
#define NRF24L01_CE_LOW          HAL_GPIO_WritePin(SPI_CE_GPIO_Port, SPI_CE_Pin, GPIO_PIN_RESET)
//CSN引脚
#define NRF24L01_SPI_CSN_DISABLE HAL_GPIO_WritePin(SPI_CSN_GPIO_Port, SPI_CSN_Pin, GPIO_PIN_SET)
#define NRF24L01_SPI_CSN_ENABLE  HAL_GPIO_WritePin(SPI_CSN_GPIO_Port, SPI_CSN_Pin, GPIO_PIN_RESET)
//IRQ引脚
#define NRF24L01_IRQ_PIN_READ    HAL_GPIO_ReadPin(SPI_IRQ_GPIO_Port, SPI_IRQ_Pin)

//状态
#define MAX_TX      0x10//达到最大发送次数中断
#define TX_OK       0x20//TX发送完成中断
#define RX_OK       0x40//接收到数据中断


void nrf_init(void);
uint8_t read_write_byte(uint8_t send_byte);
uint8_t nrf_check(void);
uint8_t nrf_write_reg(uint8_t reg,uint8_t value);
uint8_t nrf_read_reg(uint8_t reg);
uint8_t nrf_read_buf(uint8_t reg, uint8_t *buf, uint8_t len);
uint8_t nrf_write_buf(uint8_t reg, uint8_t *buf, uint8_t len);
uint8_t nrf_send_pkg(uint8_t *buf);
uint8_t nrf_receive_pkg(uint8_t *buf);
void nrf_rx_mode(void);
void nrf_tx_mode(void);
void nrf_low_power_mode(void);

void EXTI0_IRQHandler(void);
void EXTI_Init(void);



#endif


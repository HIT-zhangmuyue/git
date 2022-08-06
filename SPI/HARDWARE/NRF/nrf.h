#ifndef _NRF_H
#define _NRF_H

#include "spi.h"

//�������
#define NRF_READ_REG    0x00//������
#define NRF_WRITE_REG   0x20//д����
#define RD_RX_PLOAD     0x61//��RX��Ч����
#define WR_TX_PLOAD     0xA0//дTX��Ч����
#define FLUSH_TX        0xE1//���TX FIFO�Ĵ���
#define FLUSH_RX        0xE2//���RX FIFO�Ĵ���
#define REUSE_TX_PL     0xE3//����ʹ����һ���������Ч����
#define NOP             0xFF//�ղ���

//�Ĵ�����ַ
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

//24L01���ͽ������ݿ�ȶ���
#define TX_ADR_WIDTH    5//5�ֽڵĵ�ַ���
#define RX_ADR_WIDTH    5//5�ֽڵĵ�ַ���
#define TX_PLOAD_WIDTH  32//32�ֽڵ��û����ݿ��
#define RX_PLOAD_WIDTH  32//32�ֽڵ��û����ݿ��

#define SPI_CE_GPIO_Port    GPIOC
#define SPI_CE_Pin          GPIO_PIN_0
#define SPI_CSN_GPIO_Port   GPIOC
#define SPI_CSN_Pin         GPIO_PIN_1
#define SPI_IRQ_GPIO_Port   GPIOB
#define SPI_IRQ_Pin         GPIO_PIN_0

//CE���ſ���
#define NRF24L01_CE_HIGH         HAL_GPIO_WritePin(SPI_CE_GPIO_Port, SPI_CE_Pin, GPIO_PIN_SET)
#define NRF24L01_CE_LOW          HAL_GPIO_WritePin(SPI_CE_GPIO_Port, SPI_CE_Pin, GPIO_PIN_RESET)
//CSN����
#define NRF24L01_SPI_CSN_DISABLE HAL_GPIO_WritePin(SPI_CSN_GPIO_Port, SPI_CSN_Pin, GPIO_PIN_SET)
#define NRF24L01_SPI_CSN_ENABLE  HAL_GPIO_WritePin(SPI_CSN_GPIO_Port, SPI_CSN_Pin, GPIO_PIN_RESET)
//IRQ����
#define NRF24L01_IRQ_PIN_READ    HAL_GPIO_ReadPin(SPI_IRQ_GPIO_Port, SPI_IRQ_Pin)

//״̬
#define MAX_TX      0x10//�ﵽ����ʹ����ж�
#define TX_OK       0x20//TX��������ж�
#define RX_OK       0x40//���յ������ж�


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


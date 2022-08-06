#include "nrf.h"
#include "stdlib.h"
#include "usart.h"	 
#include "spi.h"

const uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x34, 0x43, 0x10, 0x10, 0x01}; //���͵�ַ
const uint8_t RX_ADDRESS[RX_ADR_WIDTH] = {0x34, 0x43, 0x10, 0x10, 0x01}; //���յ�ַ


/**
 * @brief  ��ʼ��NRF24l01
 * @retval none
 */
void nrf_init(void)
{
  NRF24L01_CE_LOW;          //ʹ��24L01
  NRF24L01_SPI_CSN_DISABLE; // SPIƬѡȡ��
}

/**
 * @brief  SPI����һ�ֽ����ݲ�����һ�ֽ�����
 * @param  send_byte �����͵�����
 * @retval ���յ�������
 */
uint8_t read_write_byte(uint8_t send_byte)
{
    uint8_t read_byte;
    if (HAL_SPI_TransmitReceive(&SPI1_Handler, &send_byte, &read_byte, sizeof(uint8_t), 0xFF) != HAL_OK)
    {
        read_byte = 0xFF;
    }

    return read_byte;
}

/**
 * @brief  ���nrf24l01�Ƿ����
 * @retval 0 ��⵽;1 δ��⵽
 */
uint8_t nrf_check(void)
{
    uint8_t buf[5] = {0XA5, 0XA5, 0XA5, 0XA5, 0XA5};
    uint8_t i;

    nrf_write_buf(NRF_WRITE_REG + TX_ADDR, buf, 5); //д��5���ֽڵĵ�ַ.
    nrf_read_buf(TX_ADDR, buf, 5);                  //����д��ĵ�ַ
//		printf("buf :%x\r\n",buf[0]);
//		printf("buf :%x\r\n",buf[1]);
//		printf("buf :%x\r\n",buf[2]);
//		printf("buf :%x\r\n",buf[3]);
//		printf("buf :%x\r\n",buf[4]);  //ȫ��0

    for (i = 0; i < 5; i++)
    {
        if (buf[i] != 0XA5)
        {
            break;
        }
    }
    if (i != 5)
    {
        return 1; //���24L01����
    }

    return 0; //��⵽24L01
}

/**
 * @brief  SPIд�Ĵ���
 * @param  reg �Ĵ�����ַ
 * @param  value Ҫд���ֵ
 * @retval ����״̬�Ĵ���ֵ
 */
uint8_t nrf_write_reg(uint8_t reg, uint8_t value)
{
    uint8_t status;
    NRF24L01_SPI_CSN_ENABLE;       //ʹ��SPI����
    status = read_write_byte(reg); //���ͼĴ�����
    read_write_byte(value);        //д��Ĵ�����ֵ
    NRF24L01_SPI_CSN_DISABLE;      //�ر�SPI����

    return status; //����״̬�Ĵ���ֵ
}

/**
 * @brief  SPI���Ĵ���
 * @param  reg �Ĵ�����ַ
 * @retval ���ض��Ĵ���ֵ
 */
uint8_t nrf_read_reg(uint8_t reg)
{
    uint8_t reg_val;
    NRF24L01_SPI_CSN_ENABLE;         //ʹ��SPI����
    read_write_byte(reg);            //���ͼĴ�����
    reg_val = read_write_byte(0XFF); //��ȡ�Ĵ�������
    NRF24L01_SPI_CSN_DISABLE;        //��ֹSPI����
    return reg_val;                  //����״ֵ̬
}

/**
 * @brief  SPI���Ĵ�������
 * @param  reg �Ĵ�����ַ
 * @param  *buf ��ȡ�ص�ֵ����ָ��ָ���buf
 * @param  len ��ȡ�ĳ���
 * @retval ����״̬�Ĵ���ֵ
 */
uint8_t nrf_read_buf(uint8_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t status;
    uint8_t num;
    NRF24L01_SPI_CSN_ENABLE;                      //ʹ��SPI����
    status = read_write_byte(NRF_READ_REG + reg); //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
    for (num = 0; num < len; num++)
    {
        buf[num] = read_write_byte(0XFF); //��������
    }
    NRF24L01_SPI_CSN_DISABLE; //�ر�SPI����

    return status; //����״̬�Ĵ���ֵ
}

/**
 * @brief  SPIд�Ĵ�������
 * @param  reg �Ĵ�����ַ
 * @param  *buf ��ָ��ָ���bufд��reg�Ĵ���
 * @param  len ��ȡ�ĳ���
 * @retval ����״̬�Ĵ���ֵ
 */
uint8_t nrf_write_buf(uint8_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t status;
    uint8_t num;
    NRF24L01_SPI_CSN_ENABLE;       //ʹ��SPI����
    status = read_write_byte(reg); //���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
    for (num = 0; num < len; num++)
    {
        read_write_byte(*buf++); //д������
    }
    NRF24L01_SPI_CSN_DISABLE; //�ر�SPI����

    return status; //���ض�����״ֵ̬
}

/**
 * @brief  ��������
 * @param  *buf �����͵�buf
 * @retval ����״ֵ̬
 */
uint8_t nrf_send_pkg(uint8_t *buf)
{
    uint8_t status;
    NRF24L01_CE_LOW;
    nrf_write_buf(WR_TX_PLOAD, buf, TX_PLOAD_WIDTH); //д���ݵ�TX BUF  32���ֽ�
    NRF24L01_CE_HIGH;                                  //��������

    while (NRF24L01_IRQ_PIN_READ != 0); //�ȴ��������

    status = nrf_read_reg(STATUS);                 //��ȡ״̬�Ĵ�����ֵ
    nrf_write_reg(NRF_WRITE_REG + STATUS, status); //���TX_DS��MAX_RT�жϱ�־
    if (status & MAX_TX)                           //�ﵽ����ط�����
    {
        nrf_write_reg(FLUSH_TX, 0xff); //���TX FIFO�Ĵ���
        return MAX_TX;
    }
    if (status & TX_OK) //�������
    {
        return TX_OK;
    }
    return 0xff; //����ԭ����ʧ��
}

/**
 * @brief  ��������
 * @param  *buf ���յ�buf
 * @retval ����״ֵ̬
 */
uint8_t nrf_receive_pkg(uint8_t *buf)
{
    uint8_t status;
    status = nrf_read_reg(STATUS);                 //��ȡ״̬�Ĵ�����ֵ
    nrf_write_reg(NRF_WRITE_REG + STATUS, status); //���TX_DS��MAX_RT�жϱ�־
    if (status & RX_OK)                            //���յ�����
    {
        nrf_read_buf(RD_RX_PLOAD, buf, RX_PLOAD_WIDTH); //��ȡ����
        nrf_write_reg(FLUSH_RX, 0xff);                    //���RX FIFO�Ĵ���
        return 0;
    }

    return 1; //û�յ��κ�����
}

/**
 * @brief  ����ģʽ
 * @retval none
 */
void nrf_rx_mode(void)
{
    NRF24L01_CE_LOW;
    nrf_write_buf(NRF_WRITE_REG + RX_ADDR_P0, (uint8_t *)RX_ADDRESS, RX_ADR_WIDTH);
    nrf_write_reg(NRF_WRITE_REG + EN_AA, 0x01);
    nrf_write_reg(NRF_WRITE_REG + EN_RXADDR, 0x01);
    nrf_write_reg(NRF_WRITE_REG + RF_CH, 40);
    nrf_write_reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH);
    nrf_write_reg(NRF_WRITE_REG + RF_SETUP, 0x07);
    nrf_write_reg(NRF_WRITE_REG + CONFIG, 0x0F);
    NRF24L01_CE_HIGH;
    HAL_Delay(1);
}

/**
 * @brief  ����ģʽ
 * @retval none
 */
void nrf_tx_mode(void)
{

    NRF24L01_CE_LOW;
    nrf_write_buf(NRF_WRITE_REG + TX_ADDR, (uint8_t *)TX_ADDRESS, TX_ADR_WIDTH);
    nrf_write_buf(NRF_WRITE_REG + RX_ADDR_P0, (uint8_t *)RX_ADDRESS, RX_ADR_WIDTH);
    nrf_write_reg(NRF_WRITE_REG + EN_AA, 0x01);
    nrf_write_reg(NRF_WRITE_REG + EN_RXADDR, 0x01);
    nrf_write_reg(NRF_WRITE_REG + SETUP_RETR, 0x1a);
    nrf_write_reg(NRF_WRITE_REG + RF_CH, 40);
    nrf_write_reg(NRF_WRITE_REG + RF_SETUP, 0x07);
    nrf_write_reg(NRF_WRITE_REG + CONFIG, 0x0e);
    NRF24L01_CE_HIGH;
    HAL_Delay(1);
}

/**
 * @brief  ����ģʽ
 * @retval none
 */
void nrf_low_power_mode(void)
{
    NRF24L01_CE_LOW;
    nrf_write_reg(NRF_WRITE_REG + CONFIG, 0x00);
}



//�ⲿ�жϳ�ʼ��
void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();              
    
    GPIO_Initure.Pin=GPIO_PIN_0;               
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;      //�½��ش���
    GPIO_Initure.Pull=GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    
    //�ж���
    HAL_NVIC_SetPriority(EXTI0_IRQn,0,0);       
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);            
}


//�жϷ�����
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);		//�����жϴ����ú���
}

//�жϷ����������Ҫ��������
//��HAL�������е��ⲿ�жϷ�����������ô˺���
//GPIO_Pin:�ж����ź�
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	 switch(GPIO_Pin)
		{
			case GPIO_PIN_0:
					HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9,GPIO_PIN_RESET);	
					HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_RESET);	break;
      default:
					break;
		}			
}


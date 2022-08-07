#include "nrf.h"
#include "stdlib.h"
#include "usart.h"	 
#include "spi.h"

const uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0x34, 0x43, 0x10, 0x10, 0x01}; //发送地址
const uint8_t RX_ADDRESS[RX_ADR_WIDTH] = {0x34, 0x43, 0x10, 0x10, 0x01}; //接收地址


/**
 * @brief  初始化NRF24l01
 * @retval none
 */
void nrf_init(void)
{
  NRF24L01_CE_LOW;          //使能24L01
  NRF24L01_SPI_CSN_DISABLE; // SPI片选取消
}

/**
 * @brief  SPI发送一字节数据并接收一字节数据
 * @param  send_byte 待发送的数据
 * @retval 接收到的数据
 */
uint8_t read_write_byte(uint8_t send_byte)
{
    uint8_t read_byte;
    if (HAL_SPI_TransmitReceive(&hspi1, &send_byte, &read_byte, sizeof(uint8_t), 0xFF) != HAL_OK)
    {
        read_byte = 0xFF;
    }

    return read_byte;
}

/**
 * @brief  检测nrf24l01是否存在
 * @retval 0 检测到;1 未检测到
 */
uint8_t nrf_check(void)
{
    uint8_t buf[5] = {0XA5, 0XA5, 0XA5, 0XA5, 0XA5};
    uint8_t i;

    nrf_write_buf(NRF_WRITE_REG + TX_ADDR, buf, 5); //写入5个字节的地址.
    nrf_read_buf(TX_ADDR, buf, 5);                  //读出写入的地址
//		printf("buf :%x\r\n",buf[0]);
//		printf("buf :%x\r\n",buf[1]);
//		printf("buf :%x\r\n",buf[2]);
//		printf("buf :%x\r\n",buf[3]);
//		printf("buf :%x\r\n",buf[4]);  //全是0

    for (i = 0; i < 5; i++)
    {
        if (buf[i] != 0XA5)
        {
            break;
        }
    }
    if (i != 5)
    {
        return 1; //检测24L01错误
    }

    return 0; //检测到24L01
}

/**
 * @brief  SPI写寄存器
 * @param  reg 寄存器地址
 * @param  value 要写入的值
 * @retval 返回状态寄存器值
 */
uint8_t nrf_write_reg(uint8_t reg, uint8_t value)
{
    uint8_t status;
    NRF24L01_SPI_CSN_ENABLE;       //使能SPI传输
    status = read_write_byte(reg); //发送寄存器号
    read_write_byte(value);        //写入寄存器的值
    NRF24L01_SPI_CSN_DISABLE;      //关闭SPI传输

    return status; //返回状态寄存器值
}

/**
 * @brief  SPI读寄存器
 * @param  reg 寄存器地址
 * @retval 返回读寄存器值
 */
uint8_t nrf_read_reg(uint8_t reg)
{
    uint8_t reg_val;
    NRF24L01_SPI_CSN_ENABLE;         //使能SPI传输
    read_write_byte(reg);            //发送寄存器号
    reg_val = read_write_byte(0XFF); //读取寄存器内容
    NRF24L01_SPI_CSN_DISABLE;        //禁止SPI传输
    return reg_val;                  //返回状态值
}

/**
 * @brief  SPI读寄存器区域
 * @param  reg 寄存器地址
 * @param  *buf 读取回的值放入指针指向的buf
 * @param  len 读取的长度
 * @retval 返回状态寄存器值
 */
uint8_t nrf_read_buf(uint8_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t status;
    uint8_t num;
    NRF24L01_SPI_CSN_ENABLE;                      //使能SPI传输
    status = read_write_byte(NRF_READ_REG + reg); //发送寄存器值(位置),并读取状态值
    for (num = 0; num < len; num++)
    {
        buf[num] = read_write_byte(0XFF); //读出数据
    }
    NRF24L01_SPI_CSN_DISABLE; //关闭SPI传输

    return status; //返回状态寄存器值
}

/**
 * @brief  SPI写寄存器区域
 * @param  reg 寄存器地址
 * @param  *buf 将指针指向的buf写入reg寄存器
 * @param  len 读取的长度
 * @retval 返回状态寄存器值
 */
uint8_t nrf_write_buf(uint8_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t status;
    uint8_t num;
    NRF24L01_SPI_CSN_ENABLE;       //使能SPI传输
    status = read_write_byte(reg); //发送寄存器值(位置),并读取状态值
    for (num = 0; num < len; num++)
    {
        read_write_byte(*buf++); //写入数据
    }
    NRF24L01_SPI_CSN_DISABLE; //关闭SPI传输

    return status; //返回读到的状态值
}

/**
 * @brief  发送数据
 * @param  *buf 待发送的buf
 * @retval 返回状态值
 */
uint8_t nrf_send_pkg(uint8_t *buf)
{
    uint8_t status;
    NRF24L01_CE_LOW;
    nrf_write_buf(WR_TX_PLOAD, buf, TX_PLOAD_WIDTH); //写数据到TX BUF  32个字节
    NRF24L01_CE_HIGH;                                  //启动发送

    while (NRF24L01_IRQ_PIN_READ != 0); //等待发送完成

    status = nrf_read_reg(STATUS);                 //读取状态寄存器的值
    nrf_write_reg(NRF_WRITE_REG + STATUS, status); //清除TX_DS或MAX_RT中断标志
    if (status & MAX_TX)                           //达到最大重发次数
    {
        nrf_write_reg(FLUSH_TX, 0xff); //清除TX FIFO寄存器
        return MAX_TX;
    }
    if (status & TX_OK) //发送完成
    {
        return TX_OK;
    }
    return 0xff; //其他原因发送失败
}

/**
 * @brief  接收数据
 * @param  *buf 接收的buf
 * @retval 返回状态值
 */
uint8_t nrf_receive_pkg(uint8_t *buf)
{
    uint8_t status;
    status = nrf_read_reg(STATUS);                 //读取状态寄存器的值
    nrf_write_reg(NRF_WRITE_REG + STATUS, status); //清除TX_DS或MAX_RT中断标志
    if (status & RX_OK)                            //接收到数据
    {
        nrf_read_buf(RD_RX_PLOAD, buf, RX_PLOAD_WIDTH); //读取数据
        nrf_write_reg(FLUSH_RX, 0xff);                    //清除RX FIFO寄存器
        return 0;
    }

    return 1; //没收到任何数据
}

/**
 * @brief  接收模式
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
 * @brief  发送模式
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
 * @brief  掉电模式
 * @retval none
 */
void nrf_low_power_mode(void)
{
    NRF24L01_CE_LOW;
    nrf_write_reg(NRF_WRITE_REG + CONFIG, 0x00);
}



//外部中断初始化
void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();              
    
    GPIO_Initure.Pin=GPIO_PIN_0;               
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;      //下降沿触发
    GPIO_Initure.Pull=GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    
    //中断线
    HAL_NVIC_SetPriority(EXTI0_IRQn,0,0);       
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);            
}


//中断服务函数
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);		//调用中断处理公用函数
}

//中断服务程序中需要做的事情
//在HAL库中所有的外部中断服务函数都会调用此函数
//GPIO_Pin:中断引脚号
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


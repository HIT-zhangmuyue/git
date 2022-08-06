#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"
#include "nrf.h"
#include "spi.h"

int main(void)
{
	uint8_t test_buf[33];

  HAL_Init();                   	//初始化HAL库    
  Stm32_Clock_Init(336,8,2,7);  	//设置时钟,168Mhz
	delay_init(168);               	//初始化延时函数
	uart_init(115200);             	//初始化USART
	usmart_dev.init(84); 		    //初始化USMART
	LED_Init();						      //初始化LED	
	SPI1_Init();
	nrf_init();
	EXTI_Init();
	
//	KEY_Init();						//初始化KEY
// 	LCD_Init();           			//初始化LCD	
	HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9,GPIO_PIN_SET);	
  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_SET);	
	while(nrf_check() != 0)
	{
		HAL_Delay(1000);
	}
	nrf_rx_mode();
		
	while(1)
	{
		if(nrf_receive_pkg(test_buf) == 0)
		{
			test_buf[32]=0;//加入字符串结束符
//			HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9,GPIO_PIN_RESET);	
//			HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_RESET);	
		}
	}	
}


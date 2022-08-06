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

  HAL_Init();                   	//��ʼ��HAL��    
  Stm32_Clock_Init(336,8,2,7);  	//����ʱ��,168Mhz
	delay_init(168);               	//��ʼ����ʱ����
	uart_init(115200);             	//��ʼ��USART
	usmart_dev.init(84); 		    //��ʼ��USMART
	LED_Init();						      //��ʼ��LED	
	SPI1_Init();
	nrf_init();
	EXTI_Init();
	
//	KEY_Init();						//��ʼ��KEY
// 	LCD_Init();           			//��ʼ��LCD	
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
			test_buf[32]=0;//�����ַ���������
//			HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9,GPIO_PIN_RESET);	
//			HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_RESET);	
		}
	}	
}


#include "main.h"
#include "DHT11.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//extern TIM_HandleTypeDef htim2;
//extern void FND_update(uint16_t value);

extern volatile int TDHT11_timer_counter;
extern volatile int TIM10_10ms_counter;

extern void delay_us (unsigned long us);
extern void move_cursor(uint8_t row, uint8_t column);
extern void lcd_string(uint8_t *str);

volatile int DHT11_print_flag = 1;

void DHT11_main(void);
void DHT11_Init(void);
void DHT11_processing(void);

void DHT11_main(void)
{
	uint8_t i_RH, d_RH, i_Tmp, d_Tmp;
	
	// HAL_TIM_Base_Start_IT(&htim2);
	DHT11_Init();
	while(1)
	{
		DHT11_trriger();
		DHT11_DataLine_Input();
		DHT11_dumi_read();

		i_RH = DHT11_rx_Data();
		d_RH = DHT11_rx_Data();
		i_Tmp = DHT11_rx_Data();
		d_Tmp = DHT11_rx_Data();
		
		DHT11_DataLine_Output();
		HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);

		printf("[Tmp]%d\n",(int)i_Tmp);
		printf("[Wet]%d\n",(int)i_RH);

		// FND_Update(i_Tmp*100 + i_RH);
		HAL_Delay(1500); // DHT11 프로토콜을 간격없이 무한정 불르면 DHT11이 죽는다. 그래서 그걸 막기 위해서 hal_delay()를 썼던거다. 근데 hal_delay() 뺄라면 어떻게 해야할까? DHT11_processing에 구현해보자.
	}
	
}

int dht11time = 150; // 전역변수 상태
void DHT11_processing(void)
{
	uint8_t i_RH, d_RH, i_Tmp, d_Tmp;
	char lcd_buff[20];

	if (TIM10_10ms_counter >= dht11time)
	{
		TIM10_10ms_counter = 0;

		DHT11_trriger(); // DHT11과 MCU의 Hand-shaking과정이다.
		DHT11_DataLine_Input();
		DHT11_dumi_read();

		// 여기부터 DHT11가 수집한 데이터를 읽어오는 것이다.
		i_RH = DHT11_rx_Data();
		d_RH = DHT11_rx_Data();
		i_Tmp = DHT11_rx_Data();
		d_Tmp = DHT11_rx_Data();

		DHT11_DataLine_Output();
		HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);

		if (DHT11_print_flag)
		{
			printf("[Tmp]%d\n",(int)i_Tmp);
			printf("[Wet]%d\n",(int)i_RH);

			sprintf(lcd_buff, "Tmp: %d Wet: %d", (int)i_Tmp, (int)i_RH);
			move_cursor(0,0);
			lcd_string(lcd_buff);
		}

		//	FND_update(i_Tmp*100 + i_RH);
	}
}

void DHT11_Init(void)
{
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);
	HAL_Delay(3000); // 초기화 할때는 딱 한번만 일어나기 때문에 hal_delay()를 줘도 상관 없다.
	return;
}


void DHT11_trriger(void)
{
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_RESET);
	HAL_Delay(20);
	
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);
	delay_us(7);
	return;
}


void DHT11_DataLine_Input(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/*Configure GPIO pin : PH0 */
  GPIO_InitStruct.Pin = DHT11_DATA_RIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;			//Change Output to Input
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
	
	return;
}


void DHT11_DataLine_Output(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/*Configure GPIO pin : PH0 */
  GPIO_InitStruct.Pin = DHT11_DATA_RIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;			//Change Input to Output 
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
	
	return;
}


uint8_t DHT11_rx_Data(void) // 이 함수가 데이터시트를 코드로 옮기는 핵심 포인트 함수이다. 0과 1을 판별하는 법..
{
	uint8_t rx_data = 0;
	
	for(int i = 0; i < 8; i++) // 8비트 받아야 하니까 for문을 8번 돌린다.
	{
		//when Input Data == 0
		while( 0 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
#if 1
		delay_us(40); // 0인지 1인지 구분해야 하니까 둘의 중간 값 정도인 40마이크로 세컨드 만큼 딜레이를 주고 그 다음에 오는 if문을 통해 0인지 1인지 판단하겠다는 것이다. (0이면 26마이크로 세컨드만 유지되고, 1이면 70마이크로 세컨트가 유지되기 때문)
#else  // org
		delay_us(16);
#endif
		rx_data<<=1; // 한 비트를 좌로 쉬프트 시킨다. 총 8비트를 쌓아가야 하니까...
		
		//when Input Data == 1
		if(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN))
		{
			rx_data |= 1;
			// 단 하나의 비트만 1로 바꿔주는 것이기 때문에 그냥 상수 대입 시키는 식으로 하면 안된다.
			// 이렇게 00000000000000000000000000000001 값과 OR비트 연산을 해주면
			// rx_data는 8비트 이기 때문에 00000001과 OR 연산되어 끝자리 비트만 0에서 1로 바뀌는 효과가 발생하게 된
		}
		while( 1 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
	}
	return rx_data;
}


void DHT11_dumi_read(void)
{
	while( 1 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
	while( 0 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
	while( 1 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
	return;
}

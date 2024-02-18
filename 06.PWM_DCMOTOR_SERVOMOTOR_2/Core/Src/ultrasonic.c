#include "ultrasonic.h"

extern volatile int TIM10_10ms_ultrasonic;

extern void delay_us (unsigned long us);
extern void move_cursor(uint8_t row, uint8_t column);
extern void lcd_string(uint8_t *str);

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void ultrasonic_processing(void);
void make_trigger(void);

volatile int distance; // 상승엣지부터 하강엣지까지 펄스가 몇번 카운트 되었는지 그 횟수를 담아둘 변수
volatile int ic_cpt_finish_flag = 0; // 초음파 거리 측정 완료 indicator 플래그변수
volatile uint8_t is_first_capture = 0; // 상승엣지 때문에 콜백펑션에 들어온건지 하강엣지 때문에 콜백 펑션이 들어온건지 구분하기 위한 플래그변수(0: 상승엣지, 1: 하강엣지)
volatile int ultrasonic_print_flag = 1;

// 1. Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c에 가서 가져온 call-back function
// 2. 초음파 센서의 ECHO핀의 상승edge와 하강edge 발생 시 이 함수로 들어온다!!!
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3)
	{
		if (is_first_capture == 0) // 상승엣지 때문에 콜백 펑션에 들어온 경우
		{
			__HAL_TIM_SET_COUNTER(htim, 0); // 펄스를 셀 카운터를 초기화 하고 세기 시작하는 것이다.
			is_first_capture = 1; // 다음에 콜백 펑선이 불릴 때는 당연히 하강 엣지일 때 일 것이므로 플래그변수를 1로 셋팅해준다.
		}
		else if (is_first_capture == 1) // 하강 엣지 때문에 콜백 펑션에 들어온 경우
		{

			is_first_capture = 0; // 이제 다음 펄스를 또 카운트 하기 위해서 플래그 변수를 초기화 해준다.
			distance = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // 상승엣지부터 하강엣지까지 펄스가 몇번 카운트 되었는지 그 값을 읽어온다.
			ic_cpt_finish_flag = 1; // 초음파 측정완료
		}
	}
}

void ultrasonic_processing(void)
{
	int distance_lv; // 전역변수 값을 바로 쓰지 않고 지역변수에 복사하여 사용하는 것이 안전하기 때문에 별도의 지역변수를 선언함
	char lcd_buff[20];

	if (TIM10_10ms_ultrasonic >= 100) // 10ms가 100개면 1초
	{
		TIM10_10ms_ultrasonic = 0;
		make_trigger();
		if (ic_cpt_finish_flag) // 초음파 측정이 완료 되었다면..
		{
			ic_cpt_finish_flag = 0;
			distance_lv = distance; // 전역변수 값을 바로 쓰지 않고 지역변수에 복사하여 사용하는 것임
			distance_lv = distance_lv * 0.034 / 2; // 1us마다 0.034cm를 이동한다. 그리고 초음파 센서가 리턴한 distance값은 왕복 거리이기 때문에 /2를 해서 편도 거리를 얻어낸다.

			if (ultrasonic_print_flag)
			{
				printf("distance_lv: %dcm\n", distance_lv);
				sprintf(lcd_buff, "distance: %dcm", distance_lv);
				move_cursor(1,0);
				lcd_string(lcd_buff);
			}
		}
	}
}

void make_trigger(void)
{
	HAL_GPIO_WritePin(ULTRASONIC_TRIGGER_GPIO_Port, ULTRASONIC_TRIGGER_Pin, 0);
	delay_us(2);
	HAL_GPIO_WritePin(ULTRASONIC_TRIGGER_GPIO_Port, ULTRASONIC_TRIGGER_Pin, 1);
	delay_us(10);
	HAL_GPIO_WritePin(ULTRASONIC_TRIGGER_GPIO_Port, ULTRASONIC_TRIGGER_Pin, 0);
	// 위 5줄의 코드 라인을 통해 초음파 센서에서 요구하는 트리거 신호의 전기적 파형을 MCU가 날릴 수 있도록 구현했다.

}

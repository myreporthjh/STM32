#include "dcmotor.h"
#include "button.h"

extern TIM_HandleTypeDef htim4;

void dcmotor_pwm_control(void);

uint8_t pwm_start_flag = 0; // dcmotor start/stop indicator flag
uint16_t ChannelControlRegister_UP_Value = 0; // PWM UP control // 16비트 타이머니까 uint16_t로 선언함
uint16_t ChannelControlRegister_DOWN_Value = 0; // PWM DOWN control
uint8_t forward_backwrd_dcmotor = 0; // 정방향 0, 역방향 1 플래그
uint8_t break_dcmotor = 0; // no break상태 0, break 상태 1 플래그

// BUTTON0: start/stop : 버튼이 눌리면 모터를 작동하거나 끄며 LED1 toggle
// BUTTON1: speed up : 버튼이 눌리면 모터를 가속하며 LED2 toggle
// BUTTON2: speed down : 버튼이 눌리면 모터를 감속하며 LED3 toggle
// BUTTON3: 정회전 역회전 반전
void dcmotor_pwm_control(void)
{
	// BUTTON0: start/stop : 버튼이 눌리면 모터를 작동하거나 끄며 LED1 toggle
	if(Get_Button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // LED1

		if (!forward_backwrd_dcmotor) // 역방향 회전중이라면
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
		}


		if (!pwm_start_flag) // 앞에 not 있음.. pwm이 첫음 누른 상태라면..?
		{
			pwm_start_flag = 1;
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1); // START_it가 아니라 그냥 Start이다. START_it는 main()단에서 이미 했다.
		}
		else
		{
			pwm_start_flag = 0;
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
		}
	}

	// BUTTON1: speed up : 버튼이 눌리면 모터를 가속하며 LED2 toggle
	if(Get_Button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7); // LED1

		ChannelControlRegister_UP_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
		ChannelControlRegister_UP_Value += 10;

		if (ChannelControlRegister_UP_Value > 100) // ChannelControlRegister_UP_Value의 쓰레시 홀드(최대치 상한): 듀디사이클 최대 상한치는 100%(풀스피트)
		{
			ChannelControlRegister_UP_Value = 100;
		}

		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, ChannelControlRegister_UP_Value);
	}

	// BUTTON2: speed down : 버튼이 눌리면 모터를 감속하며 LED3 toggle
	if(Get_Button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14); // LED1

		ChannelControlRegister_DOWN_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
		ChannelControlRegister_DOWN_Value -= 10;

		if (ChannelControlRegister_DOWN_Value < 60) // ChannelControlRegister_DOWN_Value의 쓰레시 홀드(최대치 하한): 듀디사이클 펄스 최대 하한치는 60%
		{
			ChannelControlRegister_DOWN_Value = 60;
		}

		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, ChannelControlRegister_DOWN_Value);
	}

	// BUTTON3: 정회전 역회전 반전
	if(Get_Button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
	{
		if (!forward_backwrd_dcmotor) // 역방향 회전중이라면
		{
			forward_backwrd_dcmotor = 1;
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
		}
		else
		{
			forward_backwrd_dcmotor = 0;
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
		}

	}

}


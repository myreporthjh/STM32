#include "button.h"
#include "dcmotor.h"

extern TIM_HandleTypeDef htim4;
extern void led_all_on(void);
extern void led_all_off(void);
void dcmotor_pwm_control(void);

extern volatile int TIM10_10ms_counter;
extern volatile int TIM10_10ms_fan;

// BUTTON0 : start/stop : LED1 (toggle)
// BUTTON1 : SPEED-UP : LED2 (toggle)
// BUTTON2 : SPEED-DOWN : LED3 (toggle)
uint8_t pwm_start_flag = 0; // dcmotor start/stop indicator flag
uint8_t mode_flag = 0; // dcmotor mode(auto, manual), default : manual
uint8_t countdown_flag = 0; // countdown flag (0:time setting, 1:start countdown)
uint8_t CCR_UP_Value = 0; // PWM UP control
uint8_t CCR_DOWN_Value = 0; // PWM DOWN control


void dcmotor_pwm_control(void)
{
	char lcd_buff[20];
	static int i=-1;
	static int timer=0; // timer 설정
	int num; // for loop

	if(!mode_flag) // 초기세팅 기본값 : manual mode, 정회전
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, 1); // LED2 on

		if(TIM10_10ms_counter >= 5) // 50ms
		{
			HAL_GPIO_TogglePin(LEDONOFF_GPIO_Port, LEDONOFF_Pin);
			TIM10_10ms_counter = 0;
		}
	}
	else // auto mode
	{
		if(TIM10_10ms_counter >= 10) // 100ms
		{
			HAL_GPIO_TogglePin(LEDONOFF_GPIO_Port, LEDONOFF_Pin);
			TIM10_10ms_counter = 0;
		}
		if(countdown_flag) // when start countdown
		{
			if(TIM10_10ms_fan>=10 && timer>0) //100ms 마다 반복
			{
				sprintf(lcd_buff, "count:%dms!!", timer*10);
				timer-=10;
				move_cursor(1,0);
				lcd_string(lcd_buff);

				TIM10_10ms_fan=0;
			}
			if(timer<=0)
			{
				sprintf(lcd_buff, "--Count End--");
				move_cursor(1,0);
				lcd_string(lcd_buff);

				pwm_start_flag=0; // STOP status
				//STOP FAN
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
			}
		}
	}


	// button0 - 전원 ON/OFF
	if(get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		if(!pwm_start_flag)
		{
			// 정방향
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);

			if(!mode_flag) 	// manual mode
			{
				// 전원 on, LED 1 UP
				HAL_GPIO_WritePin(GPIOD, 0x01, 1);
				i++;

				//CCR_UP_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
				CCR_UP_Value = 30;
				__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_UP_Value);
			}
			else // auto mode
			{
				sprintf(lcd_buff, "---auto mode---");
				move_cursor(0,0);
				lcd_string(lcd_buff);
			}

			pwm_start_flag=1; // START flag ON
		}
		else
		{
			i=-1;
			led_all_off();
			// STOP
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
			pwm_start_flag=0; // START flag OFF
		}
	}


	// button1 - 선풍기 가속, LED BAR 1 UP
	if(get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
	{
		if(!mode_flag) 		// manual mode
		{
			if(i < 7 && pwm_start_flag) // i가7보다작고 전원ON상태일때
			{
				i++;
				for(num=0; num<=i; num++)
				{
					HAL_GPIO_WritePin(GPIOD, 0x01 << num, 1);
				}

				// 가속
				CCR_UP_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
				CCR_UP_Value += 10;
				__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_UP_Value);

			}
		}
		else // auto mode
		{
			if(pwm_start_flag) // 전원ON상태일때
			{
				timer+=100; // 1초씩 증가
				sprintf(lcd_buff, "time : %dms", timer*10);
				move_cursor(1,0);
				lcd_string(lcd_buff);
			}
		}
	}


	// button2 - 선풍기 감속, LED BAR 1 DOWN
	if(get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
	{
		if(!mode_flag) 		// manual mode
		{
			if(i > 0 && pwm_start_flag) // i가0보다크고 전원ON상태일때
			{
				i--;
				led_all_off();

				for(num=0; num<=i; num++)
				{
					HAL_GPIO_WritePin(GPIOD, 0x01 << num, 1);
				}

				// 감속
				CCR_DOWN_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
				CCR_DOWN_Value -= 10;

				__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_DOWN_Value);
			}
		}
		else // auto mode
		{
			if(pwm_start_flag) // 전원ON상태일때
			{
				timer-=100; // 1초씩 감소
				sprintf(lcd_buff, "time : %dms", timer*10);
				move_cursor(1,0);
				lcd_string(lcd_buff);
			}
		}
	}


	// button3 - 정방향 역방향 조절
	if(get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
	{
		if(!mode_flag) // manual mode
		{
			if(pwm_start_flag) // 시작중일때만
			{
				HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_8);
				HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);
			}
		}
		else // auto mode
		{
			if(pwm_start_flag) // 전원ON상태일때
			{
				countdown_flag=1;
			}
		}
	}


	// button4(blue button) - auto, manual toggle 기능
	if(get_button(BUTTON4_GPIO_Port, BUTTON4_Pin, 4) == BUTTON_PRESS)
	{
		if(!pwm_start_flag) // 정지 상태일때만 모드 변경 가능
		{
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // LED1 toggle
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7); // LED2 toggle

			if(!mode_flag) // manual mode, 정회전
			{
				mode_flag=1; // mode 변경 (manual->auto)
			}
			else // auto mode, 역회전
			{
				mode_flag=0; // mode 변경 (auto->manual)
			}
		}
	}


	// start/stop : button0
//	if(get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
//	{
//		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // LED1
//
//		if(!pwm_start_flag)
//		{
//			// 정방향
//			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
//			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
//			pwm_start_flag=1;
//			//HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
//		}
//		else
//		{
//			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
//			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
//			pwm_start_flag=0;
//			//HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
//		}
//	}
//
//	//speed-up : button1
//	if(get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
//	{
//		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7); // LED2
//		CCR_UP_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
//		CCR_UP_Value += 10;
//		if(CCR_UP_Value > 100) CCR_UP_Value = 100;
//
//		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_UP_Value);
//
//		// 역방향
//		//HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
//		//HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
//
//	}
//
//	//speed-down : button2
//	if(get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
//	{
//		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14); // LED3
//		CCR_DOWN_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
//		CCR_DOWN_Value -= 10;
//		if(CCR_DOWN_Value < 10) CCR_DOWN_Value = 10;
//
//		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_DOWN_Value);
//	}

	//speed-down : button3
//	if(get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
//	{
//		//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14); // LED3
//		CCR_DOWN_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
//		CCR_DOWN_Value -= 10;
//		if(CCR_DOWN_Value < 10) CCR_DOWN_Value = 10;
//
//		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_DOWN_Value);
//	}


}

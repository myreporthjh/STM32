#include "main.h"

extern TIM_HandleTypeDef htim2;

void servo_motor_test_main(void);

// 8400000HZ / 1680 = 50000HZ
// t=1/f = 1/50000 = 0.00002sec(20us)마이크로세컨드
// 2ms (180도) : 0.00002 x 100개
// 1.5(90도) : 0.00002 x 75개
// 1ms(0도) : 0.00002 x 50개
void servo_motor_test_main(void)
{
	while(1)
	{
		// 180도 회전
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 10); //
		printf("--111--\n");
		HAL_Delay(2000); //1초 = 1000ms

		// 0도 회전
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 200);
		printf("--222--\n");
		HAL_Delay(1000); //1초 = 1000ms

		// 0도 회전
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 200);
		printf("--333--\n");
		HAL_Delay(1000); //1초 = 1000ms
	}
}

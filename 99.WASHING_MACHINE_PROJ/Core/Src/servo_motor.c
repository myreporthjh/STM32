#include "main.h"

void servo_motor_test_main(void);

extern TIM_HandleTypeDef htim2;

// 8400000HZ / 1680 = 50000HZ
// t=1/f = 1/50000 = 0.00002sec(20us)마이크로세컨드
// 2ms (180도) : 0.00002 x 100개
// 1.5(90도) : 0.00002 x 75개
// 1ms(0도) : 0.00002 x 50개
void servo_motor_test_main(void)
{
	while(1)
	{
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 5); // 0도
		printf("111111111111111\n");
		HAL_Delay(3000); //1초 = 1000ms

		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 105); // 180도
		printf("22222222222222222\n");
		HAL_Delay(3000); //1초 = 1000ms

		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 65); // 90도
		printf("333333333333333\n");
		HAL_Delay(3000); //1초 = 1000ms
	}
}

#include "main.h"
#include "button.h"

void set_rpm(int rpm);
void stepmotor_main_test();
void stepmotor_drive(int step);

extern void delay_us(unsigned long us);
extern int get_button(GPIO_TypeDef *GPIO, uint16_t GPIO_PIN, uint8_t button_number);

// RPM : 분당회전수
// 1분: 60sec: 1,000,000us(1초) x 60 = 60,000,000us
// 1,000,000us (1초)
// --> 1초(1000ms) ==>(1000us) x 1000ms ==> 1,000,000us
// 4096 스텝 : 1바퀴 (4096스텝이동)
// --> 1바퀴 도는데 4096스텝필요
// 4096 / 8(0.7) ==> 512 sequence : 360도
// 1 sequence(8step) : 0.70312도
// 0.70312도 x 512 = 360도

// -------- set_rpm(13) 으로 지정시의 동작상황 ---------
// 60,000,000us(1분) / 4096 / rpm
// 1126us(1스텝 idle 타입) x 4096 = 4,612,096us
//								= 4612ms
//								= 4.6초
// 60 / 4.6 (1회전 소요시간 초) ==> 13회전
// 시계방향으로 1회전 <---> 반시계방향으로 1회전
void set_rpm(int rpm)
{
	delay_us(60000000/4096/rpm);
	// 최대 speed 기준 (13) : delay_us(1126)
}

void stepmotor_main_test()
{
	static int button0_count = 0;
	static int direction_status = 1;
	static int moving_status = 0;
#if 1
	while(1)
	{
		// 버튼 처리에 의한 스텝모터 구동 프로그램 작성
		if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS) // BUTTON0
		{
			moving_status = 1;
			button0_count ++;
			button0_count %= 2;

			if(button0_count)
			{
				direction_status = 1;
			}
			else
			{
				direction_status = 2;
			}

		}

		if (get_button(BUTTON0_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS) // BUTTON1
		{
			moving_status = 0;
			set_rpm(0);
		}

		// 움직임 있을때
		if(moving_status)
		{
			stepmotor_drive(direction_status);
			if(direction_status==1)
				set_rpm(13);
			else
				set_rpm(10);
		}


	}
#else
	while(1)
	{
		for(int i=0; i<512; i++) // 시계방향 1회전
		{
			for(int j=0; j<8; j++)
			{
				stepmotor_drive(j);
				set_rpm(13);
			}
		}
		for(int i=0; i<512; i++) // 반시계방향 1회전
		{
			for(int j=7; j>=0; j--)
			{
				stepmotor_drive(j);
				set_rpm(13); // rpm값만큼 wait
			}
		}
	}
#endif
}

#if 1
//
// direction : 1 --> 시계방향
// 			 : 2 --> 반시계
//			   0 : idle
void stepmotor_drive(int direction)
{
	static int step=0; // static을 쓰면 전역변수 처럼 동작

	switch(step){
	case 0:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 1:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 4:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 5:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 1);
		break;
	case 6:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 1);
		break;
	case 7:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 1);
		break;
	}
	if (direction == 1) // 정회전
	{
		step++; // for (step=0; step<8; step++)
		step %= 8; // 다음 진행할 step 준비
		// if (step >= 8) step = 0;
	}
	else if(direction == 2) // 역회전
	{
		step--;
		if(step < 0) step = 7;
	}
}
#else
void stepmotor_drive(int step)
{
	switch(step){
	case 0:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 1:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 4:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 0);
		break;
	case 5:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 1);
		break;
	case 6:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 1);
		break;
	case 7:
		HAL_GPIO_WritePin(GPIOD, IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOD, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOD, IN4_Pin, 1);
		break;
	}
}
#endif

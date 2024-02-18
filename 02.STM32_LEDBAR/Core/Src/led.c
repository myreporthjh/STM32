#include "led.h" // < >

void led_main(void);
void led_all_on(void);
void led_all_off(void);
void led_on_down(void);
void led_on_up(void);
void led_keepon_up();
void led_keepon_down();
void flower_on();
void flower_off();

void led_main(void)
{
	while(1)
	{
		// [STM32] flower_on/off
		HAL_Delay(50); // 0에서 중앙 led킬때 딜레이
		flower_on();
		flower_off();

		// [STM32] led_keepon_up/down
		// 1. source 2. 작동 동영상
		// phase#4
		// 0->1->2->3->4->5->6->7
		// 앞전에 ON 했던 LED는 그대로 유지
//		led_keepon_up();
//		led_all_off();
//		// 7->6->5->4->3->2->1->0
//		// 앞전에 ON 했던 LED는 그대로 유지
//		led_keepon_down();
//		led_all_off();

		// phase#3
		// 0->1->2->3->4->5->6->7
		// 해당되는 bit의 LED만 ON
//		led_on_up();
//		// 7->6->5->4->3->2->1->0
//		led_on_down();


		// phase#2
//		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_All);
//		HAL_Delay(500);


		// phase#1
//		led_all_on();
//		HAL_Delay(300);
//		led_all_off();
//		HAL_Delay(300);
	}
}


// 0x80 0x40 0x20 0x10 0x08 0x04 0x02 0x01
// 중앙부터 led 누적 on
void flower_on()
{
	int delay_time = 100; // 100ms

	for(int i=0; i<4; i++)
	{
		HAL_Delay(delay_time); // 딜레이 100->400
		HAL_GPIO_WritePin(GPIOD, 0x10 << i, 1);
		HAL_GPIO_WritePin(GPIOD, 0x08 >> i, 1);
		delay_time += 100;
	}
}


// flower_on의 역순
void flower_off()
{
	int delay_time = 400; // 400ms

	for(int i=0; i<4; i++)
	{
		HAL_Delay(delay_time); // 딜레이 400->100
		led_all_off(); // led off시켜서 led off 표현

		// 중앙까지 led 줄여감 (비트연산자 활용)
		for(int j=i; j<4; j++)
		{
			HAL_GPIO_WritePin(GPIOD, 0x80 >> j, 1);
			HAL_GPIO_WritePin(GPIOD, 0x01 << j, 1);
		}
		delay_time -= 100;
	}

	HAL_Delay(50); // 중앙 두개 led 켜지면 딜레이 50주고 led off
	led_all_off();
}

void led_keepon_up()
{
	for(int i=0; i<8; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		HAL_Delay(200); // 200ms
	}
}

void led_keepon_down()
{
	for(int i=0; i<8; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		HAL_Delay(200); // 200ms
	}
}

// 7->6->5->4->3->2->1->0
void led_on_down()
{
	for(int i=0; i<8; i++)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		HAL_Delay(200); // 200ms
	}
}

// 0->1->2->3->4->5->6->7
// 해당되는 bit의 LED만 ON
void led_on_up()
{
	for(int i=0; i<8; i++)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		HAL_Delay(200); // 200ms
	}
}

void led_all_on(void)
{
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|
//			GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 1);
		HAL_GPIO_WritePin(GPIOD, 0xff, 1);
}

void led_all_off(void)
{
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|
//				GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0);
	HAL_GPIO_WritePin(GPIOD, 0xff, 0);
}

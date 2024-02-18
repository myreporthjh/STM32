#include "led.h" // < >
#include "button.h"

//#define LED_ON_UP 0
//#define LED_ON_DOWN 1

#define LED_ALL_ON 		0
#define LED_ALL_OFF 	1
#define FLOWER_ON 		2
#define FLOWER_OFF		3
#define LED_KEEPON_UP	4
#define LED_KEEPON_DOWN	5
#define LED_ON_UP		6
#define LED_ON_DOWN		7

void led_main(void);
void led_all_on(void);
void led_all_off(void);
void led_on_down(void);
void led_on_up(void);
void led_keepon_up();
void led_keepon_down();
void flower_on();
void flower_off();

extern volatile int t1ms_counter; // volatile : for disable optimize
int func_index=0;


void button0_led_all_on_off_toggle(void)
{
	static int button0_count = 0;		// static으로 선언하면 전역 변수 처럼 동작을 한다

	// 버튼을 한번 눌렀다 뗀 상태라면
	if(get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		button0_count++;
		button0_count %= 8; // 0~7
		if(button0_count == LED_ALL_ON)
		{
			led_all_on();
		}
		else if(button0_count == LED_ALL_OFF)
		{
			led_all_off();
		}
		else if(button0_count == FLOWER_ON)
		{
			flower_on();
		}
		else if(button0_count == FLOWER_OFF)
		{
			flower_off();
		}
		else if(button0_count == LED_KEEPON_UP)
		{
			led_keepon_up();
		}
		else if(button0_count == LED_KEEPON_DOWN)
		{
			led_keepon_down();
		}
		else if(button0_count == LED_ON_UP)
		{
			led_on_up();
		}
		else if(button0_count == LED_ON_DOWN)
		{
			led_on_down();
		}

	}
}

// 1: led_all_on()
// 2: led_all_off()
// 3: flower_on()
// 4: flower_off()
// 5: led_keepon_up
// 6: led_keepon_down
// 7: led_on_up
// 8: led_on_down
void button0_toggle()
{
	static int click_count = 0;

	if(get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		click_count++;

		if(click_count == 1)
		{
			led_all_on();
		}
		else if(click_count == 2)
		{
			led_all_off();
		}
		else if(click_count == 3)
		{
			flower_on();
		}
		else if(click_count == 4)
		{
			flower_off();
		}
		else if(click_count == 5)
		{
			led_keepon_up();
		}
		else if(click_count == 6)
		{
			led_keepon_down();
		}
		else if(click_count == 7)
		{
			led_on_up();
		}
		else if(click_count == 8)
		{
			led_on_down();
			click_count = 0;
		}
		else
		{
			led_all_off(); // 예상치못한오류
		}
	}

}


void led_main(void)
{
	while(1)
	{
#if 1
		switch(func_index)
		{
			case LED_ALL_ON:	// 0
				led_all_on();
				break;
			case LED_ALL_OFF:	// 1
				led_all_off();
				break;
			case FLOWER_ON:		// 2
				flower_on();
				break;
			case FLOWER_OFF:	// 3
				flower_off();
				break;
			case LED_KEEPON_UP:	// 4
				led_keepon_up();
				break;
			case LED_KEEPON_DOWN://5
				led_keepon_down();
				break;
			case LED_ON_UP:		// 6
				led_on_up();
				break;
			case LED_ON_DOWN:	// 7
				led_on_down();
				break;
			default:
				break;
		}
#endif

#if 0
		if(t1ms_counter >= 200) // 200ms
		{
			t1ms_counter = 0;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);	// LED1
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);	// LED2
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);	// LED3
		}
#endif

#if 0
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);	// LED1
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);	// LED2
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);	// LED3
		HAL_Delay(200);	// 200ms
#endif

//		//button0_led_all_on_off_toggle();
//		button0_toggle();

	}
}


// 0x80 0x40 0x20 0x10 0x08 0x04 0x02 0x01
// 중앙부터 led 누적 on
void flower_on()
{
	static int i=0;

	if(t1ms_counter > 200)
	{
		t1ms_counter = 0; // clear

		HAL_GPIO_WritePin(GPIOD, 0x10 << i, 1);
		HAL_GPIO_WritePin(GPIOD, 0x08 >> i, 1);
		i++;
		if(i >= 4)
		{
			i=0;
			func_index = FLOWER_OFF;
		}
	}

//	int delay_time = 100; // 100ms

//	for(int i=0; i<4; i++)
//	{
//		HAL_Delay(delay_time); // 딜레이 100->400
//		HAL_GPIO_WritePin(GPIOD, 0x10 << i, 1);
//		HAL_GPIO_WritePin(GPIOD, 0x08 >> i, 1);
//		delay_time += 100;
//	}

	//led_all_off();
}


// flower_on의 역순
void flower_off()
{
	static int i=0;

	if(t1ms_counter > 200)
	{
		t1ms_counter = 0; // clear

		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 0);
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 0);
		i++;
		if(i >= 4)
		{
			i=0;
			//func_index = LED_KEEPON_UP;
			led_all_off();
		}
	}

	//int delay_time = 400; // 400ms

	//led_all_on();
	//for(int i=0; i<4; i++)
	//{
//		HAL_Delay(delay_time); // 딜레이 400->100
	//	HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 0);
		//HAL_GPIO_WritePin(GPIOD, 0x01 << i, 0);

		//delay_time -= 100;
//	}

//	HAL_Delay(50); // 중앙 두개 led 켜지면 딜레이 50주고 led off
	//led_all_off();
}

void led_keepon_up()
{
	static int i=0;

	if(t1ms_counter > 200)
	{
		t1ms_counter = 0; // clear

		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		i++;
		if(i >= 8)
		{
			i=0;
			//func_index = LED_KEEPON_DOWN;
			led_all_off();
		}
	}


	//for(int i=0; i<8; i++)
	//{
	//	HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
	//	HAL_Delay(200); // 200ms
	//}
}

void led_keepon_down()
{
	static int i=0;

	if(t1ms_counter > 200)
	{
		t1ms_counter = 0; // clear

		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 0);
		i++;
		if(i >= 8)
		{
			i=0;
			//func_index = LED_ON_UP;
			led_all_off();
		}
	}


	//for(int i=0; i<8; i++)
	//{
	//	HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
	//	HAL_Delay(200); // 200ms
	//}
}

// 7->6->5->4->3->2->1->0
void led_on_down()
{
	static int i=0;
#if 1
	if(t1ms_counter > 200)
	{
		t1ms_counter = 0; // clear
		//led_all_off(); 함수호출X

		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		if(i>0)
				{
					HAL_GPIO_WritePin(GPIOD, 0x80 >> i-1, 0);
				}
		i++;
		if(i >= 8)
		{
			i=0;
			//func_index = LED_ALL_ON;
			led_all_off();
		}
	}
#else
	for(int i=0; i<8; i++)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		HAL_Delay(200); // 200ms
	}
	led_all_off();
#endif
}

// 0->1->2->3->4->5->6->7
// 해당되는 bit의 LED만 ON
void led_on_up()
{
	static int i =0;

#if 1
	if(t1ms_counter > 200)
	{
		t1ms_counter=0; // clear
		//led_all_off();

		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		if(i>0)
		{
			HAL_GPIO_WritePin(GPIOD, 0x01 << i-1, 0);
		}
		i++;
		if(i >= 8)
		{
			i=0;
			//func_index = LED_ON_DOWN;
			led_all_off();
		}
	}
#else // original
	for(int i=0; i<8; i++)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		HAL_Delay(200); // 200ms
	}
#endif
}

void led_all_on(void)
{
//	if(t1ms_counter > 200)
//	{
	//	t1ms_counter = 0; // clear
		HAL_GPIO_WritePin(GPIOD, 0xff, 1);
	//	func_index = LED_ALL_OFF;
	//}

//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|
//			GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 1);
//	HAL_GPIO_WritePin(GPIOD, 0xff, 1);
}

void led_all_off(void)
{
	if(t1ms_counter > 200)
	{
		t1ms_counter = 0; // clear
		HAL_GPIO_WritePin(GPIOD, 0xff, 0);
		func_index = FLOWER_ON;
	}
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|
//				GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0);
//	HAL_GPIO_WritePin(GPIOD, 0xff, 0);
}

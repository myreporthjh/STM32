#include "led.h" // < >
#include "button.h"

void led_main(void);
void led_all_on(void);
void led_all_off(void);
void led_on_down(void);
void led_on_up(void);
void led_keepon_up();
void led_keepon_down();
void flower_on();
void flower_off();

void button0_led_all_on_off_toggle(void)
{
	static int button0_count = 0;		// static으로 선언하면 전역 변수 처럼 동작을 한다

	// 버튼을 한번 눌렀다 뗀 상태라면
	if(get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		button0_count++;
		button0_count %= 2;
		if(button0_count)
		{
			led_all_on();
		}
		else
		{
			led_all_off();
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
		//button0_led_all_on_off_toggle();
		button0_toggle();
		printf("asdsa\n");
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
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 0);
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 0);

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
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 0);
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

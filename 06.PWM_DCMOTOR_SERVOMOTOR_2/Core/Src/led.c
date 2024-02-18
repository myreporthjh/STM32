#include "led.h"
#include "button.h"

#define LED_ON_UP 0
#define LED_ON_DOWN 1

#define debug

void led_main(void);
void led_all_on(void);
void led_all_off(void);
void led_on_up(void);
void led_on_down(void);
void led_keepon_up(void);
void led_keepon_down(void);
void led_flower_on(void);
void led_flower_off(void);

void button0_led_all_on_off_toggle(void);
void button0_toggle(void);
void Circular_function(void);
void One_button_function_call(void);

extern volatile int t1ms_counter;
int func_index = 0;
void(*fp[])(void) = {
		led_all_on, led_all_off, led_flower_on, led_flower_off,
		led_keepon_up, led_keepon_down, led_on_up, led_on_down
};

void led_main(void)
{
	while(1)
	{
		One_button_function_call();
//		Circular_function();

#if 0
		if (t1ms_counter >= 200)
		{
			t1ms_counter = 0;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // LED1
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7); // LED2
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14); // LED3
		}
#endif

#if 0
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // LED1
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7); // LED2
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14); // LED3
		HAL_Delay(200); // 200ms
#endif

#if 0
		// phase#6
		button0_toggle();
#endif

#if 0
		// phase#5
		led_flower_on();
		led_flower_off();
#endif

#if 0
		// phase#4
		led_keepon_up();
		led_all_off();
		led_keepon_down();
		led_all_off();
#endif

#if 0
		// phase#3
		switch (func_index)
		{
			case LED_ON_UP:
				led_on_up();
				break;

			case LED_ON_DOWN:
				led_on_down();
				break;

			default:
				break;
		}
#endif

#if 0
		// phase#2
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_ALL);
		HAL_Delay(500);
#endif

#if 0
		// phase#1
		led_all_on();
		HAL_Delay(300); // 아직 우리가 timer를 배우지 않았기 때문에, 그냥 HAL을 Sleep시키는 방법으로 딜레이를 준다.
		led_all_off();
		HAL_Delay(300);
#endif
	}
}

void Circular_function(void)
{
//	t1ms_counter = 0;
//	while (t1ms_counter < 400)
//	{
//	}
//
//	fp[func_index++]();
//	func_index %= 8;
}

void One_button_function_call(void)
{
	if (Get_Button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		func_index++;
		func_index %= 8;

#ifdef debug
	printf("current func_index: %d\n", func_index);
#endif

	}

	fp[func_index]();
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

void led_on_up(void)
{
	//	static int i = 0;
	int i = 0;
	t1ms_counter = 0;

	led_all_off();

	while(i < 8)
	{
		if (t1ms_counter >= 200)
		{
			t1ms_counter = 0;
			led_all_off();
			HAL_GPIO_WritePin(GPIOD, 0x01 << i++, 1);
		}
	}
}

void led_on_down(void)
{
	int i = 0;
	t1ms_counter = 0;

	led_all_off();

	while(i < 8)
	{
		if (t1ms_counter >= 200)
		{
			t1ms_counter = 0;
			led_all_off();
			HAL_GPIO_WritePin(GPIOD, 0x80 >> i++, 1);
		}
	}
}

void led_keepon_up(void)
{
	int i = 0;
	t1ms_counter = 0;

	led_all_off();

	while (i < 8)
	{
		if (t1ms_counter >= 200)
		{
			t1ms_counter = 0;
			HAL_GPIO_WritePin(GPIOD, 0x01 << i++, 1);
		}
	}
}

void led_keepon_down(void)
{
	int i = 0;
	t1ms_counter = 0;

	led_all_off();

	while (i < 8)
	{
		if (t1ms_counter >= 200)
		{
			t1ms_counter = 0;
			HAL_GPIO_WritePin(GPIOD, 0x80 >> i++, 1);
		}
	}
}

void led_flower_on(void)
{
	int i = 0, delay = 300;
	t1ms_counter = 0;

	led_all_off();

	while(i < 4)
	{
		if (t1ms_counter >= delay)
		{
			t1ms_counter = 0;
			HAL_GPIO_WritePin(GPIOD, 0x01 << (4 + i), 1);
			HAL_GPIO_WritePin(GPIOD, 0x01 << (3 - i), 1);
			i++;
			delay += 100;
		}
	}
}

void led_flower_off(void)
{
	int i = 0, delay = 700;
	t1ms_counter = 0;

	led_all_on();

	while(i < 4)
	{
		if (t1ms_counter >= delay)
		{
			t1ms_counter = 0;
			HAL_GPIO_WritePin(GPIOD, 0x01 << (0 + i), 0);
			HAL_GPIO_WritePin(GPIOD, 0x01 << (7 - i), 0);
			i++;
			delay -= 100;
		}
	}
}


#if 0

void led_on_up(void)
{
	static int i = 0;

	if (t1ms_counter >= 200)
	{
		t1ms_counter = 0;

		if (i == 8)
		{
			led_all_off();
			i = 0;
		}

		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x01 << i++, 1);
	}
}

void led_on_down(void)
{
	static int i = 0;

	if (t1ms_counter >= 200)
	{
		t1ms_counter = 0;

		if (i == 8)
		{
			led_all_off();
			i = 0;
		}

		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i++, 1);
	}
}

void led_keepon_up(void)
{
	static int i = 0;

	if (t1ms_counter >= 200)
	{
		t1ms_counter = 0;

		if (i == 8)
		{
			led_all_off();
			i = 0;
		}

		HAL_GPIO_WritePin(GPIOD, 0x00|(0x01 << i++), 1);
	}
}

void led_keepon_down(void)
{
	static int i = 0;

	if (t1ms_counter >= 200)
	{
		t1ms_counter = 0;

		if (i == 8)
		{
			led_all_off();
			i = 0;
		}

		HAL_GPIO_WritePin(GPIOD, 0x00|(0x80 >> i++), 1);
	}
}

void led_flower_on(void)
{
	static int i = 0;

	if (t1ms_counter >= 200)
	{
		t1ms_counter = 0;

		if (i == 4)
		{
			led_all_off();
			i = 0;
		}

		HAL_GPIO_WritePin(GPIOD, 0x08 >> i, 1);
		HAL_GPIO_WritePin(GPIOD, 0x10 << i, 1);
		i++;
	}
}

void led_flower_off(void)
{
	static int i = 0;

	if (t1ms_counter >= 200)
	{
		t1ms_counter = 0;

		if (i == 4)
		{
			led_all_off();
			i = 0;
		}

		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		i++;
	}
}

#endif

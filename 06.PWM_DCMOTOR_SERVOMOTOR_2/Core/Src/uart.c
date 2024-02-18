#include "uart.h"

// HW와 SW의 만나는 약속장소 = call back function
// 아래 함수는 Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c에서 잘라내서 붙여넣은 것이다.
// 그렇게 함으로써 이제 uart로 들어오는 rx 데이터는 모두 이 함수로 들어오게 된다.
// 결국 UART로 1byte가 수신되면 HW가 call을 해준다(MCU에 인터럽트가 들어온다).

// ComPort Master
// ledallon\n        -> 1바이트 단위로 콜백 펑션이 호출되기 때문에 l, e, d, a, l, l, o, n, \n 모두에 하나하나 콜백 펑션이 호출되는 것이다.
// ledalloff\n
// led_toggle\n

#define QUEUE_MAX 20
#define COMMAND_LENGTH 40

extern UART_HandleTypeDef huart3; // PC
extern UART_HandleTypeDef huart6; // BlueTooth
extern uint8_t rx_data; // extern from main.c
extern uint8_t bt_rx_data; // extern from main.c

extern void led_main(void);
extern void led_all_on(void);
extern void led_all_off(void);
extern void led_on_up(void);
extern void led_on_down(void);
extern void led_keepon_up(void);
extern void led_keepon_down(void);
extern void led_flower_on(void);
extern void led_flower_off(void);

extern int dht11time; // extern from DHT11.c
extern volatile int DHT11_print_flag; // extern from DHT11.c
extern volatile int ultrasonic_print_flag; // extern from ultrasonic.c

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void pc_command_processing(void);
void bt_command_processing(void);

#if 1
volatile unsigned char rx_buff[QUEUE_MAX][COMMAND_LENGTH]; // UART3으로부터 수신된 char를 저장하는 버퍼 공간(\n을 만날 때까지 저장하게 할 것이다)
volatile int queue_front = 0;
volatile int queue_rear = 0;
volatile int rx_index = 0; // rx_buff의 save위치
volatile int newline_detect_flag = 0; // \n을 만났다는 것을 알려주는 플래그변수..
// volatile로 선언하는 이유는 컴파일러가 필요 이상의 최적화를 하면서 현재 하드웨어를 초기화 하려는 나의 의도를 방해하는 상황을 방지하기 위함이다.
#else // original code
volatile unsigned char rx_buff[COMMAND_LENGTH]; // UART3으로부터 수신된 char를 저장하는 버퍼 공간(\n을 만날 때까지 저장하게 할 것이다)
volatile int rx_index = 0; // rx_buff의 save위치
volatile int newline_detect_flag = 0; // \n을 만났다는 것을 알려주는 플래그변수..
// volatile로 선언하는 이유는 컴파일러가 필요 이상의 최적화를 하면서 현재 하드웨어를 초기화 하려는 나의 의도를 방해하는 상황을 방지하기 위함이다.
#endif

volatile unsigned char bt_rx_buff[COMMAND_LENGTH]; // UART6으로부터 수신된 char를 저장하는 버퍼 공간(\n을 만날 때까지 저장하게 할 것이다)
volatile int bt_rx_index = 0; // bt_rx_buff의 save위치
volatile int bt_newline_detect_flag = 0; // \n을 만났다는 것을 알려주는 플래그변수..


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

#if 1

	if (huart == &huart3)
	{
		if ((queue_rear+1)%QUEUE_MAX != queue_front)
		{
			if (rx_index < COMMAND_LENGTH)
			{
				if (rx_data == '\n' || rx_data == '\r')
				{
					rx_buff[queue_rear][rx_index] = '\0';
					rx_index = 0;
					queue_rear++;
					queue_rear %= QUEUE_MAX;
				}
				else
				{
					rx_buff[queue_rear][rx_index++] = rx_data;
				}
			}
			else
			{
				rx_index = 0;
				printf("Message Overflow !!!\n");
			}
		}
		else
		{

		}

		// 주의: 반드시 HAL_UART_Receive_IT() 함수를 call 해줘야 다음 인터럽트가 이어서 발생이 가능해진다.
		HAL_UART_Receive_IT(&huart3, &rx_data, 1);
	}

#else
	if (huart == &huart3) // uart = ComPortMaster와 연결된 uart
	{
		if (rx_index < COMMAND_LENGTH) // 현재까지 들어온 byte가 40byte를 넘지 않으면
		{
			if(rx_data == '\n' || rx_data == '\r')
			{
				rx_buff[rx_index] = 0; // '\0'
				newline_detect_flag = 1;
				rx_index = 0; // 다음 메시지 저장을 위해서 rx_index값을 0으로 한다.
			}
			else
			{
				rx_buff[rx_index++] = rx_data;
			}
		}
		else
		{
			rx_index = 0;
			printf("Message Overflow !!!\n");
		}

		// 주의: 반드시 HAL_UART_Receive_IT() 함수를 call 해줘야 다음 인터럽트가 이어서 발생이 가능해진다.
		HAL_UART_Receive_IT(&huart3, &rx_data, 1);
	}
#endif

	if (huart == &huart6) // Bluetooth와 연결된 uart
	{
		if (bt_rx_index < COMMAND_LENGTH) // 현재까지 들어온 byte가 40byte를 넘지 않으면
		{
			if(bt_rx_data == '\n' || bt_rx_data == '\r')
			{
				bt_rx_buff[bt_rx_index] = 0; // '\0'
				bt_newline_detect_flag = 1;
				bt_rx_index = 0; // 다음 메시지 저장을 위해서 rx_index값을 0으로 한다.
			}
			else
			{
				bt_rx_buff[bt_rx_index++] = bt_rx_data;
			}
		}
		else
		{
			bt_rx_index = 0;
			printf("Message Overflow !!!\n");
		}

		// 주의: 반드시 HAL_UART_Receive_IT() 함수를 call 해줘야 다음 인터럽트가 이어서 발생이 가능해진다.
		HAL_UART_Receive_IT(&huart6, &bt_rx_data, 1);
	}
}

void pc_command_processing(void)
{
	if (queue_front != queue_rear) // 원형쿠로 구성한게 아니기 때무네, 뉴라인 플래그가 서 있을 때만 완전한 문장이 들어왔다고 판단하고 함수가 진해오디어야 하낟.
	{
		queue_front++;
		queue_front %= QUEUE_MAX;
//		newline_detect_flag = 0;

		printf("%s\n", rx_buff[queue_front - 1]);

		if (!strncmp(rx_buff[queue_front - 1], "led_all_on", strlen("led_all_on"))) // 느낌표가 들어갔다!! strcmp는 같으면 0을 리턴하기 때문에 if문에 진입하려먼 !not을 통해서 1(true)로 뒤집어 줘야 한다.
		{
			led_all_on();
			return;
		}
		if (!strncmp(rx_buff[queue_front - 1], "led_all_off", strlen("led_all_off")))
		{
			led_all_off();
			return;
		}
		if (!strncmp(rx_buff[queue_front - 1], "led_on_up", strlen("led_on_up")))
		{
			led_on_up();
			return;
		}
		if (!strncmp(rx_buff[queue_front - 1], "led_on_down", strlen("led_on_down")))
		{
			led_on_down();
			return;
		}
		if (!strncmp(rx_buff[queue_front - 1], "led_keepon_up", strlen("led_keepon_up")))
		{
			led_keepon_up();
			return;
		}
		if (!strncmp(rx_buff[queue_front - 1], "led_keepon_down", strlen("led_keepon_down")))
		{
			led_keepon_down();
			return;
		}
		if (!strncmp(rx_buff[queue_front - 1], "led_flower_on", strlen("led_flower_on")))
		{
			led_flower_on();
			return;
		}
		if (!strncmp(rx_buff[queue_front - 1], "led_flower_off", strlen("led_flower_off")))
		{
			led_flower_off();
			return;
		}


		if (!strncmp(rx_buff[queue_front - 1], "dht11time", strlen("dht11time")))
		{
			dht11time = atoi(rx_buff[queue_front - 1] + 9);
			return;
		}

		if (!strncmp(rx_buff[queue_front - 1], "dht11_on", strlen("dht11_on")))
		{
			DHT11_print_flag = 1;
			return;
		}
		if (!strncmp(rx_buff[queue_front - 1], "dht11_off", strlen("dht11_off")))
		{
			DHT11_print_flag = 0;
			return;
		}
		if (!strncmp(rx_buff[queue_front - 1], "ultra_on", strlen("ultra_on")))
		{
			ultrasonic_print_flag = 1;
			return;
		}
		if (!strncmp(rx_buff[queue_front - 1], "ultra_off", strlen("ultra_off")))
		{
			ultrasonic_print_flag = 0;
			return;
		}


	}
}

void bt_command_processing(void)
{
	if (bt_newline_detect_flag) // 원형쿠로 구성한게 아니기 때무네, 뉴라인 플래그가 서 있을 때만 완전한 문장이 들어왔다고 판단하고 함수가 진해오디어야 하낟.
	{
		bt_newline_detect_flag = 0;
		printf("%s\n", bt_rx_buff);
		if (!strncmp(bt_rx_buff, "ledallon", strlen("ledallon"))) // 느낌표가 들어갔다!! strcmp는 같으면 0을 리턴하기 때문에 if문에 진입하려먼 !not을 통해서 1(true)로 뒤집어 줘야 한다.
		{
			led_all_on();
			return;
		}
		if (!strncmp(bt_rx_buff, "ledalloff", strlen("ledalloff")))
		{
			led_all_off();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_on_up", strlen("led_on_up")))
		{
			led_on_up();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_on_down", strlen("led_on_down")))
		{
			led_on_down();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_keepon_up", strlen("led_keepon_up")))
		{
			led_keepon_up();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_keepon_down", strlen("led_keepon_down")))
		{
			led_keepon_down();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_flower_on", strlen("led_flower_on")))
		{
			led_flower_on();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_flower_off", strlen("led_flower_off")))
		{
			led_flower_off();
			return;
		}
	}
}

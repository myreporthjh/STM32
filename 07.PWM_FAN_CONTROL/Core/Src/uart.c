#include "uart.h"
#include <string.h> // strncmp, strcpy...
#include <stdlib.h>
#include <string.h>

void pc_command_processing(void);
void bt_command_processing(void);

extern int toggle_DHT;
extern int toggle_sonic;

extern void led_main(void);
extern void led_all_on(void);
extern void led_all_off(void);
extern void led_on_down(void);
extern void led_on_up(void);
extern void flower_on();
extern void flower_off();
extern void led_keepon_up();
extern void led_keepon_down();

extern uint8_t rx_data;		// uart3 rx data
extern uint8_t bt_rx_data;	// uart6 bt rx data
extern int dht11time; // 시간

extern UART_HandleTypeDef huart3; // PC
extern UART_HandleTypeDef huart6; // BT

// HW와 SW의 만나는 약속장소 : call back function
// move from HAL_UART_RxCpltCallback of stm32f4xx_hal_uart to here
// UART로 1 byte가 수신되면 HW가 call을 해줌
// UART RX INT가 발생이 되면 이곳으로 자동적으로 들어온다.
// 1. 숙제#1: 1차원 array 가지고는 overlfow가 발생되므로
// 			 2차원 array circular queue 를 구성하여 overflow가 발생되지 않도록 구현한다
// comport master
// ledallon\n
// ledalloff\n
// led_toggle\n


#if 1
#define COMMAND_SIZE 20 // queue 개수
#define COMMAND_LENGTH 40
volatile unsigned char rx_buff[COMMAND_SIZE][COMMAND_LENGTH]; // UART3으로부터 수신된 char를 저장하는 공간(\n을 만날때까지)
volatile int rx_count=0;			// rx_buff 개수
volatile int rx_index=0;			// rx_buff의 save 위치
volatile int rear=0, front=0;		// 원형큐 rear, front 0 초기화
volatile int newline_detect_flag=0;	// new line을 만났을때의 indicator ex) ledallon\n
#else
#define COMMAND_LENGTH 40
volatile unsigned char rx_buff[COMMAND_LENGTH]; // UART3으로부터 수신된 char를 저장하는 공간(\n을 만날때까지)
volatile int rx_index=0;						// rx_buff의 save 위치
volatile int newline_detect_flag=0;				// new line을 만났을때의 indicator ex) ledallon\n
#endif

volatile unsigned char bt_rx_buff[COMMAND_LENGTH]; 	// UART6으로부터 수신된 char를 저장하는 공간(\n을 만날때까지)
volatile int bt_rx_index=0;							// bt_rx_buff의 save 위치
volatile int bt_newline_detect_flag=0;				// new line을 만났을때의 indicator ex) ledallon\n

#if 0
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart3) // comport master와 연결된 uart
	{
		if(rx_index < COMMAND_LENGTH) // 현재까지 들어온 byte가 40byte를 넘지 않으면
		{
			if(rx_data == '\n' || rx_data == '\r') // newline or 캐리지리턴
			{
				rx_buff[rx_index] = 0;		// '\0'
				newline_detect_flag = 1; 	// new line을 만났다는 flag를 set함.
				rx_index = 0;				// 다음 message저장을 위해서 rx_index값을 0으로함
			}
			else
			{
				rx_buff[rx_index++] = rx_data;
			}
		}
		else // 사이즈 초과
		{
			rx_index = 0;
			printf("Message Overflow !!!!!\n");
		}
		// 주의: 반드시 HAL_UART_Receive_IT를 call해줘야 다음 INT가 발생된다.
		HAL_UART_Receive_IT(&huart3, &rx_data, 1);
	}

	if(huart == &huart6) // BT 와 연결된 uart
	{
		if(bt_rx_index < COMMAND_LENGTH) // 현재까지 들어온 byte가 40byte를 넘지 않으면
		{
			if(bt_rx_data == '\n' || bt_rx_data == '\r') // newline or 캐리지리턴
			{
				bt_rx_buff[bt_rx_index] = 0;		// '\0'
				bt_newline_detect_flag = 1; 	// new line을 만났다는 flag를 set함.
				bt_rx_index = 0;				// 다음 message저장을 위해서 rx_index값을 0으로함
			}
			else
			{
				bt_rx_buff[bt_rx_index++] = bt_rx_data;
			}
		}
		else
		{
			bt_rx_index = 0;
			printf("BT Message Overflow !!!!!\n");
		}
		// 주의: 반드시 HAL_UART_Receive_IT를 call해줘야 다음 INT가 발생된다.
		HAL_UART_Receive_IT(&huart6, &bt_rx_data, 1);
	}
}

void pc_command_processing(void)
{
	if(newline_detect_flag) // comport master로부터 완전한 문장이 들어오면 (\n을 만나면)
	{
		newline_detect_flag = 0;
		printf("%s\n", rx_buff);
		if(!strncmp(rx_buff, "led_all_on", strlen("led_all_on")))
		{
			led_all_on();
			return;
		}
		if(!strncmp(rx_buff, "led_all_off", strlen("led_all_off")))
		{
			led_all_off();
			return;

		}
		if(!strncmp(rx_buff, "led_on_down", strlen("led_on_down")))
		{
			led_on_down();
			return;

		}
		if(!strncmp(rx_buff, "led_on_up", strlen("led_on_up")))
		{
			led_on_up();
			return;

		}
		if(!strncmp(rx_buff, "flower_on", strlen("flower_on")))
		{
			flower_on();
			return;

		}
		if(!strncmp(rx_buff, "flower_off", strlen("flower_off")))
		{
			flower_off();
			return;
		}
		if(!strncmp(rx_buff, "led_keepon_up", strlen("led_keepon_up")))
		{
			led_keepon_up();
			return;
		}
		if(!strncmp(rx_buff, "led_keepon_down", strlen("led_keepon_down")))
		{
			led_keepon_down();
			return;
		}
	}
}

void bt_command_processing(void)
{
	if(bt_newline_detect_flag) // comport master로부터 완전한 문장이 들어오면 (\n을 만나면)
	{
		bt_newline_detect_flag = 0;
		printf("%s\n", bt_rx_buff);
		if(!strncmp(bt_rx_buff, "led_all_on", strlen("led_all_on")))
		{
			led_all_on();
			return;
		}
		if(!strncmp(bt_rx_buff, "led_all_off", strlen("led_all_off")))
		{
			led_all_off();
			return;

		}
		if(!strncmp(bt_rx_buff, "led_on_down", strlen("led_on_down")))
		{
			led_on_down();
			return;

		}
		if(!strncmp(bt_rx_buff, "led_on_up", strlen("led_on_up")))
		{
			led_on_up();
			return;

		}
		if(!strncmp(bt_rx_buff, "flower_on", strlen("flower_on")))
		{
			flower_on();
			return;

		}
		if(!strncmp(bt_rx_buff, "flower_off", strlen("flower_off")))
		{
			flower_off();
			return;
		}
		if(!strncmp(bt_rx_buff, "led_keepon_up", strlen("led_keepon_up")))
		{
			led_keepon_up();
			return;
		}
		if(!strncmp(bt_rx_buff, "led_keepon_down", strlen("led_keepon_down")))
		{
			led_keepon_down();
			return;
		}
	}
}

#else

void set_dht11time(char arr[])
{
	int num = atoi(arr+9);
	dht11time = num;
}

// 2차원배열 사용
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart3) // comport master와 연결된 uart
	{
		if(rx_index < COMMAND_LENGTH) // 현재까지 들어온 byte가 40byte를 넘지 않으면
		{
			if(rx_data == '\n' || rx_data == '\r') // newline or 캐리지리턴
			{
				rx_buff[rx_count][rx_index] = 0;		// '\0'
				newline_detect_flag = 1; 	// new line을 만났다는 flag를 set함.
				rx_index = 0;				// 다음 message저장을 위해서 rx_index값을 0으로함
				rx_count++;					// 문자열 개수 증가
			}
			else
			{
				rx_buff[rx_count][rx_index++] = rx_data;
			}
		}
		else // 사이즈 초과
		{
			rx_index = 0;
			printf("Message Overflow !!!!!\n");
		}
		// 주의: 반드시 HAL_UART_Receive_IT를 call해줘야 다음 INT가 발생된다.
		HAL_UART_Receive_IT(&huart3, &rx_data, 1);
	}
}

void pc_command_processing(void)
{
	if(newline_detect_flag) // comport master로부터 완전한 문장이 들어오면 (\n을 만나면)
	{
		newline_detect_flag = 0;

		printf("%s\n", rx_buff[rx_count-1]);

		if(!strncmp(rx_buff[rx_count-1], "led_all_on", strlen("led_all_on")))
		{
			led_all_on();
			return;
		}
		if(!strncmp(rx_buff[rx_count-1], "led_all_off", strlen("led_all_off")))
		{
			led_all_off();
			return;
		}
		if(!strncmp(rx_buff[rx_count-1], "led_on_down", strlen("led_on_down")))
		{
			led_on_down();
			return;
		}
		if(!strncmp(rx_buff[rx_count-1], "led_on_up", strlen("led_on_up")))
		{
			led_on_up();
			return;
		}
		if(!strncmp(rx_buff[rx_count-1], "flower_on", strlen("flower_on")))
		{
			flower_on();
			return;
		}
		if(!strncmp(rx_buff[rx_count-1], "flower_off", strlen("flower_off")))
		{
			flower_off();
			return;
		}
		if(!strncmp(rx_buff[rx_count-1], "led_keepon_up", strlen("led_keepon_up")))
		{
			led_keepon_up();
			return;
		}
		if(!strncmp(rx_buff[rx_count-1], "led_keepon_down", strlen("led_keepon_down")))
		{
			led_keepon_down();
			return;
		}

		// 출력 주기 설정(ms)
		if(!strncmp(rx_buff[rx_count-1], "dht11time150", strlen("dht11time150")))
		{
			set_dht11time(rx_buff[rx_count-1]);
			return;
		}
		if(!strncmp(rx_buff[rx_count-1], "dht11time200", strlen("dht11time200")))
		{
			set_dht11time(rx_buff[rx_count-1]);
			return;
		}
		if(!strncmp(rx_buff[rx_count-1], "dht11time300", strlen("dht11time300")))
		{
			set_dht11time(rx_buff[rx_count-1]);
			return;
		}

		// 과제2번
		if(!strncmp(rx_buff[rx_count-1], "dht11on", strlen("dht11on"))) //온습도출력
		{
			toggle_DHT=1;
			return;
		}
		if(!strncmp(rx_buff[rx_count-1], "dht11off", strlen("dht11off"))) //온습도출력종료
		{
			toggle_DHT=0;
			return;
		}
		if(!strncmp(rx_buff[rx_count-1], "ultra_on", strlen("ultra_on"))) //거리정보출력
		{
			toggle_sonic=1;
			return;
		}
		if(!strncmp(rx_buff[rx_count-1], "ultra_off", strlen("ultra_off"))) //거리정보출력종료
		{
			toggle_sonic=0;
			return;
		}

	}
}


#endif

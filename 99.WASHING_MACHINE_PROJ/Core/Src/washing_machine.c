// 세탁기
#include "main.h" // for GPIO & HAL Driver
#include "button.h"
#include "i2c_lcd.h"
#include "dcmotor.h"

extern RTC_HandleTypeDef hrtc;	// timer
extern TIM_HandleTypeDef htim4;
extern RTC_TimeTypeDef mTime; 	// time information
extern volatile int TIM10_10ms_counter; // counter
extern volatile int t1ms_wm_cnt; // washing machine counter

void washing_machine_on_off(void);	// on/off
void laundry_btn_event(void); 		// button event
void washing_machine_run(void); 	// run
void main_ui(void);

// 상태 flag
uint8_t on_off_flag = 0;	// on/off status					-BTN4-
uint8_t start_flag = 0; 	// start/stop						-BTN0-
uint8_t menu_flag = 0;		// menu on/off						-BTN1-

extern uint8_t CCR_UP_Value; // PWM UP control

// 물높이, 세탁, 헹굼, 탈수
char menu[][10] = {"", "water","washing","rinsing","drying"};

// 물높이 메뉴 - default:4
int water_menu = 3;
// 순서: 민감한 옷, 중간(표준:기본값), 강, 강력세탁(터보)
// 세탁 메뉴 - default: medium (하 중 강 터보)
char washing_menu[][10] = {"", "low", "medium", "high", "turbo"};
// 헹굼 메뉴 - default: medium (하 중 강 터보)
char rinsing_menu[][10] = {"", "low", "medium", "high", "turbo"};
// 탈수 메뉴 - default: medium (하 중 강 터보)
char drying_menu[][10] = {"", "low", "medium", "high", "turbo"};

// default: medium
int washing_select = 1;
int rinsing_select = 1;
int drying_select = 1;

// 시간저장을 위한 변수 처음한번만 실행하게끔 설정함
int save_time = 0;
// remain time 잔여시간
int remain_time = 0;
// 저장 시간
int count_time = 0;
// 진행 사항
int flow = 0;

void main_ui(void)
{
	i2c_lcd_init();

	move_cursor(0, 0);
	lcd_string("1.RunStop 2.Menu");
	move_cursor(1, 0);
	lcd_string("3.Setting 4.Init");
}

void washing_machine_on_off(void)
{
	char lcd_buff[40];

	if(get_button(GPIOC, GPIO_PIN_13, 4) == BUTTON_PRESS)
	{
		if(!on_off_flag) // OFF 상태일때 동작
		{
			main_ui();
			save_time = 0;
			on_off_flag=1; // 상태 ON으로
		}
		else // ON 상태일때 clear 후 OFF 상태로 변경
		{
			lcd_command(CLEAR_DISPLAY); // 화면 지우기

			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

			on_off_flag=0; // 상태 OFF로
			start_flag=0;  // STOP상태로
			menu_flag=0;   // 메뉴초기화
		}
	}
}

// ON일 상태에 버튼 이벤트 처리
void laundry_btn_event(void)
{
	static int menu_select = 0; 	// menu select
	static int remain_time_static = 0;
	char lcd_buff[40];

	// START/STOP BTN
	if(get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		i2c_lcd_init();
//		remain_time = water_menu + washing_select + rinsing_select + drying_select;
		if(!start_flag) // if STOP
		{
			// 처음 한번만 실행
			if(!save_time)
			{
				remain_time = water_menu + washing_select + rinsing_select + drying_select;
			}
			save_time++;

			move_cursor(0, 0);
			lcd_string("--run--");

			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);

			CCR_UP_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
			CCR_UP_Value = 100;

			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_UP_Value);

			flow = 0;

			start_flag = 1; // SET START STATUS
		}
		else // if START
		{
			move_cursor(0, 0);
			lcd_string("--pause--");
			move_cursor(1, 0);
			lcd_string("---------");

			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

			start_flag = 0; // SET STOP STATUS
		}
	}

	// MENU SELECT BTN
	if(get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
	{
		i2c_lcd_init();
		if(!start_flag) // STOP(일시중지) 상태일때
		{
			move_cursor(0, 0);
			lcd_string("--menu select--");

			menu_select %= 4; // 0~3
			menu_select++;

			sprintf(lcd_buff, "menu: %s", menu[menu_select]);
			move_cursor(1, 0);
			lcd_string(lcd_buff);

			menu_flag=1;

		}
		else
		{
			move_cursor(0, 0);
			lcd_string("Can't launch");
			move_cursor(1, 0);
			lcd_string("Pause system!");
		}
	}

	// In MENU - Setting Button
	if(get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
	{
		if(menu_flag) // MENU ON 상태일때
		{
			i2c_lcd_init();
			//lcd_command(CLEAR_DISPLAY);

			move_cursor(0, 0);
			sprintf(lcd_buff, "> menu - %s", menu[menu_select]);
			lcd_string(lcd_buff);
			move_cursor(1, 0);

			if(menu_select==1) // 물높이
			{
				water_menu %= 8; // 0~7
				water_menu++; 	 // default: 4

				sprintf(lcd_buff, "--> %d", water_menu);
				lcd_string(lcd_buff);

			}
			else if(menu_select==2) // 세탁
			{
				washing_select %= 4; // 0~3
				washing_select++;	 // 1~4

				sprintf(lcd_buff, "--> %s", washing_menu[washing_select]);
				lcd_string(lcd_buff);

			}
			else if(menu_select==3) // 헹굼
			{
				rinsing_select %= 4; // 0~3
				rinsing_select++;	 // 1~4

				sprintf(lcd_buff, "--> %s", rinsing_menu[rinsing_select]);
				lcd_string(lcd_buff);

			}
			else if(menu_select==4) // 탈수
			{
				drying_select %= 4; // 0~3
				drying_select++;	// 1~4

				sprintf(lcd_buff, "--> %s", drying_menu[drying_select]);
				lcd_string(lcd_buff);

			}
		}
	}

	// In MENU - Initial Button
	if(get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
	{
		i2c_lcd_init();
		move_cursor(0, 0);
		lcd_string("--Initial!!--");
		move_cursor(1, 0);
		lcd_string("Set Default!!");

		// setting to default
		menu_select = 0;
		water_menu = 3;
		washing_select = 1;
		rinsing_select = 1;
		drying_select = 1;

	}

}

void washing_machine_run(void)
{
	static int washing_time = 0;

	if(start_flag) // 시작버튼 눌렀을때
	{
		washing_time = washing_select;

		// 남은시간 계산
		if(t1ms_wm_cnt>1000)
		{
			char lcd_buff[40];
			int min = (remain_time-1)/60; 	// 분
			int sec = (remain_time-1)%60;	// 초

			move_cursor(1, 0);
			sprintf(lcd_buff, "remain %dm %ds", min , sec);
			lcd_string(lcd_buff);

			// washing time 보다 클동안 역회전
			if(remain_time>washing_time)
			{
				HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_8);
				HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);
			}

			remain_time--;
			t1ms_wm_cnt=0;
		}

		if(remain_time < 0)
		{
			// motor break
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);

			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 0);	// LED1
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);	// LED2
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);	// LED3

			// set off
			on_off_flag=0; // 상태 OFF로
			start_flag=0;  // STOP상태로
			menu_flag=0;   // 메뉴초기화

			i2c_lcd_init();
			move_cursor(0, 0);
			lcd_string("--End--");

			return;
		}

		if(TIM10_10ms_counter>100)
		{
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);	// LED1
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);	// LED2
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);	// LED3
			TIM10_10ms_counter = 0;
		}

	}
}



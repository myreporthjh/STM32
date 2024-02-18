#include "main.h"
#include "button.h"

// step motor
extern void set_rpm(int rpm);
extern void stepmotor_drive(int step);

// dot matrix
//extern void init_dotmatrix(void);
extern void init_dotmatrix(int num);
extern void print_LED(void);

// time, button event
extern void delay_us(unsigned long us);
extern int get_button(GPIO_TypeDef *GPIO, uint16_t GPIO_PIN, uint8_t button_number);

// main export
void elevator_main(void);
void elevator_move(int photo_num);

//unsigned char display_data2[8]; // 최종 8x8 출력할 데이터
//unsigned char scroll_buffer2[50][8] = {0}; // 스크롤할 데이터가 들어있는 버퍼
//int number_of_character2 = 11; // 출력할 문자 개수

int prev_floor = 999;
int floor = 998; // 현재 층
void elevator_move(int photo_num)
{
	// photo_num 값 매핑
	if(photo_num == 1){
		floor = 3;
	} else if(photo_num == 4){
		floor = 2;
	} else if(photo_num == 8){
		floor = 1;
	} else if(photo_num == 256){
		floor = 4;
	}
	printf("current floor : %d\n", floor);

	if(prev_floor == 999) {
		prev_floor = floor;
	}
}

// 엘베 동작
int up_status = 0;
int down_status = 0;
int stop_status = 1; // 멈춰있는 상태로 시작
void set_btn_status(int btn_num)
{
	if(prev_floor != 999) {
		if(prev_floor > btn_num) { // 위에서 아래로 이동
			printf("%d -> %d\n", prev_floor, btn_num);
			printf("DOWN!!!!!!!!!!\n");
			up_status=0;
			down_status=1;
			stop_status=0;
		} else if(prev_floor < btn_num) { // 아래에서 위로 이동
			printf("%d -> %d\n", prev_floor, btn_num);
			printf("UP!!!!!!!!!!!\n");
			up_status=1;
			down_status=0;
			stop_status=0;
		} else { // 같은 층
			printf("%d -> %d\n", prev_floor, btn_num);
			printf("Same floor!!!!!!!\n");
			up_status=0;
			down_status=0;
			stop_status=1;
		}

		prev_floor = btn_num; // 이전층 저장
	}
}


int floor_btn; // 버튼 층
int moving_status = 0; // 동작 상태 (1일때만 동작)
void elevator_main()
{
	//=== dot matrix ===
	//static int count = 0; // 컬럼 count
	//static int index = 0; // scroll_buffer의 2차원 index값
	//static uint32_t past_time=0; // 이전 tick값 저장

	//init_dotmatrix(1);
	//=================

	// step motor
	static int button0_count = 0;

	//while(1)
	//{

		//print_LED();

		if(stop_status) {
			// -------------------- 1층 --------------------
			if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS) { // BUTTON0
				floor_btn = 1;
				set_btn_status(floor_btn);
			}

			// -------------------- 2층 --------------------
			if (get_button(BUTTON0_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS) { // BUTTON1
				floor_btn = 2;
				set_btn_status(floor_btn);
			}

			// -------------------- 3층 --------------------
			if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS) { // BUTTON2
				floor_btn = 3;
				set_btn_status(floor_btn);
			}

			// -------------------- 4층 --------------------
			if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS) { // BUTTON3
				floor_btn = 4;
				set_btn_status(floor_btn);
			}
		}
		if(up_status) {
			init_dotmatrix(1);
		}
		if(down_status) {
			init_dotmatrix(2);
		}
		if(stop_status) {
			init_dotmatrix(3);
		}

		if(floor == floor_btn) { // 같은 층 도달시
			up_status=0;
			down_status=0;
			stop_status=1;
		}

		// 움직임 있을때
		if(!stop_status) {
			if(up_status) {
				stepmotor_drive(1); // 정회전
			}
			if(down_status) {
				stepmotor_drive(2); // 역회전
			}
			set_rpm(13);
		} else {
			set_rpm(0);
		}
	//}
}

#include "main.h"
#include "fnd4digit.h"
#include "button.h"

extern volatile int fnd1ms_counter;  // fnd1ms
extern uint8_t on_off_flag;
extern uint8_t start_flag;
extern int remain_time;
void fnd4digit_sec_clock(void);

uint16_t FND_digit[4] =
{
	FND_d1, FND_d10, FND_d100, FND_d1000
};

uint32_t FND_font[10] =
{
  FND_a|FND_b|FND_c|FND_d|FND_e|FND_f,   // 0
  FND_b|FND_c,                           // 1
  FND_a|FND_b|FND_d|FND_e|FND_g,         // 2
  FND_a|FND_b|FND_c|FND_d|FND_g,         // 3
  FND_b|FND_c|FND_f|FND_g,   // 4
  FND_a|FND_c|FND_d|FND_f|FND_g,  // 5
  FND_a|FND_c|FND_d|FND_e|FND_f|FND_g,  // 6
  FND_a|FND_b|FND_c,      // 7
  FND_a|FND_b|FND_c|FND_d|FND_e|FND_f|FND_g,   // 8
  FND_a|FND_b|FND_c|FND_d|FND_f|FND_g   // 9
};


uint16_t FND[4];    // FND에 쓰기 위한 값을 준비하는 변수

void FND_update(unsigned int value)
{
	FND[0] = FND_font[value % 10];
	FND[1] = FND_font[value / 10 % 10];
	FND[2] = FND_font[value / 100 % 10];
	FND[3] = FND_font[value / 1000 % 10];

	return;
}
void FND4digit_off(void)
{
 // common 캐소우드 CL5642AH30
	HAL_GPIO_WritePin(FND_COM_PORT, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);
	HAL_GPIO_WritePin(FND_DATA_PORT,FND_font[8]|FND_p, GPIO_PIN_RESET);
	return;
}
void FND4digit_on(void)
{
	// CL5642AH30
	HAL_GPIO_WritePin(FND_COM_PORT, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(FND_DATA_PORT,FND_font[8]|FND_p, GPIO_PIN_SET);
	return;
}


void fnd4digit_sec_clock(void)
{
   static unsigned int value=0;   // 1초가 되었을때 up count
   static unsigned int msec=0;    // ms counter
   static unsigned int cycle=0;		// 빙글빙글 효과
   static int i=0;     // FND position indicator

   if(on_off_flag) // ON 상태일때만
   {

	   if (fnd1ms_counter >= 2)   // 2ms reached
	   {
		   fnd1ms_counter=0;
		   msec += 2;   // 2ms

		   if(start_flag) //동작상태일때
		   {
			   value=remain_time;
			   FND_update(value);
			   FND4digit_off();

			   HAL_GPIO_WritePin(FND_COM_PORT,FND_digit[i], GPIO_PIN_RESET);
			   HAL_GPIO_WritePin(FND_DATA_PORT, FND[i], GPIO_PIN_SET);

			   i++;   // 다음 display할 FND를 가리킨다.
			   i %= 4;
		   }
      }
   }
   else // OFF
   {
	   FND4digit_off();
   }

}

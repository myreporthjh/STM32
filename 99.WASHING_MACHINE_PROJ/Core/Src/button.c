#include "button.h"
#include "i2c_lcd.h"

// 초기버튼상태 table
char button_status[BUTTON_NUMBER] =
{BUTTON_RELEASE, BUTTON_RELEASE, BUTTON_RELEASE, BUTTON_RELEASE, BUTTON_RELEASE};
uint8_t lcd_display_mode_flag = 0;

extern RTC_HandleTypeDef hrtc;
RTC_TimeTypeDef mTime; // time information

extern uint8_t menu_flag;


// get_button(gpio, pin, button 번호)
// 완전히 눌렀다 떼면 BUTTON_RELEASE(1)을 리턴
int get_button(GPIO_TypeDef *GPIO, uint16_t GPIO_PIN, uint8_t button_number)
{
	unsigned char curr_state;

	curr_state = HAL_GPIO_ReadPin(GPIO, GPIO_PIN);	// 0, 1
	// 버튼이 눌려졌으나 처음상태
	if(curr_state == BUTTON_PRESS && button_status[button_number] == BUTTON_RELEASE)
	{
		HAL_Delay(80); // noise가 지나가기를 기다림
		button_status[button_number] = BUTTON_PRESS; // 현재의 버튼 상태를 저장
		return BUTTON_RELEASE; // 버튼이 눌려진 상태이나 아직은 noise 상태로 인정
	}
	else if(curr_state == BUTTON_RELEASE && button_status[button_number] == BUTTON_PRESS)
	{													// 1. 이전에 버튼이 눌려진 상태였고 지금은 버튼을 뗀 상태이면
		button_status[button_number] = BUTTON_RELEASE; // 2. button_status table을 초기화 하고
		HAL_Delay(30);
		return BUTTON_PRESS; 							// 3. 버튼을 1번 눌렀다 뗸것으로 인정함
	}
	return BUTTON_RELEASE;
}

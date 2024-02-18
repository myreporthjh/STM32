#include "button.h"

// 버튼 4개의 초기 상태를 초기화 해주는 코드. 항상 S/W프로그래밍의 시작은 "초기화"이다.
// 또한 이 배열은 각 버튼의 상태를 저장하는 역할도 수행한다.
char button_status[BUTTON_NUMBER] = {
		BUTTON_RELEASE, BUTTON_RELEASE, BUTTON_RELEASE, BUTTON_RELEASE
};

/*
 * desc: 사용자가 한번 버튼을 누를때마다(떼는 행동 포함) led의 on/off상태를 바꾸는 것이 목표
 * return: 완전히 눌렀다 떼면 BUTTON_RELEASE(1) 상태를 리턴
 * */
int Get_Button(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t button_number)
{
	unsigned char curr_state;
	curr_state = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin); // curr_state에는 0 또는 1의 상태가 걸리게 됨

	// 원치않는 노이즈를 디바운스? 채터링? 하는 if문 코드
	if (curr_state == BUTTON_PRESS && button_status[button_number] == BUTTON_RELEASE) // 처음 노이즈가 발생하는 상황을 체크
	{
		HAL_Delay(100); // 노이즈가 지나가기를 기다리는 것
		button_status[button_number] = BUTTON_PRESS;
		return BUTTON_RELEASE; // 버튼이 눌려진 상태이나 아직은 noise상태로 인정
	}
	else if (curr_state == BUTTON_RELEASE && button_status[button_number] == BUTTON_PRESS) // 이전에 버튼이 눌려진 상태였고, 지금은 버튼을 뗀 상태이면
	{
		button_status[button_number] = BUTTON_RELEASE; // button_status_table을 초기화 하고
		HAL_Delay(30);
		return BUTTON_PRESS; // 버튼을 한번 눌렀다 뗀 것으로 인정
	}
	return BUTTON_RELEASE;
}

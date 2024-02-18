#include "main.h" // HAL 드라이버 & GPIO 정보가 모두 main.h에 있기 때문에, 이걸 가장 먼저 인클루드

#define BUTTON_PRESS 0 // 버튼을 누른 상태를 0으로 본다. => 이를 active low라고 한다.
#define BUTTON_RELEASE 1 // 브레드보드 회로 기판 구성을 pull-up으로 했기 때문에, 뗀 상태에 전류가 흐르게 된다.
#define BUTTON_NUMBER 4

int Get_Button(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t button_number);

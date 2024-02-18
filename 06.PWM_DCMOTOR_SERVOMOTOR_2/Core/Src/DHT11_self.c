#if 0

#include "main.h"
#include "DHT11_self.h"

void DHT11_processing(void);
void DHT11_init(void);
void DHT11_trigger_HandShake_firstStep(void);
void DHT11_dumiRead_HandShake_secondStep(void);
void DHT11_GPIO_InputSetting_init(void);
void DHT11_GPIO_OnputSetting_init(void);
uint8_t DHT11_rx_Data(void);

void DHT11_processing(void)
{
	uint8_t RH_i, RH_d, Tmp_i, Tmp_d;

	DHT11_trigger_HandShake_firstStep();
	DHT11_dumiRead_HandShake_secondStep();

	DHT11_GPIO_InputSetting_init();

	///////////////////////////////////////////////////
	// 값을 읽어오는 함수의 핵심 과정은 수업에서 교수님께서 다루어주심
	// 핵심은 ReadPin을 해오는데
	// DHT11의 데이터 시트에서 정의하는 0과 1 각각의 High 전위 인가시간의 중앙 값 정도를 기다린 뒤
	// 바로 이어서 측정한 전위가 Low면 0으로 간주하고, High면 1롤 간주하는 것이었음
	///////////////////////////////////////////////////

	DHT11_GPIO_OnputSetting_init();
}

void DHT11_init(void)
{
	HAL_GPIO_WritePin(DHT11_GPIO_Port, GPIO_PIN_0, 1); // 풀업저항이기 때문에 normal high
	HAL_Delay(2000); // 초기화 하고 나서 일정 시간동안 high신호를 유지해야 첫 active low신호 받았을 때 데이터가 의미를 가짐
}

void DHT11_trigger_HandShake_firstStep(void)
{
	HAL_GPIO_WritePin(DHT11_GPIO_Port, GPIO_PIN_0, 0); // active low이기 때문에 low 신호를 일정시간 날리는 것으로 handshake
	delay_us(19000); // active signal이 18ms 이상이어야 start signal로 간주
	HAL_GPIO_WritePin(DHT11_GPIO_Port, GPIO_PIN_0, 1); // 다시 normal high로 복귀
	delay_us(10); // 다음 신호를 위해 약간의 대기시간 설정
}


void DHT11_GPIO_InputSetting_init(void)
{
	/*
	typedef struct
	{
	  uint32_t Pin; // GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_15 와 같 GPIO 핀이 지정됨
	  uint32_t Mode; // GPIO_MODE_INPUT, GPIO_MODE_OUTPUT_PP 와 같이 모드 설정 값이 들어감
	  uint32_t Pull; // Pull-up / Pull-down 설정 값이 들어감
	  uint32_t Speed; // GND 에서 Vdd 까지 전환되는 속도 설정 값
	} GPIO_InitTypeDef;
	*/
	GPIO_InitTypeDef GPIO_initStruct = {0};

	GPIO_initStruct.Pin = GPIO_PIN_0;
	GPIO_initStruct.Mode = GPIO_MODE_INPUT; /*!< Input Floating Mode                   */
	/*
	GPIO_MODE_INPUT 인 경우,
	- Output buffer 비활성화
	- 슈미트 트리거 활성화
	- Pull 변수 설정값에 따라서 풀업/풀다운 저항 활성화
	- Input data register 를 통해서 I/O 핀 상태 Read
	 */
	GPIO_initStruct.Pull = GPIO_NOPULL; // 풀업저항이 DHT11에 회로적으로 이미 구현되어 있기 때문에 GPIO셋팅에서 따로 풀업/풀다운 셋팅은 안하는듯?
	GPIO_initStruct.Speed = GPIO_SPEED_FREQ_LOW; // /*!< IO works at 2 MHz, please refer to the product datasheet */
	// 여기서 속도란 스위칭 속도를 의미하는 것이 아니라, 0~Vdd 까지 전압이 올라가는 속도의 빠르기를 의미함. (Slew Rate)
	// 근데 DHT11 데이터 시트에서 위에 해당하는 내용을 못찾았음..

	HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_initStruct); // DHT11과 연결된 GPIO 포트를 위 셋팅값으로 설정해준다.
}

void DHT11_GPIO_OnputSetting_init(void)
{
	GPIO_InitTypeDef GPIO_initStruct = {0};

	GPIO_initStruct.Pin = GPIO_PIN_0;
	GPIO_initStruct.Mode = GPIO_MODE_OUTPUT_PP; /*!< Output Push Pull Mode                 */
	/*
	GPIO_MODE_OUTPUT_PP 인 경우,
	- P-MOS 가 비활성화 되어 핀은 Hi-Z 상태가 됨 따라서 별도 외부에 풀업 회로 등 구성 필요
	- 슈미트 트리거 활성화
	- Pull 변수 설정값에 따라서 풀업/풀다운 저항 활성화
	- Input data register 를 통해서 I/O 핀 상태 Read
	- Output data register 를 통해서 마지막 제어값 Read
	- Output 레지스터가 0 인 경우 N-MOS 가 활성화 되고 1인 경우 P-MOS 가 활성화 되어 외부 회로 구성 필요 없이 상태를 구분할 수 있음
	 */
	GPIO_initStruct.Pull = GPIO_NOPULL;
	GPIO_initStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_initStruct); // DHT11과 연결된 GPIO 포트를 위 셋팅값으로 설정해준다.
}

uint8_t DHT11_rx_Data(void) // 이 함수가 데이터시트를 코드로 옮기는 핵심 포인트 함수이다. 0과 1을 판별하는 법..
{
	uint8_t rx_data = 0;

	for(int i = 0; i < 8; i++) // 8비트 받아야 하니까 for문을 8번 돌린다.
	{
		//when Input Data == 0
		while( 0 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
		delay_us(40); // 0인지 1인지 구분해야 하니까 둘의 중간 값 정도인 40마이크로 세컨드 만큼 딜레이를 주고 그 다음에 오는 if문을 통해 0인지 1인지 판단하겠다는 것이다. (0이면 26마이크로 세컨드만 유지되고, 1이면 70마이크로 세컨트가 유지되기 때문)

		rx_data<<=1; // 한 비트를 좌로 쉬프트 시킨다. 총 8비트를 쌓아가야 하니까...

		//when Input Data == 1
		if(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN))
		{
			rx_data |= 1;
			// 단 하나의 비트만 1로 바꿔주는 것이기 때문에 그냥 상수 대입 시키는 식으로 하면 안된다.
			// 이렇게 00000000000000000000000000000001 값과 OR비트 연산을 해주면
			// rx_data는 8비트 이기 때문에 00000001과 OR 연산되어 끝자리 비트만 0에서 1로 바뀌는 효과가 발생하게 된
		}
		while( 1 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
	}
	return rx_data;
}

void DHT11_dumiRead_HandShake_secondStep(void)
{

}

#endif

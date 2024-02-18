/*
 * i2c_lcd.c
 *
 *  Created on: 2019. 9. 4.
 *      Author: k
 */
#include "main.h"
#include "i2c_lcd.h"  // < >

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart3;

void i2c_lcd_main(void);

void i2c_lcd_main(void)
{

	uint8_t value=0;

#if 0 // 과제용 전처리문.. 과제 제출 후 비활성화 할것

	unsigned char i2c_test[] = {'5', 0};

	while(1)
	{
		while(HAL_I2C_Master_Transmit(&hi2c1, I2C_LCD_ADDRESS,
				i2c_test, 1, 100)!=HAL_OK){
			//HAL_Delay(1);
		}
		HAL_Delay(200);
	}

#else

 	i2c_lcd_init();

	while(1)
	{
		move_cursor(0,0);
		lcd_string("Hello World!!!"); // lcd_data() 함수가 0x20 이상의 ASCII값만 LCD에 출력할 수 있기 때문에, 그보다 작은 \n(0x0A)을 여기에 넣으면 출력겨로가가 깨지게 된다.
		move_cursor(1,0);
		lcd_data(value + '0');
		value++;
		if(value>9)value=0;
		HAL_Delay(500); // I2C의 통신 프로토콜을 통해서 정보가 왔다갔다 하고 있기 때문에 그것이 이루어지기 위한 최소한의 시간을 LCD단에서 보장해야 한다.(만약 이 딜레이가 없으면 I2C가 죽어버린다.)
	}

#endif

}

void lcd_command(uint8_t command)
{

	uint8_t high_nibble, low_nibble;
	uint8_t i2c_buffer[4];
	high_nibble = command & 0xf0;
	low_nibble = (command<<4) & 0xf0;
	i2c_buffer[0] = high_nibble | 0x04 | 0x08; //en=1, rs=0, rw=0, backlight=1
	i2c_buffer[1] = high_nibble | 0x00 | 0x08; //en=0, rs=0, rw=0, backlight=1
	i2c_buffer[2] = low_nibble  | 0x04 | 0x08; //en=1, rs=0, rw=0, backlight=1
	i2c_buffer[3] = low_nibble  | 0x00 | 0x08; //en=0, rs=0, rw=0, backlight=1
	while(HAL_I2C_Master_Transmit(&hi2c1, I2C_LCD_ADDRESS,
			i2c_buffer, 4, 100)!=HAL_OK){
		//HAL_Delay(1);
	}
	return;
}

// 1 byte write... 딱 한개의 캐릭터만 찍는 것이다.
void lcd_data(uint8_t data)
{

	uint8_t high_nibble, low_nibble;
	uint8_t i2c_buffer[4];
	high_nibble = data & 0xf0;
	low_nibble = (data<<4) & 0xf0;
	i2c_buffer[0] = high_nibble | 0x05 | 0x08; //en=1, rs=1, rw=0, backlight=1
	i2c_buffer[1] = high_nibble | 0x01 | 0x08; //en=0, rs=1, rw=0, backlight=1
	i2c_buffer[2] = low_nibble  | 0x05 | 0x08; //en=1, rs=1, rw=0, backlight=1
	i2c_buffer[3] = low_nibble  | 0x01 | 0x08; //en=0, rs=1, rw=0, backlight=1
	while(HAL_I2C_Master_Transmit(&hi2c1, I2C_LCD_ADDRESS,
			i2c_buffer, 4, 100)!=HAL_OK){
		//HAL_Delay(1);
	}
	return;
}

// lcd 초기화
void i2c_lcd_init(void)
{

	lcd_command(0x33);
	lcd_command(0x32);
	lcd_command(0x28);	//Function Set 4-bit mode
	lcd_command(DISPLAY_ON);
	lcd_command(0x06);	//Entry mode set
	lcd_command(CLEAR_DISPLAY);
	HAL_Delay(2);
}

// null을 만날때 까지 string을 LCD에 출력
void lcd_string(uint8_t *str)
{
	while(*str) // 널문자를 만날 때까지
	{
		lcd_data(*str++);
	}
}

// 해당 줄,col으로 이동 하는 함수
void move_cursor(uint8_t row, uint8_t column)
{
	lcd_command(0x80 | row<<6 | column);
	return;
}












#include "main.h"

extern void lcd_string(uint8_t *str);
extern void move_cursor(uint8_t row, uint8_t column);

int dotmatrix_main_test(void);
int dotmatrix_main();
void init_dotmatrix(void);

GPIO_TypeDef *col_port[] =
{
		COL1_GPIO_Port, COL2_GPIO_Port, COL3_GPIO_Port, COL4_GPIO_Port,
		COL5_GPIO_Port, COL6_GPIO_Port, COL7_GPIO_Port, COL8_GPIO_Port

};
GPIO_TypeDef *row_port[] =
{
		ROW1_GPIO_Port, ROW2_GPIO_Port, ROW3_GPIO_Port, ROW4_GPIO_Port,
		ROW5_GPIO_Port, ROW6_GPIO_Port, ROW7_GPIO_Port, ROW8_GPIO_Port

};

uint16_t row_pin[] =
{
		ROW1_Pin, ROW2_Pin, ROW3_Pin, ROW4_Pin,
		ROW5_Pin, ROW6_Pin, ROW7_Pin, ROW8_Pin
};

uint16_t col_pin[] =
{
		COL1_Pin, COL2_Pin, COL3_Pin, COL4_Pin,
		COL5_Pin, COL6_Pin, COL7_Pin, COL8_Pin
};



unsigned char all_on[] = { // all on 臾몄옄 ?뺤쓽
#if 0

	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111
#else
	0b00000000,
	0b11100111,
	0b10111101,
	0b10000001,
	0b01000010,
	0b01100010,
	0b00110100,
	0b00011000
#endif
};

const uint8_t blank[8] = {
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000
};

uint8_t number_data[20][10] =
{
	{
		0b01110000,	//0
		0b10001000,
		0b10011000,
		0b10101000,
		0b11001000,
		0b10001000,
		0b01110000,
	    0b00000000
	},
	{
		0b01000000,	//1
		0b11000000,
		0b01000000,
		0b01000000,
		0b01000000,
		0b01000000,
		0b11100000,
	    6   // ??0b00000110
	},
	{
		0b01110000,	//2
		0b10001000,
		0b00001000,
		0b00010000,
		0b00100000,
		0b01000000,
		0b11111000,
	    6
	},
	{
		0b11111000,	//3
	    0b00010000,
		0b00100000,
		0b00010000,
		0b00001000,
		0b10001000,
		0b01110000,
	    6
	},
	{
		0b00010000,	//4
		0b00110000,
		0b01010000,
		0b10010000,
		0b11111000,
		0b00010000,
		0b00010000,
	    6
	},
	{
		0b11111000,	//5
		0b10000000,
		0b11110000,
		0b00001000,
		0b00001000,
		0b10001000,
		0b01110000,
	    6
	},
	{
		0b00110000,	//6
		0b01000000,
		0b10000000,
		0b11110000,
		0b10001000,
		0b10001000,
		0b01110000,
	    6
	},
	{
		0b11111000,	//7
		0b10001000,
		0b00001000,
		0b00010000,
		0b00100000,
		0b00100000,
		0b00100000,
	    6
	},
	{
		0b01110000,	//8
		0b10001000,
		0b10001000,
		0b01110000,
		0b10001000,
		0b10001000,
		0b01110000,
	    6
	},
	{
		0b01110000,	//9
		0b10001000,
		0b10001000,
		0b01111000,
		0b00001000,
		0b00010000,
		0b01100000,
	    6
	},
	{
		0b00000000,    // hart
		0b01100110,
		0b11111111,
		0b11111111,
		0b11111111,
		0b01111110,
		0b00111100,
		0b00011000
	},
	{
		0b00000000,    // E
	    0b00111100,
		0b00100000,
		0b00111100,
		0b00100000,
		0b00100000,
		0b00111100,
		0b00000000
	},
	{
		0b00000000,    // Y
		0b01000010,
		0b00100100,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00011000,
		0b00000000
	},
	{
		0b00000000,    // J
		0b01111110,
		0b00011000,
		0b00011000,
		0b00011000,
		0b01011000,
		0b00111000,
		0b00000000
	}
};

unsigned char display_data[8];    // 理쒖쥌 8*8 異쒕젰???곗씠??
unsigned char scroll_buffer[50][8] = {0};    // ?ㅽ겕濡ㅽ븷 ?곗씠?곌? ?ㅼ뼱?덈뒗 踰꾪띁
int number_of_character = 15;    // 異쒕젰??臾몄옄 媛?닔

// 珥덇린???묒뾽
// 1. display_data ??number_data[0]???덈뒗 ?댁슜 蹂듭궗
// 2. number_data瑜?scroll_buffer??蹂듭궗
// 3. dotmatrix??led瑜?off
void init_dotmatrix(void)
{
	for (int i=0; i<14; i++)
	{
		display_data[i] = number_data[i];
	}
	for (int i=1; i<number_of_character+1; i++)
	{
		for (int j=0; j<14; j++)    // scroll_buffer[0] = blank
		{
			scroll_buffer[i][j] = number_data[i-1][j];
		}
	}
	for (int i=0; i<14; i++)
	{
		HAL_GPIO_WritePin(col_port[i], col_pin[i], 1);    // 罹먯냼?곕뱶 諛⑹떇? 諛섎?濡?
	}
}
void write_column_data(int col)
{
	for (int i = 0; i < 8; i++)
	{
#if 1
// -----common ?좊끂?곕뱶 諛⑹떇
		if(i == col)
			HAL_GPIO_WritePin(col_port[i], col_pin[i], 0); // on
		else HAL_GPIO_WritePin(col_port[i], col_pin[i], 1); // off
#else
// -----common 罹먯냼?곕뱶 諛⑹떇
		if(i == col)
					HAL_GPIO_WritePin(col_port[i], col_pin[i], 1); // off
		else HAL_GPIO_WritePin(col_port[i], col_pin[i], 0); // on
#endif
	}
}


int dotmatrix_main(void)
{
	static int count=0;    // colum count
	static int index=0;    // scrol_buffer??2李⑥썝 index媛?
	static uint32_t past_time=0;

//	init_dotmatrix();

	uint32_t now = HAL_GetTick();    // 1ms tick?대씪??寃껋? time 媛?
	// 泥섏쓬 ?쒖옉 ??past_time=0;    now: 500 --> past_time=500
	if (now - past_time >= 500)    // 500ms 留덈떎 scroll
	{
		sprintf(lcd_buff, "now: %d", now);

	}
	for (int i=0; i < 8; i++)
	{
		// 怨듯넻 ?묎레 諛⑹떇
		// column?먮뒗 0??ROW?먮뒗 1??異쒕젰?댁빞 ?대떦 LED媛 on?쒕떎.
		write_column_data(i);
		write_row_data(display_data[i]);
		HAL_Delay(1);
	}
	return 0; // ?먮윭 ?놁씠 ?뺤긽?곸쑝濡??앸궗??
}

// 0b00111100瑜?李띾뒗?ㅺ퀬 移⑹떆??
void write_row_data(unsigned char data)
{
	unsigned char d; // ?꾩떆 蹂??

	d = data;

	for(int i = 0; i < 8; i++)
	{
		if ( d & (1 << i)) // 00000001??寃쎌슦 -> ??移몄뵫 ?놁쑝濡??대룞?섍퀬 &?곗궛
			HAL_GPIO_WritePin(row_port[i], row_pin[i], 1);  // 10?쇰줈 2硫??ш린濡?
		else
			HAL_GPIO_WritePin(row_port[i], row_pin[i], 0); // off
	}
}

#if 1
// scroll 臾몄옄 異쒕젰 ?꾨줈洹몃옩
int dotmatrix_main_test(void)
{
	static int count=0;    // colum count
	static int index=0;    // scrol_buffer??2李⑥썝 index媛?
	static uint32_t past_time=0;

//	init_dotmatrix();

	uint32_t now = HAL_GetTick();    // 1ms tick?대씪??寃껋? time 媛?
	// 泥섏쓬 ?쒖옉 ??past_time=0;    now: 500 --> past_time=500
	if (now - past_time >= 500)    // 500ms 留덈떎 scroll
	{
		past_time = now;
		for (int i=0; i<8; i++)
		{
			display_data[i] = (scroll_buffer[index][i] >> count) |
					(scroll_buffer[index+1][i] << 8 - count);
		}
		if (++count == 8)    // 8移쇰엺????泥섎━?덉쑝硫??ㅼ쓬 scroll_buffer濡??대룞
		{
			count=0;
			index++;   // ?ㅼ쓬 scroll_buffer濡??대룞
			if (index == number_of_character+1) index=0;
			// 11媛쒖쓽 臾몄옄瑜???泥섎━?덉쑝硫?0踰?scroll_buffer 泥섎━?섍린 ?꾪빐 ?대룞
		}
	}
	for (int i=0; i < 8; i++)
	{
		// 怨듯넻 ?묎레 諛⑹떇
		// column?먮뒗 0??ROW?먮뒗 1??異쒕젰?댁빞 ?대떦 LED媛 on?쒕떎.
		write_column_data(i);
		write_row_data(display_data[i]);
		HAL_Delay(1);
	}
	return 0; // ?먮윭 ?놁씠 ?뺤긽?곸쑝濡??앸궗??
}
#else
// 怨좎젙臾몄옄 異쒕젰 demo program
int dotmatrix_main_test(void)
{
	while(1)
	{
		for (int i=0; i < 8; i++)
		{
			// 怨듯넻 ?묎레 諛⑹떇
			// column?먮뒗 0??ROW?먮뒗 1??異쒕젰?댁빞 ?대떦 LED媛 on?쒕떎.
			write_column_data(i);
			write_row_data(all_on[i]);

			HAL_Delay(1);
		}
	}
	return 0; // ?먮윭 ?놁씠 ?뺤긽?곸쑝濡??앸궗??
}
#endif





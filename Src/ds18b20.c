#include "ds18b20.h"


__STATIC_INLINE void DelayMicro(__IO uint32_t micros) {
	micros *= (SystemCoreClock / 1000000) / 9;
	/* Wait till done */
	while (micros--) ;
}

void GPIO_CUSTOM_INIT(void) {
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_11);
	GPIOB->CRH |= GPIO_CRH_MODE11;
	GPIOB->CRH |= GPIO_CRH_CNF11_0;
	GPIOB->CRH &= ~GPIO_CRH_CNF11_1;
}


uint8_t ds18b20_Reset(void) {
	uint16_t status;
	GPIOB->ODR &= ~GPIO_ODR_ODR11;	//низкий уровень
	DelayMicro(485);				//задержка как минимум на 480 микросекунд
	GPIOB->ODR |= GPIO_ODR_ODR11;	//высокий уровень
	DelayMicro(65);					//задержка как минимум на 60 микросекунд
	status = GPIOB->IDR & GPIO_IDR_IDR11;//проверяем уровень
	DelayMicro(500);				//задержка как минимум на 480 микросекунд
									//(на всякий случай подождём побольше, так как могут быть неточности в задержке)
	return (status ? 1 : 0);		//вернём результат
}

uint8_t ds18b20_ReadBit(void) {
	uint8_t bit = 0;
	GPIOB->ODR &= ~GPIO_ODR_ODR11;	//низкий уровень
	DelayMicro(2);
	GPIOB->ODR |= GPIO_ODR_ODR11;	//высокий уровень
	DelayMicro(13);
	bit = (GPIOB->IDR & GPIO_IDR_IDR11 ? 1 : 0);	//проверяем уровень
	DelayMicro(45);
  	return bit;
}

uint8_t ds18b20_ReadByte(void) {
  	uint8_t data = 0;
  	uint8_t i = 0;
  	for(i = 0; i <= 7; i++)
  		data += ds18b20_ReadBit() << i;
  	return data;
}

void ds18b20_WriteBit(uint8_t bit) {
  	GPIOB->ODR &= ~GPIO_ODR_ODR11;
  	DelayMicro(bit ? 3 : 65);
  	GPIOB->ODR |= GPIO_ODR_ODR11;
  	DelayMicro(bit ? 65 : 3);
}

void ds18b20_WriteByte(uint8_t dt) {
	uint8_t i;
  	for(i = 0; i < 8; i++) {
   		ds18b20_WriteBit(dt >> i & 1);    
    	DelayMicro(5);	//Delay Protection
  	}
}

uint8_t ds18b20_init(uint8_t mode) {
	if(ds18b20_Reset()) 
		return 1;
	if(mode == SKIP_ROM) {		
		ds18b20_WriteByte(0xCC);			// SKIP ROM		
		ds18b20_WriteByte(0x4E);			// WRITE SCRATCHPAD		
		ds18b20_WriteByte(0x64);			// TH REGISTER 100 градусов		
		ds18b20_WriteByte(0x9E);			// TL REGISTER - 30 градусов		
		ds18b20_WriteByte(RESOLUTION_12BIT);// Resolution 12 bit
	}
  	return 0;
}


void ds18b20_MeasureTemperCmd(uint8_t mode, uint8_t DevNum) {
	ds18b20_Reset();
	if(mode == SKIP_ROM) {
		//SKIP ROM
		ds18b20_WriteByte(0xCC);
	}
	//CONVERT T
	ds18b20_WriteByte(0x44);
}

void ds18b20_ReadStratcpad(uint8_t mode, uint8_t *Data, uint8_t DevNum) {
	uint8_t i;
	ds18b20_Reset();
	if(mode == SKIP_ROM) {
		ds18b20_WriteByte(0xCC);	//SKIP ROM
	}
	ds18b20_WriteByte(0xBE);		//READ SCRATCHPAD
	for(i = 0; i < 8; i++) {
		Data[i] = ds18b20_ReadByte();
	}
}

uint8_t ds18b20_GetSign(uint16_t dt) {
	//Проверим 11-й бит
	if (dt&(1 << 11)) 
		return 1;
	else 
		return 0;
}

float ds18b20_Convert(uint16_t dt) {
	float t = (float)((dt&0x07FF) >> 4);	//отборосим знаковые и дробные биты
	t += (float)(dt&0x000F) / 16.0f;		//Прибавим дробную часть
	return t;
}


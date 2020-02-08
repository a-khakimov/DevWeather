#include "ds18b20.h"

UART_HandleTypeDef huart1;
uint8_t LastDeviceFlag, LastDiscrepancy, LastFamilyDiscrepancy;
uint8_t ROM_NO[8];
extern uint8_t Dev_ID[8][8];
extern uint8_t Dev_Cnt = 0;


__STATIC_INLINE void DelayMicro(__IO uint32_t micros) {
    micros *= (SystemCoreClock / 1000000) / 9;
    /* Wait till done */
    while (micros--) ;
}

// Иницивлизация вывода микроконтроллера к которому подключены датчики
void GPIO_CUSTOM_INIT(void) {
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_11);
    GPIOB->CRH |= GPIO_CRH_MODE11;
    GPIOB->CRH |= GPIO_CRH_CNF11_0;
    GPIOB->CRH &= ~GPIO_CRH_CNF11_1;
}


uint8_t ds18b20_Reset(void) {
    uint16_t status = 0;
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
    uint8_t data = 0, i = 0;
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
    uint8_t i = 0;
    for(i = 0; i < 8; i++) {
        ds18b20_WriteBit(dt >> i & 1);    
        DelayMicro(5);	//Delay Protection
    }
}

uint8_t ds18b20_SearhRom(uint8_t *Addr) {
	int i = 0;
    uint8_t id_bit_number;
    uint8_t last_zero, rom_byte_number, search_result;
    uint8_t id_bit, cmp_id_bit;
    uint8_t rom_byte_mask, search_direction;    
	//проинициализируем переменные
    id_bit_number = 1;
    last_zero = 0;
    rom_byte_number = 0;
    rom_byte_mask = 1;
    search_result = 0;
    if(!LastDeviceFlag)	{
        ds18b20_Reset();
        ds18b20_WriteByte(0xF0);
    }
    do
    {
        id_bit = ds18b20_ReadBit();
        cmp_id_bit = ds18b20_ReadBit();
        if ((id_bit == 1) && (cmp_id_bit == 1))
            break;
        else
        {
            if (id_bit != cmp_id_bit)
                search_direction = id_bit; // bit write value for search
            else
            {
                if (id_bit_number < LastDiscrepancy)
                    search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
                else
                    search_direction = (id_bit_number == LastDiscrepancy);
                if (search_direction == 0)
                {
                    last_zero = id_bit_number;
                    if (last_zero < 9)
                        LastFamilyDiscrepancy = last_zero;
                }
            }
            if (search_direction == 1)
                ROM_NO[rom_byte_number] |= rom_byte_mask;
            else
                ROM_NO[rom_byte_number] &= ~rom_byte_mask;
            ds18b20_WriteBit(search_direction);
            id_bit_number++;
            rom_byte_mask <<= 1;
            if (!rom_byte_mask)
            {
                ++rom_byte_number;
                rom_byte_mask = 1;
            }
        }
    } while(rom_byte_number < 8);		// ñ÷èòûâàåì áàéòû ñ 0 äî 7 â öèêëå
    if (!(id_bit_number < 65))
    {
        LastDiscrepancy = last_zero;	// search successful so set LastDiscrepancy,LastDeviceFlag,search_result
        if (!LastDiscrepancy)		// check for last device
            LastDeviceFlag = 1;
        search_result = 1;	
    }
    if (!search_result || !ROM_NO[0])
    {
        LastDiscrepancy = 0;
        LastDeviceFlag = 0;
        LastFamilyDiscrepancy = 0;
        search_result = 0;
    }
    else
    {
        for(i = 0; i < 8; i++) 
            Addr[i] = ROM_NO[i];
    }	
    return search_result;
}

uint8_t ds18b20_init(uint8_t mode)
{
    int i = 0, j = 0;
    uint8_t dt[8];
    if(mode == SKIP_ROM)
    {
        if(ds18b20_Reset()) 
        	return 1;
        ds18b20_WriteByte(0xCC);	//SKIP ROM
        ds18b20_WriteByte(0x4E);	//WRITE SCRATCHPAD
        ds18b20_WriteByte(0x64);	//TH REGISTER 100 градусов
        ds18b20_WriteByte(0x9E);	//TL REGISTER - 30 градусов
        ds18b20_WriteByte(RESOLUTION_12BIT);	//Resolution 12 bit
    }
    else
    {
        for(i = 1; i <= 8; ++i)
        {
            if(ds18b20_SearhRom(dt))
            {
                Dev_Cnt++;
                memcpy(Dev_ID[Dev_Cnt-1], dt, sizeof(dt));
            }
            else 
            	break;
        }
        for(i = 1; i <= Dev_Cnt; i++)
        {
            if(ds18b20_Reset()) 
            	return 1;
            ds18b20_WriteByte(0x55);	//Match Rom
            for(j = 0; j <= 7; ++j)
                ds18b20_WriteByte(Dev_ID[i-1][j]);
            ds18b20_WriteByte(0x4E);	//WRITE SCRATCHPAD
            ds18b20_WriteByte(0x64);	//TH REGISTER 100 градусов
            ds18b20_WriteByte(0x9E);	//TL REGISTER - 30 градусов
            ds18b20_WriteByte(RESOLUTION_12BIT);	//Resolution 12 bit
        }
    }
    return 0;
}

void ds18b20_all_sensors_init()
{
	uint8_t status = ds18b20_init(NO_SKIP_ROM);
    _printf(str1, "Init Status: %d\r\n", status);
    _printf(str1, "Dev count: %d\r\n", Dev_Cnt);
    int i = 0;
    for(i = 1; i <= Dev_Cnt; i++)
    {
        _printf(str1,"Device %d\r\n", i);
        _printf(str1,"ROM RAW: %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\r\n", Dev_ID[i-1][0], Dev_ID[i-1][1], Dev_ID[i-1][2], Dev_ID[i-1][3], Dev_ID[i-1][4], Dev_ID[i-1][5], Dev_ID[i-1][6], Dev_ID[i-1][7]);
        _printf(str1,"Family CODE: 0x%02X\r\n", Dev_ID[i-1][0]);
        _printf(str1,"ROM CODE: 0x%02X%02X%02X%02X%02X%02X\r\n", Dev_ID[i-1][6], Dev_ID[i-1][5], Dev_ID[i-1][4], Dev_ID[i-1][3], Dev_ID[i-1][2], Dev_ID[i-1][1]);
        _printf(str1,"CRC: 0x%02X\r\n", Dev_ID[i-1][7]);
    }
}


void ds18b20_MeasureTemperCmd(uint8_t mode, uint8_t DevNum)
{
    ds18b20_Reset();
    if(mode == SKIP_ROM)
        ds18b20_WriteByte(0xCC);	//SKIP ROM
    else
    {
        ds18b20_WriteByte(0x55);	//Match Rom
        int i = 0;
        for(i = 0; i <= 7; i++)
            ds18b20_WriteByte(Dev_ID[DevNum-1][i]);
    }
    ds18b20_WriteByte(0x44);		//CONVERT T
}

void ds18b20_ReadStratcpad(uint8_t mode, uint8_t *Data, uint8_t DevNum)
{
    uint8_t i = 0;
    ds18b20_Reset();
    if(mode == SKIP_ROM)
        ds18b20_WriteByte(0xCC);	//SKIP ROM
    else
    {
        ds18b20_WriteByte(0x55);	//Match Rom
        for(i = 0; i <= 7; i++)
            ds18b20_WriteByte(Dev_ID[DevNum-1][i]);
    }
    
    ds18b20_WriteByte(0xBE);		//READ SCRATCHPAD
    for (i = 0; i < 8; i++)
        Data[i] = ds18b20_ReadByte();
}

uint8_t ds18b20_GetSign(uint16_t dt) {    
    if (dt&(1 << 11)) //Проверим 11-й бит
        return 1;
    else 
        return 0;
}

float ds18b20_Convert(uint16_t dt) {
    float t = (float)((dt & 0x07FF) >> 4);	//отборосим знаковые и дробные биты
    t += (float)(dt & 0x000F) / 16.0f;		//Прибавим дробную часть
    return (t);
}


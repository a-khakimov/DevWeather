#include "display2004.h"


char simb[90] = " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!#$%&()*+-./@:;<=>?[]^_`{|}";
int cod_simb[90] = {
    0x80,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,
    0x5A,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,
    0x7A,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x21,0x23,0x24,0x25,0x26,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2F,0x40,0x3A,0x3B,
    0x3C,0x3D,0x3E,0x3F,0x5B,0x5D,0x5E,0x5F,0x60,0x7B,0x7C,0x7D
};

void Clear_ports(void) {
    D4_OFF;
    D5_OFF;
    D6_OFF;
    D7_OFF;
}

void Init_ports(void) {
    Clear_ports(); 
}

void SendByte(int ByteToSend, int IsData) {
    HAL_Delay(3);
    if (IsData == 1)
        RS_ON;
    else
        RS_OFF;
    E_ON;
    HAL_Delay(1);
    
    D4((ByteToSend& 16) >> 4);  
    D5((ByteToSend& 32) >> 5);
    D6((ByteToSend& 64) >> 6);  
    D7((ByteToSend& 128) >> 7);
    
    HAL_Delay(2);
    E_OFF;
    HAL_Delay(1);
    Clear_ports();
    
    E_ON;
    if(IsData == 1)
        RS_ON;
    else
        RS_OFF;
    HAL_Delay(1);
    
    D4((ByteToSend& 1) >> 0); 
    D5((ByteToSend& 2) >> 1);
    D6((ByteToSend& 4) >> 2); 
    D7((ByteToSend& 8) >> 3);   
    
    HAL_Delay(2);
    E_OFF;
    HAL_Delay(1);
    Clear_ports();
    HAL_Delay(3);
}

int strLength(const char* str) {
    int size = 0;
    while(*str++) 
    	++size;
    return size;
}

int cod(char b) {
    int f = 1, j = 0;
    while(j < 90){
        if(b == simb[j]) {
            f = cod_simb[j];  
        }
        j++;   
    }
    return f;
}

void PrintF(int str, int num, const char* hh) {
	curs(str, num);
    int lenght = 0, i = 0, y = 0;
    lenght = strLength(hh);
    while(i < lenght) {  
        y = cod(hh[i]);
        i++;
        SendByte(y, 1);
    }    
}

void Clr_display(void) {
    SendByte(0x1, 0);
}

void Return_Home(void) {
    SendByte(0x2, 0);
}

void Display_On(void) {
    SendByte(0xC, 0);
}

void Shift_Curs_Left(void) {
    SendByte(0x10, 0);
}

void Shift_Curs_Right(void) {
    SendByte(0x14, 0);
}
void Curs_Off(void) {
    SendByte(0x0C, 0);
}

void Shift_Display_Left(void) {
    SendByte(0x18, 0);
}

void Shift_Display_Right(void) {
    SendByte(0x1C, 0);
}

void Function_Set(void) {
    SendByte(0x38, 0);
}

void Enty_Mode_Set(void) {
    SendByte(0x6, 0);
}

void Init_LCD(void) {
    HAL_Delay(30);
    Clear_ports();
    //Function_Set
    E_ON;
    HAL_Delay(1);
    D5_ON;
    HAL_Delay(2);
    E_OFF;
    HAL_Delay(1);
    Clear_ports();
    HAL_Delay(3);
    E_ON;
    HAL_Delay(1);
    D5_ON;
    HAL_Delay(2);
    E_OFF;
    HAL_Delay(1);
    Clear_ports();
    HAL_Delay(2);
    E_ON;
    HAL_Delay(1);
    D7_ON;
    HAL_Delay(2);
    E_OFF;
    HAL_Delay(1);
    Clear_ports();
    HAL_Delay(3);
    E_ON;
    HAL_Delay(3);
    E_OFF;
    HAL_Delay(1);
    Clear_ports();
    HAL_Delay(2);
    E_ON;
    HAL_Delay(1);
    D7_ON;
    D6_ON;
    D5_ON;
    HAL_Delay(2);
    E_OFF;
    HAL_Delay(1);
    Clear_ports();
    
    HAL_Delay(3);
    E_ON;
    HAL_Delay(3);
    E_OFF;
    HAL_Delay(1);
    Clear_ports();
    HAL_Delay(2);
    E_ON;
    HAL_Delay(1);
    D6_ON;D5_ON;
    HAL_Delay(2);
    E_OFF;
    HAL_Delay(1);
    Clear_ports();
    
    HAL_Delay(3);
    E_ON;
    HAL_Delay(1);
    HAL_Delay(2);
    E_OFF;
    HAL_Delay(1);
    Clear_ports();
    HAL_Delay(2);
    E_ON;
    HAL_Delay(1);
    D4_ON;
    HAL_Delay(2);
    E_OFF;
    HAL_Delay(1);
    Clear_ports();

	HAL_Delay(10);
	Curs_Off();
}

void curs(int str, int num) {
    int c = 0x0;
    int s = 0x0;
    switch(str) {
    case 1:
        s = 0x80;
        break;
    case 3:
        s = 0x93;
        break;
    case 2:
        s = 0xC0;
        break;
    case 4:
        s = 0xD3;
        break;
    default:
    	break;
    } 
    c = s + num;
    SendByte(c, 0);
}

void Print_SQRT(void) {
    SendByte(0xE8, 1);
}

#ifndef DISPLAY2004_H_
#define DISPLAY2004_H_


#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define LCM_OUT         GPIOB->ODR
#define LCM_PIN_MASK    ((GPIO_ODR_ODR_8 | GPIO_ODR_ODR_10 | GPIO_ODR_ODR_7 | GPIO_ODR_ODR_6 | GPIO_ODR_ODR_5 | GPIO_ODR_ODR_4))
#define RS              GPIO_ODR_9
#define E               GPIO_ODR_ODR_7
#define D7_ON           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET)
#define D7_OFF          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET)
#define D6_ON           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET)
#define D6_OFF          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET)
#define D5_ON           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET)
#define D5_OFF          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET)
#define D4_ON           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)
#define D4_OFF          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)
#define RS_ON           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET)
#define RS_OFF          HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET)
#define E_ON            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)
#define E_OFF           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET)

#define D7(bit)           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, bit)
#define D6(bit)           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, bit)
#define D5(bit)           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, bit)
#define D4(bit)           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, bit)


void Init_LCD(void);
void curs(int str, int num);
void PrintF(uint8_t str, uint8_t num, const char* hh);


#endif /* DISPLAY2004_H_ */
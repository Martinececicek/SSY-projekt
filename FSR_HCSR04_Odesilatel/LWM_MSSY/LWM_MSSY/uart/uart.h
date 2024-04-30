/*
 * uart.h
 *
 * Created: 2/13/2024 14:51:14
 *  Author: Student
 */ 

#include <stdio.h>
#ifndef UART_H_
#define UART_H_
void UART_init(uint16_t Baudrate);
void UART_SendChar(uint8_t data);
void UART_SendString(char *text);
uint8_t UART_GetChar( void );
int printCHAR(char character, FILE *stream);




#endif /* UART_H_ */
/*
 * i2c.c
 *
 * Created: 2/27/2024 14:48:58
 *  Author: Student
 */ 

#include "../makra.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void i2cInit(void){
	
	//timto setneme SCL--400kHz
	TWSR = 0x00;
	TWBR = 0x02;
	
	//enable TWI
	TWCR = (1<<TWEN);
}

void i2cStart(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
}
//send stop signal
void i2cStop(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

void i2cWrite(uint8_t u8data)
{
	TWDR = u8data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
}

uint8_t i2cReadACK(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}
//read byte with NACK
uint8_t i2cReadNACK(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}

uint8_t i2cGetStatus(void)
{
	uint8_t status;
	//mask status
	status = TWSR & 0xF8;
	//vyznam viz datasheet
	return status;
}
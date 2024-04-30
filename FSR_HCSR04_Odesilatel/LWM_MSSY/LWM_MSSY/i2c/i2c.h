/*
 * i2c.h
 *
 * Created: 2/27/2024 14:49:11
 *  Author: Student
 */ 


#ifndef I2C_H_
#define I2C_H_

void i2cInit(void);
void i2cStart(void);
void i2cStop(void);
void i2cWrite(uint8_t u8data);
uint8_t i2cReadACK(void);
uint8_t i2cReadNACK(void);
uint8_t i2cGetStatus(void);




#endif /* I2C_H_ */
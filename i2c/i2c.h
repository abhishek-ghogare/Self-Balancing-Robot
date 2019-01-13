/*
*
* Team Id           : self_balancing_bot
* Author List       : Abhishek Ghogare
* Filename          : i2c.h
* Functions         : void      I2C_init_I2C0(),
*                     void      I2CSend(uint8_t slave_addr, uint8_t num_of_args, ...),
*                     void      I2CSendString(uint32_t slave_addr, char array[]),
*                     uint32_t  I2CReceive(uint32_t slave_addr, uint8_t reg)
* Global Variables  : None
*
*/

#ifndef I2C_H_
#define I2C_H_

#include "../common.h"


//initialize I2C module 0
//Slightly modified version of TI's example code
void I2C_init_I2C0(void);

//sends an I2C command to the specified slave
void I2CSend(uint8_t slave_addr, uint8_t num_of_args, ...);



//sends an array of data via I2C to the specified slave
void I2CSendString(uint32_t slave_addr, char array[]);


//read specified register on slave device
uint32_t I2CReceive(uint32_t slave_addr, uint8_t reg);

#endif /* I2C_H_ */

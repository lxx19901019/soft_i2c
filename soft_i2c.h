/*
  * @file    soft_i2c.h
  * @author  taoshi
  * @version V1.0.0
  * @date    2018/9/16
*/
 
#ifndef __SOFT_I2C_H
#define __SOFT_I2C_H
 
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stdio.h"
#include "stdbool.h"
#include "sys.h"
//---------------------------------------------------------------------------------------------------
//i2c引脚设置，我这里用的是C0和C2
#define I2C_GPIO                  GPIOC
#define I2C_CLOCK_ON              RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE)
#define I2C_SDA_PIN               0
#define I2C_SCL_PIN               2
#define GPIO_I2C_SDA_PIN          GPIO_Pin_0
#define GPIO_I2C_SCL_PIN          GPIO_Pin_2
#define SDA_PIN_0_7               1              //SDA引脚范围（0~7或8~15）
//---------------------------------------------------------------------------------------------------
#define SOFT_I2C_DELAY_LEVEL      10            //模拟i2c延时等级
//---------------------------------------------------------------------------------------------------
#if SDA_PIN_0_7                   //SDA引脚在0~7内，所以用CLR寄存器                                                                                                                                                   
#define I2C_SDA_OUT()           { I2C_GPIO->CRL &= ~((uint32_t)0xf << 4 * I2C_SDA_PIN);\
                                  I2C_GPIO->CRL |=  ((uint32_t)0x3 << 4 * I2C_SDA_PIN); }        //推挽输出，速度50MHZ     
#define I2C_SDA_IN()            { I2C_GPIO->CRL &= ~((uint32_t)0xf << 4 * I2C_SDA_PIN);\
                                  I2C_GPIO->CRL |=  ((uint32_t)0x4 << 4 * I2C_SDA_PIN); }          //浮空输入
 
#else                             //SDA引脚在8~15内，所以CRH寄存器                                                                                                                        
#define I2C_SDA_OUT()           { I2C_GPIO->CRH &= ~((uint32_t)0xf << 4 * (I2C_SDA_PIN - 8));\
                                  I2C_GPIO->CRH |=  ((uint32_t)0x3 << 4 * (I2C_SDA_PIN - 8)); }  //推挽输出，速度50MHZ      
#define I2C_SDA_IN()            { I2C_GPIO->CRL &= ~((uint32_t)0xf << 4 * (I2C_SDA_PIN - 8));\
                                  I2C_GPIO->CRL |=  ((uint32_t)0x4 << 4 * (I2C_SDA_PIN - 8)); }  //浮空输入
#endif
//---------------------------------------------------------------------------------------------------
//更换GPIO时一定不能忘了改这里
#define I2C_SDA_H                (PCout(I2C_SDA_PIN) = 1)                  
#define I2C_SDA_L                (PCout(I2C_SDA_PIN) = 0)
#define I2C_SCL_H                (PCout(I2C_SCL_PIN) = 1)                  
#define I2C_SCL_L                (PCout(I2C_SCL_PIN) = 0)                                               
#define I2C_SDA_IN_DATA          PCin(I2C_SDA_PIN)                                                                          
//---------------------------------------------------------------------------------------------------
                                                                        
void Soft_I2C_Init(void);
bool I2C_Read_Buff(uint8_t addr_with_read, uint8_t reg_addr, uint8_t *buff, size_t size);
bool I2C_Read_Reg(uint8_t addr_with_read, uint8_t reg_addr, uint8_t *data);
bool I2C_Write_Reg(uint8_t addr_with_write, uint8_t reg_addr, uint8_t data);                                                                        
                                                                        
#endif

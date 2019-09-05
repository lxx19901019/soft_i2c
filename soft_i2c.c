 #include "soft_i2c.h"
 

static void I2C_Delay(uint8_t t)
{
    uint8_t i = 0;
    
    while (t--)
    {
        i = SOFT_I2C_DELAY_LEVEL;
        while (i--);
    }
}
 

static void I2C_Start(void)
{
    I2C_SDA_OUT();
    I2C_SDA_H;
    I2C_SCL_H;
    I2C_Delay(2);
    I2C_SDA_L;
    I2C_Delay(2);
    I2C_SCL_L;  
    I2C_Delay(2);
}
 

static void I2C_Stop(void)
{
    I2C_SDA_OUT();
    I2C_SCL_L;  
    I2C_SDA_L;
    I2C_Delay(2);
    I2C_SCL_H;
    I2C_Delay(2);
    I2C_SDA_H;  
    I2C_Delay(2);
}
 
static bool I2C_Wait_Ack(void)
{
    uint8_t err_time = 0;
    
    I2C_SCL_L;
    I2C_Delay(1);
    I2C_SDA_H;
    I2C_Delay(1);
    I2C_SDA_IN();
    I2C_Delay(1);
    I2C_SCL_H;
    I2C_Delay(1);
    
    while (I2C_SDA_IN_DATA)
    {
        err_time++;
        if (err_time > 250)
            return true;
    }
    
    I2C_SCL_L;
    I2C_Delay(1);
    
    return false;
}
 
static void I2C_Ack(void)
{
    I2C_SCL_L;
    I2C_Delay(1);
    I2C_SDA_OUT();
    I2C_SDA_L;
    I2C_Delay(1);
    I2C_SCL_H;
    I2C_Delay(1);
    I2C_SCL_L;
    I2C_Delay(1);   
}

static void I2C_NAck(void)
{
    I2C_SCL_L;
    I2C_Delay(1);
    I2C_SDA_OUT();
    I2C_SDA_H;
    I2C_Delay(1);
    I2C_SCL_H;
    I2C_Delay(1);
    I2C_SCL_L;
    I2C_Delay(1);       
}

static void I2C_Send_Byte(uint8_t data)
{
    uint8_t i;
    
    I2C_SDA_OUT();
    I2C_SCL_L;
    
    for (i = 0; i < 8; i++)
    {
        if ((data & (uint8_t)0x80) >> 7)
            I2C_SDA_H;
        else
            I2C_SDA_L;      
        data <<= 1;
        I2C_Delay(1);
        I2C_SCL_H;
        I2C_Delay(1);       
        I2C_SCL_L;
        I2C_Delay(1);
    }
}   
 
static uint8_t I2C_Read_Byte(void)
{
    uint8_t i, reveive_byte = 0;
    
    I2C_SDA_IN();
    
    for (i = 0; i < 8; i++)
    {
        I2C_SCL_L;
        I2C_Delay(1);
        I2C_SCL_H;
        I2C_Delay(1);   
        
        reveive_byte <<= 1;
        if (I2C_SDA_IN_DATA)
        reveive_byte |= (uint8_t)0x01;
    }
    
    return reveive_byte;
}

bool I2C_Write_Reg(uint8_t addr_with_write, uint8_t reg_addr, uint8_t data)
{
    I2C_Start();
    
    I2C_Send_Byte(addr_with_write);
    if (I2C_Wait_Ack())  return false;
    
    I2C_Send_Byte(reg_addr);
    if (I2C_Wait_Ack())  return false;
    
    I2C_Send_Byte(data);
    if (I2C_Wait_Ack())  return false;
 
    I2C_Stop();
    return true;    
}   

bool I2C_Read_Reg(uint8_t addr_with_read, uint8_t reg_addr, uint8_t *data)
{   
    I2C_Start();
    
    I2C_Send_Byte(addr_with_read & (uint8_t)0xfe);
    if (I2C_Wait_Ack())  return false;  
 
    I2C_Send_Byte(reg_addr);
    if (I2C_Wait_Ack())  return false;  
    
    I2C_Start();
    
    I2C_Send_Byte(addr_with_read);
    if (I2C_Wait_Ack())  return false;  
 
    *data = I2C_Read_Byte();
    I2C_NAck();
    I2C_Stop();
 
    return true;    
}

bool I2C_Read_Buff(uint8_t addr_with_read, uint8_t reg_addr, uint8_t *buff, size_t size)
{
    uint8_t i;
    
    I2C_Start();
    
    I2C_Send_Byte(addr_with_read & (uint8_t)0xfe);
    if (I2C_Wait_Ack())  return false;  
 
    I2C_Send_Byte(reg_addr);
    if (I2C_Wait_Ack())  return false;  
    
    I2C_Start();
    
    I2C_Send_Byte(addr_with_read);
    if (I2C_Wait_Ack())  return false;  
 
    for (i = 0; i < size; i++)
    {
        *buff++ = I2C_Read_Byte();
        if (i < size - 1)
            I2C_Ack();
    }
    I2C_NAck();
    I2C_Stop();
    
    return true;
}
 

void Soft_I2C_Init(void)
{
    GPIO_InitTypeDef GPIO_Init_Structure;
    
    I2C_CLOCK_ON;   
    GPIO_Init_Structure.GPIO_Pin = GPIO_I2C_SDA_PIN | GPIO_I2C_SCL_PIN;
    GPIO_Init_Structure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init_Structure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_GPIO, &GPIO_Init_Structure);
    GPIO_SetBits(I2C_GPIO, GPIO_I2C_SDA_PIN | GPIO_I2C_SCL_PIN);    
}

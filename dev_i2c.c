#include "dev_color.h"
#include "global.h"
#include "adc.h"

#define delay_ms(x) soft_osc16m_delay(x)

#define SDA P03
#define SCL P02

#define SDA_INPUT   do { P0DIR |= (1<<3);  P0PU |= (1<<3); }while(0)
#define SDA_OUTPUT  do { P0DIR &= ~(1<<3); P0PU |= (1<<3); }while(0)

#define SCL_INPUT   do {P0DIR |= (1<<2); }while(0)
#define SCL_OUTPUT  do {P0DIR &= ~(1<<2); P0PU |= (1<<2); }while(0)

#define SDA_HIGH  P03 = 1
#define SDA_LOW   P03 = 0
//#define SDA_HIGH  P0 |= (1<<3)
//#define SDA_LOW   P0 &= ~(1<<3)
#define SCL_HIGH  P02 = 1
#define SCL_LOW   P02 = 0
//#define SCL_HIGH  P0 |= (1<<2)
//#define SCL_LOW   P0 &= ~(1<<2)

void delay(void)
{
    uint16_t i;
    for(i = 0; i < 100; i++);
}
static void delay_us(int us)
{
    uint8_t i;
    for(i = 0; i < 10 * us; i++);
}

void i2c_init(void)
{
    P0SEL &= ~(0x0c);   //P02 P03 数字口
    P0DIR &= ~0x0c;     //p3.2 set 0, output
    P0PU |= (0x0c);     //P02 P03上拉使能
    P0PD &= ~0x0c;      //P02 P03上拉使能
    SCL_OUTPUT;
    SDA_OUTPUT;
	SDA_HIGH;
	SCL_HIGH;
}

void i2c_start(void)
{
    SDA_OUTPUT;
    SDA_HIGH;
    SCL_HIGH;
    delay();
    SDA_LOW;
    delay();
	SCL_LOW;
	delay();
}
void i2c_stop()
{
    SDA_OUTPUT;
	//SCL_LOW;
	delay();
    SDA_LOW;
	delay();
	SCL_HIGH;
	delay();   
    SDA_HIGH;
	//delay();
}
#if 0
uint8_t i2c_read_ack(void)
{
    uint8_t err_time = 0;
	
    SCL_LOW;
    delay();
    SDA_HIGH;
    delay();
    SDA_INPUT;
    delay();
    SCL_HIGH;
    delay();
	
    while(SDA)
    {
        err_time++;
        if (err_time > 250)
            return 0;
    }
	
    SCL_LOW;
    delay();
	
    return 1;
}
#else
uint8_t i2c_read_ack()
{
    uint8_t ack = 0;
    SDA_INPUT;
    SCL_LOW;
	delay();
	SCL_HIGH;
	delay();
    ack = SDA;
    delay();
    SCL_LOW;
    delay();
    return !ack;
}
#endif
void i2c_send_ack(uint8_t ack)
{
    SDA_OUTPUT;
	if(ack) {
		SDA_LOW;
	} else {
		SDA_HIGH;
	}
    delay();
    SCL_HIGH;
    delay();
    SCL_LOW;
    delay();
}

void i2c_send_nack()
{
    SDA_OUTPUT;
    SCL_LOW;
    SDA_HIGH;
    delay();
    SCL_HIGH;
    delay();
    SCL_LOW;
    delay();
}

void i2c_write_byte(uint8_t byte)
{
    uint8_t i;
    SDA_OUTPUT;
    for(i = 0; i < 8; i++) {
		if(byte & 0x80) {
            SDA_HIGH;
        } else {
            SDA_LOW;
        }
		byte = byte << 1;
		delay();
        SCL_HIGH;
        delay();
        SCL_LOW;
		delay();
    }
}
uint8_t i2c_read_byte()
{
    uint8_t i;
    uint8_t r = 0;
    SDA_INPUT;
	delay_us(5);
    for(i = 0; i < 8; i++) {
        SCL_HIGH;
        delay();
        r = (r << 1) | SDA;
		delay();
        SCL_LOW;
        delay();
    }
    return r;
}
uint8_t i2c_read(uint8_t addr,uint8_t *buf, uint16_t len)
{
    uint8_t i;
    uint8_t t;
    i2c_start();
    t = (addr << 1) & 0xfe;
    i2c_write_byte(t);
	if (!i2c_read_ack())
	{
		i2c_stop();
		return 0;
	}
    for(i = 0; i < len; i++) {
        buf[i] = i2c_read_byte();
		if(i == (len - 1)) {
			i2c_send_ack(0);
		} else {
			i2c_send_ack(1);
		}
	}

    i2c_stop();
	return 1;
}
uint8_t i2c_read_buffer(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
	
    uint8_t i;
    uint8_t t;
    i2c_start();
    t = (addr << 1) & 0xfe;
    i2c_write_byte(t);
	if (!i2c_read_ack())
	{
		i2c_stop();
		return 0;
	}
    i2c_write_byte(reg);
	if (!i2c_read_ack())
	{
		i2c_stop();
		return 0;
	}

	i2c_start();
    t = (addr << 1) | 1;
    i2c_write_byte(t);
	if (!i2c_read_ack())
	{
		i2c_stop();
		return 0;
	}
    for(i = 0; i < len; i++) {
        buf[i] = i2c_read_byte();
		if(i == (len - 1)) {
			i2c_send_ack(0);
		} else {
			i2c_send_ack(1);
		}
	}
    i2c_stop();
	return 1;
}

uint8_t i2c_read_reg(uint8_t addr, uint8_t reg, uint8_t *buf)
{
    uint8_t t;
    i2c_start();
    t = (addr << 1) & 0xfe;
    i2c_write_byte(t);
	if (!i2c_read_ack())
	{
		i2c_stop();
		return 0;
	}
    i2c_write_byte(reg);
	if (!i2c_read_ack())
	{
		i2c_stop();
		return 0;
	}

	i2c_start();
    t = (addr << 1) | 1;
    i2c_write_byte(t);
	if (!i2c_read_ack())
	{
		i2c_stop();
		return 0;
	}
	*buf = i2c_read_byte();
	i2c_send_ack(0);

    i2c_stop();
	return 1;
}
uint8_t i2c_write_buffer(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    uint8_t i;
    uint8_t t;
    i2c_start();
    t = (addr << 1) | 0;
    i2c_write_byte(t);
	if (!i2c_read_ack())
	{
		i2c_stop();
		return 0;
	}
	i2c_write_byte(reg);
	if (!i2c_read_ack())
	{
		i2c_stop();
		return 0;
	}
	i2c_start();
	i2c_write_byte(t);
	if (!i2c_read_ack())
	{
		i2c_stop();
		return 0;
	}
    for(i = 0; i < len; i++) {
        i2c_write_byte(buf[i]);
		if (!i2c_read_ack())
		{
			i2c_stop();
			return 0;
		}
	}
    i2c_stop();
	return 1;
}
uint8_t i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t value)
{
    uint8_t t;
    i2c_start();
    t = (addr << 1) | 0;
    i2c_write_byte(t);
	if (!i2c_read_ack())
	{
		PRINT("write addr failed\n");
		i2c_stop();
		return 0;
	}
	i2c_write_byte(reg);
	if (!i2c_read_ack())
	{
		PRINT("write reg failed\n");
		i2c_stop();
		return 0;
	}
	i2c_write_byte(value);
	if (!i2c_read_ack())
	{
		PRINT("write value failed\n");
		i2c_stop();
		return 0;
	}
    i2c_stop();
	return 1;
}
uint8_t i2c_write(uint8_t addr, uint8_t *buf, uint16_t len)
{
    uint8_t i;
    uint8_t t;
    i2c_start();
    t = (addr << 1) | 0;
    i2c_write_byte(t);
	if (!i2c_read_ack())
	{
		i2c_stop();
		return 0;
	}

    for(i = 0; i < len; i++) {
        i2c_write_byte(buf[i]);
		if (!i2c_read_ack())
        {
			i2c_stop();
			return 0;
        }
	}
    i2c_stop();
	return 1;
}

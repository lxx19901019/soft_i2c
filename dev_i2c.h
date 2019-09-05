#ifndef _dev_i2c_h_
#define _dev_i2c_h_

#include "global.h"

/* Extern functions ----------------------------------------------------------*/
extern uint8_t i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t buf);
extern uint8_t i2c_read_reg(uint8_t addr, uint8_t reg, uint8_t *buf);
extern uint8_t i2c_write_buffer(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);
extern uint8_t i2c_write_buffer(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len);
extern void i2c_init(void);
#endif

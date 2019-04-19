#ifndef __EM85XX_I2C_MASTER_H__
#define __EM85XX_I2C_MASTER_H__

#include <linux/spinlock.h>

struct em85xx_i2c_dev {
	int timeout;
	char dev_addr_len;
	char reg_addr_len;
	char dev_addr_disable;
	char reg_addr_disable;
	int clk_div;
    spinlock_t lock;
};

extern int em85xx_i2c_write( char addr, char reg, const char* data, int size );
extern int em85xx_i2c_read( char addr, char reg, char* data, int size );
extern int em85xx_i2c_seqwrite( char addr, char reg, const char* data, int size );
extern int em85xx_i2c_seqread( char addr, char reg, char* data, int size );
extern struct em85xx_i2c_dev em85xx_i2c_get_ctrl();
extern int em85xx_i2c_set_ctrl( struct em85xx_i2c_dev* dev );
extern int em85xx_i2c_noreg_read( char addr, char* byte, int size );
extern int em85xx_i2c_noreg_write( char addr, char* data, int size );

#endif /* __EM85XX_I2C_MASTER_H__ */

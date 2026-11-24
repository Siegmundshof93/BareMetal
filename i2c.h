#ifndef I2C_H
#define I2C_H







void i2c1_init(void);
void i2c1_transmit(const unsigned char, const unsigned char, const unsigned char);
void i2c1_receive(const unsigned char, unsigned char*, const unsigned char);



#endif

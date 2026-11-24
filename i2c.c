#include "i2c.h"
#include "registers.h"

void i2c1_init(void)
{
  RCC->APB1ENR1 |= (1U << 21); //enable peripheral clock for I2C1
  RCC->CCIPR &= ~(3U << 12);  //clear selected as I2C1 clock
  RCC->CCIPR |= (1U << 12); //HSI16 clock selected as I2C1 clock (SYS CLOCK)
  GPIOB->MODER &= ~(0xFU << (8 * 2)); //MODE8 and MODE9 clean
  GPIOB->MODER |=  (0xAU << (8 * 2)); // MODE8 and MODE9 Alternate function mode
  GPIOB->AFRH &= ~(0xFFU << 0); //clean alternate function of pin B8 and B9
  GPIOB->AFRH |=(0x44U << 0); //set alternate function of pin B8 and B9 as (I2C AF4)
  GPIOB->PUPDR &= ~(0xFU << (8 * 2)); //clear the pins PB8 and PB9
  GPIOB->PUPDR |= (0x5U << (8 * 2)); //set pins PB8 and PB9 as pull-up
  GPIOB->OTYPER |= (1U << 8) | (1U << 9); //set pins 8 and 9 as Open drain
  I2C1->CR1 &= ~(0x1U << 0); //disable I2C
  I2C1->CR1 &= ~(0x1FU << 8); //enable analog and clear disable digital filters;
  I2C1->TIMINGR = STANDARDSPEED; //prescaler 1, scldel 3, sdadel 2, sclH 3, scll 9. configure time delay and clock high and low time
  I2C1->CR1 |= (0x1U << 0); //enable I2C
  I2C1->CR2 &= ~(0x1U << 11); //setup address for 7 bit
  I2C1->CR2 |= (0x1U << 25); //Autoend enable stop after NBYTES are sent
  I2C1->CR2 &= ~(0x1U << 24); //RELOAD The transfer is completed after the NBYTES data transfer
}

void i2c1_transmit(const unsigned char address, const unsigned char send_byte, const unsigned char number_bytes)
{
//transmit setup
I2C1->CR2 &= ~(0x1U << 10); // controller request a write function
I2C1->CR2 &= ~(0x1FFU << 0); //clear the address
I2C1->CR2 |= (address << 1); //0x23 Address for the light sensor
I2C1->CR2 &= ~(0xFFU << 16); //Clear number of send/receive bytes
I2C1->CR2 |= ((number_bytes) << 16); //send/receive n byte NBYTES
//start transmit
I2C1->CR2 |= (0x1U << 13); //I2C START
  for (unsigned char i = 0; i < number_bytes; i++)
  {
    while(!(I2C1->ISR & (0x1U << 1))); //wait until the TXDR is empty
    I2C1->TXDR = send_byte;
  }
while(!(I2C1->ISR & (0x1U << 5)));
I2C1->ICR |= (0x1U << 5); //Clear STOP
}

void i2c1_receive(const unsigned char address, unsigned char* receive_8, const unsigned char number_bytes)
{

  //transmit setup
  I2C1->CR2 |= (0x1U << 10); // controller request a read function
  I2C1->CR2 &= ~(0x1FFU << 0); //clear the address
  I2C1->CR2 |= (address << 1); //set address
  I2C1->CR2 &= ~(0xFFU << 16); //Clear number of send/receive bytes
  I2C1->CR2 |= (number_bytes << 16); //send/receive 2 bytes NBYTES
  //start transmit
  I2C1->CR2 |= (0x1U << 13); //I2C START

    for (unsigned char i = 0; i < number_bytes; i++) {
      while(!(I2C1->ISR & (0x1U << 2))); //wait until the RXDR is full
      receive_8[i] = I2C1->RXDR & 0xFF;
    }

  while(!(I2C1->ISR & (0x1U << 5)));
  I2C1->ICR |= (0x1U << 5); //Clear STOP

}

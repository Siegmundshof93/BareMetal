#include "usart.h"
#include "registers.h"

void usart2_init(void)
{
	// Usart2 clock enable PA2, PA3
	RCC->APB1ENR1 |= (1U << 17); //enable peripheral clock for USART2
	RCC->CCIPR &= ~(3U << 2);  //clear selected as USART2 clock
	RCC->CCIPR |= (1U << 2); //HSI16 clock selected as USART2 clock
  GPIOA->MODER &= ~(3U << (2 * 2)); //MODE2 clean
  GPIOA->MODER |=  (2U << (2 * 2)); // MODE2 Alternate function mode (PA2 set as alternate)
	GPIOA->AFRL &= ~(0xFU << 8); //clean alternate function of pin 2
	GPIOA->AFRL |=(0x7U << 8); //set alternate function of pin 2 as USART2 TX (AF7)
	USART2->BRR |= (0x8BU << 0); //setup baudrate 16MHz/115200Bd=139->0x8B
	USART2->CR1 |= (1U << 3); //enable Transmitter
	//USART2->CR1 |= (1U << 7); // TXE = 1 interrupt enable
	USART2->CR1 |= (1U << 6); // TC = 1 interrupt enable
	USART2->CR1 |= (1U << 0); //enable USART
	NVIC->ISER1 |= (1U << 6); // IRQ38, in ISER 32 bits > 38-32=6 bit is located in ISER1 6th bit.
}

void usart2_transmit(const unsigned char *data, const unsigned char size)
{

	for(unsigned char i=0; i<size; i++)
	{
    while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
    USART2->TDR = data[i]; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
	}
  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
  USART2->TDR = 0x0D;   //return (\r)
  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
  USART2->TDR = 0x0A;  //newline (\n)
  while (!(USART2->ISR & (1U << 6))); //transmission complete flag check
}


void usart2_transmit_uint8t_minicom(unsigned char data_8)
{
  unsigned char three;
  unsigned char two;
  unsigned char one;

    if(data_8 < 10)
    {
	  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
      USART2->TDR = data_8 + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
    }
    else if(data_8 > 9 && data_8 < 100)
    {
      two = data_8 / 10;
      one = data_8 % 10;

	  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
      USART2->TDR = two + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
	  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
      USART2->TDR = one + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
    }
    else
    {
      three = data_8 / 100;
      data_8 = data_8 % 100;
      two = data_8 / 10;
      one = data_8 % 10;

	  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
      USART2->TDR = three + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
	  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
      USART2->TDR = two + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
	  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
      USART2->TDR = one + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
    }

  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
  USART2->TDR = 0x0D;   //return (\r)
  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
  USART2->TDR = 0x0A;  //newline (\n)
  while (!(USART2->ISR & (1U << 6))); //transmission complete flag check
}


void usart2_transmit_unit16t_minicom(unsigned short data_16)
{
  unsigned char five;
  unsigned char four;
  unsigned char three;
  unsigned char two;
  unsigned char one;

if(data_16 < 10)
    {
	  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
      USART2->TDR = data_16 + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
    }
    else if(data_16 > 9 && data_16 < 100)
    {
      two = data_16 / 10;
      one = data_16 % 10;

	  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
      USART2->TDR = two + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
  	  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
      USART2->TDR = one + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
    }
    else if(data_16 > 99 && data_16 < 1000)
    {
      three = data_16 / 100;
      data_16 = data_16 % 100;
      two = data_16 / 10;
      one = data_16 % 10;

	  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
      USART2->TDR = three + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
      while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
      USART2->TDR = two + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
      while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
      USART2->TDR = one + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
    }
	else if(data_16 >999 && data_16 < 10000)
	{
	  four = data_16 / 1000;
	  data_16 = data_16 % 1000;
	  three = data_16 / 100;
      data_16 = data_16 % 100;
      two = data_16 / 10;
      one = data_16 % 10;

	  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
	  USART2->TDR = four + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
	  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
	  USART2->TDR = three + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
      while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
      USART2->TDR = two + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
      while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
      USART2->TDR = one + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string

	}
	else
	{
	  five = data_16 / 10000;
	  data_16 = data_16 % 10000;
	  four = data_16 / 1000;
	  data_16 = data_16 % 1000;
	  three = data_16 / 100;
      data_16 = data_16 % 100;
      two = data_16 / 10;
      one = data_16 % 10;

	  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
	  USART2->TDR = five + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
	  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
	  USART2->TDR = four + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
	  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
	  USART2->TDR = three + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
      while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
      USART2->TDR = two + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
      while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
      USART2->TDR = one + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
	}

  while (!(USART2->ISR & (1U << 7)));
  USART2->TDR = 0x0D;   //return (\r)
    while (!(USART2->ISR & (1U << 7)));
  USART2->TDR = 0x0A;  //newline (\n)
  while (!(USART2->ISR & (1U << 6))); //transmission complete flag check

}

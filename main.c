#include "registers.h"

#define GPIOA_BASE      0x48000000
#define GPIOA_MODER     (*(volatile unsigned int *)(GPIOA_BASE + 0x00))
#define GPIOA_BSRR      (*(volatile unsigned int *)(GPIOA_BASE + 0x18))

#define GPIOC_BASE      0x48000800
#define GPIOC_MODER     (*(volatile unsigned int *)(GPIOC_BASE + 0x00))
#define GPIOC_PUPDR     (*(volatile unsigned int *)(GPIOC_BASE + 0x0C))
#define GPIOC_IDR       (*(volatile unsigned int *)(GPIOC_BASE + 0x10))
#define GPIOA_AFRL 	    (*(volatile unsigned int *)(GPIOA_BASE + 0x20))

#define EXTI_BASE       0x40010400
#define EXTI_IMR1       (*(volatile unsigned int *)(EXTI_BASE + 0x00))
#define EXTI_FTSR1      (*(volatile unsigned int *)(EXTI_BASE + 0x0C))
#define EXTI_PR1        (*(volatile unsigned int *)(EXTI_BASE + 0x14))

#define SYSCFG_BASE     0x40010000
#define SYSCFG_EXTICR4  (*(volatile unsigned int *)(SYSCFG_BASE + 0x14))

#define NVIC_BASE       0xE000E100
#define NVIC_ISER1      (*(volatile unsigned int *)(NVIC_BASE + 0x04))



void delay(const unsigned int milisec) {
    volatile unsigned int count = milisec*3000;
    while (count--) ;
}

void system_clock_config(void)
{
    RCC->CR |= 1U << 8; //enable 16MHz Clock HSI
    while(!(RCC->CR&(1U<<10))); //wait until HSI16 flag is ready
    RCC->CFGR &= ~(3U << 0); //clear system clock switch
    RCC->CFGR |= 1U << 0; //set HSI16 as a system clock
    while(!(RCC->CFGR&(1U<<2))); // SWS bit 2 has to be 1 ->01:HSI16 used as a sys clock
    while(RCC->CFGR&(1U<<3)); // SWS bit 3 has to be 0 ->01:HSI16 used as a sys clock
}

void gpio_config(void)
{
  RCC->AHB2ENR |= (1U << 0) | (1U << 2); // Enable Clocks for GPIOA (bit 0) and GPIOC (bit 2)

  GPIOA_MODER &= ~(3U << (5 * 2)); //PA5 pin input mode erase
  GPIOA_MODER |=  (1U << (5 * 2)); // PA5 Output mode

  GPIOC_MODER &= ~(3U << (13 * 2)); //PC13 Input mode
}

void interrupt_init(void) {

    RCC->APB2ENR |= (1U << 0); //Enable SYSCFG clock


    SYSCFG_EXTICR4 &= ~(0xFU << 4); //clear 4 bits
    SYSCFG_EXTICR4 |=  (0x2U << 4); //set 0b0010 to enable interrupt for PC13


    EXTI_IMR1 |= (1U << 13); //Unmask interrupt for line 13


    EXTI_FTSR1 |= (1U << 13); //Enable Falling Edge Trigger (Button press pulls pin low) for line 13


    NVIC_ISER1 |= (1U << 8); // EXTI15_10_IRQn  Enable IRQ 40 in NVIC from corem4 typedef
}

void EXTI15_10_IRQHandler(void) {
    if (EXTI_PR1 & (1U << 13)) { // checks 13 bit

        GPIOA_BSRR = (1U << 5); // turn on PA5

        EXTI_PR1 = (1U << 13); //write in PR1 1 -> write 1 to clean the flag
    }
}

void usart2_init(void)

{
	// Usart2 clock enable PA2, PA3
	RCC->APB1ENR1 |= (1U << 17); //enable peripheral clock for USART2
	RCC->CCIPR &= ~(3U << 2);  //clear selected as USART2 clock
	RCC->CCIPR |= (1U << 2); //HSI16 clock selected as USART2 clock
  GPIOA_MODER &= ~(3U << (2 * 2)); //MODE2 clean
  GPIOA_MODER |=  (2U << (2 * 2)); // MODE2 Alternate function mode (PA2 set as alternate)
	GPIOA_AFRL &= ~(0xFU << 8); //clean alternate function of pin 2
	GPIOA_AFRL |=(0x7U << 8); //set alternate function of pin 2 as USART2 TX
	USART2->BRR |= (0x8BU << 0); //setup baudrate 16MHz/115200Bd=139->0x8B
	USART2->CR1 |= (1U << 3); //enable Transmitter
	USART2->CR1 |= (1U << 0); //enable USART

}

void usart2_transmit(const unsigned char *data)
{
	for(unsigned char i=0; i<5; i++)
	{
	USART2->TDR = data[i] + '0'; //register for sending a data (only 8 bits) +'0' or + 48 means send the ascii string
	while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
	}
	USART2->TDR = 0x0D;   //return (\r)
  while (!(USART2->ISR & (1U << 7))); //wait until the TX register is empty
  USART2->TDR = 0x0A;  //newline (\n)
  while (!(USART2->ISR & (1U << 6))); //transmission complete flag check
}

int main(void) {
    system_clock_config();
    gpio_config();

   const unsigned char test_array[5] = {0,1,2,3,4};

    interrupt_init();
    usart2_init();

    while (1)
    {
	delay(1000);
	GPIOA_BSRR = 1U << 21;
	usart2_transmit(test_array);

    }
}

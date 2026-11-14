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



void delay(volatile unsigned int count) {
    while (count--) ;
}

void interrupt_init(void) {
    // 1. Enable SYSCFG clock
    RCC->APB2ENR |= (1U << 0);

    // 2. Route EXTI Line 13 to GPIOC
    SYSCFG_EXTICR4 &= ~(0xFU << 4);
    SYSCFG_EXTICR4 |=  (0x2U << 4);

    // 3. Unmask interrupt for line 13
    EXTI_IMR1 |= (1U << 13);

    // 4. Enable Falling Edge Trigger (Button press pulls pin low)
    EXTI_FTSR1 |= (1U << 13);

    // 5. Enable IRQ 40 in NVIC
    NVIC_ISER1 |= (1U << 8);
}

void EXTI15_10_IRQHandler(void) {
    if (EXTI_PR1 & (1U << 13)) {
        // Turn ON LED on PA5
        GPIOA_BSRR = (1U << 5);
        // Clear the pending flag. Write 1 to clear
        EXTI_PR1 = (1U << 13);
    }
}

void usart2_init(void)

{
	// Usart2 clock enable PA2, PA3
	RCC->APB1ENR1 |= (1U << 17); //
	RCC->CCIPR &= ~(3U << 2);
	RCC->CCIPR |= (1U << 2);
  GPIOA_MODER &= ~(3U << (2 * 2));
  GPIOA_MODER |=  (2U << (2 * 2));
	GPIOA_AFRL &= ~(0xFU << 8);
	GPIOA_AFRL |=(0x7U << 8);
	USART2->BRR |= (0x8BU << 0);
	USART2->CR1 |= (1U << 3);
	USART2->CR1 |= (1U << 0);

}

void usart2_transmit(const unsigned char *data)
{
	for(unsigned char i=0; i<5; i++)
	{
	USART2->TDR = data[i] + '0';
	while (!(USART2->ISR & (1U << 7)));
	}
	USART2->TDR = 0x0D;   //return
  while (!(USART2->ISR & (1U << 7)));
  USART2->TDR = 0x0A;  //newline
  while (!(USART2->ISR & (1U << 6)));
}

int main(void) {
    RCC->CR |= 1U << 8;
    while(!(RCC->CR&(1U<<10)));
    RCC->CFGR &= ~(3U << 0);
    RCC->CFGR |= 1U << 0;
    while(!(RCC->CFGR&(1U<<2)));
    while(RCC->CFGR&(1U<<3));
    // 1. Enable Clocks for GPIOA (bit 0) and GPIOC (bit 2)
    RCC->AHB2ENR |= (1U << 0) | (1U << 2);

    // 2. Configure PA5 as Output
    GPIOA_MODER &= ~(3U << (5 * 2));
    GPIOA_MODER |=  (1U << (5 * 2));

    // 3. Configure PC13 as Input with Pull-Up
    GPIOC_MODER &= ~(3U << (13 * 2));

   const unsigned char test_array[5] = {0,1,2,3,4};
    // 4. Initialize Interrupts
    interrupt_init();
    usart2_init();

    while (1) {
        // Main loop remains idle; work is handled entirely by the interrupt
	delay(500000);
	GPIOA_BSRR = 1U << 21;
	usart2_transmit(test_array);

    }
}

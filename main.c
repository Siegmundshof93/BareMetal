#include "registers.h"
#include "i2c.h"
#include "usart.h"


#define EXTI_BASE       0x40010400
#define EXTI_IMR1       (*(volatile unsigned int *)(EXTI_BASE + 0x00))
#define EXTI_FTSR1      (*(volatile unsigned int *)(EXTI_BASE + 0x0C))
#define EXTI_PR1        (*(volatile unsigned int *)(EXTI_BASE + 0x14))

#define SYSCFG_BASE     0x40010000
#define SYSCFG_EXTICR4  (*(volatile unsigned int *)(SYSCFG_BASE + 0x14))





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
  RCC->AHB2ENR |= (1U << 0) | (1U << 2) | (1U <<1); // Enable Clocks for GPIOA (bit 0), GPIOB and GPIOC (bit 2)

  GPIOA->MODER &= ~(3U << (5 * 2)); //PA5 pin input mode erase
  GPIOA->MODER |=  (1U << (5 * 2)); // PA5 Output mode

  GPIOC->MODER &= ~(3U << (13 * 2)); //PC13 Input mode
}

void interrupt_init(void) {

    RCC->APB2ENR |= (1U << 0); //Enable SYSCFG clock


    SYSCFG_EXTICR4 &= ~(0xFU << 4); //clear 4 bits
    SYSCFG_EXTICR4 |=  (0x2U << 4); //set 0b0010 to enable interrupt for PC13


    EXTI_IMR1 |= (1U << 13); //Unmask interrupt for line 13


    EXTI_FTSR1 |= (1U << 13); //Enable Falling Edge Trigger (Button press pulls pin low) for line 13


    NVIC->ISER1 |= (1U << 8); // EXTI15_10_IRQn  Enable IRQ 40 in NVIC from corem4 typedef
}

void EXTI15_10_IRQHandler(void) {
    if (EXTI_PR1 & (1U << 13)) { // checks 13 bit

      //  GPIOA->BSRR = (1U << 5); // turn on PA5

        EXTI_PR1 = (1U << 13); //write in PR1 1 -> write 1 to clean the flag
    }
}

void USART2_IRQHandler(void)
{
  if(USART2->ISR & (1U << 6))
  {
  GPIOA->BSRR = (1U << 5); // turn on PA5
  USART2->ICR = (1U << 6); //clear TC bit
  }
}







int main(void)
 {
	static const unsigned char address = 0x23;
	static const unsigned char config_value = 0x10;
  unsigned char receive_8[2] = {0};
  static unsigned short lux_value;


    system_clock_config();
    gpio_config();
    interrupt_init();
    usart2_init();
	  i2c1_init();
	  i2c1_transmit(address, config_value, 1);

    while (1)
    {

	          i2c1_receive(address, receive_8, 2);
            lux_value = ((unsigned short)receive_8[0] << 8 | receive_8[1]);
            usart2_transmit_unit16t_minicom(lux_value);
            delay(300);
            GPIOA->BSRR = 1U << 21;
            delay(300);

    }
}

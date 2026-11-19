#include "registers.h"



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


    NVIC_ISER1 |= (1U << 8); // EXTI15_10_IRQn  Enable IRQ 40 in NVIC from corem4 typedef
}

void EXTI15_10_IRQHandler(void) {
    if (EXTI_PR1 & (1U << 13)) { // checks 13 bit

        GPIOA->BSRR = (1U << 5); // turn on PA5

        EXTI_PR1 = (1U << 13); //write in PR1 1 -> write 1 to clean the flag
    }
}

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

//TODO make i2c init funktion with &I2C1 and address as an input

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
  I2C1->TIMINGR = FASTSPEED; //prescaler 1, scldel 3, sdadel 2, sclH 3, scll 9. configure time delay and clock high and low time
  I2C1->CR1 |= (0x1U << 0); //enable I2C
  I2C1->CR2 &= ~(0x1U << 11); //setup address for 7 bit
  I2C1->CR2 &= ~(0x1FFU << 0); //clear the address
  I2C1->CR2 |= (0x23 << 1); //0x23 Address for the light sensor
  I2C1->CR2 &= ~(0x1U << 10); //set as a write transfer
  I2C1->CR2 &= ~(0xFFU << 16); //Clear number of send/receive bytes
  I2C1->CR2 |= (0x1U << 16); //send/receive 1 byte NBYTES
  I2C1->CR2 |= (0x1U << 25); //Autoend enable stop after NBYTES are sent
  I2C1->CR2 &= ~(0x1U << 24); //RELOAD The transfer is completed after the NBYTES data transfer
}

void i2c1_transmit(void)
{
I2C1->CR2 |= (0x1U << 13); //I2C START
while(!(I2C1->ISR & (0x1U << 1))); //wait until the
I2C1->TXDR = 0XAAU;
while(!(I2C1->ISR & (0x1U << 5)));
I2C1->ICR |= (0x1U << 5); //Clear STOP
}

void i2c_data_transfer(void)
{


}
int main(void) {
    system_clock_config();
    gpio_config();
    interrupt_init();
    usart2_init();

   const unsigned char test_array[5] = {0,1,2,3,4};



    while (1)
    {
	delay(1000);
	GPIOA->BSRR = 1U << 21;
	usart2_transmit(test_array);

    }
}

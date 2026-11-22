#ifndef REGISTERS_H
#define REGISTERS_H

typedef struct {
    volatile unsigned int CR;              // Offset 0x00
    volatile unsigned int RESERVED0;       // Offset 0x04
    volatile unsigned int CFGR;            // Offset 0x08
    volatile unsigned int RESERVED1[16];   // Pads up to 0x4C

    volatile unsigned int AHB2ENR;         // Offset 0x4C
    volatile unsigned int RESERVED2[2];    // Pads 0x50 and 0x54 (Fixed from 3)

    volatile unsigned int APB1ENR1;        // Offset 0x58
    volatile unsigned int RESERVED3;       // Offset 0x5C (APS1ENR2 / reserved)
    volatile unsigned int APB2ENR;         // Offset 0x60

    volatile unsigned int RESERVED4[9];    // Pads 0x64 to 0x84 (Fixed from 10)

    volatile unsigned int CCIPR;           // Offset 0x88
} RCC_TypeDef;

#define RCC  ((RCC_TypeDef *) 0x40021000U)

typedef struct {
	unsigned int CR1;
	unsigned int CR2;
	unsigned int CR3;
	unsigned int BRR;
	unsigned int GTPR;
	unsigned int RTOR;
	unsigned int RQR;
	unsigned int ISR;
	unsigned int ICR;
	unsigned int RDR;
	unsigned int TDR;
} USART_Typedef;

#define USART2 ((volatile USART_Typedef *) 0x40004400U)

typedef struct {
	unsigned int CR1;
	unsigned int CR2;
	unsigned int OAR1;
	unsigned int OAR2;
	unsigned int TIMINGR;
	unsigned int TIMOUTR;
	unsigned int ISR;
	unsigned int ICR;
	unsigned int PECR;
	unsigned int RXDR;
	unsigned int TXDR;
} I2C_Typedef;

#define I2C1 ((volatile I2C_Typedef *) 0x40005400U)
#define FASTSPEED 0x10320309U
#define STANDARDSPEED 0x30420F13U


typedef struct {
  unsigned int MODER;
  unsigned int OTYPER;
  unsigned int OSPEEDR;
  unsigned int PUPDR;
  unsigned int IDR;
  unsigned int ODR;
  unsigned int BSRR;
  unsigned int LCKR;
  unsigned int AFRL;
  unsigned int AFRH;
  unsigned int BRR;
  unsigned int ASCR;
} GPIO_Typedef;

#define GPIOA ((volatile GPIO_Typedef *) 0x48000000U)
#define GPIOB ((volatile GPIO_Typedef *) 0x48000400U)
#define GPIOC ((volatile GPIO_Typedef *) 0x48000800U)

#endif

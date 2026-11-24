#ifndef USART_H
#define USART_H

void usart2_init(void);
void usart2_transmit(const unsigned char*, const unsigned char);
void usart2_transmit_uint8t_minicom(unsigned char);
void usart2_transmit_unit16t_minicom(unsigned short);





#endif

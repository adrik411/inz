/*
 * moj_usart.h
 *
 * Created: 06.04.2023 22:57:11
 *  Author: guzik
 */ 


#ifndef USART_H_
#define USART_H_

#define FOSC 16000000 // Clock Speed
#define BAUD 38400 // ???
#define MYUBRR FOSC/16/BAUD-1

void USART_Init(unsigned int ubrr); // 9600 dla 8mhz jest niby git ALE korzytam z wewn oscylatora który jest mniej stabilny


unsigned char USART_Receive(void);


void USART_Transmit(unsigned char data);


void USART_Transmit_arr(unsigned char *data, uint8_t dlugosc);


uint8_t usart_putchar_printf(unsigned char var, FILE *stream);


#endif /* MOJ_USART_H_ */
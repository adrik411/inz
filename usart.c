/*
 * sart.c
 *
 * Created: 29/12/2023 09:23:22
 *  Author: ak
 */ 



#include <avr/io.h>
#include <stdio.h>
#include "usart.h"

void USART_Init(unsigned int ubrr) 
{
	/*Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	//Enable receiver and transmitter */
	UCSR0B = (1<<TXEN0);
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (3<<UCSZ00);
}

unsigned char USART_Receive(void)
{
	/* Wait for data to be received */
	while (!(UCSR0A & (1<<RXC0)));
	/* Get and return received data from buffer */
	return UDR0;
}

void USART_Transmit(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1<<UDRE0)))
	;
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void USART_Transmit_arr(unsigned char *data, uint8_t dlugosc)
{
	
	uint8_t i = 0;
	
	for(i = 0; i < dlugosc; i++){
		
		/* Wait for empty transmit buffer */
		while (!(UCSR0A & (1<<UDRE0)));
		/* Put data into buffer, sends the data */
		UDR0 = data[i];
		
	}

}

uint8_t usart_putchar_printf(unsigned char var, FILE *stream)
{
	if (var == '\n') USART_Transmit('\r');
	USART_Transmit(var);
	return 0;
}

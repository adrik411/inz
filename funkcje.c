/*
 * funkcje.c
 *
 * Created: 29/12/2023 09:24:41
 *  Author: guzik
 */ 
#include <avr/delay.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/atomic.h>
#include <stdio.h>
#include "funkcje.h"

void usart_transmit_9bit(unsigned int data)
{
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1<<UDRE0)));
	/* Copy 9th bit to TXB8 */
	UCSR0B &= ~(1<<TXB80);
	if (data & 0x0100){
		UCSR0B |= (1<<TXB80);
	}
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void usart_transmit(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1<<UDRE0)));
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void usart_init(unsigned int ubrr)
{
	/*Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	//Enable receiver and transmitter */
	UCSR0B = (1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (3<<UCSZ00);
}

void usart_init_9bit(unsigned int ubrr)
{
	/*Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	//Enable receiver and transmitter */
	UCSR0B = (1<<TXEN0) | (1<<UCSZ02);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<UCSZ00) | (1<<UCSZ01);
}

void init_int0(){
	
	EICRA |= (1<<ISC01);
	
}

void init_timer_licznik_hz(){
	
	TCCR0B = (1<<CS02) | (1<<CS01) | (1<<CS00);
	TIMSK0 = (1<<TOIE0);
	
}

void init_timer(){
	
	//OCR1AH = 0xF3;
	//OCR1AL = 0x2f;
	OCR1AH = 0xF4;
	OCR1AL = 0x3b;
	//OCR1AH = 0xf0;
	//OCR1AL = 0x2a; // 62500 max aby liczylo co sekunde
	TCCR1B |= (1<<WGM12) | (1<<CS12);  // 16 mhz / 256 = 62500
	TIMSK1 |= (1<<OCIE1A);
	
}

void enable_int0(){
	
	// jedna operacja niem usz eblokowac ATOMIC
	EIMSK |= (1<<INT0);
	
}

void disable_int0(){
	
	// jedna operacja niem usz eblokowac ATOMIC
	EIMSK &= ~(1<<INT0);
	
}


void init_adc()
{
	// Select Vref=AVcc
	ADMUX |= (1<<REFS0);
	//set prescaller to 128 and enable ADC
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADEN);
}

int64_t kompensacja_vzas(float pomiar_vzas){
	
	//float x = pomiar_vzas;
	float x = pomiar_vzas;
	int64_t kompensacja = (35) + (-64586) + (45241*x) + (-9720*pow(x,2)) + (643*pow(x,3));
	
	return kompensacja;
	
}

uint16_t ReadADC(uint8_t ADCchannel)
{
	   ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
	//select ADC channel with safety mask
	   ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F);
	//single conversion mode
	   ADCSRA |= (1<<ADSC);
	// wait until ADC conversion is complete
	   while( ADCSRA & (1<<ADSC));
	   return ADC;
	}
}

void WDT_off(void) // to dziala jak chce i nie myli w przeciwienstwie do wdt_disable
{
	asm volatile(
	"cli"      "\n\t"
	"push r16" "\n\t" // bo nwm czy jest uzywane gdzies wiec musze dodac na stacka
	"wdr"      "\n\t"
	"in r16, 0x34" "\n\t"
	"andi r16, (0xff & (0 << 3))" "\n\t"
	"out 0x34, r16" "\n\t"
	"lds r16, 0x60" "\n\t"
	"ori r16, (1 << 4) | (1 << 3)" "\n\t"
	"sts 0x60, r16" "\n\t"
	"ldi r16, (0 << 3)" "\n\t"
	"sts 0x60, r16" "\n\t"
	"pop r16"  "\n\t"
	"sei" "\n\t"
	//"ret" "\n\t"
	);
}

float pomiar_adc_usredniony(uint8_t kanal, uint16_t ile_razy){
	
	float srednia = 0;
	
	for(uint16_t i = 0; i<ile_razy; i++){
		
		_delay_us(25);
		float vbg = (VZAS/1024) * ReadADC(kanal); //avcc okolo 4.97v
		srednia = srednia + vbg;
		
	}
	
	srednia = srednia / ile_razy;
	
	return srednia;
	
	
}

float hz_na_um_fe(int32_t x){
	
	return 2.078 + (-9.88E-04*x) + (2.44E-07*pow(x,2)) + (-4.3E-11*pow(x,3)) + (3.33E-15*pow(x,4));                  
	
}

float hz_na_um_al(int32_t x){
	
	return 1.68 + (-2.69E-04*x) + (2.14E-08*pow(x,2)) + -(1.28E-12*pow(x,3)) + (3.54E-17*pow(x,4));
	
}

int16_t kalibracja_0_um_fe(int16_t blad,float um){
	
	if(um < 0) um = 0; // czasami wychodzi wynik pomiaru poni¿ej 0 um
	
	float x = um;
	
	if(x >= 0){
	   float procent_bledu =  100 + (-239*x) + (874*pow(x,2)) + (-1957*pow(x,3)) + (2395*pow(x,4)) + (-1609*pow(x,5)) + (562*pow(x,6)) + (-83.1*pow(x,7)) + (1.46*pow(x,8));
	   blad = blad * (procent_bledu/100);
	   printf("PROCENT BLEDU %.3f, BLAD %d \n",procent_bledu,blad);
	   return blad;
	}
	else return 0;
	
}

int16_t kalibracja_0_um_al(int16_t blad,float um){
	
	if(um < 0) um = 0; // czasami wychodzi wynik pomiaru poni¿ej 0 um
	
	float x = um;
	
	if(x >= 0){
		float procent_bledu =  100 + (-119*x)+ (65.6*pow(x,2)) + (-26.5*pow(x,3)) + (5.16*pow(x,4));
		blad = blad * (procent_bledu/100);
		printf("PROCENT BLEDU %.3f, BLAD %d \n",procent_bledu,blad);
		return blad;
	}
	else return 0;
	
}

void kompensacja_temp(float roznica_adc, int16_t *kompensacja_hz){
	
	if(roznica_adc > 0){
		while(roznica_adc > 0.105){
			roznica_adc = roznica_adc - 0.105;
			*kompensacja_hz++;
		}

	}

	else if(roznica_adc < 0){
		while(roznica_adc < -0.105){
			roznica_adc = roznica_adc + 0.105;
			*kompensacja_hz--;
		}

	}
	
}
/*
 * proj_inz.c
 *
 * Created: 28/12/2023 03:40:34
 * Author : guzik
 */ 

#define F_CPU 16000000UL
#define VREF 5
#define POT 10000
#define _OPEN_SYS_ITOA_EXT

#include <avr/delay.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/atomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "funkcje.h"
#include "usart.h"


const uint32_t zero_um_fabrycznie_fe = 92378; 
const uint32_t zero_um_fabrycznie_al = 119470;

volatile uint32_t HZ_prostokata = 0;
volatile uint32_t HZ_prostokata_ost = 0;
volatile uint32_t wart_kalibracja_powietrze = 0;
volatile uint32_t wart_kalibracja_metal = 0;
volatile uint32_t zapetlono_tyle = 0;
volatile int32_t pomiar_2 = 0;
volatile uint8_t toggle = 0;
volatile uint8_t t0 = 0;
volatile char buffer [sizeof(uint32_t)*8+1];
volatile uint32_t startowe_HZ_prostokata = 0;

int16_t kompensacja_hz = 0;

float startowy_pomiar_adc = 0.0;
float pomiar_vzas = 0.0;
float pomiar_adc = 0.0;
float roznica_adc = 0.0;
float um = 0;
int32_t pomiar_2_c = 0;

volatile uint16_t blad_zero_al = 0;
volatile uint16_t blad_zero_fe = 0;

static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);

int main(void)
{
	wdt_enable(WDTO_4S); 
	stdout = &mystdout;
	init_int0();
	init_timer();
	USART_Init(MYUBRR);
	init_timer_licznik_hz();
	init_adc();
	enable_int0();

	sei();
	
    while (1) 
    { 
		wdt_reset();

		if(toggle){ // toggle dla licznika
			
	        toggle = !(toggle);
			if(!(startowe_HZ_prostokata) && pomiar_2 > 98500) startowe_HZ_prostokata = pomiar_2; // dla debugowania zapisuje początkową częstotlwiość
			if(!(startowy_pomiar_adc)) startowy_pomiar_adc = pomiar_adc_usredniony(1,500); // dla debugowania zapisuje początkowe napięcie na wejściu analogowym (termistor)
			
			pomiar_adc = pomiar_adc_usredniony(1,500);
			roznica_adc = startowy_pomiar_adc - pomiar_adc; // od startowego pomiaru termistora robi róznice i na podstawie jej zmiany kompensuje Hz
			
                kompensacja_temp(roznica_adc, &kompensacja_hz);

			
			if(pomiar_2 <= (96969)){ // wykryta stal 
				
				pomiar_2_c = pomiar_2 - (96969);
				um = hz_na_um_fe(abs(pomiar_2_c+kompensacja_hz));
				int16_t blad = kalibracja_0_um_fe(blad_zero_fe,um);
				um = hz_na_um_fe(abs(pomiar_2_c+blad+kompensacja_hz));
				printf("FE %d | ",blad);
			}
			else if (pomiar_2 >= (106700)) // wykryte aluminium / miedź
			{
				
				pomiar_2_c = pomiar_2 - (106700);
				um = hz_na_um_al(abs(pomiar_2_c));
				int16_t blad = kalibracja_0_um_al(blad_zero_al,um);
				um = hz_na_um_al(abs(pomiar_2_c+blad));
				printf("AL %d | ",blad);;

			}
			
			
			else um = 0;
			
			printf("%.3F startowe mv | ", startowy_pomiar_adc);
			printf("%ld startowe hz | ", startowe_HZ_prostokata);
			printf("%ld hz | ", pomiar_2 );
			printf("%ld hz roznica | ", pomiar_2_c );
			printf("%.3F um | ", um );
			printf("%d kompensacja hz | ", kompensacja_hz);
			printf("Vthermo = %.3F\n", pomiar_adc_usredniony(1,800));
			kompensacja_hz = 0;
			}
		

    }
}

ISR(TIMER1_COMPA_vect){ // co 1 sekundę się liczy
	
	toggle = !(toggle);

	  if(toggle){
		  pomiar_2 = (zapetlono_tyle*255) + t0; 
		  zapetlono_tyle = 0;
	  }
	t0 = TCNT0;
	TCNT0=0;
	
}

ISR(INT0_vect){ // kalibracaja punktu zero um
	
	if(pomiar_2 <= (96969)) // fe
	{
		printf("KALIB FE ");
		blad_zero_fe = zero_um_fabrycznie_fe - pomiar_2;
		printf("%d\n",blad_zero_fe);
	}
	
	
	else if (pomiar_2 >= (106700)) // al
	{
		printf("KALIB AL ");
		blad_zero_al = zero_um_fabrycznie_al - pomiar_2;
		printf("%d\n",blad_zero_al);
	}
	else printf("ERR\n");
	
}

ISR(TIMER0_OVF_vect){ // licznik pod który podpięty jest komparator LM393P
	
	zapetlono_tyle++;
	
}

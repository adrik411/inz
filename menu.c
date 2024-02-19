/*
 * menu.c
 *
 * Created: 11.04.2023 14:53:04
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
#include "OnLCDLib.h"
#include "menu.h"


volatile uint8_t blokada_count = 0;
volatile unsigned char keypad_bufor_volatile[2] = {0,0};
	
volatile uint8_t menu = 1;

void update_menu(){
	
	static uint8_t zamknij = 1;
	
	switch(menu){
		
		case 0:{
			
			menu = 1;
			break;
			
		}
		
		case 1:{

				//LCDClear();
				//LCDGotoXY(1,1);
				//LCDWriteString("Zmien PIN"); // tu jak sie wpisze poprawny pin (1,2,3) to potem sie odpowiedni zmieni
				
			while(1){ // petla kod ma tu siedziec dopoki nie wpiszesz dobrego pinu lub nie anulujesz wpisywnaia
				wdt_reset();
			}
			
			break;
			
		}
		
		
		default:{
			
			//LCDClear();
			//LCDWriteString("ERROR");
			//_delay_ms(2000);
			wdt_reset();
			menu = 1;
		}
		
	}
	
}

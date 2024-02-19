/*
 * menu.h
 *
 * Created: 11.04.2023 14:53:55
 *  Author: guzik
 */ 


#ifndef MENU_H_
#define MENU_H_

#define SLEEP_TIMER_WYDLUZ 300
#define MAX_DLUGOSC_WIAD 64
#define BLOKADA_TRIES 2
#define ILOSC_OST_LOG 4

void WDT_off(void);
void update_menu();

#endif /* MENU_H_ */
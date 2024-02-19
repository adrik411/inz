/*
 * funkcje.h
 *
 * Created: 29/12/2023 09:25:00
 *  Author: ak
 */ 


#ifndef FUNKCJE_H_
#define FUNKCJE_H_


void init_int0();
void init_timer();
void enable_int0();
void init_adc();
void disable_int0();
uint16_t ReadADC(uint8_t ADCchannel);
void usart_init(unsigned int ubrr);
void usart_transmit(unsigned char data);
void usart_transmit_9bit(unsigned int data);
void usart_init_9bit(unsigned int ubrr);
float pomiar_adc_usredniony(uint8_t kanal, uint16_t ile_razy);
int64_t kompensacja_vzas(float pomiar_vzas);
float hz_na_um_fe(int32_t x);
float hz_na_um_al(int32_t x);
int16_t kalibracja_0_um_fe(int16_t blad,float um);
int16_t kalibracja_0_um_al(int16_t blad,float um);
void kompensacja_temp(float roznica_adc, int16_t *kompensacja_hz);

#endif /* FUNKCJE_H_ */

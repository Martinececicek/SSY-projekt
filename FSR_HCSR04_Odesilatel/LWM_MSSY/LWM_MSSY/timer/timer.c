/*
 * timer.c
 *
 * Created: 2/20/2024 10:45:16
 *  Author: Student
 */ 
#include "timer.h"
#include "../makra.h"
#include <avr/interrupt.h>
void Timer1_cmp_start2 (uint16_t promenna){
cli(); //zakazat preruseni
TCCR1A = 0; //vycistit kontrolni registry
TCCR1B = 0; //
TIMSK1 = 0; //

//nastavit hodnotu pro porovnani
//OCR1A = porovnani;
OCR1A = ((F_CPU/2/1024/promenna)-1);
// CTC mod :
TCCR1B |= ( 1 << WGM12);
// 1024 preddelicka:
//TCCR1B |= ( 1 << CS10 ) ;
//TCCR1B |= ( 1 << CS12 ) ;
TCCR1B |= 5;

// povolit preruseni , pokud budete POTREBOVAT:
TIMSK1 |= ( 1 << OCIE1A);

// vystup na pin OC1A, toggle
TCCR1A |= ( 1 << COM1A0);
sei(); //povolit globalni preruseni
}

void Timer1_cmp_start (){
	cli(); //zakazat preruseni
	TCCR1A = 0; //vycistit kontrolni registry
	TCCR1B = 0; //
	TIMSK1 = 0; //
	
	//nastavit hodnotu pro porovnani
	//OCR1A = porovnani;
	OCR1A = ((F_CPU/2/1024/2)-1);
	// CTC mod :
	TCCR1B |= ( 1 << WGM12);
	// 1024 preddelicka:
	//TCCR1B |= ( 1 << CS10 ) ;
	//TCCR1B |= ( 1 << CS12 ) ;
	TCCR1B |= 5;
	
	// povolit preruseni , pokud budete POTREBOVAT:
	TIMSK1 |= ( 1 << OCIE1A);
	
	// vystup na pin OC1A, toggle
	TCCR1A |= ( 1 << COM1A0);
	sei(); //povolit globalni preruseni
}void Timer1Stop(){
	TCCR1B=0;
}
void Timer2_fastpwm_start (uint8_t strida) {
	cli(); //zakazat preruseni
	TCCR2A = 0; //vycistit kontrolni registry
	TCCR2B = 0;
	TIMSK2 = 0; //
	//nastavit hodnotu pro PWM
	OCR2A = (255*strida)/100;
	// fastpwm mod:
	TCCR2A |= (1<<WGM21);
	TCCR2A |= (1<<WGM20);
	// 1024 preddelicka:
	TCCR2B |= 5;
	// povolit preruseni , pokud bude pot?eba...:
	TIMSK2 |= (1<<TOIE2);
	// vystup na pin OC2A
	TCCR2A |= (1<<COM2A1) ;
	sei(); // povolit globalni preruseni
}

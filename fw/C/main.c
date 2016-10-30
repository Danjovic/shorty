/*
Shorty - Short Circuit Finder
from Jaromir Sukuba https://hackaday.io/project/3635
Adapted for ATTiny45/85 by Danjovic
danjovic@hotmail.com

*/
// Libraries
#include <avr/io.h>
#include <util/delay.h>

//#include "pinout_atmegax8.h"
#include "pinout_attynyx5.h"

#ifndef F_CPU
#define F_CPU 12000000UL
#endif

/*
// Pinout Definition
#define  ADC_DDR        DDRC
#define  ADC_BIT        5
#define  ADC_CHANNEL    5

#define  BUTTON_PORT    PORTD
#define  BUTTON_DDR     DDRD
#define  BUTTON_PIN     PIND
#define  BUTTON_BIT     3

#define  LED_PORT       PORTB
#define  LED_DDR        DDRB
#define  LED_BIT        5

#define  BUZZER_PORT    PORTD
#define  BUZZER_DDR     DDRD
#define  BUZZER_BIT     6

#define  PROBE_PWR_PORT PORTC
#define  PROBE_PWR_DDR  DDRC
#define  PROBE_PWR_BIT  3
*/

#define LED_TIMER_ON    500
#define LED_TIMER_MAX   550

// Auxiliary Macros
#define BUTTON_PRESS() ((BUTTON_PIN & (1<<BUTTON_BIT))==0)

#define LED_ON()  LED_PORT |=  (1<<LED_BIT)
#define LED_OFF() LED_PORT &= ~(1<<LED_BIT)

#define BUZZER_ENABLE()  BUZZER_DDR |=  (1<<BUZZER_BIT)
#define BUZZER_DISABLE() BUZZER_DDR &= ~(1<<BUZZER_BIT)

#define PROBE_PWR_OFF() PROBE_PWR_PORT |=  (1<<PROBE_PWR_BIT)
#define PROBE_PWR_ON()  PROBE_PWR_PORT &= ~(1<<PROBE_PWR_BIT)

// Program definitions
#define AVERAGE_SIZE      5
#define ADC_TRESHOLD    200
#define PROBE_TIMEOUT 15000

// Variables
unsigned int led_timer,led_;
unsigned int probe_timer;

unsigned int adc_val;
unsigned int adc_val_avg[AVERAGE_SIZE],adc_val_acc;
unsigned char adc_val_avg_ptr,i,probe_state;

// Main Function
int main (void)
{
	// Initialize IO ports
	BUTTON_DDR      &= ~(1 << BUTTON_BIT);     // Button pin as input
	BUTTON_PORT     |=  (1 << BUTTON_BIT);     // Button pull up 	
	BUZZER_DDR      |=  (1 << BUZZER_BIT);     // Buzzer pin as ouptut
	LED_DDR         |=  (1 << LED_BIT);        // LED pin as ouptut
	PROBE_PWR_DDR   |=  (1 << PROBE_PWR_BIT); // Transistor pin as ouptut
	
	
	// Initialize Timer
	OCR0A = 120;                        // Timer 0 initial compare value 
	TCCR0A = (1<<COM0A0) | (1<<WGM01) ; // CTC mode, TOP=OCR0A, toggle output on match
	TCCR0B = (1<<CS02) | (1<<CS00);     // Prescaler factor 1024
	
	// Initialize ADC
	ADCSRA = (1<<ADEN)  | (1<<ADPS2) | (1<<ADPS0); // Enable ADC with prescaler factor 32
	ADMUX  = (1<<REFS0) | (ADC_CHANNEL & 0x07) ;  // Vref=AVcc, Singled ended input 
	//  ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading
	
	
	// Set initial state for output pins
	PROBE_PWR_ON();    
	BUZZER_DISABLE();
	LED_OFF();       
	
	// Initialize variables
	probe_timer = 0;
	led_timer   = 0;
	probe_state = 1;
	
	// Enter the measurement loop
	while (1) {
		//if probe is active, blink a LED
		led_timer++;
		if ((led_timer>LED_TIMER_ON) && (probe_state==1))
			LED_ON();
		else
			LED_OFF() ;
			
		if (led_timer>LED_TIMER_MAX)
			led_timer = 0;

		//now decide, if the probe wasn't inactive for too long  
		probe_timer++;
		if (probe_timer>PROBE_TIMEOUT && (probe_state==1)  ) {
			probe_state=0;  
		}

		//inactive probe code
		if (probe_state==0) {	
			PROBE_PWR_OFF();
			BUZZER_DISABLE();
			_delay_ms(1);
			if ( BUTTON_PRESS() ) {  
				probe_state=1;
				probe_timer = 0;
			}
		}

		//active probe code  
		if (probe_state==1) {
			PROBE_PWR_ON();
			_delay_ms(1);
			//take adc sample
			ADCSRA |= (1<<ADSC);          // Start conversion
			while (ADCSRA & (1<<ADSC));   // wait for conversion to complete
			adc_val = ADCW;

			//take value and put it into floating average
			adc_val_avg[adc_val_avg_ptr++] = adc_val;
			if (adc_val_avg_ptr>AVERAGE_SIZE) 
				adc_val_avg_ptr = 0;

			adc_val_acc = 0;
			for (i=0;i<AVERAGE_SIZE;i++)
				adc_val_acc = adc_val_acc + adc_val_avg[i];
				
			adc_val_acc = adc_val_acc / 20;
			adc_val = adc_val_acc;

			//decide what to do
			if (adc_val>ADC_TRESHOLD) adc_val = ADC_TRESHOLD;
				adc_val = adc_val + 2;
				
			if (adc_val<ADC_TRESHOLD) {
				OCR0A = adc_val;
				BUZZER_ENABLE();
				probe_timer = 0;
			} else 	{
				BUZZER_DISABLE();
			}
		}
	}
}



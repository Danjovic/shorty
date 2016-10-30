#include <avr/io.h>

// Pinout Definition Atmega 48 88 168 328
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
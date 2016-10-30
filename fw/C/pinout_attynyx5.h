#include <avr/io.h>

// Pinout Definition Attiny 45 85
#define  ADC_DDR        DDRB
#define  ADC_BIT        2
#define  ADC_CHANNEL    1

#define  BUTTON_PORT    PORTB
#define  BUTTON_DDR     DDRB
#define  BUTTON_PIN     PINB
#define  BUTTON_BIT     4

#define  LED_PORT       PORTB
#define  LED_DDR        DDRB
#define  LED_BIT        1

#define  BUZZER_PORT    PORTB
#define  BUZZER_DDR     DDRB
#define  BUZZER_BIT     0

#define  PROBE_PWR_PORT PORTB
#define  PROBE_PWR_DDR  DDRB
#define  PROBE_PWR_BIT  3
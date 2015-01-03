#include <avr/io.h>
#include <util/delay.h>

#ifndef F_CPU
#define F_CPU 12000000UL
#endif
#define _BAUD	9600	// Baud rate (9600 is default)
#define _UBRR	(F_CPU/16)/_BAUD - 1	// Used for UBRRL and UBRRH 


#define  BUZZER_PORT PORTD
#define  BUZZER_DDR DDRD
#define  BUZZER 6

#define LED_TIMER_ON  500
#define LED_TIMER_MAX  550

unsigned int adc_val;
#define AVERAGE_SIZE 5
unsigned int adc_val_avg[AVERAGE_SIZE],adc_val_acc;
unsigned char adc_val_avg_ptr,i,probe_state;
unsigned int led_timer,led_;
unsigned int probe_timer;

int main (void)
{
  UBRR0H = ((_UBRR) & 0xF00);
  UBRR0L = (uint8_t) ((_UBRR) & 0xFF); 
  UCSR0B |= _BV(TXEN0);
  UCSR0B |= _BV(RXEN0);
  
    DDRD |= (1 << DDD6);
    DDRD &= ~(1 << DDD3);
    PORTD |= (1 << PORTD3);
    DDRC |= (1 << DDC3);
    DDRB |= (1 << DDC5);
    PORTC &= ~(1 << PORTC3);
    OCR0A = 120;
    TCCR0A = 0x42;
    TCCR0B = 0x05;
  ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS0);
  ADMUX=0x45;
//  ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading
probe_timer = 0;
led_timer = 0;
probe_state=1;
while (1)
{
  //if probe is active, blink a LED
  led_timer++;
  if ((led_timer>LED_TIMER_ON)&(probe_state==1))
    PORTB |= (1<<5);
  else
    PORTB &= ~(1<<5);
  if (led_timer>LED_TIMER_MAX)
    led_timer = 0;
  
  //now decide, if the probe wasn't inactive for too long  
  probe_timer++;
  if (probe_timer>15000)
    {
    if (probe_state==1)
      probe_state=0;
    }
  
  //inactive probe code
  if (probe_state==0)
    {
    PORTC |= (1 << PORTC3);
    DDRD &= ~(1 << DDD6);
    _delay_ms(1);
    if ((PIND&(0x08))==0)
      {
      probe_state=1;
      probe_timer = 0;
      }
    }

  //active probe code  
  if (probe_state==1)
  {
  PORTC &= ~(1 << PORTC3);
  _delay_ms(1);
  //take adc sample
  ADCSRA |= (1<<ADSC);          // Start conversion
  while (ADCSRA & (1<<ADSC));   // wait for conversion to complete
  adc_val = ADCW;
  
  //take value and put it into floating average
  adc_val_avg[adc_val_avg_ptr++] = adc_val;
  if (adc_val_avg_ptr>AVERAGE_SIZE) adc_val_avg_ptr = 0;
  adc_val_acc = 0;
  for (i=0;i<AVERAGE_SIZE;i++)
    adc_val_acc = adc_val_acc + adc_val_avg[i];
  adc_val_acc = adc_val_acc / 20;
  adc_val = adc_val_acc;

  //decide what to do
  if (adc_val>200) adc_val = 200;
  adc_val = adc_val + 2;
  if (adc_val<200)
    {
    OCR0A = adc_val;
    DDRD |= (1 << DDD6);
    probe_timer = 0;
    }
  else
    {
    DDRD &= ~(1 << DDD6);
    }
  }
  
  
  }
  
}
/*

void usart_tx_b(uint8_t data)
{
while (!(UCSR0A & _BV(UDRE0)));
UDR0 = data;
}

void usart_tx_hexa (uint8_t value)
{
uint8_t temp;
temp = value;
usart_tx_b('0');
usart_tx_b('x');
temp = ((temp>>4)&0x0F);
if (temp<10) temp = temp + '0';
else temp = temp + 'A'- 10;
usart_tx_b(temp);
temp = value;
temp = ((temp>>0)&0x0F);
if (temp<10) temp = temp + '0';
else temp = temp + 'A' - 10;
usart_tx_b(temp);
usart_tx_b(' ');
}
*/


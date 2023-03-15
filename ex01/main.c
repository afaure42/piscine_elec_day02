#include <avr/io.h>
#include <avr/interrupt.h>

//every page reference to the atmega238p datasheet is based on this link 
//https://pdf1.alldatasheet.fr/datasheet-pdf/download/1425004/MICROCHIP/ATMEGA328.html

#define PWM_FREQ F_CPU / 250
#define BRIGHT_INCREMENTS 64

volatile uint8_t cycle = 0;
volatile uint8_t direction = 1;

ISR(TIMER0_COMPA_vect)
{
	if (direction)
		cycle++;
	else
		cycle--;
	
	if (cycle == BRIGHT_INCREMENTS)
		direction = 0;
	if (cycle == 0)
		direction = 1;

	OCR1A =  cycle * (PWM_FREQ / BRIGHT_INCREMENTS);
}

int main()
{
	//we want to output to the led on PB1 wich is by luck OC1A as well

	//set port PB1 to output ( true ) using direction register DDRB
	DDRB = 1 << PB1;

	//clearing registers
	TCCR1B = 0;
	TCCR1A = 0;
	//as per the table 16-5 found in section 16.11.2 page 142-143
	//setting the clock source as the base clock on the timer counter control register B
	TCCR1B |= (1 << CS10);

	//setting fast pwm mode ( mode 14) as per the table 16-4 secion 16.11.1 page 141
	TCCR1B |= (1 << WGM12) | (1 << WGM13);
	TCCR1A |= (1 << WGM11);

	//as per the table 16-1 in section 16.11.1 page 140
	//setting port OC1A as output clear on match set a bottom
	TCCR1A |= (1 << COM1A1);

	//as per the CTC mode, specified on table 16-4 page 141
	//setting top at 250hz
	ICR1 = PWM_FREQ;

	//setting the output compare register at 1/10th of a second
	OCR1A = PWM_FREQ / 2;

	//setting timer0
	TCCR0B = 0;
	TCCR0A = 0;

	//mode CTC and base clock with 1024 prescaler
	TCCR0B |= 1 << CS00 | 1 << CS02;
	TCCR0A |= ( 1 << WGM01);

	TIMSK0 = 1 << OCIE0A; //enabling compA interrupt
	//setting
	
	OCR0A = F_CPU / 1024 / BRIGHT_INCREMENTS;


	sei();
	while (1)
	{
		//nothing
	}
}

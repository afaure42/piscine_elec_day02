#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

uint8_t volatile g_switch1_counter = 0;
uint8_t volatile g_switch2_counter = 0;
uint8_t volatile g_counter = 0;

void display()
{
	//clearing output port flags
	PORTB = ~(1 << PB0 | 1 << PB1 | 1 << PB2 | 1 << PB4);

	PORTB |= (g_counter & 0b0111) | ((g_counter & 0b1000) << 1);
}

//SW1 interrupt
ISR(INT0_vect)
{
	g_switch1_counter++; 
	if (g_switch1_counter & 1)
	{
		g_counter++;
		display();
	}
	_delay_ms(1);
	EIFR = 1 << INTF0;
}

//SW2 interrupt
ISR(TIMER0_COMPA_vect)
{
	g_counter--;	
	display();

}

ISR(TIMER0_COMPB_vect)
{
	g_counter--;
	display();
}

int main()
{
	//setting PD2 as input
	DDRD &= ~(1 << PD2);

	//this will be ugly bit since i have the timer0
	//as a interrupt for my switch i'm gonna have to manually do a step
	//so i need the switch as an output
	DDRD |= 1 << PD4;

	//setting pin PB0 PB1 PB2 and PB4 ( LEDS D1-4 ) as outputs
	DDRB = 1 << PB0 | 1 << PB1 | 1 << PB2 | 1 << PB4;


	//setting INT0 as logical change interrupts int the external interrupt control register
	EICRA = 1 << ISC00 | 1 << ISC10;

	//enabling interrupts INT0 and INT1 in the external interrupt mask register
	EIMSK = 1 << INT0 | 1 << INT1;


	TCCR0B = 0;
	TCCR0A = 0;

	//external clock with 0 prescaler
	TCCR0B |= 1 << CS02 | 1 << CS01;

	//mode CTC
	TCCR0A |= 1 << WGM01;

	//enabling COMPA interrupt
	TIMSK0 = 1 << OCIE0A | 1 << OCIE0B;

	OCR0A = 1;
	OCR0B = 0;


	//single step to timer0 so next time the button is pushed it will
	//trigger an interrupt
	PORTD = 1 << PD4;
	PORTD = 0;
	DDRD = 0;


	sei();

	while (1)
	{
		//nothing
	}
}

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t volatile counter = 0;

ISR(INT0_vect)
{
		counter++;
		if (counter & 1)
			PORTB ^= 1 << PB0;
		_delay_ms(1);
		EIFR = 1 << INTF0;
}

int main()
{
	//setting pin (PD2) connected to SW1 as input ( false ) so we can detect an interupt on INT0
	DDRD &= ~(1 << PD2);

	//setting pin ( PBo ) connected to LED D1 as ouput ( true )
	DDRB = 1 << PB0;

	//setting INT0 as logical change interrupt
	//in the external interrupt control register A
	EICRA = 1 << ISC00;

	//setting the bit INT0 so the external pin interrupt is enabled
	//in the external interrupt mask register
	EIMSK = 1 << INT0;

	//setting global interrupt enable bit
	sei();


	while (1)
	{
		// nothing
	}
}

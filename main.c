/*
 * desk_environ_mon.c
 *
 * Created: 2026-06-12 00:05:49
 * Author : JR
 */ 

#define F_CPU 1000000UL // Crystal Frequency, 1MHz (by default for a fresh ATmega328P).

#include <avr/io.h>
#include "driver/i2c/i2c.h"
#include <util/delay.h> // For blink

int main(void)
{
    /* Testing with Blink Program 
		PB1 -> Red LED -> 220ohms -> GND
		
		PB1 will be set as output. 
		
		Each I/O port consists of three register bits (DDxn, PORTxn and PINxn).
		
		DDxn bit DDRx Register sets direction of port. (1 for output, 0 for input).
		PORTxn activates internal pull-up resistor (1 for activated, for deactivated set to 0 OR pin already set as output).
		
		If pin is configured as output, 
			if PORTxn is logic one, port pin is driven high.
			if PORTxn is logic zero, port pin is driven low.
			
		x - numbering letter for port. 
		n - bit number.
	*/
	DDRB |= (1 << DDB1); // Configure direction of pin, OUTPUT.
	PORTB &= ~(1 << PORTB1); // Explicit pull-up resistor deactivation (this is already done implicitly by DDB1 = 1);
	
    while (1) 
    {
		// Repeat this sequence indefinitely.
		PORTB |= (1 << PORTB1); // Turn on LED
		_delay_ms(2000); // Turn on for 2 seconds.
		PORTB &= ~(1 << PORTB1); // Turn off LED
		_delay_ms(2000); // Turn off for 2 seconds.
	
    }
}


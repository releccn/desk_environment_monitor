/*
 * desk_environ_mon.c
 *
 * Created: 2026-06-12 00:05:49
 * Author : JR
 */ 

#define F_CPU 8000000UL // Crystal Frequency, configured to use internal 8MHz CO.
#include <avr/io.h>
#include "driver/i2c/i2c.h"
#include "driver/usart/usart.h"
#include <util/delay.h>
int main(void)
{
	usart_init();
	
    while (1) {
		usart_print("Hello World!\r\n");
		_delay_ms(1000);
    }
}


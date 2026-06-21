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
#include "tools/i2c_scanner.h"

int main(void)
{
	usart_init();
	i2c_init();
	
	usart_print("Starting scan...\r\n");
	i2c_scanner();
	usart_print("Scan complete.\r\n");
	while (1) {
		
	}
}


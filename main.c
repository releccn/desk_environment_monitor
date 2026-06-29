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
#include "driver/aht20/aht20.h"
#include "driver/bmp280/bmp280.h"
#include "driver/lcd/lcd.h"
#include "tools/i2c_scanner.h"
#include <util/delay.h>


int main(void) {

	usart_init();
	i2c_init();
	usart_print("USART and I2C Initialized\r\n");
	lcd_init();
	usart_print("LCD Initialized\r\n");
	lcd_setcursor(1, 1);
	lcd_print("Hello World");

	while (1) {
	
	}
	
}


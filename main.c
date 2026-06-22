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
#include "tools/i2c_scanner.h"
#include <util/delay.h>


int main(void) {

	usart_init();
	i2c_init();
	usart_print("USART and I2C Initialized\r\n");
	
	aht20_init();
	usart_print("AHT20 Initialized\r\n");
	
	uint8_t data[7];
	float convertedData[2];
	
	usart_print("Starting measurement taking!\r\n");
	while (1) {
		aht20_trigger_measurement();
		aht20_read_data(data);
		aht20_conversion(data, convertedData);
		

		usart_print("RH (%): ");
		usart_print_float(convertedData[0], 1);
		
		usart_print("Temperature (C): ");
		usart_print_float(convertedData[1], 1);
		_delay_ms(3000);
	}
	
}


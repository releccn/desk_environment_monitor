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
#include "tools/i2c_scanner.h"
#include <util/delay.h>


int main(void) {

	usart_init();
	i2c_init();
	usart_print("USART and I2C Initialized\r\n");
	
	uint8_t calibArr[24];
	
	aht20_init();
	usart_print("AHT20 Initialized\r\n");
	bmp280_init(calibArr);
	usart_print("BMP280 Initialized\r\n");
	
	uint8_t data_AHT20[7];
	uint8_t data_BMP280[6];
	
	float convertedData_AHT20[2];
	float convertedData_BMP280[2];
	
	usart_print("Starting AHT20+BMP280 measurement taking!\r\n");
	while (1) {
		
		aht20_trigger_measurement();
		aht20_read_raw(data_AHT20);
		aht20_conversion(data_AHT20, convertedData_AHT20);
		
		bmp280_read_raw(data_BMP280);
		bmp280_conversion(calibArr, data_BMP280, convertedData_BMP280);

		usart_print("Pressure (Pa): ");
		usart_print_float(convertedData_BMP280[1], 1);
		
		usart_print("RH (%): ");
		usart_print_float(convertedData_AHT20[0], 1);

		usart_print("(AHT20) Temperature (C): ");
		usart_print_float(convertedData_AHT20[1], 1);
		
		usart_print("(BMP280) Temperature (C): ");
		usart_print_float(convertedData_BMP280[0], 1);
		_delay_ms(3000);
	}
	
}


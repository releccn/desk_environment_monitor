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

#include <stdlib.h>
#include <string.h>

int main(void) {
	// LCD
	char lcd_line1[16];
	char lcd_line2[16];
	char val[4];
		
	// BMP280
	uint8_t bmp280_calibArr[24];
	uint8_t bmp280_dataArr[6];
	float bmp280_convertedValArr[2];
		
	// AHT20
	uint8_t aht20_dataArr[7];
	float aht20_convertedValArr[2];
	
	// USART (for monitoring).
	char uart_buf[8];
	
	usart_init();
	usart_print("USART: Init Complete\r\n");
	
	i2c_init();
	usart_print("I2C: Init Complete\r\n");
	
	aht20_init();
	usart_print("AHT20: Init Complete\r\n");
	
	bmp280_init(bmp280_calibArr);
	usart_print("BMP280: Init Complete\r\n");
	
	lcd_init();
	usart_print("LCD: Init Complete\r\n");

	while (1) {
		
		// Measure using AHT20. Place T and H values in aht20_convertedValArr.
		aht20_trigger_measurement();
		aht20_read_raw(aht20_dataArr);
		aht20_conversion(aht20_dataArr, aht20_convertedValArr);
		
		// Measure using BMP280. Place T and P values in bmp280_convertedValArr.
		bmp280_read_raw(bmp280_dataArr);
		bmp280_conversion(bmp280_calibArr, bmp280_dataArr, bmp280_convertedValArr);
		
		// USART (for monitoring).
		dtostrf(aht20_convertedValArr[1], 5, 2, uart_buf); // Temperature with 2 decimal places
		usart_print("T:");
		usart_print(uart_buf);

		dtostrf(aht20_convertedValArr[0], 5, 2, uart_buf); // Humidity with 2 decimal places
		usart_print(" H:");
		usart_print(uart_buf);

		dtostrf(bmp280_convertedValArr[1], 8, 2, uart_buf); // Pressure with 2 decimal places
		usart_print(" P:");
		usart_print(uart_buf);
		usart_print("\r\n");
		
		// Temperature, Humidity, Pressure as Integers.
		int8_t temperature_rounded = (int8_t)(aht20_convertedValArr[1] + 0.5f); 
		uint8_t humidity_rounded = (uint8_t)(aht20_convertedValArr[0] + 0.5f);
		uint8_t pressure_rounded = (uint8_t)(bmp280_convertedValArr[1] + 0.5f);
		
		// Display values on LCD
		itoa(temperature_rounded, val, 10);
		strcpy(lcd_line1, "T:");
		strcat(lcd_line1, val);
		lcd_setcursor(0, 5);
		lcd_print(lcd_line1);
		lcd_send_byte(0xDF, 1); // degree symbol
		lcd_send_byte('C', 1);  // C after degree
		
		itoa(humidity_rounded, val, 10);
		strcpy(lcd_line2, "H:");
		strcat(lcd_line2, val);
		strcat(lcd_line2, "%  P:");
		itoa(pressure_rounded, val, 10);
		strcat(lcd_line2, val);
		strcat(lcd_line2, "kPa");
	
		
		lcd_setcursor(1, 1);
		lcd_print(lcd_line2);
		
		_delay_ms(2000);
	}
	
}


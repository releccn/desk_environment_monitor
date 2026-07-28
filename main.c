
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
	
	_delay_ms(500); // Allow for hardware to stabilize.
	
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
	
	usart_init();
	
	i2c_init();
	
	aht20_init();
	
	bmp280_init(bmp280_calibArr);
	
	lcd_init();


	while (1) {
		
		// Measure using AHT20. Place T and H values in aht20_convertedValArr.
		aht20_trigger_measurement();
		aht20_read_raw(aht20_dataArr);
		aht20_conversion(aht20_dataArr, aht20_convertedValArr);
		
		// Measure using BMP280. Place T and P values in bmp280_convertedValArr.
		bmp280_read_raw(bmp280_dataArr);
		bmp280_conversion(bmp280_calibArr, bmp280_dataArr, bmp280_convertedValArr);
		

		// Temperature, Humidity, Pressure as Integers.
		int8_t temperature_rounded = (int8_t)(aht20_convertedValArr[1]); 
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




/*
 * i2c_scanner.c
 *
 * Created: 2026-06-20 15:58:25
 *  Author: JR
 */ 

/*
This program scans all devices on I2C bus. 

The purpose of this program is to determine the correct slave addresses
for the AHT20+BMP280, and OLED display.

The program will attempt to communicate to all possible 7-bit slave addresses (0x00 - 0x7F). 
If ACK is received, then the slave addresses is used by the device. 

General Flow:
	Start Condition -> SLA+W (I2C_Write_Byte function) -> I2C_SUCCESS (slave address used by device).
	
If I2C_ERROR is returned, then just skip it (i.e do not print it to UART, or else it will flood terminal).

This program will ONLY be used for the purposes of verifying the addresses of the devices thus will not
be over-engineered or further complicated.
*/

#include "i2c_scanner.h"

void i2c_scanner() {
	uint8_t address = 0x00;
	uint8_t endAddr = 0x7F;

	while (address <= endAddr) {
		if (i2c_start() == I2C_SUCCESS) {
			if (i2c_write_byte(address << 1) == I2C_SUCCESS) { // SLA+W (shifting address by 1, automatically makes LSB 0).
				usart_print("!!! DEVICE FOUND at 0x");
				usart_print_hex(address);
				usart_print("\r\n");
			}
		}
		i2c_stop();
		address++;
	}
}


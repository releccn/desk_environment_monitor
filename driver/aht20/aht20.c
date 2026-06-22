/*
 * aht20.c
 *
 * Created: 2026-06-21 22:12:42
 *  Author: JR
 */ 

/* 
This driver was created by referencing the ASAIR AHT20 datasheet found online.
*/

#define F_CPU 8000000UL // Crystal Frequency, configured to use internal 8MHz CO.

#include <util/delay.h>
#include "../i2c/i2c.h"

#define aht20SLA_R ((0x38 << 1) | 1) // 0x01111001 (SLA + R)
#define aht20SLA_W (0x38 << 1) // 0x01111000 (SLA + W)

void aht20_init() {
	/* According to data sheet, 
		Wait 40ms after power on.
		
		*Status word is obtained by sending 0x71 to the device.
		Before reading temperature and humidity values, check if calibration enable bit [3] of status word is SET.
			If calibration enable bit [3] is NOT SET, send initialization command (1. 0x08 2. 0x00) then wait 10ms.
	*/
	_delay_ms(40);
	uint8_t init_command[] = {0xBE, 0x08, 0x00}; // Sequence for calibration initialization.
	uint8_t status_command[] = {0x71}; // To obtain status word as stated in datasheet.
		
	i2c_write_bytes(aht20SLA_W, 1, status_command); // Select slave device.

	uint8_t status = 0x00; // Status word buffer
	i2c_read_bytes(aht20SLA_R, 1 , &status); // Write status word to status variable.
	
	
	if (status & (1 << 3)) { // If calibration enable bit is SET.
		return; // Complete initialization, no need to calibrate.
	} else {
		// Not calibrated. 
		i2c_write_bytes(aht20SLA_W, 3, init_command); // Perform initialization/calibration.
		_delay_ms(10);
	}
	return;
}
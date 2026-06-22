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

void aht20_trigger_measurement() {
	/* To trigger measurement,
		Send 0xAC command. Parameters of this command has two bytes: 0x33 and 0x00.
		Then wait 80ms for measurement to be completed.
	*/
	uint8_t trigger_command[] = {0xAC, 0x33, 0x00};
	i2c_write_bytes(aht20SLA_W, 3, trigger_command);
	_delay_ms(80);
}

void aht20_read_data(uint8_t *dataArr) {
	/* After 80ms has surpassed (after trigger measurement), measurement should be completed.
	
	To verify, check the status word bit [7] is 0, then the measurement is complete and
	the next six consecutive bytes can be read. 
	
	Otherwise, wait till the bit [7] is 0. (Poll every 15ms after the initial 80ms).
	
	This function modifies user defined data array (this array should hold 7 elements ONLY).
	*/
		// State, Humidity Data, Humidity Data,
		// Humidity Temperature, Temperature Data, Temperature Data
		// CRC data
	
	// Check if ready to read.
	uint8_t status = 0x80; // Status word buffer (Initial: 0b1000000)
	while (status & (1 << 7)) { // Wait till status word bit 7 is cleared.
		i2c_read_bytes(aht20SLA_R, 1 , &status); // Write status word to status variable.
		_delay_ms(15);
	}
	
	// Measurement ready.
	i2c_read_bytes(aht20SLA_R, 6, dataArr); // Write all 7 consecutive bytes to data array (includes CRC data).
}

void aht20_conversion(uint8_t *dataArr, float *convertedValArr) {
	/*
	Convert the data from byte form to readable values.
	dataArr - contains 7 consecutive bytes read from AHT20.
	convertedValArr - user-defined array to write converted values to.
	
	20 Humidity Data bits.
	20 Temperature Data bits.
	
	Extraction of these bits will be done by brute force since shifts are non uniform.
	
	dataArr[1] - Humidity Data [19:12] - MSB
	dataArr[2] - Humidity Data [11:4] 
	dataArr[3] - Humidity Data [3:0] (MSB nibble, four higher bits) - LSB
	
	dataArr[3] - Temperature Data [19:16] (LSB nibble, four lower bits) - MSB
	dataArr[4] - Temperature Data [15:8]
	dataArr[5] - Temperature Data [7:0] - LSB
	dataArr[6] - CRC Data (non relevant).
	
	
	*/
	
	uint32_t humidityBits = 0x00000000;
	uint32_t temperatureBits = 0x00000000;
	
	humidityBits = (uint32_t)dataArr[1] << 12; // MSB in bit 19 position.
	humidityBits |= (uint32_t)dataArr[2] << 4; 
	humidityBits |= (uint32_t)dataArr[3] >> 4; // LSB 
	
	temperatureBits = (uint32_t)(dataArr[3] & 0x0F) << 16; // Isolate lower nibble.
	temperatureBits |= (uint32_t)dataArr[4] << 8;
	temperatureBits |= (uint32_t)dataArr[5];
	
	// Conversion Calculations (floats)
	// 2^20 = 1048576
	// S_RH = Output Signal Relative Humidity
	// S_T = Output Signal Temperature
	
	float rh = ( (float)humidityBits / 1048576.0f ) * 100.0f; // Percentage
	float t =  (( (float)temperatureBits / 1048576.0f ) * 200.0f ) - 50f; // Celcius
	
	convertedValArr[0] = rh;
	convertedValArr[1] = t;
}
/*
 * bmp280.c
 *
 * Created: 2026-06-22 13:52:03
 *  Author: JR
 */ 

#include "../i2c/i2c.h"
#include "../usart/usart.h"

void bmp280_init(uint8_t *calibArr) {
	// calibArr should be 24 bytes large.
	/*
	To initialize the following is done:
		Verifying Chip ID == 0x58 in 0xD0 (ID Register),
		Read and populate user-defined calibration array,
		Write to 0xF4 (ctrl_meas):
			- initialize oversampling for temperature and pressure.
			- initialize power mode.
		Write to 0xF5 (config):
			- initialize rate (inactivity duration in normal mode).
			- initialize IIR filter time constant.
			- initialize I2C mode.
	*/ 
	uint8_t chip_id = 0x00;
	/*
	to verify chip_id,
		START -> SLA+W -> Send register address you want to read from -> STOP
		START -> SLA+R -> Read byte -> STOP
	*/ 
	i2c_write_bytes(bmp280SLA_W, 1, R_chip_id); // Selects chip_id register on BMP280.
	i2c_read_bytes(bmp280SLA_R, 1, &chip_id);
	
	if (chip_id != 0x58) {
		// Print to UART failure message.
		return;
	}
	
	// Calibration Array should be able to fit all 24 bytes. (192 bits)
	uint8_t compensationAddrStart = R_compensation_start; // Initial: 0x88.
	i2c_write_bytes(bmp280SLA_W, 1, compensationAddrStart); // Select start of compensation register on BMP280. (BMP280 auto increments register pointer).
	
	i2c_read_bytes(bmp280SLA_R, 24, calibArr); // Populate calibArr with calibration values.
	
	/*
	Write to 0xF4 (ctrl_meas).
		For the purposes of my environment monitor device, I will only be display pressure
		value calculated from the BMP280. Temperature display is obtained from the AHT20.
		
		Temperature Oversampling (according to purposes and datasheet): x1 (001) (osrs_t[2:0]).
		Pressure Oversampling (does not need to be crazy accurate): 
	*/
}
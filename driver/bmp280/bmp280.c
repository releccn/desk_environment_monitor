/*
 * bmp280.c
 *
 * Created: 2026-06-22 13:52:03
 *  Author: JR
 */ 

#include "../i2c/i2c.h"
#include "../usart/usart.h"

#define R_chip_id 0xD0

#define R_compensation_start 0x88
#define R_compensation_end 0x9F

#define R_ctrl_meas 0xF4
#define R_config 0xF5

#define R_pressureData_start 0xF7
#define R_pressureData_end 0xF9

#define bmp280SLA_R ((0x77 << 1) | 1) // 0x01111001 (SLA + R)
#define bmp280SLA_W (0x77 << 1) // 0x01111000 (SLA + W)

void bmp280_init(uint8_t *calibArr) {
	// calibArr should be 24 bytes large. (array of uint8_t's with size 24, uint8_t calibArr[24] = {...,}).
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
	uint8_t chipIdRegister = R_chip_id;
	i2c_write_bytes(bmp280SLA_W, 1, &chipIdRegister); // Selects chip_id register on BMP280.
	i2c_read_bytes(bmp280SLA_R, 1, &chip_id);
	
	if (chip_id != 0x58) {
		// Print to UART failure message.
		usart_print("BMP280 Chip ID not 0x58!\r\n");
		return;
	}
	
	// Calibration Array should be able to fit all 24 bytes. (192 bits)
	uint8_t compensationAddrStart = R_compensation_start; // Initial: 0x88.
	i2c_write_bytes(bmp280SLA_W, 1, &compensationAddrStart); // Select start of compensation register on BMP280. (BMP280 auto increments register pointer).
	
	i2c_read_bytes(bmp280SLA_R, 24, calibArr); // Populate calibArr with calibration values.
	
	/*
	Write to 0xF4 (ctrl_meas).
		For the purposes of my environment monitor device, I will only be display pressure
		value calculated from the BMP280. Temperature display is obtained from the AHT20.
		
		Temperature Oversampling (according to purposes and datasheet): x1 (001) (osrs_t[2:0]).
		Pressure Oversampling (does not need to be super accurate, low power): x1 (001) (osrs_p[2:0]).
		Power Mode: Normal (11) (mode[1:0]).
		
		ctrl_meas == 0b00100111 (39 in decimal).
	*/
	uint8_t ctrl_meas = 0x27; // 
	uint8_t ctrl_meas_command[] = {R_ctrl_meas, ctrl_meas};
	
	i2c_write_bytes(bmp280SLA_W, 2, ctrl_meas_command);
	
	/*
	Write to 0xF5 (config).
		The device is polling every 2 seconds for new temperature/pressure data using I2C.
		
		Rate (inactivity duration during normal mode): ~250ms (011) (t_sb[1:0]).
		IIR Filter: OFF (000) (filter[2:0]).
		SPI Interface: Disabled (using I2C). (0) (spi3w_en[0]).
		
		config == 0b0110000 (48 in decimal);
	*/
	uint8_t config = 0x60;
	uint8_t config_command[] = {R_config, config};
		
	i2c_write_bytes(bmp280SLA_W, 2, config_command);
		
	// Configuration complete.
}

void bmp280_read_raw(uint8_t *dataArr) {
	/*
	(XLSB) (Only relevant values are the MSB of the register: bit 7, 6, 5, 4).
	
	dataArr should be able to hold all 40 bits (uint8_t of length 6).
	
	Pressure data lives on register(s): 0xF7 - 0xF9 (20 bits). 
	Temperature data lives on register(s): 0xFA - 0xFC (20 bits).
	
	The data bits are contiguous so read straight through 0xF7-0xFC (and organize the bits later).
	
	Mutates/writes to user-defined dataArr.
	*/
	uint8_t pressureData_start = R_pressureData_start;
	i2c_write_bytes(bmp280SLA_W, 1, &pressureData_start);
	i2c_read_bytes(bmp280SLA_R, 6, dataArr); 
}

void bmp280_conversion(uint8_t *calibArr, uint8_t *dataArr, float *convertedValArr) {
	/*
	calibArr begins with dig_T1, and ends with dig_P9. 
	calibArr[0] = 0x88,
	calibArr[1] = 0x89,
	...
	calibArr[22] = 0x9E,
	calibArr[23] = 0x9F
	
	dataArr contains all 40 bits for pressure and temperature.
	dataArr[0] = press_msb[7:0],
	dataArr[1] = press_lsb[7:0],
	dataArr[2] = press_xlsb[3:0] (only bits 7, 6, 5, 4 are relevant).
	
	
	
	*/
	
	// Temperature compensation related values.
	uint16_t dig_T1 = (uint16_t)((uint16_t)calibArr[1] << 8 | calibArr[0]); 
	int16_t dig_T2 = (int16_t)((uint16_t)calibArr[3] << 8 | calibArr[2]);
	int16_t dig_T3 = (int16_t)((uint16_t)calibArr[5] << 8 | calibArr[4]);
	
	// Pressure compensation related values.
	uint16_t dig_P1 = (uint16_t)((uint16_t)calibArr[7] << 8 | calibArr[6]);
	int16_t dig_P2_P9[8];
	for (int i = 0; i < 8; ++i) {
		dig_P2_P9[i] = (int16_t)((uint16_t)calibArr[i*2 + 9] << 8 | calibArr[i*2 + 8]);
	}
	/*
	dig_P2 = dig_P2_P9[0],
	dig_P3 = dig_P2_P9[1],
	dig_P4 = dig_P2_P9[2],
	dig_P5 = dig_P2_P9[3],
	dig_P6 = dig_P2_P9[4],
	dig_P7 = dig_P2_P9[5],
	dig_P8 = dig_P2_P9[6],
	dig_P9 = dig_P2_P9[7],
	*/
	
	// Pressure bits.
	uint32_t pressureBits = (uint32_t)dataArr[0] << 12 | // Shift left 12.
							(uint32_t)dataArr[1] << 4 |  // Shift left 4.
							(uint32_t)dataArr[2] >> 4;   // Isolate high nibble (set as last 4 bits).
	// Temperature bits.		
	uint32_t temperatureBits = (uint32_t)dataArr[3] << 12 | // Shift left 12.
							   (uint32_t)dataArr[4] << 4 |  // Shift left 4.
							   (uint32_t)dataArr[5] >> 4;   // Isolate high nibble (set as last 4 bits).
	
	
	// Compensation formula (from the data sheet).
	double var1, var2, T;
	long signed int t_fine;
	
	var1 = (((double)temperatureBits)/16384.0 - ((double)dig_T1)/1024.0) * ((double)dig_T2);
	var2 = ((((double)temperatureBits)/131072.0 - ((double)dig_T1)/8192.0) * 
			(((double)temperatureBits)/131072.0 - ((double)dig_T1)/8192.0)) * ((double)dig_T3);
	t_fine = (long signed int)(var1 + var2);
	T = (var1 + var2) / 5120.0; // Temperature: "xx.xx"
	
	double var3, var4, p;
	var3 = ((double)t_fine/2.0) - 64000.0;
	var4 = var3 * var3 * ((double)dig_P2_P9[4]) / 32768.0;
	var4 = var4 + var3 * ((double)dig_P2_P9[3]) *2.0;
	var4 = (var4/4.0) + ((double)dig_P2_P9[2]) * 65536.0;
	var3 = (((double)dig_P2_P9[1]) * var3 * var3 / 524288.0 + ((double)dig_P2_P9[0]) * var3) / 524288.0;
	var3 = (1.0 + var3 / 32768.0) * ((double)dig_P1);
	if (var3 == 0.0) {
		return;
	}
	p = 1048576.0 - (double)pressureBits;
	p = (p - (var4 / 4096.0)) * 6250.0 / var3;
	var3 = ((double)dig_P2_P9[7]) * p * p / 2147483648.0;
	var4 = p * ((double)dig_P2_P9[6]) / 32768.0;
	p = (p + (var3 + var4 + ((double)dig_P2_P9[5])) / 16.0) / 1000; // Pressure: "xxxxx.x" (divided by 1000 for kPa)
	
	convertedValArr[0] = T; // Temperature in index 0.
	convertedValArr[1] = p; // Pressure in index 1.
}
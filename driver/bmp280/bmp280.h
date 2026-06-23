/*
 * bmp280.h
 *
 * Created: 2026-06-22 13:52:10
 *  Author: JR
 */ 
#ifndef BMP280_H
#define BMP280_H

#define R_chip_id 0xD0

#define R_compensation_start 0x88
#define R_compensation_end 0x9F

#define R_ctrl_meas 0xF4
#define R_config 0xF5

#define R_pressureData_start 0xF7
#define R_pressureData_end 0xF9

#define bmp280SLA_R ((0x77 << 1) | 1) // 0x01111001 (SLA + R)
#define bmp280SLA_W (0x77 << 1) // 0x01111000 (SLA + W)

void bmp280_init(uint8_t *calibArr);

void bmp280_read_raw(uint8_t *dataArr);

void bmp280_conversion(uint8_t *calibArr, uint8_t *dataArr, float *convertedValArr);

#endif
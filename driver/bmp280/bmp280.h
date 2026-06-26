/*
 * bmp280.h
 *
 * Created: 2026-06-22 13:52:10
 *  Author: JR
 */ 
#ifndef BMP280_H
#define BMP280_H


void bmp280_init(uint8_t *calibArr);

void bmp280_read_raw(uint8_t *dataArr);

void bmp280_conversion(uint8_t *calibArr, uint8_t *dataArr, float *convertedValArr);

#endif
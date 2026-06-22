/*
 * aht20.h
 *
 * Created: 2026-06-21 22:12:50
 *  Author: JR
 */ 
#ifndef AHT20_H
#define AHT20_H

void aht20_init();

void aht20_trigger_measurement();

void aht20_read_data(uint8_t *dataArr);

void aht20_conversion(uint8_t *dataArr, float *convertedValArr);

#endif
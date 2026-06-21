/*
 * i2c.h
 *
 * Created: 2026-06-12 16:50:29
 *  Author: Rey Eleccion
 */ 

#include <stdint.h>

#ifndef I2C_H
#define I2C_H

#define I2C_SUCCESS 0x00
#define I2C_ERROR 0x01

#define ACK 0
#define NACK 1

// Initialize I2C Master.
void i2c_init();

// Start and Stop Conditions.
uint8_t i2c_start();
void i2c_stop();

// Write data to bus.
uint8_t i2c_write_byte(uint8_t data);
uint8_t i2c_write_bytes(uint8_t sla_w, uint8_t len, uint8_t *buff);


// Read data from bus.
uint8_t i2c_read_byte(uint8_t ACK_NACK, uint8_t *data);
uint8_t i2c_read_bytes(uint8_t sla_r, uint8_t len, uint8_t *buff);

// Reset bus if transmission fails (implement IF necessary).
void i2c_reset();

// Error message sent to UART.
void i2c_error(uint8_t twsr_bits);

#endif
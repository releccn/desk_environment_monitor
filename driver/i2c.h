/*
 * i2c.h
 *
 * Created: 2026-06-12 16:50:29
 *  Author: Rey Eleccion
 */ 

// Initialize I2C Master
void i2c_init();

// Start and Stop Conditions
void i2c_start();
void i2c_stop();

// Write data to bus
void i2c_write_byte();

// Read data to bus
void i2c_read_byte();

// Reset bus if transmission fails
void i2c_reset();

// Error message sent to UART
void i2c_error();
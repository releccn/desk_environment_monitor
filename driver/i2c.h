/*
 * i2c.h
 *
 * Created: 2026-06-12 16:50:29
 *  Author: Rey Eleccion
 */ 

#define I2C_SUCCESS 0x00
#define I2C_ERROR 0x01

// Initialize I2C Master.
void i2c_init();

// Start and Stop Conditions.
uint8_t i2c_start();
void i2c_stop();

// Write data to bus.
uint8_t i2c_write_byte(uint8_t data);

// Read data to bus.
uint8_t i2c_read_byte(uint8_t ACK_NACK, uint8_t *data);

// Reset bus if transmission fails (implement IF necessary).
void i2c_reset();

// Error message sent to UART (implement later, after UART Driver).
void i2c_error();
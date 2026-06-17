/*
 * i2c.c
 *
 * Created: 2026-06-12 16:50:02
 *  Author: Rey Eleccion
 */ 

#include <avr/io.h>
#include <util/twi.h>
#include "i2c.h"

void i2c_init() {
	/* To initialize TWI/I2C, the following must be configured:
		TWBR (bit rate register), 
		TWSR (status register) pre-scaler bit (1, 4, 16, 64), 
		TWCR (control register) enable bit set (bit 2, TWEN)
	*/
	
	/* I2C Standard Mode: 100kHz
	   f_SCL = 100kHz
	   f_CPU = 8MHz
	   Pre-scaler = 1
	   TWBR = 32
	*/
	
	TWBR = (1 << 5); // 32, 0b00100000 (Modify ALL bits in register)
	
	TWSR &= ~((1 << TWPS1) | (1 << TWPS0)); // 0bxxxxx-00 (TWPS1 = 0, TWPS0 = 0, Prescaler Value = 1) (Modify ONLY prescalar bits in register)
	
	TWCR |= (1 << TWEN); // Set Enable Bit
}

uint8_t i2c_start() {
	/* 
	To enter MT (Master Transmitter) or MR (Master Receiver) mode.
	
	START condition transmission occurs when TWCR (control register) bits:
		Set TWSTA (to transmit START)
		Set TWINT (to clear TWINT)
	*/
	
	// Send start condition.
	TWCR |= (1 << TWSTA) | (1 << TWINT); // In datasheet, example code sets TWEN here BUT
										 // TWEN is already set in initialization.
	
	// Wait for TWINT Flag to set by hardware to indicate START condition has been transmitted.			
	while (!(TWCR & (1 << TWINT))); // Poll till TWCR & (1 << TWINT) == TRUE (i.e TWINT is set).
	
	// Verify that successful START condition. Otherwise, error. 
	if ((TWSR & 0xF8) != TW_START) { // TW_START = 0x08 (Status Code for TWSR)
		i2c_error(); // for UART
		return I2C_ERROR;
	}
	
	// Clear TWSTA Bit once START condition has been transmitted.
	TWCR &= ~(1 << TWSTA);
	return I2C_SUCCESS;
}

void i2c_stop() {
	/* 
	To terminate data transmission.
	
	STOP condition transmission occurs when TWCR (control register) bits:
		Set TWSTO (to transmit STOP)
		Set TWINT (to clear TWINT)
	*/
	
	// Send STOP condition.
	TWCR |= (1 << TWSTO) | (1 << TWINT); // In datasheet, example code sets TWEN here BUT
										 // TWEN is already set in initialization.
	
	// TWINT Flag is NOT set to indicate STOP has been transmitted.
}

uint8_t i2c_write_byte(uint8_t data) {
	// Assume START condition has been successfully transmitted.
	
	// Note: To enter MT (Master Transmitter) mode, SLA+W must be written to TWDR.
	// 		 To enter MR (Master Receiver) mode. SLA+R must be written to TWDR.

	// This function writes a byte to the bus (regardless of if it is an address or actual data).
	
	// Address/Data transmission.
	TWDR = data;
	TWCR |= (1 << TWINT); // Clear TWINT to start address/data transmission.
	
	// Wait for TWINT flag to set by hardware to indicate address/data has been transmitted
	//												  and ACK/NACK has been received
	while (!(TWCR & (1 << TWINT)));
	
	// Verify DATA has been transmitted and ACK is received.
	if (((TWSR & 0xF8) != TW_MT_SLA_ACK) && ((TWSR & 0xF8) != TW_MT_DATA_ACK)) { // TW_MT_DATA_ACK = 0x28 (Status Code for TWSR)
		i2c_error(); // for UART												 // TW_MT_SLA_ACK = 0x18 (Status Code for TWSR)
		return I2C_ERROR;
	}
	
	return I2C_SUCCESS;
}

uint8_t i2c_read_byte(uint8_t ACK_NACK, uint8_t *data) {
	// Assume START condition has been successfully transmitted and MR mode.
	// A - ACK (0) , N - NACK (1)
	
	// Returns byte received from TWDR (Data Register).
	
	if (ACK_NACK == ACK) {
		TWCR |= (1 << TWEA) | (1 << TWINT); // Set TWEA (enable ack) bit, and clear TWINT to begin read.
		
		while (!(TWCR & (1 << TWINT))); // Poll till TWINT flag is set by hardware
										// to indicate data can be read from TWDR.
										
		// Verify that data byte has been received, ACK returned. (Check TWSR).
		if ((TWSR & 0xF8) != TW_MR_DATA_ACK) {
			i2c_error(); // for UART													
			return I2C_ERROR;
		}
		*data = TWDR; // Write TWDR to user-defined variable address.
		return I2C_SUCCESS; // Return byte from TWDR (data register).
		
	} else if (ACK_NACK == NACK) { // Last byte has been received.
		// MR should inform ST by sending NACK (i.e. clearing TWEA) after last received data byte.
		TWCR = (TWCR & ~(1 << TWEA)) | (1 << TWINT); // Clear TWEA (enable ack) bit, and clear TWINT to begin read.
		
		while (!(TWCR & (1 << TWINT))); // Poll till TWINT flag is set by hardware
										// to indicate data can be read from TWDR.
		
		// Verify that data byte has been received, NACK returned.	(Check TWSR).
		if ((TWSR & 0xF8) != TW_MR_DATA_NACK) {
			i2c_error(); // for UART
			return I2C_ERROR;
		}
		
		*data = TWDR; // Write TWDR to user-defined variable address.
		return I2C_SUCCESS; // Return byte from TWDR (data register).
	} 
	
	return I2C_ERROR;
}

uint8_t i2c_write_bytes(uint8_t sla_w, uint8_t len, uint8_t *buff) {
	/*
	sla_w - Slave address + Write bit.
	len - Amount of bytes to write.
	buff - Pointer to the array where bytes to write are stored.
	*/
	
	// Start condition.
	if (i2c_start() != I2C_SUCCESS) { i2c_stop(); return I2C_ERROR; }
	
	// Write slave address + write bit.
	if (i2c_write_byte(sla_w) != I2C_SUCCESS) { i2c_stop(); return I2C_ERROR; }
	
	// Write all bytes from array. 
	for (uint8_t i = 0; i < len; ++i) {
		if (i2c_write_byte(buff[i]) != I2C_SUCCESS) { i2c_stop(); return I2C_ERROR; }
	}
	
	i2c_stop(); // Stop condition.
	return I2C_SUCCESS;
}

uint8_t i2c_read_bytes(uint8_t sla_r, uint8_t len, uint8_t *buff) {
	/*
	sla  - slave address.
	len  - amount of bytes to write.
	buff - pointer to array where read bytes are stored.
	*/
	
	// Start condition.
	if (i2c_start() != I2C_SUCCESS) { i2c_stop(); return I2C_ERROR; }
	
	// Write slave address + read bit.
	if (i2c_write_byte(sla_r) != I2C_SUCCESS) { i2c_stop(); return I2C_ERROR; }
	
	// Read (len - 2) bytes from array with ACK.
	for (uint8_t i = 0; i < len - 1; ++i) {
		if (i2c_read_byte(ACK, &buff[i]) != I2C_SUCCESS) { i2c_stop(); return I2C_ERROR; }
	}
	
	// Read (len - 1), last byte from array with NACK.
	if (i2c_read_byte(NACK, &buff[len - 1]) != I2C_SUCCESS) { i2c_stop(); return I2C_ERROR; }
	
	i2c_stop(); // Stop condition.
	return I2C_SUCCESS;
	
}

void i2c_error() {
	// TODO: Implement printing error to UART
	
}
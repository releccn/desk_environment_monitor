/*
 * i2c.c
 *
 * Created: 2026-06-12 16:50:02
 *  Author: Rey Eleccion
 */ 

#include <avr/io.h>
#include <util/twi.h>
#include <stdint.h>
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
	
	TWSR &= ~(1 << TWPS1) | ~(1 << TWPS0); // 0b00000000 (TWPS1 = 0, TWPS0 = 0, Prescaler Value = 1) (Modify ONLY prescalar bits in register)
	
	TWCR |= (1 << TWEN); // Set Enable Bit
}

void i2c_start() {
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
		i2c_error();
		return;
	}
	
	// Clear TWSTA Bit once START condition has been transmitted.
	
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

void i2c_write_byte(uint8_t data) {
	// Assume START condition has been successfully transmitted.
	
	// Note: To enter MT (Master Transmitter) mode, SLA+W must be written to TWDR.

	// This function writes a byte to the bus (regardless of if it is an address or actual data).
	
	// Data transmission.
	TWDR = data;
	TWCR |= (1 << TWINT); // Clear TWINT to start data transmission.
	
	// Wait for TWINT flag to set by hardware to indicate DATA has been transmitted
	//												  and ACK/NACK has been received
	while (!(TWCR & (1 << TWINT)));
	
	// Verify DATA has been transmitted and ACK is received.
	// !!! There is a logic error here. Look again.
	if (((TWSR & 0xF8) != TW_MT_SLA_ACK)) | ((TWSR & 0xF8) != TW_MT_DATA_ACK))) { // TW_MT_DATA_ACK = 0x28 (Status Code for TWSR)
		i2c_error();															  // TW_MT_SLA_ACK = 0x18 (Status Code for TWSR)
		return;
	}
	
	// End by sending STOP condition.
}


void i2c_error() {
	// Print error to UART
	return 0;
}
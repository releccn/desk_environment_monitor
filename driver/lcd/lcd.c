
/*
 * lcd.c
 *
 * Created: 2026-06-25 23:57:05
 *  Author: JR
 */ 

#define F_CPU 8000000UL
#include "../i2c/i2c.h"
#include <util/delay.h>

#define lcdSLA_W (0x27 << 1)

void lcd_write_pcf(uint8_t data) {
	/*
	This function will write data (byte) to the PCF8574 through I2C which is directly
	connected to the SPLC780D0 (LCD).
	*/
	i2c_write_bytes(lcdSLA_W, 1, &data);
}

void lcd_pulse_enable(uint8_t data) {
	/*
	This function will pulse E bit (enable bit) high then low (with delays stated in the datasheet).
	The falling edge of E allows the LCD to read data bits in (DB7-DB4).
	*/
	
	// Current data bits are passed as the parameter (data).
	uint8_t highE = data | (1 << 2); // 0bxxxxx1xx, this sets E.
	uint8_t lowE = data & ~(1 << 2); // 0bxxxxx0xx, this clears E.
	
	
	// Begin pulse.
	lcd_write_pcf(highE); // Set E high.
	_delay_us(1); // minimum 230ns (1us is sufficient, and reliable at 8MHz).
	lcd_write_pcf(lowE); // Set E low.
}

void lcd_send_nibble(uint8_t nibble, uint8_t rs) {
	/*
	This function takes a nibble as a parameter and send it to the LCD and changes (fills) DB7-DB4 ONLY.
	RS: 0 (Instruction Register) or 1 (Data Register) (LSB)
	R/W: 0 (Write)
	E: -
	LED+: 1 (High, ON)
	DB7-DB4: xxxx (data). (MSB)
	
	Nibble: 0b0000NNNN, 0x0N
	Byte sent looks like this: 0bNNNN1x0(rs)
	*/
	uint8_t nibbleShift = (nibble << 4); // 0bNNNN0000
	nibbleShift = (nibbleShift & ~((1 << 0) | (1 << 1))) | (1 << 3); // 0bNNNN1000
	nibbleShift |= rs; // Set or clear rs depending on parameter given.
	lcd_pulse_enable(nibbleShift); 
}

void lcd_send_byte(uint8_t data, uint8_t rs) {
	/*
	This function sends a full byte (2 nibbles) to fill LCD's
	DB7-DB4.
	*/
	uint8_t upper = (data >> 4); // 0b0000UUUU.
	uint8_t lower = (data & 0x0F); // 0b0000LLLL.
	
	lcd_send_nibble(upper, rs); // Send upper.
	lcd_send_nibble(lower, rs); // Send lower.
}

void lcd_init() {
	/*
	To initialize the LCD using 4-bit interface, 
		- Write sequence of bits stated in datasheet to set 4 bit inteface,
			- 0x3 (three times)
			- 0x2 (one time)
		- Configure number of display lines (2), and character font (restricted by number of display lines, 5 x 8 dots).
		- Turn display off
		- Clear display
		- Entry mode set
		
	Turn on display (D),
	disable cursor (C) and cursor blinking (B)
	*/
	lcd_send_nibble(0x3, 0); // Function Set
	_delay_ms(4.1); // Wait time > 4.1ms
	lcd_send_nibble(0x03, 0); // Function Set
	_delay_us(100); // Wait time > 100us
	lcd_send_nibble(0x03, 0); // Function Set
	
	lcd_send_nibble(0x02, 0); // Function Set (Now interface is 4 bits).
	
	// Number of display lines is two. N = 1, F = -
	// Sets number of display lines & character font
	lcd_send_byte(0x28, 0); // 0x00101000
	
	// Display off
	lcd_send_byte(0x08, 0);
	_delay_us(38); // 38us execution
	
	// Display clear
	lcd_send_byte(0x01, 0);
	_delay_ms(1.52); // 1.52ms to clear (stated in datasheet).
	
	// Entry mode set
	// I/D = 1 (Increment)
	// S = 0 (Display does not shift)
	lcd_send_byte(0x06, 0);
	_delay_us(38); // 38us execution
	
	// 4-Bit Interface Complete (Initialization Ends).
	
	// Display ON, Cursor OFF, Cursor Blink OFF.
	// 0b00001100 = 0x0D (DB7-DB0)
	lcd_send_byte(0x0C, 0);
	_delay_us(38); // 38us execution
}

void lcd_clear() {
	/*
	Clear LCD display (1.52ms execution)
	0b00000001 (byte to send to clear).
	*/
	lcd_send_byte(0x01, 0);
	_delay_ms(1.52); // 1.52ms execution.
}

void lcd_setcursor(uint8_t row, uint8_t col) {
	/*
	Set cursor position on LCD by writing to DDRAM.
	
	Line 1: 0x00 - 0x27 
	Line 2: 0x40 - 0x67
	
	There are only 16 cols on the LCD display so the only addresses that get written to are actually:
	Line 1: 0x00 + 16 = 0x10
	Line 2: 0x40 + 16 = 0x50
	*/
	uint8_t address = 0x00;
	
	if (!row) { // Row 0
		address = 0x80 | (0x00 + col);
		lcd_send_byte(address, 0);
		_delay_us(38);
	} else { // Row 1
		address =  0x80 | (0x40 + col);
		lcd_send_byte(address, 0);
		_delay_us(38);
	}
}

void lcd_print(char *str) {
	/*
	Prints a string to the LCD display
	
	Strings should be restricted to at most 16 characters.
	
	The LCD display increments DDRAM automatically for each char written.
	*/
	char *temp = str; // Temporary pointer for pointer arithmetic. (Similar to USART driver).
	while (*temp != '\0') {
		// Continually send the characters of string till null-terminator is reached.
		lcd_send_byte(*temp, 1); // Transmit character (to data register, RS = 1).
		temp++; // Increment to next character.
	}
}
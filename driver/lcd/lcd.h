
/*
 * lcd.h
 *
 * Created: 2026-06-25 23:57:10
 *  Author: JR
 */ 
#ifndef LCD_H
#define LCD_H

void lcd_write_pcf(uint8_t data);

void lcd_pulse_enable(uint8_t data);

void lcd_send_nibble(uint8_t nibble, uint8_t rs);

void lcd_send_byte(uint8_t data, uint8_t rs);

void lcd_init();

void lcd_clear();

void lcd_setcursor(uint8_t row, uint8_t col);

void lcd_print(char *str);

#endif
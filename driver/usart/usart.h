/*
 * usart.h
 *
 * Created: 2026-06-18 18:10:36
 *  Author: JR
 */ 

#include <stdint.h>

#ifndef USART_H
#define USART_H

void usart_init();

void usart_transmit_byte(uint8_t data);

void usart_print(char *msg);

void usart_print_hex(uint8_t twsr_bits);

#endif
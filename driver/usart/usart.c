/*
 * usart.c
 *
 * Created: 2026-06-18 18:10:26
 *  Author: JR
 */ 

 #include "usart.h"
 #include <avr/io.h>

#define UBRR 51


 void usart_init() {
    /* To initialize USART/UART, the following must be configured: 
        UMSELn bits, 1:0, in UCSRnC Register (to configure asynchronous USART mode),
        UCSRnB and UCSRnC Register, UCSZN 2:0, UPMn 1:0, USBSn (to configure frame format),
        UBRRnH and UBRRnL Register, UBRRn (to configure baud rate),
        
        UCSRnA Register, clear U2Xn bit (to disable double USART transmission speed, Normal),
        UCSRnB Register, set TXEN bit (to enable Transmitter Mode).
    */

    /* Asynchronous USART Mode (Normal)
        Baud Rate: 9600 (UBRRn = 51).
        Frame Format: 
            1 Start Bit,
            8 Data Bits (UCSZn2 = 0, UCSZn1 = 1, UCSZn0 = 1)
            0 Parity Bits,
            1 Stop Bit.
        U2Xn = 0 (for normal transmission speed).

    */

    // Enable asynchronous USART mode, disable parity bits, 
    UCSR0C &= ~((1 << UMSEL01) | (1 << UMSEL00)); // UMSELn1 = 0, UMSELn0 = 0 (Asynchronous USART).

    // Disable parity bits.
    UCSR0C &= ~((1 << UPM01) | (1 << UPM00));  // UPMn1 = 0, UPMn0 = 0 (Disable Parity Mode).

    // Configure data bit size (8 bits).
    UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01); // UCSZn1 = 1, UCSZn0 = 1
    UCSR0B &= ~(1 << UCSZ02); // UCSZn2 = 0

    // Configure stop bits (1 only).
    UCSR0C &= ~(1 << USBS0);

    // Set baud rate to 9600.
    UBRR0L = UBRR; // 51, 0b00110011 
    UBRR0H = (UBRR >> 8); // 0, 0b0000

    // Disable double USART transmission speed (configure to normal).
    UCSR0A &= ~(1 << U2X0);
    
    // Enable Transmitter Mode.
    UCSR0B |= (1 << TXEN0);
 }



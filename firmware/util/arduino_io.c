/* include this to get stdout "just working" on the uart @19200baud */

#ifdef AVR_STDOUT

#include <avr/io.h>
#define BAUD 19200
#include <util/setbaud.h>

/* from avr libc stdio example */
static int uart_putchar(char c, FILE *stream)
{
    if (c == '\n')
        uart_putchar('\r', stream);
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
    return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

static void init_uart()
{
   UBRR0H = UBRRH_VALUE;
   UBRR0L = UBRRL_VALUE;
   #if USE_2X
   UCSR0A |= (1 << U2X0);
   #else
   UCSR0A &= ~(1 << U2X0);
   #endif

   stdout = &mystdout;
}
#endif
#ifndef AVR_STDOUT
static void init_uart() {}
#endif

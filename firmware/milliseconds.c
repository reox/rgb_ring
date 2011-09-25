/* this is a 10ms clock */

uint8_t secondsem4 = 0; // counter for seconds^-4. not volatile as it's only ever used in the interrupt.

void milliseconds_setup()
{
	/* ctc mode, divider 128 */
	TCCR2A = (1<<WGM21); 
	TCCR2B = (1<<CS22) | (1<<CS20);

	// using powers of 5 to compensate for the powers of 2 in the divisor
	OCR2A = 25;
}

ISR(TIMER2_COMPA_vect)
{
	secondsem4 += 2; // 0.2ms passed
	if (secondsem4 >= 100) {
		secondsem4 -= 100;
		tick_10ms();
	}
}

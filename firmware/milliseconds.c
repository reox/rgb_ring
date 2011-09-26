/* this is a 10ms clock
 *
 * this relies on nothing to disable interrupts for longer than 2.0ms. if this
 * is too short (FIXME: check for overflow -- but this would have to happen at
 * *all* sei() positions), we have to trigger the interrupts in larger
 * intervals and accpet some jitter in the timing.
 * */

uint8_t secondsem4 = 0; // counter for seconds^-4. not volatile as it's only ever used in the interrupt.
volatile uint8_t unhandled_10ms_ticks = 0;

void milliseconds_setup()
{
	/* ctc mode, divider 128 */
	TCCR2A = (1<<WGM21); 
	TCCR2B = (1<<CS22) | (1<<CS20);

	// using powers of 5 to compensate for the powers of 2 in the divisor
	OCR2A = 250;

	TIMSK2 = (1<<OCIE2A);
}

ISR(TIMER2_COMPA_vect)
{
	secondsem4 += 20; // 2ms passed
	if (secondsem4 >= 100) {
		secondsem4 -= 100;
		unhandled_10ms_ticks += 1;
	}
}

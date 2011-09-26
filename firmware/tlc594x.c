#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "tlc594x.h"
#include "tlc594x_config.h"

#define XLAT_ACTIVE() XLAT_PORT |= (1<<XLAT_BIT)
#define XLAT_INACTIVE() XLAT_PORT &= ~(1<<XLAT_BIT)

#define BLANK_INACTIVE() BLANK_PORT |= (1<<BLANK_BIT)
#define BLANK_ACTIVE() BLANK_PORT &= ~(1<<BLANK_BIT)

uint8_t buffer[DEVICES*24];

static void latch()
{
	XLAT_ACTIVE();
	// min. 20ns -- even at 20mhz, a clock cycle is 50ns.
	XLAT_INACTIVE();
}

void pull_blank()
{
	BLANK_INACTIVE();
	// min. 20ns, as with latch
	BLANK_ACTIVE();
}

static void populate_buffer(uint16_t *data)
{
	uint8_t i, read_cursor, write_cursor;

	for (i = 0; i < 8 * DEVICES; ++i)
	{
		read_cursor = 2*i;
		write_cursor = 3*i;

                buffer[write_cursor+0] = (data[read_cursor+0] >> 4) & 0xff;
                buffer[write_cursor+1] = (data[read_cursor+0] & 0xff) << 4;

                buffer[write_cursor+1] |= (data[read_cursor+1] >> 8) & 0x0f;
                buffer[write_cursor+2] = data[read_cursor+1] & 0xff;
	}
}


void tlc_setup()
{
	XLAT_INACTIVE();
	XLAT_DDR |= (1<<XLAT_BIT);

	BLANK_DDR |= (1<<BLANK_BIT);
	BLANK_INACTIVE();

	/* timer 1 flicks the grayscale line */

	GS_DDR |= (1<<GS_BIT);

	/* ctc mode -- reset timer on every match */
	TCCR1A = TCCR1A & ~(1<<WGM11) & ~(1<<WGM10);
	TCCR1B = (TCCR1B & ~(1<<WGM13)) | (1<<WGM12);
	/* set prescaler to /1, resulting in a 16mhz clock */
	TCCR1B = (TCCR1B & ~((1<<CS10) | (1<<CS11) | (1<<CS12))) | (1<<CS10);
	/* match as often as possible; matches every second tick and has raising edge every 4th, resulting in a 4mhz clock */
	// OCR1A = OCR1B = 1;
        /* don't exaggerate -- we can tune here easily as long as OCR0A, which is derived from this, doesn't wrap */
	OCR1A = OCR1B = 4;
	TCNT1 = 0;
	/* toggle PB2 (OC1B) very often */
	TCCR1A |= (1<<COM1B0);

	/* timer 2 calls blank from the interrupt when gsclk wraps */

	/* set prescaler to clk/1024 */
	TCCR0B = (1<<CS02);
	/* we need a tick every 4096 ticks of timer1's 4mhz clock, that's at
	 * 1khz. our base clock is 16khz. */
	OCR0A = (int)(16 * OCR1A * 2.5); // factor 2.5 experimentally from oscilloscope. FIXME 'zup?
	/* on match, reset and call interrupt */
	TCCR0A = (1<<WGM01);
	TIMSK0 = (1<<OCIE0A);

	/* as opposed to uart, spi has to be explicitly set to output */

	DDR_MOSI   |= (_BV(BIT_MOSI));
	DDR_SCK    |= (_BV(BIT_SCK));
	DDR_MISO   &= ~(_BV(BIT_MISO));

	//SPCR = (1<<SPE) | (1<<MSTR); // enable, master, f_osc/2
	//SPSR = (1<<SPI2X);
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR1) | (1<<SPR0); // enable, master, f_osc/128
}

ISR(TIMER0_COMPA_vect)
{
	pull_blank();
}

void tlc_send_blocking(uint16_t data[16])
{
	uint8_t i;
	populate_buffer(data);
	for (i = 0; i < sizeof(buffer)/sizeof(buffer[0]); ++i)
	{
		SPDR = buffer[i];
		while (!(SPSR & (1<<SPIF)));
		buffer[i] = SPDR;
	}
	latch();
}

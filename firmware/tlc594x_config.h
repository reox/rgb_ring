#define XLAT_PORT PORTB
#define XLAT_DDR DDRB
#define XLAT_BIT 1

#define BLANK_PORT PORTB
#define BLANK_DDR DDRB
#define BLANK_BIT 0

#define GS_PORT PORTB
#define GS_DDR DDRB
#define GS_BIT 2

#define DEVICES 1


/* atmega?8 hardware */

#define DDR_SPI DDRB
#define PORT_SPI PORTB
#define PIN_SPI PINB
#define BIT_MOSI 3
#define BIT_MISO 4
#define BIT_SCK  5
#define DDR_MOSI DDR_SPI
#define PORT_MOSI PORT_SPI
#define DDR_MISO DDR_SPI
#define PIN_MISO PIN_SPI
#define DDR_SCK DDR_SPI
#define PORT_SCK PORT_SPI

/* quite minimal TLC594X library
 *
 * can't handle dot correction ('cause we need it) or eeprom, but reads error
 * states.
 */

#ifndef TLC594X_H
#define TLC594X_H

void tlc_setup();

// actually, it's uint16_t data[DEVICES*16]. each entry's most significant 4 bits are ignored.
void tlc_send_blocking(uint16_t *data);

void pull_blank();

#endif

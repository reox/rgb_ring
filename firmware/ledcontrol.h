/* low level led control
 *
 * this is where you can specify which 12bit value a particular (numbered) led
 * channel should be assigned. this module supports linear fading (implementing
 * bresenham's algorithm), but no grouping of pins or color handling.
 */

#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include <stdint.h>
#include <stdbool.h>

extern bool led_requires_retransmit;

extern uint16_t led_value[];

void led_fade_to(uint8_t channel, uint16_t target, uint16_t steps);
void led_fade_step_all();

#endif

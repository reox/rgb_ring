#include "ledcontrol.h"
#include "tlc594x_config.h"

#include "util/bresenham.c"

bool led_requires_retransmit = true;

uint16_t led_value[16*DEVICES] = {0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff, 0x0000, 0xffff};
bresenham_state led_fading[16*DEVICES];

void led_fade_to(uint8_t channel, uint16_t target, uint16_t steps)
{
    bresenham_init(&led_fading[channel], led_value[channel], target, steps);
}

uint16_t led_get_current(uint8_t channel)
{
	return led_value[channel];
}

void led_set_to(uint8_t channel, uint16_t value)
{
    bresenham_uninit(&led_fading[channel]);
    led_value[channel] = value;
}

static void led_fade_step(uint8_t channel)
{
    led_requires_retransmit |= bresenham_step(&led_fading[channel], &led_value[channel]);
}

void led_fade_step_all()
{
    uint8_t i;
    for (i = 0; i < 16*DEVICES; ++i)
        led_fade_step(i);
}

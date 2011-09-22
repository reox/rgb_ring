/* small utility include for bresenham interpolation over time
 *
 * implemented from the "simplified" code snipplet at
 * wp-en:Bresenham's_line_algorithm, modified for discrete time steps in x
 * axis ("ticks")
 *
 * i'm not sure this is accurate for steep cases (in that sense that i didn't
 * bother which of the "pixels" is shown for a given x value). plus, in the steep case, this might be sped up as we only need one pixel anyway.
 * */

#include <stdint.h>
#include <stdbool.h>

#define abs(x) (x > 0 ? x : -x)

typedef struct {
    uint16_t dx;
    int16_t _dy;
#define sx 1
    int16_t err;
    uint16_t dx_remaining;
    uint16_t target; // we could do without as well, but then we're off by rounding errors
} bresenham_state;

static uint16_t dy(bresenham_state *state) { return abs(state->_dy); }
static int8_t sy(bresenham_state *state) { return state->_dy > 0 ? 1 : -1; }

static void bresenham_init(bresenham_state *state, uint16_t initial, uint16_t target, uint16_t steps)
{
    state->target = target;
    state->dx = steps;
    state->dx_remaining = steps;
    state->_dy = target - initial;
    state->err = state->dx - dy(state);
}

static bool bresenham_finished(bresenham_state *state)
{
    return state->dx_remaining == 0;
}

/* update the y value according to the next x value, return true if it was
 * modified. */
static bool bresenham_step(bresenham_state *state, uint16_t *value)
{
    bool did_increment = false;
    bool did_change = false;
    int16_t e2;
    if (bresenham_finished(state)) return false;

    while (1) {
        e2 = 2*state->err;

#ifdef BRESENHAM_DEBUG
        printf("in loop. did_increment %c, did_change %c, e2 %d, dx %d, _dy %d, err %d, dx_reaining %d, target %d\n", did_increment ? 'y' : 'n', did_change ? 'y' : 'n', e2, state->dx, state->_dy, state->err, state->dx_remaining, state->target);
#endif

        if (e2 > -dy(state)) {
#ifdef BRESENHAM_DEBUG
            printf("took first if.\n");
#endif
            state->err -= dy(state);
            state->dx_remaining--;
            did_increment = true;
            if (state->dx_remaining == 0) {
                *value = state->target;
                return true; // might not really have changed, but who cares
            }
            break;
        }
	/* be very careful when comparing int16_t and uint16_t -- explicitly
	 * splitting and casting to avoid big problems */
        //if (e2 < state->dx) {
        if (e2 < 0 || (uint16_t)e2 < state->dx) {
#ifdef BRESENHAM_DEBUG
            printf("took second if.\n");
#endif
            state->err += state->dx;
            *value += sy(state);
            did_change = true;
        }
    }

    return did_change;
}

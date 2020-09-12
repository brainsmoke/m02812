#ifndef BITBANG16_H
#define BITBANG16_H

/* configurable parameters (SRAM needed: more than 9*8*N_LEDS_PER_STRIP + 512 bytes) */

/* no bigger than 80x3 or 60x4 */
#ifndef N_VALUES_PER_LED
#define N_VALUES_PER_LED  (3)
#endif

#ifndef N_LEDS_PER_SEGMENT
#define N_LEDS_PER_SEGMENT (4)
#endif

#ifndef N_SEGMENTS
#define N_SEGMENTS (15)
#endif

#define N_LEDS_PER_STRIP ((N_LEDS_PER_SEGMENT)*(N_SEGMENTS))

/* static constants */

#define N_STRIPS (4) /* code changes needed to change this */

/* derived constants */

#define N_VALUES_PER_SEGMENT (N_LEDS_PER_SEGMENT*N_VALUES_PER_LED)
#define N_VALUES_PER_STRIP (N_LEDS_PER_STRIP*N_VALUES_PER_LED)
#define N_LEDS (N_LEDS_PER_STRIP*4)
#define N_VALUES (N_VALUES_PER_STRIP*4)
#define N_BYTES (N_VALUES*2)

#define ROUTING_TABLE_SIZE (N_STRIPS*(N_SEGMENTS-1))

#if N_VALUES_PER_STRIP > 240
#error "strips too long"
#endif

#ifndef __ASSEMBLER__

#include <stdint.h>

extern uint8_t remainders[N_VALUES];

void bitbang16(uint16_t *buf, volatile uint16_t *gpio_out);

#endif

#endif // BITBANG16_H

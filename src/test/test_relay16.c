#include <stdlib.h>
#include "stm32f0xx.h"

#include "bitbang16_relay_4strip.h"
#include "util.h"

uint16_t frame_a[N_VALUES];
uint8_t table[ROUTING_TABLE_SIZE_PADDED];
uint8_t *routing_table;
uint16_t *cur;
uint32_t pulse_width8x;

int main(void)
{
	cur = frame_a;
	bitbang16(frame_a, &GPIOA->ODR, 8*60);
}


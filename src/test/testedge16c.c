#include <stdlib.h>
#include "stm32f0xx.h"

#include "bitbang_relay.h"
#include "util.h"

uint16_t frame_a[N_VALUES];
uint8_t routing_table[ROUTING_TABLE_SIZE];
uint16_t *cur;

int main(void)
{
	cur = frame_a;
	bitbang16(frame_a, &GPIOA->ODR);
}


#include <stdlib.h>
#include "stm32f0xx.h"

#include "bitbang16_4strip.h"
#include "util.h"

uint16_t frame_a[N_VALUES];

int main(void)
{
	bitbang16(frame_a, &GPIOA->ODR);
}


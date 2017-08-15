
#include <stdlib.h>
#include "stm32f0xx.h"

#include "bitbang.h"

uint8_t framebuf[N_BYTES];
//static uint8_t *outbuf;
uint8_t *frame;

#define O(c) (1<<(2*c))
#define ALT_FN(c) (2<<(2*c))
#define SWD (ALT_FN(13)|ALT_FN(14))

int main(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; 	// enable the clock to GPIOA
	GPIOA->ODR = 0;
	GPIOA->MODER = SWD|O(0)|O(1)|O(2)|O(3)|O(4)|O(5)|O(6)|O(7);

	uint32_t i;

	for(;;)
	{
		bitbang(framebuf, &GPIOA->ODR);
		for (i=0; i<20000; i++);
	}
}


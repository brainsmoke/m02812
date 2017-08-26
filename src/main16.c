#include <stdlib.h>
#include "stm32f0xx.h"

#include "bitbang16.h"
#include "util.h"

uint16_t frame_a[N_VALUES];
uint16_t frame_b[N_VALUES];
uint16_t *cur;
uint16_t *next;
uint8_t recv_buf[512];

#define O(c) (1<<(2*c))
#define ALT_FN(c) (2<<(2*c))
#define SWD (ALT_FN(13)|ALT_FN(14))

void SysTick_Handler(void)
{
	bitbang16(cur, &GPIOA->ODR);
}

void init(void)
{
	clock48mhz();
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; 	// enable the clock to GPIOA
	GPIOA->ODR = 0;
	GPIOA->MODER = SWD|O(0)|O(1)|O(2)|O(3)|O(4)|O(5)|O(6)|O(7);
	enable_sys_tick(F_SYS_TICK_CLK/400);
	cur = frame_a;
	next = frame_b;
	usart1_rx_pa10_dma3_enable(recv_buf, sizeof(recv_buf), 48e6/500000);
}

void input_loop(void)
{

	for(;;)
	{
		uint16_t *tmp = cur;
		cur = next; /* swap is not atomic, but only the assignment of cur needs to be */
		next = tmp;

		/* */
	}
}

int main(void)
{
	init();

	input_loop();

	for(;;);

	return 0;
}


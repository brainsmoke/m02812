#include <stdlib.h>
#include "stm32f0xx.h"

#include "bitbang_relay.h"
#include "util.h"
#include "fsm.h"

uint16_t frame_a[N_VALUES];
uint16_t frame_b[N_VALUES];
uint8_t table[ROUTING_TABLE_SIZE];
uint16_t *cur;
uint16_t *next;
uint8_t *routing_table;
volatile uint8_t *recv_p;

#define RECV_BUF_SZ (320)
volatile uint8_t recv_buf[RECV_BUF_SZ];

#define O(c) (1<<(2*c))
#define ALT_FN(c) (2<<(2*c))
#define SWD (ALT_FN(13)|ALT_FN(14))

static void init(void)
{
	int i;
	for (i=0; i<N_VALUES; i++)
		remainders[i] = i*153;
	cur = frame_a;
	next = frame_b;
	recv_p=recv_buf;
	clock48mhz();
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN; 	// enable the clock to GPIOA
	GPIOA->ODR = 0;
	GPIOA->MODER = SWD|O(0)|O(1)|O(2)|O(3)|O(4)|O(5)|O(6)|O(7);

	GPIOB->ODR = 1<<1;
	GPIOB->MODER = O(1);
	usart1_rx_pa10_dma3_enable(recv_buf, RECV_BUF_SZ, 48e6/1e6);
	enable_sys_tick(F_SYS_TICK_CLK/400);
}

static int read_next_frame(void)
{
	int i, c;

	for(i=0; i<N_VALUES; i++)
	{
		if (recv_p == &recv_buf[RECV_BUF_SZ])
			recv_p -= RECV_BUF_SZ;

		while(recv_p == &recv_buf[RECV_BUF_SZ-DMA1_Channel3->CNDTR]);

		c = *recv_p++;

		if (recv_p == &recv_buf[RECV_BUF_SZ])
			recv_p -= RECV_BUF_SZ;

		while(recv_p == &recv_buf[RECV_BUF_SZ-DMA1_Channel3->CNDTR]);

		c |= (*recv_p++)<<8;

		if (c > 0xff00)
			break;

		next[i] = c;
	}
	for (; i<N_VALUES; i++)
	{
		next[i] = 0;
	}

	int s=GOOD;

	if (c == 0xffff)
		s = GOOD_FFFF;
	else if (c > 0xff00)
		s = BAD_FF;

	for(;;)
	{
		if (recv_p == &recv_buf[RECV_BUF_SZ])
			recv_p -= RECV_BUF_SZ;

		while(recv_p == &recv_buf[RECV_BUF_SZ-DMA1_Channel3->CNDTR]);

		c = *recv_p++;

		if (c == 0)
			i = IN_00;
		else if (c < 0xf0)
			i = IN_01_EF;
		else if (c == 0xf0)
			i = IN_F0;
		else if (c == 0xff)
			i = IN_FF;
		else
			i = IN_F1_FE;

		s = fsm[s][i];

		if (s == GOOD_RETURN)
			return 1;
		else if (s == BAD_RETURN)
			return 0;
	}
}

int main(void)
{
	init();
	for(;;)
	{
		uint16_t *tmp = cur;
		cur = next; /* swap is not atomic, but only the assignment of cur needs to be */
		next = tmp;
		while (! read_next_frame() );
	}
}


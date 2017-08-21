#include <stdlib.h>
#include "stm32f0xx.h"

void enable_sys_tick(uint32_t ticks)
{
	SysTick->LOAD = ticks;
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk|SysTick_CTRL_TICKINT_Msk;
}

void clock48mhz(void)
{
	/* 1 wait state required for flash accesses at clockspeeds > 24MHz */
	/* enable prefetching */
	FLASH->ACR |= (1 * FLASH_ACR_LATENCY)  |  FLASH_ACR_PRFTBE;

	/* HSI is 8MHz, PLL has a 2x divider & config a 12x multplier -> 48MHz */
	RCC->CFGR = ( RCC->CFGR & ~RCC_CFGR_PLLMULL ) | RCC_CFGR_PLLMULL12;

	/* turn on PLL */
	RCC->CR |= RCC_CR_PLLON;
	while( !(RCC->CR & RCC_CR_PLLRDY) );

	/* switch to PLL for system clock */
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}


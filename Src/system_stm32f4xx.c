#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f407xx.h"
#include "Helpers/logger.h"

/**
 * Set system clock frequency to 72Mhz
 */
uint32_t SystemCoreClock = 72000000U; /*!< System Clock Frequency (Core Clock) */

/**
 * System log debug level
 */
LogLevel system_log_level = LOG_LEVEL_DEBUG;

/**
 * Configure and initialize clock
 * 	PLL
 * 	/M => 4
 * 	*N => 72
 * 	/p => /2
 * 	/Q => /3
 * 	AHB => /1
 * 	APB1 => /2
 * 	APB2 => /2
 */
static void ConfigureClock(void) {

	// flash read latency
	MODIFY_REG(FLASH->ACR,
			FLASH_ACR_LATENCY,
			_VAL2FLD(FLASH_ACR_LATENCY,FLASH_ACR_LATENCY_2WS));

	// Turn on HSE
	SET_BIT(RCC->CR,RCC_CR_HSEON);

	// wait until HSE is stable
	while(!READ_BIT(RCC->CR,RCC_CR_HSERDY));

	// configure PLL
	MODIFY_REG(RCC->PLLCFGR,
			RCC_PLLCFGR_PLLM|
			RCC_PLLCFGR_PLLN|
			RCC_PLLCFGR_PLLP|
			RCC_PLLCFGR_PLLQ|
			RCC_PLLCFGR_PLLSRC,
			_VAL2FLD(RCC_PLLCFGR_PLLM,4)|
			_VAL2FLD(RCC_PLLCFGR_PLLN,72)|
			_VAL2FLD(RCC_PLLCFGR_PLLP,0)|
			_VAL2FLD(RCC_PLLCFGR_PLLQ,3)|
			_VAL2FLD(RCC_PLLCFGR_PLLSRC,1));

	// enable PLL
	SET_BIT(RCC->CR,RCC_CR_PLLON);

	// wait until PLL is stable
	while(!READ_BIT(RCC->CR,RCC_CR_PLLRDY));

	// set AHB,APB1 and APB2 dividers.
	MODIFY_REG(RCC->CFGR,
			RCC_CFGR_HPRE|
			RCC_CFGR_PPRE1|
			RCC_CFGR_PPRE2,
			_VAL2FLD(RCC_CFGR_HPRE,RCC_CFGR_HPRE_DIV1)|
			_VAL2FLD(RCC_CFGR_PPRE1,RCC_CFGR_PPRE1_DIV2)|
			_VAL2FLD(RCC_CFGR_PPRE2,RCC_CFGR_PPRE2_DIV2)
			);
	// switch clock to HSE
	MODIFY_REG(RCC->CFGR,RCC_CFGR_SW,_VAL2FLD(RCC_CFGR_SW,RCC_CFGR_SW_PLL));

	// wait until PLL is stable
	while(READ_BIT(RCC->CFGR,RCC_CFGR_SWS)!= RCC_CFGR_SWS_PLL);

	// Disable HSI
	CLEAR_BIT(RCC->CR,RCC_CR_HSION);
}

static void EnableMC01(void){
	MODIFY_REG(RCC->CFGR,
			RCC_CFGR_MCO1|
			RCC_CFGR_MCO1PRE
			,_VAL2FLD(RCC_CFGR_MCO1,3)|
			_VAL2FLD(RCC_CFGR_MCO1PRE,5)
			);
	// Enable GPIOA port clock
	SET_BIT(RCC->AHB1ENR,RCC_AHB1ENR_GPIOAEN);

}

static void ConfigureMC01_Gpio(void){

	// Configure PA8 speed to medium
	MODIFY_REG(GPIOA->OSPEEDR,
			GPIO_OSPEEDER_OSPEEDR8,
			_VAL2FLD(GPIO_OSPEEDR_OSPEED8,1));

	// Put PA8 alternate function mode
	MODIFY_REG(GPIOA->MODER,GPIO_MODER_MODE8,_VAL2FLD(GPIO_MODER_MODE8,2));


//	// Set PA8 alternate function
//	MODIFY_REG(GPIOA->AFR[1],
//			GPIO_AFRH_AF,
//			_VAL2FLD(GPIO_AFRH_AFRH8,0));

}

void SystemInit(void) {

//	EnableMC01();
//	ConfigureMC01_Gpio();
	ConfigureClock();

}

void SystemCoreClockUpdate(void) {

}

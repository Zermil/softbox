// @ToDo: Error codes as return values? Or some additional info as to what kind of error occured

internal b32 uc_is_init(void)
{
    return(uc_base_is_init);
}

internal b32 uc_tick_init(u32 priority)
{
    b32 error = 0;
    if (SysTick_Config(SystemCoreClock / 1000) > 0) {
        error = 1;
    }

    if (!error) {
        if (priority < (1 << __NVIC_PRIO_BITS)) {
            u32 group_priority = NVIC_GetPriorityGrouping();
            NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(group_priority, priority, 0));
            uc_tick_priority = priority;
        } else {
            error = 1;
        }
    }
    
    b32 result = !error;
    return(result);
}

internal b32 uc_init(void)
{
    b32 error = 0;
    
    // @ToDo: We could also set prefetching from here? Worth considering at some point
    // FLASH->ACR |= ...

    NVIC_SetPriorityGrouping(UC_INIT_GROUP_PRIORITY);
    
    if (!uc_tick_init(UC_INIT_TICK_PRIORITY)) {
        error = 1;
    }

    if (!error) {
        uc_base_is_init = 1;
    }

    b32 result = !error;
    return(result);
}

// @ToDo: Timeouts when turning on different peripherals.
// while (ready) should also check for timeouts so that it won't cause infinite loops
internal b32 uc_init_hse_clock(u32 m, u32 n, u32 p, u32 q)
{
    b32 error = 0;
    if (!uc_is_init()) {
        error = 1;
    } else {
        volatile u32 dummy = 0;
    
        // @Note: Enable power control register.
        // Dummy reads are needed, they work as a sort of 'wait' between: 'want to enable' -> 'enabled'
        // some sources say to do two dummy reads, but afaik one is good enough
        RCC->APB1ENR |= RCC_APB1ENR_PWREN;
        dummy = RCC->APB1ENR;
        PWR->CR |= PWR_CR_VOS;
        dummy = PWR->CR;

        // @Note: Enable HSE
        RCC->CR |= RCC_CR_HSEON;
        while (!(RCC->CR & RCC_CR_HSERDY));

        // @Note: Configure PLL with all prescalers (mults and divs), HSE is set as PLL source
        RCC->PLLCFGR = ((RCC_PLLCFGR_PLLSRC_HSE) |
                        (m << RCC_PLLCFGR_PLLM_Pos) |
                        (n << RCC_PLLCFGR_PLLN_Pos) |
                        (((p >> 1) - 1) << RCC_PLLCFGR_PLLP_Pos) |
                        (q << RCC_PLLCFGR_PLLQ_Pos));

        // @Note: Enable PLL
        RCC->CR |= RCC_CR_PLLON;
        while (!(RCC->CR & RCC_CR_PLLRDY));

        // @Note: Set appropriate latency for flash, this is _last when decreasing_ CPU frequency
        FLASH->ACR |= FLASH_ACR_LATENCY_2WS;
        dummy = FLASH->ACR;
        
        if ((dummy & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_2WS) {
            error = 1;
        } else {
            // @Note: Configure PLL as systemclock and more prescalers
            RCC->CFGR |= (RCC_CFGR_SW_PLL << RCC_CFGR_SW_Pos);
            while ((RCC->CFGR & RCC_CFGR_SWS_PLL) != (RCC_CFGR_SW_PLL << RCC_CFGR_SWS_Pos));

            // @Note: Prescalers for PCLK1
            RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

            // @Note: Update SystemCoreClock variable and configure 1ms timeouts
            SystemCoreClockUpdate();
            if (!uc_tick_init(uc_tick_priority)) {
                error = 1;
            }
        }
    }

    b32 result = !error;
    return(result);
}

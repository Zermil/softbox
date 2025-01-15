#ifndef BASE_INIT_H
#define BASE_INIT_H

// @Note: 4 bits for pre-emption priority, 0 bits for subpriority
#define UC_INIT_GROUP_PRIORITY 3
// @Note: 15 (0x0F) is all 4 bits set, which is exactly what STM32F4xx uses
#define UC_INIT_TICK_PRIORITY 15

internal b32 uc_is_init(void);
internal b32 uc_tick_init(u32 priority);
internal b32 uc_init(void);
internal b32 uc_init_hse_clock(u32 m, u32 n, u32 p, u32 q);

global b32 uc_base_is_init = 0;
global u32 uc_tick_priority = (1 << __NVIC_PRIO_BITS);

#endif // BASE_INIT_H

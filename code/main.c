#include <stm32f4xx.h>

#define LED_PIN 13

#include "./base/base_inc.h"
#include "./usb/usb.h"

#include "./base/base_inc.c"
#include "./usb/usb.c"

extern u32 uc_ticks;
internal void delay_ms(u32 ms)
{
    u32 start = uc_ticks;
    u32 end = uc_ticks + ms;

    if (start > end) {
        while (uc_ticks > start);
    }

    while (uc_ticks < end);
}

void usb_handler(void)
{
    usb_interrupt_routine();
}

int main(void)
{
    uc_init();
    
    // @Note: These values are very important, corresponding to each
    // multiplier and divider in clock diagram. Make sure you at least
    // have a vague idea of what you're doing before changing those.
    uc_init_hse_clock(25, 336, 4, 7);
    
    volatile u32 dummy;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    dummy = RCC->AHB1ENR;

    // @Note: This immediately lights up the led
    GPIOC->MODER |= (0x01 << GPIO_MODER_MODER13_Pos);

    usb_init();    
    usb_start();
    
    while (1) {
    }
    
    return 0;
}

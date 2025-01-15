#include "./base/base_types.h"

#define ISR_ARRAY_SIZE 101

extern u32 _idata, _sdata, _edata, _sbss, _ebss, _stack;

volatile u32 uc_ticks;

int main(void);
void default_handler(void);

void reset_handler(void);
void nmi_handler(void) __attribute__((weak, alias("default_handler")));
void hardfault_handler(void) __attribute__((weak, alias("default_handler")));
void systick_handler(void);
void usb_handler(void) __attribute__((weak, alias("default_handler")));

__attribute__((used, section(".isr_vector")))
u32 isr_vector[ISR_ARRAY_SIZE] = {
    (u32) &_stack,
    (u32) &reset_handler, // Reset
    (u32) &nmi_handler, // NMI
    (u32) &hardfault_handler, // HardFault
    0, // MemManage
    0, // BusFault
    0, // UsageFault
    0, // -
    0, // -
    0, // -
    0, // -
    0, // SVCall
    0, // Debug Monitor
    0, // -
    0, // PendSV
    (u32) &systick_handler, // Systick

    // External interrupts
    
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    (u32) &usb_handler,
};

void reset_handler(void)
{
    u32 *flash_data = &_idata;
    u32 *ram_data = &_sdata;
    u32 *ram_end = &_edata;
    
    while (ram_data < ram_end) {
        *ram_data++ = *flash_data++;
    }

    u32 *bss_start = &_sbss;
    u32 *bss_end = &_ebss;
    
    while (bss_start < bss_end) {
        *bss_start++ = 0;
    }
    
    main();

    // @Note: We should never really return from main, but just in case something goes wrong
    while (1);
}

void default_handler(void)
{
    while (1);
}

void systick_handler(void)
{
    uc_ticks += 1;
}

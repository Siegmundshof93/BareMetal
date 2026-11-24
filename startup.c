typedef void (*function_ptr)(void);

extern unsigned int _sidata, _sdata, _edata, _sbss, _ebss;
int main(void);

void EXTI15_10_IRQHandler(void);
void USART2_IRQHandler(void);

void Reset_Handler(void) {
    // Copy data segment from flash to RAM
    unsigned int *src = &_sidata;
    unsigned int *dest = &_sdata;
    while (dest < &_edata) {
        *dest++ = *src++;
    }

    // Zero-initialize bss segment
    dest = &_sbss;
    while (dest < &_ebss) {
        *dest++ = 0;
    }

    main();
}

void Default_Handler(void) {
    while (1);
}

// Vector table padded up to index 56 for EXTI Line [15:10] (IRQ 40)
__attribute__((section(".isr_vector")))
function_ptr vector_table[] = {
    (function_ptr)0x20018000, // 0: Initial Stack Pointer (96K RAM top)
    Reset_Handler,            // 1: Reset
    Default_Handler,          // 2: NMI
    Default_Handler,          // 3: HardFault
    Default_Handler,          // 4: MemManage
    Default_Handler,          // 5: BusFault
    Default_Handler,          // 6: UsageFault
    0, 0, 0, 0,               // 7-10: Reserved
    Default_Handler,          // 11: SVCall
    Default_Handler,          // 12: Debug Monitor
    0,                        // 13: Reserved
    Default_Handler,          // 14: PendSV
    Default_Handler,          // 15: SysTick

    // IRQs 0 to 39 placeholders
    Default_Handler, Default_Handler, Default_Handler, Default_Handler,
    Default_Handler, Default_Handler, Default_Handler, Default_Handler,
    Default_Handler, Default_Handler, Default_Handler, Default_Handler,
    Default_Handler, Default_Handler, Default_Handler, Default_Handler,
    Default_Handler, Default_Handler, Default_Handler, Default_Handler,
    Default_Handler, Default_Handler, Default_Handler, Default_Handler,
    Default_Handler, Default_Handler, Default_Handler, Default_Handler,
    Default_Handler, Default_Handler, Default_Handler, Default_Handler,
    Default_Handler, Default_Handler, Default_Handler, Default_Handler,
    Default_Handler, Default_Handler,

    USART2_IRQHandler, // USART2 (IRQ38)

    Default_Handler,

    // Index 56 (IRQ 40)
    EXTI15_10_IRQHandler      // 56: EXTI Line[15:10]
};

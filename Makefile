TARGET = firmware
CPU = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = $(CPU) -O0 -g -Wall -Wextra -std=c99 -ffreestanding
LDFLAGS = $(CPU) -Tstm32l476rg.ld -nostdlib -lgcc

all: $(TARGET).bin

$(TARGET).elf: main.o startup.o i2c.o usart.o
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f *.o *.elf *.bin

flash: all
	/usr/local/bin/st-flash write $(TARGET).bin 0x08000000

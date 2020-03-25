TARGET     = kernel8
TARGET_ELF = kernel8.elf
TARGET_IMG = kernel8.img

PREFIX  = aarch64-linux-gnu-
GCC     = $(PREFIX)gcc
LD      = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS  = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
LDFLAGS = -nostdlib -nostartfiles

QEMU         = qemu-system-aarch64
MEMORY       = 128
MACHINE_TYPE = raspi3
DEV          = stdio # "vc" in graphical mode and "stdio" in non graphical mode.

%.o: %.S
	$(GCC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(GCC) $(CFLAGS) -c $< -o $@

$(TARGET_IMG): boot.o font.o $(OBJS)
	$(LD) $(LDFLAGS) boot.o font.o $(OBJS) -T linker.ld -o $(TARGET_ELF)
	$(OBJCOPY) -O binary $(TARGET_ELF) $(TARGET_IMG)

font.o: font.psf
	$(LD) -r -b binary -o font.o font.psf

qemu:
	$(QEMU) -m $(MEMORY) -M $(MACHINE_TYPE) -kernel $(TARGET_IMG) -serial $(DEV)

clean:
	rm $(TARGET_ELF) $(TARGET_IMG) *.o

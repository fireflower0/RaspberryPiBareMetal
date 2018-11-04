TARGET     = kernel8
TARGET_ELF = kernel8.elf
TARGET_IMG = kernel8.img

PREFIX  = aarch64-linux-gnu-
GCC     = $(PREFIX)gcc
AS      = $(PREFIX)as
LD      = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy

CFLAGS  = -fno-stack-protector -c -Wall -Wextra

OBJS   = boot.o main.o mmio.o uart.o mailbox.o

STEXT  = 0x8000

RM     = rm -f

$(TARGET_IMG): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

$(TARGET_ELF): $(OBJS)
	$(LD) -Ttext $(STEXT) -o $(TARGET_ELF) $+

%.o: %.S
	$(AS) -o $@ $<

%.o: %.c
	$(GCC) $(CFLAGS) -o $@ $<

clean:
	$(RM) *~ *.o *.elf *.img *.map

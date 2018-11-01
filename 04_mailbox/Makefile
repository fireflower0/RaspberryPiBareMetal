TARGET = kernel8
CROSS  = aarch64-linux-gnu-
CFLAGS = -c -Wall -Wextra
OBJS   = boot.o main.o mmio.o uart.o
STEXT  = 0x80000
# STEXT  = 0x40080000

$(TARGET).img: $(TARGET).elf
	$(CROSS)objcopy -O binary $< $@

$(TARGET).elf: $(OBJS)
	$(CROSS)ld -Ttext $(STEXT) -o $(TARGET).elf $+

.S.o:
	$(CROSS)as -o $@ $<

.c.o:
	$(CROSS)gcc -fno-stack-protector $(CFLAGS) -o $@ $<

clean:
	rm -f *~ *.o *.elf *.img *.map

.PHONY: clean

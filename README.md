# RaspberryPiBareMetal

## Files to prepare

Insert the following files into FAT32 format microSD card.  

* [bootcode.bin](https://github.com/raspberrypi/firmware/blob/master/boot/bootcode.bin)
* [start.elf](https://github.com/raspberrypi/firmware/blob/master/boot/start.elf)
* config.txt : Write by yourself
* kernel8.img : Apps you created

```
# Example of config.txt

# Activate the ARM core on AArch64
arm_control=0x200

# Enable UART
enable_uart=1
```

## Compiler

Linaro's GCC for AArch 64 Bare-Metal  

[linaro(downloads)](https://www.linaro.org/downloads/)  

## Qemu

* Qemu 2.12.1
* qemu-system-aarch64

```
$ make qemu
```

or

```
$ qemu-system-aarch64 -m 128 -M raspi3 -kernel kernel8.img -serial stdio
```

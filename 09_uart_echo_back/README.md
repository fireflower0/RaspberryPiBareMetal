# UART

## Qemu

* Start Address
  * 80000 => 0x40080000
* Qem monitor
  * Command "Ctrl + Alt + 2"

```
qemu-system-aarch64 -cpu cortex-a57 -M virt -kernel kernel8.img -serial stdio
```

## Demo

![qemu_uart_echoback](https://github.com/fireflower0/RaspberryPiBareMetal/blob/master/09_uart_echo_back/img/echoback.png)

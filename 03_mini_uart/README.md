# Mini UART

## PC side setting

USB serial conversion cable: ttl-232r-5v  
* Red: Vcc (5V)
* Black: GND
* Orange: TXD
* Yellow: RXD

I used "gtkterm".  
* Port: /dev/ttyUSB0
* Baud Rate: 115200
* Data: 8 bit
* Parity: none
* Stop: 1 bit
* Flow Control: none

## Raspberry Pi side setting

config.txt

```
enable_uart = 1
```

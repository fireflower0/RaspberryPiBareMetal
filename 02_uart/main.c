#define UARTDR      (*(volatile unsigned int *)0x09000000)
#define UARTFR      (*(volatile unsigned int *)0x09000018)
#define UARTFR_TXFF (1U << 5)

void put_char(char ch){
    while(UARTFR & UARTFR_TXFF);
    UARTDR = (unsigned int)ch;
}

void put_str(char *str){
    while(*str != '\0')
        put_char(*str++);
}

int main(void){
    put_str("Hello, world!");

    while(1)
        ;

    return 0;
}

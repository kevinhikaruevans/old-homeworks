#include <stdio.h>

/******************************************/
typedef unsigned int u32;

char *ctable = "0123456789ABCDEF";

int rpu(u32 x, int base)
{  
    char c;
    if (x){
       c = ctable[x % base];
       rpu(x / base, base);
       putchar(c);
    }
}

int printu(u32 x)
{
   (x==0)? putchar('0') : rpu(x, 10);
   putchar(' ');
}
/******************************************/

void prints(char *s) {
    while(*s != '\0') {
        putchar(*s++);
    }
}

void printd(int x) {
    if (x < 0) {
        putchar('-');
        printu((u32)-x);
    } else {
        printu((u32)x);
    }
}

void printx(u32 x) {
    if (x == 0) {
        prints("0x0");
    } else {
        prints("0x");
        rpu(x, 16);
        putchar(' ');
    }
}

void printo(u32 x) {
    if (x == 0) {
        prints("00");
    } else {
        prints("0");
        rpu(x, 8);
        putchar(' ');
    }
}

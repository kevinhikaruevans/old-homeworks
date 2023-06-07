#ifndef PRINTUTIL_H
#define PRINTUTIL_H

/* decorations to make the output prettier */
#define ANSI_ESC_RESET "\x1b[0m"
#define ANSI_ESC_BLUE  "\x1b[34m"
#define ANSI_ESC_GREEN "\x1b[32m"
#define ANSI_ESC_RED   "\x1b[31m"
#define ANSI_ESC_BOLD  "\x1b[1m"

// gcc -DDEBUG_ON lab2.c && ./a.out
#ifdef DEBUG_ON
    #define DEBUG(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#else
    #define DEBUG(fmt, ...) fmt
#endif

// print to stderr for errors and add some pretty red color
#define ERROR(fmt, ...)    fprintf(stderr, ANSI_ESC_BOLD ANSI_ESC_RED "error: " ANSI_ESC_RESET fmt "\n", ##__VA_ARGS__)


#endif
#include "../part2/part2.h"

/**
 * @brief Prints a percent-formatted string.
 * 
 * @param fmt The format
 * @param ... The arguments
 * @return int The number of bytes printed, -ish.
 */
int myprintf(char *fmt, ...)
{
    char *cp = fmt;
    int *ip = ((int *)&cp) + sizeof(char *) + sizeof(char *);
    /*                       ^                ^
     *                      /                /
     *                     +----------------+
     *                    /
     * using an offset of 8 bytes, to compensate for both char *cp and 
     * char *fmt preceding *ip on the stack
     */

    while (*cp != '\0')
    {
        if (*cp == '%')
        {
            char c = *++cp;
            switch (c)
            {
            default:
            case '\0':
                break;
            case 'c':
                //putchar(*++cp);
                putchar(*ip);
                break;
            case 's':
                prints((char *)*ip);
                break;

            case 'u':
                printu(*ip);
                break;

            case 'i':
            case 'd':
                printd(*ip);
                break;

            case 'o':
                printo(*ip);
                break;

            case 'x':
                printx(*ip);
                break;

            case '%':
                putchar('%');
                break;
            }

            ip++;
            cp++;
        }
        else
        {
            putchar(*cp++);

            if (*cp == '\n')
            {
                /* "for each \n, spit out an extra \r" */
                putchar('\r');
            }
        }
    }

    return fmt - cp;
}

int main(int argc, char *argv[ ], char *env[ ])
{
    // //myprintf("test %c", 'B');
    // myprintf("test %c %s\n", 'a', "testing");

    // myprintf("my %s number is %d\n", "favorite", 12345);

    // myprintf("the number %d in hex is %x and in octal is %o\n", 123, 123, 123);

    myprintf("argc = %d; argv = ", argc);
    for(int i = 0; i < argc; i++) {
        myprintf("%s ", argv[i]);
    }
    myprintf("; env = \n");
    
    char *ptr = env[0];
    for(int i = 0; env[i] != NULL; i++) {
        myprintf("\t%s\n", env[i]);
    }

    myprintf("cha=%c string=%s      dec=%d hex=%x oct=%o neg=%d\n", 
	       'A', "this is a test", 100,    100,   100,  -100);
    return 0;
}
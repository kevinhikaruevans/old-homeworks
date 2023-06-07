#include <stdio.h>
#include <string.h>

struct {
    char protocol[8];
    char domain[32];
    char uri[32];
} request;
int main() {
    char url[] = "http://example.com/";

    char *protocol = strtok(url, "//");
    char *domain = strtok(NULL, "/");
    char *uri = strtok(NULL, "\0") || "";

    
    printf("done!\r\n");
    //printf("ptr = %s", ptr);
    return 0;

}
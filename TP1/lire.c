#include <stdio.h>

int main () {
    int c;
	char buf[1000];
	setvbuf(stdout, buf, _IOLBF, 200);
    while ((c = getchar()) != EOF) {
        putchar(c);
    }
    return (0);
}

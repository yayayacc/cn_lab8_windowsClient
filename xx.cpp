#include <iostream>
#include <stdio.h>
#include <string.h>

int main() {
    FILE* fp = fopen("info.txt", "wb");
    char  buffer[10];
    for (int i = 0; i < 1000; ++i) {
        memset(buffer, (i % 23) + 95, sizeof(buffer));
        fwrite(buffer, 1, sizeof(buffer), fp);
        fwrite("\r\n", 1, 2, fp);
    }
    fclose(fp);
    return 0;
}
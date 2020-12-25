#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "disk.h"
#include "file.c"
#include "shell.h"

int main(){
    if (!open_system()) {
        printf("Open File System failed.\n");
    }
    flag_close = 0;
    while(!flag_close) {
        printf(">>>> ");
        fgets(buf, MAX_ARG, stdin);
        get_command();
        run_command();
    }
}


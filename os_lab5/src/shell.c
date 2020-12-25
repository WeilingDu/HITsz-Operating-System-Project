#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "shell.h"
#include "file.h"

char* skip_space(char* s) {
    while (isspace(*s)) s++;
    return s;
}

void get_command() {
    char* cmd = buf;
    cmd = skip_space(cmd);
    char* space = strchr(cmd, ' ');
    int i = 0;
    while (space != NULL) {
        space[0] = '\0';
        argv[i] = cmd;
        i++;
        cmd = skip_space(space + 1);
        space = strchr(cmd, ' ');
    }

    if (cmd[0] != '\0') {
        argv[i] = cmd;
        space = strchr(cmd, '\n');
        space[0] = '\0';
        i++;
    }
    argv[i] = NULL;
    argc = i;
}


void run_command() {
    char* command = argv[0];
    if (!strcmp(command, "touch")) 
    {
        if (argc != 2) 
        {
            printf("Arguments nums error!!!\n");
            return;
        }
        touch(argv[1]);
        return;
    } else if (!strcmp(command, "mkdir")) 
    {
        if (argc != 2) 
        {
            printf("Arguments nums error!!!\n");
            return;
        }
        mkdir(argv[1]); 
    } else if (!strcmp(command, "ls"))
    {
        if (argc < 1)
        {
            printf("Arguments nums error!!!\n");
            return;
        }
        if (argc == 1)
        {
            argv[1] = "/\0";
            ls(argv[1]);
            return;
        }
        if (argc > 2)
        {
            printf("Arguments nums error!!!\n");
            return;
        }
        ls(argv[1]);
    } else if (!strcmp(command, "cp")){
        if (argc != 3)
        {
            printf("Arguments nums error!!!\n");
            return;
        }
        cp(argv[1], argv[2]);
        // printf("argv[1]: %s\n", argv[1]);
        // printf("argv[2]: %s\n", argv[2]);

    } else if (!strcmp(command, "shutdown"))
    {
        if (argc != 1)
        {
            printf("Arguments nums error!!!\n");
            return;
        }
        shutdown();
        flag_close = 1;
        return;
    } else
    {
        printf("Can't find this command.\n");
        return;
    }
}
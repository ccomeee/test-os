#include "terminal.h"
#include "io.h"
#include "stdlib.h"
#include <string.h>
#include "stdio.h"

void launch_terminal(void) {
    char command[512];
    printf("簡易終端（輸入 exit 離開）\n");
    while (1) {
        printf("> ");
        if (!fgets(command, sizeof(command), stdin)) break;
        if (strncmp(command, "exit", 4) == 0) break;
        system(command);
    }
}
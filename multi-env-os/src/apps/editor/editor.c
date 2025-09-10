#include "editor.h"
#include "stdio.h"
#include "stdlib.h"
#include "io.h"

void launch_editor(void) {
    char filename[256];
    printf("請輸入要編輯的檔名: ");
    scanf("%255s", filename);
    char command[512];
    snprintf(command, sizeof(command), "notepad %s", filename); // windows下用notepad
    system(command);
}
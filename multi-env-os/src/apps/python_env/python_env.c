#include "python_env.h"
#include <stdio.h>
#include <stdlib.h>

int is_python_installed(void) {
    // Windows: where python
    return system("where python >nul 2>nul") == 0;
}
void install_python(void) {
    printf("正在下載並安裝 Python...\n");
    // 這裡可以呼叫 winget, scoop, chocolatey, 或直接下載安裝包
    // 範例: 用 winget
    system("winget install -e --id Python.Python.3");
}
void setup_python_env(void) {
    if (!is_python_installed()) {
        install_python();
    } else {
        printf("Python 已安裝。\n");
    }
}
void launch_python_env(void) {
    printf("啟動 Python 互動環境...\n");
    system("python");
}
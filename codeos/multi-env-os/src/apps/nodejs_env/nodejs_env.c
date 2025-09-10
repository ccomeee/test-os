#include "nodejs_env.h"
#include "stdio.h"
#include <stdlib.h>

int is_node_installed(void) {
    return system("where node >nul 2>nul") == 0;
}
void install_node(void) {
    printf("正在下載並安裝 Node.js...\n");
    system("winget install -e --id OpenJS.NodeJS");
}
void setup_nodejs_env(void) {
    if (!is_node_installed()) {
        install_node();
    } else {
        printf("Node.js 已安裝。\n");
    }
}
void launch_nodejs_env(void) {
    printf("啟動 Node.js 互動環境...\n");
    system("node");
}
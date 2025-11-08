#include <libc.h>
#include <resources.h>
#include <string.h>

typedef struct {
    const char* name;
    const unsigned char* ptr;
    unsigned int size;
} res_entry_t;

static res_entry_t g_entries[32];
static unsigned int g_count = 0;
static int g_inited = 0;

static unsigned int read_u32(const char** p) {
    // ASCII 十進位，直到非數字
    unsigned int v = 0; const char* s = *p;
    while (*s >= '0' && *s <= '9') { v = v*10 + (unsigned)(*s - '0'); s++; }
    *p = s; return v;
}

static void resources_init_if_needed() {
    if (g_inited) return; g_inited = 1;
    // 讀取 0x800C 處的 {addr,size}
    const unsigned short* meta = (const unsigned short*)0x800C;
    unsigned int addr = ((unsigned int)meta[1] << 16) | meta[0];
    unsigned int size = ((unsigned int)meta[3] << 16) | meta[2];
    if (addr == 0 || size < 6) return;
    const char* cur = (const char*)(addr);
    const char* end = (const char*)(addr + size);
    // 檢查魔數 RPK1\n
    if (end - cur < 5) return;
    if (!(cur[0]=='R' && cur[1]=='P' && cur[2]=='K' && cur[3]=='1' && cur[4]=='\n')) return;
    cur += 5;
    while (cur < end && g_count < 32) {
        // 讀 name 到空白或換行
        const char* name_start = cur;
        while (cur < end && *cur!=' ' && *cur!='\n' && *cur!='\r') cur++;
        if (cur>=end) break;
        int name_len = (int)(cur - name_start);
        if (name_len <= 0) break;
        // 跳過空白
        if (*cur==' ') cur++;
        // 讀 size
        unsigned int fsize = read_u32(&cur);
        // 跳到行末（\n）
        while (cur < end && *cur!='\n') cur++;
        if (cur<end && *cur=='\n') cur++;
        if (end - cur < (int)fsize) break;
        // 記錄條目
        static char names_buf[32][64];
        if ((unsigned)name_len >= sizeof(names_buf[0])) name_len = sizeof(names_buf[0])-1;
        for (int i=0;i<name_len;i++) names_buf[g_count][i]=name_start[i];
        names_buf[g_count][name_len]='\0';
        g_entries[g_count].name = names_buf[g_count];
        g_entries[g_count].ptr = (const unsigned char*)cur;
        g_entries[g_count].size = fsize;
        g_count++;
        cur += fsize;
    }
}

int find_resource(const char* name, const unsigned char** data, unsigned int* len) {
    if (!name || !data || !len) return -1;
    resources_init_if_needed();
    for (unsigned int i=0;i<g_count;i++) {
        if (strcmp(g_entries[i].name, name)==0) {
            *data = g_entries[i].ptr;
            *len = g_entries[i].size;
            return 0;
        }
    }
    // 特別處理：若找 wallpaper.png 但找不到，嘗試第一個 .png 或 .bmp
    if (strcmp(name, "wallpaper.png")==0) {
        for (unsigned int i=0;i<g_count;i++) {
            const char* n = g_entries[i].name; int L=(int)strlen(n);
            if (L>=4 && n[L-4]=='.' && (n[L-3]=='p'||n[L-3]=='P') && (n[L-2]=='n'||n[L-2]=='N') && (n[L-1]=='g'||n[L-1]=='G')) {
                *data = g_entries[i].ptr; *len = g_entries[i].size; return 0;
            }
        }
    }
    if (strcmp(name, "wallpaper.bmp")==0) {
        for (unsigned int i=0;i<g_count;i++) {
            const char* n = g_entries[i].name; int L=(int)strlen(n);
            if (L>=4 && (n[L-4]=='.') && (n[L-3]=='b'||n[L-3]=='B') && (n[L-2]=='m'||n[L-2]=='M') && (n[L-1]=='p'||n[L-1]=='P')) {
                *data = g_entries[i].ptr; *len = g_entries[i].size; return 0;
            }
        }
    }
    return -1;
}

#include "video_mode.h"

static unsigned short g_w = 320;
static unsigned short g_h = 200;
static unsigned short g_bpp = 8;
static unsigned short g_pitch = 320;
static unsigned char* g_fb = (unsigned char*)0xA0000;

void video_mode_init() {
    // 在本專案中沒有開分頁，實體=線性，直接讀 0x8000
    volatile vbe_mode_info_simple_t* info = (volatile vbe_mode_info_simple_t*)VBE_INFO_PHYS_ADDR;
    if (info->physbase != 0 && info->width >= 320 && info->height >= 200 && (info->bpp == 8 || info->bpp == 16)) {
        g_w = info->width;
        g_h = info->height;
        g_bpp = info->bpp;
        g_pitch = info->pitch ? info->pitch : (info->width * (info->bpp / 8));
        g_fb = (unsigned char*)(unsigned int)(info->physbase);
    } else {
        // 回退 VGA 320x200x8bpp
        g_w = 320;
        g_h = 200;
        g_bpp = 8;
        g_pitch = 320;
        g_fb = (unsigned char*)0xA0000;
    }
}

unsigned short get_screen_width()  { return g_w; }
unsigned short get_screen_height() { return g_h; }
unsigned short get_screen_bpp()    { return g_bpp; }
unsigned short get_screen_pitch()  { return g_pitch; }
unsigned char* get_framebuffer_ptr(){ return g_fb; }

#ifndef VIDEO_MODE_H
#define VIDEO_MODE_H

#include <libc.h>

typedef struct {
    unsigned short width;
    unsigned short height;
    unsigned short bpp;     // bits per pixel
    unsigned short pitch;   // bytes per scanline
    unsigned int   physbase; // physical framebuffer address
} vbe_mode_info_simple_t;

// Bootloader writes this struct at physical 0x00008000
#define VBE_INFO_PHYS_ADDR 0x00008000

// Kernel-side initialization: read the struct from physical mapping (identity)
void video_mode_init();

// Getters
unsigned short get_screen_width();
unsigned short get_screen_height();
unsigned short get_screen_bpp();
unsigned short get_screen_pitch();
unsigned char* get_framebuffer_ptr();

#endif // VIDEO_MODE_H

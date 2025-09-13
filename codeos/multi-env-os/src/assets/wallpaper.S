.section .rodata
.global wallpaper_start
.global wallpaper_end

wallpaper_start:
    .incbin "../src/wallpaper/wallpaper.bmp"
wallpaper_end:

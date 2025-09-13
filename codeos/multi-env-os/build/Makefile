ASM     = nasm
CC      = i686-elf-gcc
CXX     = i686-elf-g++
LD      = i686-elf-ld
SRC_DIR = ../src
BUILD_DIR = .
INCLUDES = -I$(SRC_DIR)/include

# 编译标志
CFLAGS = -g -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -Wall -Wextra
CXXFLAGS = $(CFLAGS) -g -fno-exceptions -fno-rtti -nostdinc++

# libc sources
LIBC_SRC = $(SRC_DIR)/libc/io.c \
           $(SRC_DIR)/libc/signal.c \
           $(SRC_DIR)/libc/fs.c \
           $(SRC_DIR)/libc/mem.c \
           $(SRC_DIR)/libc/process.c \
           $(SRC_DIR)/libc/dl.c \
           $(SRC_DIR)/libc/socket.c \
           $(SRC_DIR)/libc/time.c \
           $(SRC_DIR)/libc/string.c \
           $(SRC_DIR)/libc/stdio.c \
           $(SRC_DIR)/libc/stdlib.c \
           $(SRC_DIR)/libc/errno.c \
           $(SRC_DIR)/libc/port_io.c

# kernel, app and driver sources
KERNEL_SRC = $(SRC_DIR)/kernel/graphics_main.c
APP_SRC = $(SRC_DIR)/apps/browser/browser.c \
          $(SRC_DIR)/apps/editor/editor.c \
          $(SRC_DIR)/apps/terminal/terminal.c \
          $(SRC_DIR)/apps/python_env/python_env.c \
          $(SRC_DIR)/apps/cpp_env/cpp_env.cpp \
          $(SRC_DIR)/apps/nodejs_env/nodejs_env.c \
          $(SRC_DIR)/apps/file_manager/file_manager.c
DRIVER_SRC = $(SRC_DIR)/drivers/driver.c

# 完整的OBJS列表 - 包含所有檔案
OBJS = $(BUILD_DIR)/start.o \
       $(BUILD_DIR)/graphics_main.o \
       $(BUILD_DIR)/bmp.o \
       $(BUILD_DIR)/resources.o \
       $(BUILD_DIR)/video_mode.o \
       $(BUILD_DIR)/desktop_fs.o \
       $(BUILD_DIR)/desktop_gui.o \
       $(BUILD_DIR)/mouse.o \
       $(BUILD_DIR)/driver.o \
       $(BUILD_DIR)/io.o \
       $(BUILD_DIR)/signal.o \
       $(BUILD_DIR)/fs.o \
       $(BUILD_DIR)/mem.o \
       $(BUILD_DIR)/process.o \
       $(BUILD_DIR)/dl.o \
       $(BUILD_DIR)/socket.o \
       $(BUILD_DIR)/time.o \
       $(BUILD_DIR)/string.o \
       $(BUILD_DIR)/stdio.o \
       $(BUILD_DIR)/stdlib.o \
       $(BUILD_DIR)/errno.o \
       $(BUILD_DIR)/port_io.o \
       $(BUILD_DIR)/browser.o \
       $(BUILD_DIR)/editor.o \
       $(BUILD_DIR)/terminal.o \
       $(BUILD_DIR)/python_env.o \
       $(BUILD_DIR)/cpp_env.o \
       $(BUILD_DIR)/nodejs_env.o \
       $(BUILD_DIR)/file_manager.o

KERNEL_ELF = $(BUILD_DIR)/kernel.elf
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
BOOT_BIN   = $(BUILD_DIR)/boot.bin
IMG        = $(BUILD_DIR)/os.img

# 编译规则
all: $(IMG)

$(BUILD_DIR)/boot.bin: $(SRC_DIR)/boot.s
	$(ASM) -f bin $< -o $@

# 启动代码编译规则 - 添加这一行！
$(BUILD_DIR)/start.o: $(SRC_DIR)/start.s
	$(ASM) -f elf32 $< -o $@

# 通用C编译规则
$(BUILD_DIR)/%.o: $(SRC_DIR)/kernel/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

$(BUILD_DIR)/%.o: $(SRC_DIR)/apps/*/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

$(BUILD_DIR)/%.o: $(SRC_DIR)/drivers/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

$(BUILD_DIR)/%.o: $(SRC_DIR)/libc/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

$(BUILD_DIR)/%.o: $(SRC_DIR)/desktop/%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

# 資產彙編（GNU as 語法 .S）
$(BUILD_DIR)/%.o: $(SRC_DIR)/assets/%.S
	$(CC) -c $< -o $@

# C++编译规则
$(BUILD_DIR)/%.o: $(SRC_DIR)/apps/*/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)

$(KERNEL_ELF): $(OBJS) $(SRC_DIR)/linker.ld
	$(LD) -T $(SRC_DIR)/linker.ld -o $@ $(OBJS)

$(KERNEL_BIN): $(KERNEL_ELF)
	i686-elf-objcopy -O binary $< $@

$(IMG): $(BOOT_BIN) $(KERNEL_BIN)
	cat $^ > $@
	# 確保映像有足夠的扇區
	dd if=/dev/zero bs=512 count=1 >> $@ 2>/dev/null || echo "" >> $@
	@echo "Image created: $@"
	@echo "Boot sector size:" $$(stat -c%s $(BOOT_BIN)) "bytes"
	@echo "Kernel size:" $$(stat -c%s $(KERNEL_BIN)) "bytes"

.PHONY: debug

debug:
	@echo "\n=== GDB 調試方式 ==="
	@echo "1. 啟動 QEMU: qemu-system-i386 -s -S -drive file=os.img,format=raw,if=floppy"
	@echo "2. 另開一 terminal，進入 build 目錄: cd .../build"
	@echo "3. 啟動 GDB: i686-elf-gdb kernel.elf"
	@echo "4. 在 GDB prompt 輸入: target remote localhost:1234"

clean:
	rm -f $(BUILD_DIR)/*.bin $(BUILD_DIR)/*.o $(IMG)

.PHONY: all clean
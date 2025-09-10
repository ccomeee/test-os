#ifndef PORT_IO_H
#define PORT_IO_H

// x86 輸出一個 byte 到 port
void outb(unsigned short port, unsigned char val);

#endif // PORT_IO_H
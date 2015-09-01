#pragma once

#include <termios.h>

int mdv_serial_init(const char *dev);
void mdv_serial_read(int fd, unsigned char* buf, void (*read_cb)(int, unsigned char *));
void mdv_serial_write(int fd, unsigned char* buf);


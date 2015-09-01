#include <stdio.h>
#include "mdv_serial.h"

int main (int argc, char** argv) {
  int fd;
  unsigned char buf[16];
  fd = mdv_serial_init("/dev/ttyUSB0");
  mdv_serial_read(fd, buf, mdv_serial_write);
  return 0;
}

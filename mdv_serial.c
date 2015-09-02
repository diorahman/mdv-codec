#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "mdv_serial.h"

#define MDV_DATA_LEN 16
#define MDV_BAUD_RATE B4800

static int mdv_serial_wait_for_header(int fd, unsigned char *buf);
static void mdv_serial_read_debug(int fd, unsigned char *buf);

static struct termios tio_original_opts;

int mdv_serial_init(const char *dev)
{
    int fd = -1;
    struct termios tio;
    fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
         return -1;
    }
    if (ioctl(fd, TIOCEXCL) < 0) {
         return -1;
    }
    if (fcntl(fd, F_SETFL, 0) < 0) {
         return -1;
    }
    if (tcgetattr(fd, &tio_original_opts) < 0) {
         return -1;
    }
    tio = tio_original_opts;
    cfmakeraw(&tio);
    cfsetspeed(&tio, MDV_BAUD_RATE);
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 0;
    tio.c_cflag |= (CS8 | CLOCAL | CREAD);
    tio.c_iflag = 0;
    tio.c_oflag = 0;
    tio.c_lflag = 0;

    // check if we need to use ioctl here, especially on mac
    // ioctl(fd, IOSSIOSPEED, &speed), where speed is speed_t
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &tio);
    return fd;
}

void mdv_serial_write(int fd, unsigned char *buf) {
    write(fd, buf, MDV_DATA_LEN);
}

void mdv_serial_read(int fd, unsigned char *buf, void (*read_cb)(int, unsigned char *)) {
    int rc = -1;
    int hc = -1;
    do {
        while(hc < 0) {
            hc = mdv_serial_wait_for_header(fd, buf);  
        }
        rc = read(fd, buf, MDV_DATA_LEN);       
        if (read_cb != NULL) {
            return (*read_cb)(fd, buf);
        }
        mdv_serial_read_debug(fd, buf);
    }
    while(rc == MDV_DATA_LEN && (buf[0] == 0x4C && buf[1] == 0x4E));
    
    fprintf(stderr, "%s: data dropped, with header: %.2x %2.x\n", __FILE__, buf[0], buf[1]);
    mdv_serial_read(fd, buf, read_cb);
}

int mdv_serial_wait_for_header(int fd, unsigned char *buf) {
    struct termios tio;
    int i;
    while (1) {
      read(fd, buf, 1);
      if (buf[0] == 0x4C) {
          read(fd, buf, 1);
          if (buf[0] == 0x4E) {
              for (i = 2; i < MDV_DATA_LEN; i++) {
                  read(fd, buf, 1);
              }
              if (tcgetattr(fd, &tio) < 0) {
                  return -1;
              }
              tio.c_cc[VMIN] = MDV_DATA_LEN;
              tcflush(fd, TCIFLUSH);
              tcsetattr(fd, TCSANOW, &tio);
              return 0;
          }
      }
    }
    return -1;
}

void mdv_serial_read_debug(int fd, unsigned char *buf) {
    int i;
    for (i = 0; i < MDV_DATA_LEN; i++) {
        printf("%.2x", buf[i]);
    }
    printf("\n");
}

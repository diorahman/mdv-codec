#include <stdio.h>
#include "mdv_serial.h"

#define FRAMES 45

FILE *fp;
int count;

void write_to_file(int fd, unsigned char *buf) {
  fwrite(buf, 1, 16, fp);
  fflush(fp);
  count++;
  /*
  if (count == FRAMES) {
    fclose(fp);
    printf("closed\n");
  }
  */
}

void write_to_both(int fd, unsigned char *buf)
{
    write_to_file(fd, buf);
    mdv_serial_write(fd, buf);
}

void read_from_file(int fd, unsigned char *buf) {
    static FILE *f;
    static int cnt = 0;

    printf(".");
    fflush(stdout);

    if (!f) {
      f = fopen("1.mdv", "rb");
      if (!f) {
          puts("Error opening file");
          return;
      }
    }

    if (fread(buf, 16, 1, f) == 1) {
      mdv_serial_write(fd, buf);
      cnt++;
    } else {
      printf("EOF at frame %d\n", cnt);
      mdv_serial_write(fd, buf);
    }
}

void play_from_file(int fd) 
{
    FILE *f;
    char buf[16];
    int cnt = 0;

    f = fopen("1.mdv", "rb");
    if (!f) {
        puts("Error opening file");
        return;
    }

    while (fread(buf, 16, 1, f) == 1) {
      mdv_serial_write(fd, buf);
      cnt++;
    } 

    printf("EOF at frame %d\n", cnt);
}
#if 0
int main()
{
    char s[10];
    int fd = mdv_serial_init("/dev/ttyUSB0");
    if (fd < 0)
      return 1;
    play_from_file(fd);
    puts("Press ENTER to quit");
    fgets(s, sizeof(s), stdin);
    return 0;
}
#else
int main (int argc, char** argv) {
  int fd;
  count = 0;
  unsigned char buf[16];
  puts("Serial init..");
  fd = mdv_serial_init("/dev/ttyUSB0");
#if 0
  fp = fopen("1.mdv", "wb");
  if (!fp) {
    puts("Error opening file");
    return 1;
  }
  puts("Recording...");
  mdv_serial_read(fd, buf, write_to_both);
#else
  puts("Reading...");
  mdv_serial_read(fd, buf, read_from_file);
#endif
  // mdv_serial_read(fd, buf, write_to_file);
  return 0;
}
#endif


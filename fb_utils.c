#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#define PIXEL_POINTER(x, y) ((fbd+x+(y*vinfo.xres)))

char *fb_name = "/dev/fb0";
int fb_fd;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
long int screensize = 0;
char *fbd = 0;

void erreur(char *message, int errorcode)
{
  perror(message);
  exit(errorcode);
}

void init_fb(char *fb_name)
{
  fb_fd = open(fb_name, O_RDWR);
  if (!fb_fd)
    erreur("\nfopen :\
      ouverture du framebuffer impossible\n", 1);
  
  if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo))
    erreur("FBIOGET_FSCREENINFO : \
      erreur à la lecture des informations fixes.\n", 2);
  
  if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo))
    erreur("FBIOGET_VSCREENINFO : \
      erreur à la lecture des informations variables.\n", 3);
  
  screensize = (vinfo.xres * vinfo.yres)
    * ((vinfo.bits_per_pixel + 7) >> 3);
  
  fbd = (char *)mmap(0, screensize,
    PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
  if ((int)fbd == -1)
    erreur("échec de mmap() du framebuffer.\n", 4);
}

inline set_pixel8(int x, int y, char c)
{
  *PIXEL_POINTER(x, y) = c;
}

inline set_pixel8_clip(int x, int y, char c)
{
  if (((unsigned int)x < vinfo.xres)
    && ((unsigned int)y < vinfo.yres))
  set_pixel8(x, y, c);
}

int main(int argc, char *argv[])
{
  init_fb(fb_name);
  
  int i, j, k;
  
  for (k = 0; k < 32; k++)
  {
    usleep(2000);
    for (i = 0; i < 100; i++)
      for (j = 0; j < 1000; j++)
        set_pixel8_clip(i+k*100, j, k%16);
  }
  
  return EXIT_SUCCESS;
}

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
unsigned char *fbd = 0;

void erreur(char *message, int errorcode);
void init_fb(char *fb_name);

int main(int argc, char *argv[])
{
  init_fb(fb_name);
  
  return EXIT_SUCCESS;
}

void erreur(char *message, int errorcode)
{
  perror(message);
  exit(errorcode);
}

void init_fb(char *fb_name)
{
  int i, j;
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
  
  printf("smem_len      = %d\n", finfo.smem_len);
  printf("xpanstep      = %d\n", finfo.xpanstep);
  printf("ypanstep      = %d\n", finfo.ypanstep);
  printf("ywrapstep     = %d\n", finfo.ywrapstep);
  printf("line_length   = %d\n", finfo.line_length);
  
  printf("screenwidth   = %d\n", vinfo.xres);
  printf("screenheight  = %d\n", vinfo.yres);
  printf("vscreenwidth  = %d\n", vinfo.xres_virtual);
  printf("vscreenheight = %d\n", vinfo.yres_virtual);
  printf("screendepth   = %d\n", vinfo.bits_per_pixel);
  
  fbd = (unsigned char*)mmap(0, finfo.smem_len,
    PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
  
  printf("fdb = %x\n", fbd);
  printf("smem_len = %d\n", finfo.smem_len);
  printf("smem_start = %x\n", finfo.smem_start);
  
  
  if ((int)fbd == -1)
    erreur("échec de mmap() du framebuffer.\n", 4);
  
  for (i = 100; --i >= 0;)
    for (j = 100; --j >= 0;)
      *PIXEL_POINTER(i, j) = 0x00;
}

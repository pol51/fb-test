#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#define PIXEL_POINTER(x, y) ((buffer+x+(y*vinfo.xres)))

struct _rgba
{
  char b;
  char g;
  char r;
  char a;
};

union _pixel32
{
  struct _rgba rgba;
  unsigned int raw;
};

typedef union _pixel32 pixel32;

char *fb_name = "/dev/fb0";
int fb_fd = 0;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
long long screensize = 0;
pixel32 *fbd = NULL;
pixel32 *buffer = NULL;
pixel32 **lines = NULL;

int bufferX   = 0;
int bufferY   = 0;
int visibleX  = 0;
int visibleY  = 0;

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
  
  bufferX   = (finfo.smem_len / vinfo.yres)>>2;
  bufferY   = vinfo.yres;
  visibleX  = vinfo.xres;
  visibleY  = vinfo.yres;
  screensize = finfo.smem_len;
  
  fbd = (pixel32 *)mmap(0, screensize,
    PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
  if (fbd == -1)
    erreur("échec de mmap() du framebuffer.\n", 4);
  
  buffer = (pixel32*)malloc(screensize);
  memset(buffer, 0xff, screensize);
  lines = (pixel32**)malloc(vinfo.yres * sizeof(pixel32*));
  int i = vinfo.yres;
  for ( ; --i >= 0; )
    lines[i] = buffer + i * bufferX;
}

void finalize_fb()
{
  if (buffer) free(buffer);
  if (lines) free(lines);
  if (fbd) munmap(fbd, screensize);
  if (fb_fd) close(fb_fd);
}

inline void reverse_pixel(int x, int y)
{
  pixel32 *old = lines[y] + x;
  old->raw = ~old->raw;
}

inline void blit_screen()
{
  memcpy(fbd, buffer, screensize);
}

inline void draw_rect(int x, int y, int w, int h, pixel32 color)
{
  int i = y + h;
  for (; --i >= y; )
    wmemset(lines[i] + x, color.raw, w);
}

inline void draw_hline(int x, int y, int l, pixel32 color)
{
  wmemset(lines[y] + x, color.raw, l);
}

inline void draw_vline(int x, int y, int l, pixel32 color)
{
  int i = y + l;
  for (; --i >= y; )
    lines[i][x] = color;
}

int main(int argc, char *argv[])
{
  init_fb(fb_name);
  
  pixel32 red;  red.raw  = 0xffff0000;
  pixel32 blue; blue.raw = 0xff0000ff;
  int i = 0xff;
  for ( ; --i >= 0; )
  {
    blue.rgba.g = i;
    draw_rect(i, i, 200, 200, blue);
    draw_vline(400 + i<<1, i<<1, 50, red);
    draw_hline(400 + i<<1, i<<1, 50, red);
  }
  
  while (1)
    blit_screen();
  
  finalize_fb();
  
  return EXIT_SUCCESS;
}

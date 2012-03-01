#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <linux/input.h>

#define test_bit(bit, array) (array[bit/8] & (1<<(bit%8)))

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "systax error!\nusage: %s <input_device>\n", argv[0]);
    return EXIT_FAILURE;
  }
  
  int fd = -1;
  char name[256] = "Unknown";
  
  if ((fd = open(argv[1], O_RDONLY)) < 0)
  {
    perror("evdev open");
    return EXIT_FAILURE;
  }
  
  if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0)
  {
    perror("evdev ioctl EVIOCGNAME");
    close(fd);
    return EXIT_FAILURE;
  }
  
  printf("Device name: %s\n", name);
  
  struct input_id device_info;
  u_int8_t evtype_b[(EV_MAX + 7)/8];
  
  if (ioctl(fd, EVIOCGID, &device_info))
  {
    perror("evdev ioctl EVIOCGID");
    close(fd);
    return EXIT_FAILURE;
  }
  
  if (ioctl(fd, EVIOCGBIT(0, EV_MAX), evtype_b) < 0)
  {
    perror("evdev ioctl EVIOCGBIT");
    close(fd);
    return EXIT_FAILURE;
  }
  
  int i;
  for (i = 0; i < EV_MAX; i++)
  {
    if (test_bit(i, evtype_b))
    {
      printf("  Event type 0x%02x ", i);
      switch (i)
      {
        case EV_SYN:        printf(" (Synch Events)\n");          break;
        case EV_KEY:        printf(" (Key Events)\n");            break;
        case EV_REL:        printf(" (Relative Events)\n");       break;
        case EV_ABS:        printf(" (Absolute Events)\n");       break;
        case EV_MSC:        printf(" (Msc Events)\n");            break;
        case EV_SW:			    printf(" (Sw Events)\n");             break;
        case EV_LED:		    printf(" (Led)\n");                   break;
        case EV_SND:		    printf(" (Sound)\n");                 break;
        case EV_REP:		    printf(" (Repeat Events)\n");         break;
        case EV_FF:			    printf(" (ForceFeedback Events)\n");  break;
        case EV_PWR:		    printf(" (Power)\n");                 break;
        case EV_FF_STATUS:  printf(" (ForceFeedbackStatus)\n");   break;
        default:        printf(" ?\n");
      }
    }
  }
  
  close(fd);
  
  return EXIT_SUCCESS;
}

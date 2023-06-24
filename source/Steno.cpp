#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include <iostream>
using namespace std;

int main()
{
  cout << "Hello, world!" << endl;
  int fd = open("/dev/input/js1", O_RDONLY);
  if (fd < 0) {
    perror("Could not open joystick");
    exit(EXIT_FAILURE);
  }

  cout << "Reading Gamepad" << endl;
  while (1) {
    struct js_event event;
    if (read(fd, &event, sizeof(event)) != sizeof(event)) {
      perror("Error reading joystick");
      exit(EXIT_FAILURE);
    }

    // Handle joystick events here
    if (event.type == JS_EVENT_BUTTON) {
      printf("Button %u state: %u\n", event.number, event.value);
    } else if (event.type == JS_EVENT_AXIS) {
      printf("Axis %u value: %d\n", event.number, event.value);
    }
  }

  return 0;
}

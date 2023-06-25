#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <libnotify/notify.h>

using namespace std;

const int A_BUTTON = 0;
const int B_BUTTON = 1;
const int X_BUTTON = 2;
const int Y_BUTTON = 3;
const int LB_BUTTON = 4;
const int RB_BUTTON = 5;
const int BACK_BUTTON = 6;
const int START_BUTTON = 7;
const int JOY_BUTTON_LEFT = 8;
const int JOY_BUTTON_RIGHT = 9;
const int X_AXIS = 0;
const int Y_AXIS = 1;
const int Z_AXIS = 2;
const int RX_AXIS = 3;
const int RY_AXIS = 4;
const int RZ_AXIS = 5;
const int MAX_AXIS = 5;

int getFileDescriptor(){
  int fileDescriptor = open("/dev/input/js0", O_RDONLY);
  if (fileDescriptor < 0) {
    perror("Could not open joystick");
    exit(EXIT_FAILURE);
  }

  return fileDescriptor;
}

int main()
{
  // Initialize the libnotify library
  notify_init("My Program");

  // Create a notification object with the desired message and title
  NotifyNotification* notification = notify_notification_new("Title", "Message", NULL);

  // Show the notification
  notify_notification_show(notification, NULL);

  // Free the memory used by the notification object
  g_object_unref(G_OBJECT(notification));

  // Gamepad init
  int fileDescriptor = getFileDescriptor();
  cout << "Hello, world!" << endl;
  int numButtons;
  int numAxes;
  ioctl(fileDescriptor, JSIOCGAXES, &numAxes);
  ioctl(fileDescriptor, JSIOCGBUTTONS, &numButtons);
  unsigned char buttonStates[numButtons];
  int axisValues[numAxes];
  cout << "Reading Gamepad" << endl;
  std::thread thread;
  while (1) {
    struct js_event event;

    if (read(fileDescriptor, &event, sizeof(event)) != sizeof(event)) {
      perror("Error reading joystick");
      exit(EXIT_FAILURE);
    }

    // Handle joystick events here
    if (event.type == JS_EVENT_BUTTON) {
      printf("Button %u state: %u\n", event.number, event.value);
      buttonStates[event.number] = event.value;
    } else if (event.type == JS_EVENT_AXIS) {
      printf("Axis %u value: %d\n", event.number, event.value);
      axisValues[event.number] = event.value;
    }

    if (event.type == JS_EVENT_BUTTON && event.number == 0 && event.value == 1 && event.type & ~JS_EVENT_INIT == JS_EVENT_AXIS && event.number == 0 && event.value < -32767) {
      printf("Left analogue stick to the left and X button pushed\n");
    }
    if (axisValues[X_AXIS] < -20000) {
      cout << "Left analog stick pushed to the left" << std::endl;
    }

    if (axisValues[X_AXIS] > 20000) {
      cout << "Left analog stick pushed to the right" << std::endl;
    }

    if (axisValues[Y_AXIS] < -20000) {
      cout << "Left analog stick pushed to the top" << std::endl;
    }

    if (axisValues[Y_AXIS] > 20000) {
      cout << "Left analog stick pushed to the bottom" << std::endl;
    }

    if (event.number == 0 && buttonStates[0] && axisValues[0] < -5000 && axisValues[1] < -5000) {
      cout << "Left analog stick and X button pressed!" << std::endl;
    }

    if (event.type == JS_EVENT_AXIS && event.number == 0 && event.value < -32000 && event.type == JS_EVENT_BUTTON && event.number == X_BUTTON && event.value == 1) {
      cout << "Left analogue stick to the left and X being pressed" << endl;
    }
  }
  close(fileDescriptor);
  return 0;
}

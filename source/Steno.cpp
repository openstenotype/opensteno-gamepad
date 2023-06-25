#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include <iostream>
using namespace std;

const int A_BUTTON = 0; // button mapping for "A" button on Xbox controller
const int B_BUTTON = 1; // mapping for "B" button
const int X_BUTTON = 2; // mapping for "X" button
const int Y_BUTTON = 3; // mapping for "Y" button
const int LB_BUTTON = 4; // mapping for left bumper button
const int RB_BUTTON = 5; // mapping for right bumper button
const int BACK_BUTTON = 6; // mapping for the "back" button
const int START_BUTTON = 7; // mapping for the "start" button
const int JOY_BUTTON_LEFT = 8; // mapping for the left joystick button
const int JOY_BUTTON_RIGHT = 9; // mapping for the right joystick button
const int X_AXIS = 0;
const int Y_AXIS = 1;
const int Z_AXIS = 2;
const int RX_AXIS = 3;
const int RY_AXIS = 4;
const int RZ_AXIS = 5;
const int MAX_AXIS 5;

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
  int fileDescriptor = getFileDescriptor();
  cout << "Hello, world!" << endl;
  int numButtons;
  int numAxes;
  ioctl(fileDescriptor, JSIOCGAXES, &numAxes);
  ioctl(fileDescriptor, JSIOCGBUTTONS, &numButtons);
  unsigned char buttonStates[numButtons];
  int axisValues[numAxes];
  cout << "Reading Gamepad" << endl;
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
  }

  close(fileDescriptor);
  return 0;
}

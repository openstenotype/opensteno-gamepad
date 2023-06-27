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
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

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
const int GAMEPAD_REGISTER_ZONE = 20000;

void showNotification(const char* message) {
  notify_init("Opensteno");
  NotifyNotification* notification = notify_notification_new("Opensteno", message, NULL);
  notify_notification_show(notification, NULL);
  // Free the memory used by the notification object
  g_object_unref(G_OBJECT(notification));
}

void simulateKeyPress(Display* display, const char* character)
{
  KeyCode key = XKeysymToKeycode(display, XStringToKeysym(character));
  XTestFakeKeyEvent(display, key, True, 0); //press
  XSync(display, False);
  XTestFakeKeyEvent(display, key, False, 0); //release
  XSync(display, False);
  XFlush(display);
}

int getFileDescriptor(){
  int fileDescriptor = open("/dev/input/js0", O_RDONLY);
  if (fileDescriptor < 0) {
    perror("Could not open joystick");
    exit(EXIT_FAILURE);
  }

  return fileDescriptor;
}

bool isAnalogueLeft(int axisValues[]) {
  return axisValues[X_AXIS] < -GAMEPAD_REGISTER_ZONE;
}

bool isAnalogueRight(int axisValues[]) {
  return axisValues[X_AXIS] > GAMEPAD_REGISTER_ZONE;
}

bool isAnalogueTop(int axisValues[]) {
  return axisValues[Y_AXIS] < -GAMEPAD_REGISTER_ZONE;
}

bool isAnalogueBottom(int axisValues[]) {
  return axisValues[Y_AXIS] > GAMEPAD_REGISTER_ZONE;
}

bool isAnalogueTopLeft(int axisValues[]) {
  return isAnalogueLeft(axisValues) && isAnalogueTop(axisValues);
}

bool isAnalogueTopRight(int axisValues[]) {
  return isAnalogueRight(axisValues) && isAnalogueTop(axisValues);
}

bool isAnalogueBottomLeft(int axisValues[]) {
  return isAnalogueLeft(axisValues) && isAnalogueBottom(axisValues);
}

bool isAnalogueBottomRight(int axisValues[]) {
  return isAnalogueRight(axisValues) && isAnalogueBottom(axisValues);
}

int main()
{
  showNotification("Listening for gamepad input");
  //open a display
  Display* display = XOpenDisplay(NULL);
  if (display == NULL){
    printf("Error: Unable to connect to X11\n");
    return 1;
  }
  // Gamepad init
  int fileDescriptor = getFileDescriptor();
  cout << "Opensteno Started" << endl;
  int numButtons;
  int numAxes;
  ioctl(fileDescriptor, JSIOCGAXES, &numAxes);
  ioctl(fileDescriptor, JSIOCGBUTTONS, &numButtons);
  unsigned char buttonStates[numButtons]={0};
  int axisValues[numAxes];
  memset(axisValues, 0, sizeof(axisValues));
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
      if (buttonStates[RB_BUTTON] == 1) {
        simulateKeyPress(display, "BackSpace");
      } else if (isAnalogueTopLeft(axisValues)) {

      } else if (isAnalogueTopRight(axisValues)) {
      } else if (isAnalogueLeft(axisValues)) {
        switch (event.number) {
        case X_BUTTON:
          simulateKeyPress(display, "a");
          break;
        case A_BUTTON:
          simulateKeyPress(display, "b");
          break;
        case B_BUTTON:
          simulateKeyPress(display, "c");
          break;
        }

        cout << "Left analog stick pushed to the left" << endl;
      } else if (isAnalogueRight(axisValues)) {

      }
    } else if (event.type == JS_EVENT_AXIS) {
      printf("Axis %u value: %d\n", event.number, event.value);
      axisValues[event.number] = event.value;
    }

    if (axisValues[Y_AXIS] < -GAMEPAD_REGISTER_ZONE) {
      cout << "Left analog stick pushed to the top" << endl;
    }

    if (axisValues[Y_AXIS] > GAMEPAD_REGISTER_ZONE) {
      cout << "Left analog stick pushed to the bottom" << endl;
    }
  }
  close(fileDescriptor);
  XCloseDisplay(display);
  return 0;
}

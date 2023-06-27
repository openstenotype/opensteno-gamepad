#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
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

// Left Analogue Stick
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

// Right Analogue Stick
bool isRightAnalogueLeft(int axisValues[]) {
  return axisValues[RX_AXIS] < -GAMEPAD_REGISTER_ZONE;
}

bool isRightAnalogueRight(int axisValues[]) {
  return axisValues[RX_AXIS] > GAMEPAD_REGISTER_ZONE;
}

bool isRightAnalogueTop(int axisValues[]) {
  return axisValues[RY_AXIS] < -GAMEPAD_REGISTER_ZONE;
}

bool isRightAnalogueBottom(int axisValues[]) {
  return axisValues[RY_AXIS] > GAMEPAD_REGISTER_ZONE;
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
  struct js_event event;
  bool spaceActive = false;
  bool enterActive = false;

  while (1) {
    if (read(fileDescriptor, &event, sizeof(event)) != sizeof(event)) {
      perror("Error reading joystick");
      exit(EXIT_FAILURE);
    }

    // Handle joystick events here
    if (event.type == JS_EVENT_BUTTON) {
      printf("Button %u state: %u\n", event.number, event.value);
      buttonStates[event.number] = event.value;

      // Layer 6 Left Analogue Stick to the top left
      //   ###        ###
      //    P
      //  Z   M
      //    B
      if (isAnalogueTopLeft(axisValues)) {
        switch (event.number) {
        case X_BUTTON:
          simulateKeyPress(display, "z");
          break;
        case A_BUTTON:
          simulateKeyPress(display, "b");
          break;
        case B_BUTTON:
          simulateKeyPress(display, "m");
          break;
        case Y_BUTTON:
          simulateKeyPress(display, "p");
          break;
        }
      } else if (isAnalogueTopRight(axisValues)) {
      } else if (isAnalogueTop(axisValues)  && event.value == 1) {
        // Layer 3 Left Analogue Stick to the top
        //   ###        ###
        //    Q
        //  O   Y
        //    S
        switch (event.number) {
        case X_BUTTON:
          simulateKeyPress(display, "o");
          break;
        case A_BUTTON:
          simulateKeyPress(display, "s");
          break;
        case B_BUTTON:
          simulateKeyPress(display, "y");
          break;
        case Y_BUTTON:
          simulateKeyPress(display, "q");
          break;
        }

        // Layer 5 Left Analogue Stick to the bottom
        //   ###        ###
        //    F
        //  V   X
        //    G
      } else if (isAnalogueBottom(axisValues)  && event.value == 1) {
        switch (event.number) {
        case X_BUTTON:
          simulateKeyPress(display, "v");
          break;
        case A_BUTTON:
          simulateKeyPress(display, "g");
          break;
        case B_BUTTON:
         simulateKeyPress(display, "x");
         break;
        case Y_BUTTON:
          simulateKeyPress(display, "f");
          break;
        }
      } else if (isAnalogueLeft(axisValues) && event.value == 1) {
        // Layer 2 Left Analogue Stick to the left
        //   ###        3x <--
        //    D
        //  R   T
        //    N
        switch (event.number) {
        case RB_BUTTON:
          simulateKeyPress(display, "BackSpace");
          simulateKeyPress(display, "BackSpace");
          simulateKeyPress(display, "BackSpace");
          break;
        case X_BUTTON:
          simulateKeyPress(display, "r");
          break;
        case A_BUTTON:
          simulateKeyPress(display, "n");
          break;
        case B_BUTTON:
          simulateKeyPress(display, "t");
          break;
        case Y_BUTTON:
          simulateKeyPress(display, "d");
          break;
        }

        cout << "Left analog stick pushed to the left" << endl;
      } else if (isAnalogueRight(axisValues) && event.value == 1) {
      // Layer 4 Left Analogue Stick to the right
      //   ###        ###
      //    H
      //  W   L
      //    C
        switch (event.number) {
        case X_BUTTON:
          simulateKeyPress(display, "w");
          break;
        case A_BUTTON:
          simulateKeyPress(display, "c");
          break;
        case B_BUTTON:
          simulateKeyPress(display, "l");
          break;
        case Y_BUTTON:
          simulateKeyPress(display, "h");
          break;
        }

      // Layer 1
      //   ###        <--
      //    I           SPC
      //  U   A      SHT   RTN
      //    E
      } else if (buttonStates[X_BUTTON] == 1) {
        simulateKeyPress(display, "u");
      } else if (buttonStates[A_BUTTON] == 1) {
      simulateKeyPress(display, "e");
      } else if (buttonStates[B_BUTTON] == 1) {
        simulateKeyPress(display, "a");
      } else if (buttonStates[Y_BUTTON] == 1) {
        simulateKeyPress(display, "i");
      } else if (buttonStates[RB_BUTTON] == 1) {
        simulateKeyPress(display, "BackSpace");
      }

    } else if (event.type == JS_EVENT_AXIS) {


      printf("Axis %u value: %d\n", event.number, event.value);
      axisValues[event.number] = event.value;
    }

    //     SPC
    //  SHI   RTN

    if (axisValues[RY_AXIS] < -GAMEPAD_REGISTER_ZONE) {
      if (!spaceActive){
        simulateKeyPress(display, "U0020"); //space
      }
      spaceActive = true;
    }

    if (axisValues[RY_AXIS] <= 5000 && axisValues[RY_AXIS] >= -5000) {
      spaceActive = false;
    }

    if (axisValues[RX_AXIS] > GAMEPAD_REGISTER_ZONE) {
      if (!enterActive){
        simulateKeyPress(display, "Return");
      }
      enterActive = true;
    }

    if (axisValues[RX_AXIS] <= 5000 && axisValues[RX_AXIS] >= -5000) {
      enterActive = false;
    }

    if (axisValues[RY_AXIS] > GAMEPAD_REGISTER_ZONE) {
      cout << "Left analog stick pushed to the bottom" << endl;
    }
  }
  close(fileDescriptor);
  XCloseDisplay(display);
  return 0;
}

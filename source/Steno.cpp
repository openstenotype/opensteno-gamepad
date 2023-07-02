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
#include <map>
#include "StenoLib.cpp"

using namespace std;

map<int, map<int, const char*>> layerButtonMaps;
//std::map<int, std::function<bool(int, int)>> layerConditionsMap;

void showNotification(const char* message) {
  notify_init("Opensteno");
  NotifyNotification* notification = notify_notification_new("🎮 Opensteno", message, NULL);
  notify_notification_set_timeout(notification, 10000);
  notify_notification_show(notification, NULL);
  // Free the memory used by the notification object
  g_object_unref(G_OBJECT(notification));
}


void sendKeyEventOld(Display* display, const char* character) {
  XEvent event;
  XKeyEvent keyEvent;

  // Set up the event structure
  event.xkey.type = KeyPress;
  event.xkey.window = DefaultRootWindow(display);
  event.xkey.display = display;
  event.xkey.state = ShiftMask; // Add any modifiers if needed

  // Get the keycode for the character
  KeyCode keycode = XKeysymToKeycode(display, XStringToKeysym(character));
  event.xkey.keycode = keycode;

  // Set up the fields in the key event structure
  keyEvent = event.xkey;
  keyEvent.state = event.xkey.state;

  // Send the event
  XSendEvent(display, event.xkey.window, True, KeyPressMask, &event);
}

void sendKeyEvent(Display *display, const char* character, int modifiers) {
  XEvent event;
  Window focusedWindow;
  int revertTo;
  XGetInputFocus(display, &focusedWindow, &revertTo);

  // Create a key press event
  event.xkey.type = KeyPress;
  event.xkey.display = display;
  event.xkey.window = focusedWindow;
  event.xkey.root = DefaultRootWindow(display);
  //event.xkey.subwindow = None;
  //  event.xkey.time = CurrentTime;
  event.xkey.keycode = XKeysymToKeycode(display, XStringToKeysym(character));
  event.xkey.state = modifiers;

  // Send the key press event
  XSendEvent(display, DefaultRootWindow(display), True, KeyPressMask, &event);

  // Create a key release event
  event.xkey.type = KeyRelease;

  // Send the key release event
  XSendEvent(display, DefaultRootWindow(display), True, KeyReleaseMask, &event);

  // Flush the display to ensure the events are processed
  XFlush(display);
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
    cerr << "Could not open joystick: " << endl;
    exit(EXIT_FAILURE);
  }

  return fileDescriptor;
}

int main()
{
  layerButtonMaps[1]  = {{Y_BUTTON, "i"}, {X_BUTTON, "u"}, {A_BUTTON, "e"}, {B_BUTTON, "a"}, {RB_BUTTON, "BackSpace"}};
  layerButtonMaps[2]  = {{Y_BUTTON, "d"}, {X_BUTTON, "r"}, {A_BUTTON, "n"}, {B_BUTTON, "t"}};
  layerButtonMaps[3]  = {{Y_BUTTON, "q"}, {X_BUTTON, "o"}, {A_BUTTON, "s"}, {B_BUTTON, "y"}};
  layerButtonMaps[4]  = {{Y_BUTTON, "h"}, {X_BUTTON, "w"}, {A_BUTTON, "c"}, {B_BUTTON, "l"}};
  layerButtonMaps[5]  = {{Y_BUTTON, "f"}, {X_BUTTON, "v"}, {A_BUTTON, "g"}, {B_BUTTON, "x"}};
  layerButtonMaps[6]  = {{Y_BUTTON, "p"}, {X_BUTTON, "z"}, {A_BUTTON, "b"}, {B_BUTTON, "m"}};
  layerButtonMaps[7]  = {{Y_BUTTON, "?"}, {X_BUTTON, "."}, {A_BUTTON, "j"}, {B_BUTTON, "k"}};
  layerButtonMaps[8]  = {{Y_BUTTON, "="}, {X_BUTTON, "!"}, {A_BUTTON, "+"}, {B_BUTTON, "-"}};
  layerButtonMaps[9]  = {{Y_BUTTON, ">"}, {X_BUTTON, "<"}, {A_BUTTON, "("}, {B_BUTTON, ")"}};
  layerButtonMaps[10] = {{Y_BUTTON, "&"}, {X_BUTTON, "\""}, {A_BUTTON, "{"}, {B_BUTTON, "}"}};
  layerButtonMaps[11] = {{Y_BUTTON, "#"}, {X_BUTTON, "|"}, {A_BUTTON, "@"}, {B_BUTTON, "'"}};
  layerButtonMaps[12] = {{Y_BUTTON, "ß"}, {X_BUTTON, "ü"}, {A_BUTTON, "ä"}, {B_BUTTON, "ö"}};
  layerButtonMaps[13] = {{Y_BUTTON, "3"}, {X_BUTTON, "4"}, {A_BUTTON, "1"}, {B_BUTTON, "2"}, {RB_BUTTON, "5"}};
  layerButtonMaps[14] = {{Y_BUTTON, "8"}, {X_BUTTON, "9"}, {A_BUTTON, "6"}, {B_BUTTON, "7"}, {RB_BUTTON, "0"}};

  showNotification("Listening for gamepad input");

  showNotification(getHelp(layerButtonMaps));
  //open a display
  Display* display = XOpenDisplay(NULL);
  if (display == NULL){
    cout << "Error: Unable to connect to X11" << endl;
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
      cerr << "Error reading joystick: " << endl;
      exit(EXIT_FAILURE);
    }

    // Handle joystick events here
    if (event.type == JS_EVENT_BUTTON) {
      cout << "Button " << event.number << " state: " << event.value << endl;
      buttonStates[event.number] = event.value;

      if (isAnalogueTopLeft(axisValues)) {
        if (layerButtonMaps[6].count(event.number) != 0) {
          simulateKeyPress(display, layerButtonMaps[6].at(event.number));
        }
      } else if (isAnalogueTopRight(axisValues)) {
        if (layerButtonMaps[7].count(event.number) != 0) {
          simulateKeyPress(display, layerButtonMaps[7].at(event.number));
        }
      } else if (isAnalogueBottomLeft(axisValues)) {
      } else if (isAnalogueBottomRight(axisValues)) {
      } else if (isAnalogueTop(axisValues)  && event.value == 1) {
        if (layerButtonMaps[3].count(event.number) != 0) {
          simulateKeyPress(display, layerButtonMaps[3].at(event.number));
        }
      } else if (isAnalogueBottom(axisValues)  && event.value == 1) {
        if (layerButtonMaps[5].count(event.number) != 0) {
          simulateKeyPress(display, layerButtonMaps[5].at(event.number));
        }
      } else if (isAnalogueLeft(axisValues) && event.value == 1) {
        switch (event.number) {
        case RB_BUTTON:
          simulateKeyPress(display, "BackSpace");
          simulateKeyPress(display, "BackSpace");
          simulateKeyPress(display, "BackSpace");
          break;
        default:
          if (layerButtonMaps[2].count(event.number) != 0) {
            simulateKeyPress(display, layerButtonMaps[2].at(event.number));
          }
          break;
        }
      } else if (isAnalogueRight(axisValues) && event.value == 1) {
        if (layerButtonMaps[4].count(event.number) != 0) {
          simulateKeyPress(display, layerButtonMaps[4].at(event.number));
        }
      } else if (isPadTopLeft(axisValues) && event.value == 1) {
        if (layerButtonMaps[14].count(event.number) != 0) {
          simulateKeyPress(display, layerButtonMaps[14].at(event.number));
        }
      } else if (isPadTopRight(axisValues) && event.value == 1) {
      } else if (isPadBottomRight(axisValues) && event.value == 1) {
      } else if (isPadBottomLeft(axisValues) && event.value == 1) {
      } else if (isPadTop(axisValues) && event.value == 1) {
        if (layerButtonMaps[10].count(event.number) != 0) {
          simulateKeyPress(display, layerButtonMaps[10].at(event.number));
        }
      } else if (isPadRight(axisValues) && event.value == 1) {
        if (layerButtonMaps[11].count(event.number) != 0) {
          simulateKeyPress(display, layerButtonMaps[11].at(event.number));
        }
      } else if (isPadBottom(axisValues) && event.value == 1) {
        if (layerButtonMaps[12].count(event.number) != 0) {
          simulateKeyPress(display, layerButtonMaps[12].at(event.number));
        }
      } else if (isPadLeft(axisValues) && event.value == 1) {
        if (layerButtonMaps[13].count(event.number) != 0) {
          simulateKeyPress(display, layerButtonMaps[13].at(event.number));
        }
      } else if (event.value == 1) {
        if (layerButtonMaps[1].count(event.number) != 0) {
          simulateKeyPress(display, layerButtonMaps[1].at(event.number));
        }
      }
    } else if (event.type == JS_EVENT_AXIS) {


      cout << "Axis " << event.number << " Value: " << event.value << endl;
      axisValues[event.number] = event.value;
    }

    //     SPC
    //  SHI   RTN

    if (axisValues[RY_AXIS] < -GAMEPAD_REGISTER_ZONE) {
      if (!spaceActive){
        simulateKeyPress(display, UNICODE_SPACE);
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

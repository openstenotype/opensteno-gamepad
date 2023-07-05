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
#include <X11/Xutil.h>
#include <locale.h>
#include <cstdlib>
#include <algorithm>

using namespace std;

struct ButtonKeyEvent {
  unsigned long int keySymbol; // constants from /usr/include/X11/keysymdef.h
  unsigned int keyMask; // constants from /usr/include/X11/X.h None as default
};

struct ButtonConfiguration {
  string description;
  ButtonKeyEvent keyEvents[5];
};

map<int, map<int, ButtonConfiguration>> layerButtonMaps;

void showNotification(const char* message) {
  notify_init("Opensteno");
  NotifyNotification* notification = notify_notification_new("🎮 Opensteno", message, NULL);
  notify_notification_set_timeout(notification, 10000);
  notify_notification_show(notification, NULL);
  // Free the memory used by the notification object
  g_object_unref(G_OBJECT(notification));
}

void simulateKeyPress(Display* display, ButtonKeyEvent buttonKeyEvent) {
  XEvent event;
  XKeyEvent keyEvent;
  Window focusedWindow;
  int revertTo;
  XGetInputFocus(display, &focusedWindow, &revertTo);
  // Set up the event structure
  event.xkey.type = KeyPress;
  event.xkey.window = focusedWindow;
  event.xkey.display = display;
  event.xkey.state = buttonKeyEvent.keyMask;
  KeyCode keycode;
  keycode = XKeysymToKeycode(display, buttonKeyEvent.keySymbol);
  event.xkey.keycode = keycode;

  keyEvent = event.xkey;
  keyEvent.state = event.xkey.state;
  // Send the event
  XSendEvent(display, event.xkey.window, True, KeyPressMask, &event);
  XFlush(display);
}

int getFileDescriptor(){
  const char* devicePath = "/dev/input/js0";

  if (!(access(devicePath, F_OK) != -1)) {
      devicePath = "/dev/input/js1";
  }

  int fileDescriptor = open(devicePath, O_RDONLY);
  if (fileDescriptor < 0) {
    cerr << "Could not open joystick: " << endl;
    exit(EXIT_FAILURE);
  }

  return fileDescriptor;
}

int main()
{

  layerButtonMaps =
    {
      {1, {
          {
            Y_BUTTON,
            {"i", {XK_i, None}}
          },
          {
            X_BUTTON,
            {"u", {XK_u, None}}
          },
          {
            A_BUTTON,
            {"e", {XK_e, None}}
          },
          {
            B_BUTTON,
            {"a", {XK_a, None}}
          },
          {
            RB_BUTTON,
            {"BackSpace", {XK_BackSpace, None}}
          }
        }
      },
      {2, {
          {
            Y_BUTTON,
            {"d", {XK_d, None}}
          },
          {
            X_BUTTON,
            {"r", {XK_r, None}}
          },
          {
            A_BUTTON,
            {"n", {XK_n, None}}
          },
          {
            B_BUTTON,
            {"t", {XK_t, None}}
          },
          {
            RB_BUTTON,
            {"BackSpace", {{XK_BackSpace, None}, {XK_BackSpace, None}, {XK_BackSpace, None}}}
          }
        }
      },
      {3, {
          {
            Y_BUTTON,
            {"q", {XK_q, None}}
          },
          {
            X_BUTTON,
            {"o", {XK_o, None}}
          },
          {
            A_BUTTON,
            {"s", {XK_s, None}}
          },
          {
            B_BUTTON,
            {"y", {XK_y, None}}
          }
        }
      },
      {4, {
          {
            Y_BUTTON,
            {"h", {XK_h, None}}
          },
          {
            X_BUTTON,
            {"w", {XK_w, None}}
          },
          {
            A_BUTTON,
            {"c", {XK_c, None}}
          },
          {
            B_BUTTON,
            {"l", {XK_l, None}}
          }
        }
      },
      {5, {
          {
            Y_BUTTON,
            {"f", {XK_f, None}}
          },
          {
            X_BUTTON,
            {"v", {XK_v, None}}
          },
          {
            A_BUTTON,
            {"g", {XK_g, None}}
          },
          {
            B_BUTTON,
            {"x", {XK_x, None}}
          }
        }
      },
      {6, {
          {
            Y_BUTTON,
            {"p", {XK_p, None}}
          },
          {
            X_BUTTON,
            {"z", {XK_z, None}}
          },
          {
            A_BUTTON,
            {"b", {XK_b, None}}
          },
          {
            B_BUTTON,
            {"m", {XK_m, None}}
          }
        }
      },
      {7, {
          {
            Y_BUTTON,
            {"?", {XK_question, Mod5Mask}}
          },
          {
            X_BUTTON,
            {".", {XK_period, None}}
          },
          {
            A_BUTTON,
            {"j", {XK_j, None}}
          },
          {
            B_BUTTON,
            {"k", {XK_k, None}}
          }
        }
      },
      {8, {
          {
            Y_BUTTON,
            {"=", {XK_equal, Mod5Mask}}
          },
          {
            X_BUTTON,
            {"!", {XK_exclam, Mod5Mask}}
          },
          {
            A_BUTTON,
            {"+", {XK_plus, Mod5Mask}}
          },
          {
            B_BUTTON,
            {"-", {XK_minus, None}}
          }
        }
      },
      {9, {
          {
            Y_BUTTON,
            {">", {XK_greater, Mod5Mask}}
          },
          {
            X_BUTTON,
            {"<", {XK_less, Mod5Mask}}
          },
          {
            A_BUTTON,
            {"(", {XK_parenleft, Mod5Mask}}
          },
          {
            B_BUTTON,
            {")", {XK_parenright, Mod5Mask}}
          }
        }
      },
      {10, {
          {
            Y_BUTTON,
            {"&", {XK_ampersand, Mod5Mask}}
          },
          {
           X_BUTTON,
           {"\"", {XK_quotedbl, Mod5Mask}}
          },
          {
            A_BUTTON,
            {"{", {XK_braceleft, Mod5Mask}}
          },
          {
            B_BUTTON,
            {"}", {XK_braceright, Mod5Mask}}
          }
        }
      },
      {11, {
          {
            Y_BUTTON,
            {"#", {XK_numbersign, Mod5Mask}}
          },
          {
            X_BUTTON,
            {"|", {XK_bar, Mod5Mask}}
          },
          {
            A_BUTTON,
            {"@", {XK_at, Mod5Mask}}
          },
          {
            B_BUTTON,
            {"'", {XK_quoteleft, Mod5Mask}}
          }
        }
      },
      {12, {
          {
            Y_BUTTON,
            {"ß", {XK_ssharp, None}}
          },
          {
            X_BUTTON,
            {"ü", {XK_udiaeresis, None}}
          },
          {
            A_BUTTON,
            {"ä", {XK_adiaeresis, None}}
          },
          {
            B_BUTTON,
            {"ö", {XK_odiaeresis, None}}
          }
        }
      },
      {13, {
          {
            Y_BUTTON,
            {"3", {XK_3, None}}
          },
          {
            X_BUTTON,
            {"4", {XK_4, None}}
          },
          {
            A_BUTTON,
            {"1", {XK_1, None}}
          },
          {
            B_BUTTON,
            {"2", {XK_2, None}}
          },
          {
            RB_BUTTON,
            {"5", {XK_5, None}}
          }
        }
      },
      {14, {
          {
            Y_BUTTON,
            {"8", {XK_8, None}}
          },
          {
            X_BUTTON,
            {"9", {XK_9, None}}
          },
          {
            A_BUTTON,
            {"6", {XK_6, None}}
          },
          {
            B_BUTTON,
            {"7", {XK_7, None}}
          },
          {
            RB_BUTTON,
            {"0", {XK_0, None}}
          }
        }
      },
      {21, {
          {
            Y_BUTTON,
            {"I", {XK_I, ShiftMask}}
          },
          {
            X_BUTTON,
            {"U", {XK_U, ShiftMask}}
          },
          {
            A_BUTTON,
            {"E", {XK_E, ShiftMask}}
          },
          {
            B_BUTTON,
            {"A", {XK_A, ShiftMask}}
          },
          {
            RB_BUTTON,
            {"backspace", {XK_BackSpace, None}}
          }
        }
      }
  };

  showNotification("Listening for gamepad input");

  //  showNotification(getHelp(layerButtonMaps));
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
  //unsigned char buttonStates[numButtons]={0};
  //int axisValues[numAxes];

  //  unsigned char buttonStates[20]={0};
  unsigned char buttonStates[20];
  int axisValues[20];
  memset(buttonStates, 0, sizeof(buttonStates));
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
    }

    ButtonKeyEvent currentEvents[5];
    if (event.type == JS_EVENT_BUTTON && event.value == 1) {
      if (isAnalogueTopLeft(axisValues) && layerButtonMaps[6].count(event.number) != 0) {
        copy(begin(layerButtonMaps[6].at(event.number).keyEvents), end(layerButtonMaps[6].at(event.number).keyEvents), begin(currentEvents));
      } else if (isAnalogueTopRight(axisValues) && layerButtonMaps[7].count(event.number) != 0) {
        copy(begin(layerButtonMaps[7].at(event.number).keyEvents), end(layerButtonMaps[7].at(event.number).keyEvents), begin(currentEvents));
      } else if (isAnalogueBottomLeft(axisValues) && layerButtonMaps[9].count(event.number) != 0) {
        copy(begin(layerButtonMaps[9].at(event.number).keyEvents), end(layerButtonMaps[9].at(event.number).keyEvents), begin(currentEvents));
      } else if (isAnalogueBottomRight(axisValues) && layerButtonMaps[8].count(event.number) != 0) {
        copy(begin(layerButtonMaps[8].at(event.number).keyEvents), end(layerButtonMaps[8].at(event.number).keyEvents), begin(currentEvents));
      } else if (isAnalogueTop(axisValues) && layerButtonMaps[3].count(event.number) != 0) {
        copy(begin(layerButtonMaps[3].at(event.number).keyEvents), end(layerButtonMaps[3].at(event.number).keyEvents), begin(currentEvents));
      } else if (isAnalogueBottom(axisValues) && layerButtonMaps[5].count(event.number) != 0) {
        copy(begin(layerButtonMaps[5].at(event.number).keyEvents), end(layerButtonMaps[5].at(event.number).keyEvents), begin(currentEvents));
      } else if (isAnalogueLeft(axisValues) && layerButtonMaps[2].count(event.number) != 0) {
        copy(begin(layerButtonMaps[2].at(event.number).keyEvents), end(layerButtonMaps[2].at(event.number).keyEvents), begin(currentEvents));
      } else if (isAnalogueRight(axisValues) && layerButtonMaps[4].count(event.number) != 0) {
        copy(begin(layerButtonMaps[4].at(event.number).keyEvents), end(layerButtonMaps[4].at(event.number).keyEvents), begin(currentEvents));
      } else if (isPadTopLeft(axisValues) && layerButtonMaps[14].count(event.number) != 0) {
        copy(begin(layerButtonMaps[14].at(event.number).keyEvents), end(layerButtonMaps[14].at(event.number).keyEvents), begin(currentEvents));
      } else if (isPadTopRight(axisValues) && event.value == 1) {
      } else if (isPadBottomRight(axisValues) && event.value == 1) {
      } else if (isPadBottomLeft(axisValues) && event.value == 1) {
      } else if (isPadTop(axisValues) && layerButtonMaps[10].count(event.number) != 0) {
        copy(begin(layerButtonMaps[10].at(event.number).keyEvents), end(layerButtonMaps[10].at(event.number).keyEvents), begin(currentEvents));
      } else if (isPadRight(axisValues) && layerButtonMaps[11].count(event.number) != 0) {
        copy(begin(layerButtonMaps[11].at(event.number).keyEvents), end(layerButtonMaps[11].at(event.number).keyEvents), begin(currentEvents));
      } else if (isPadBottom(axisValues) && layerButtonMaps[12].count(event.number) != 0) {
        copy(begin(layerButtonMaps[12].at(event.number).keyEvents), end(layerButtonMaps[12].at(event.number).keyEvents), begin(currentEvents));
      } else if (isPadLeft(axisValues) && layerButtonMaps[13].count(event.number) != 0) {
        copy(begin(layerButtonMaps[13].at(event.number).keyEvents), end(layerButtonMaps[13].at(event.number).keyEvents), begin(currentEvents));
      } else if (layerButtonMaps[1].count(event.number) != 0) {
        if (isLeftTriggerPulled(axisValues)) {
          copy(begin(layerButtonMaps[21].at(event.number).keyEvents), end(layerButtonMaps[21].at(event.number).keyEvents), begin(currentEvents));
        } else {
          copy(begin(layerButtonMaps[1].at(event.number).keyEvents), end(layerButtonMaps[1].at(event.number).keyEvents), begin(currentEvents));
        }

      }
      for(ButtonKeyEvent &buttonEvent : currentEvents){
        if (buttonEvent.keySymbol == 0 && buttonEvent.keyMask == 0) {
          continue;
        }
        simulateKeyPress(display, buttonEvent);
      }
    } else if (event.type == JS_EVENT_AXIS) {


      cout << "Axis " << event.number << " Value: " << event.value << endl;
      axisValues[event.number] = event.value;
    }

    //     SPC
    //  SHI   RTN

    if (axisValues[RY_AXIS] < -GAMEPAD_REGISTER_ZONE) {
      if (!spaceActive){
        //simulateKeyPress(display, UNICODE_SPACE);
      }
      spaceActive = true;
    }

    if (axisValues[RY_AXIS] <= 5000 && axisValues[RY_AXIS] >= -5000) {
      spaceActive = false;
    }

    if (axisValues[RX_AXIS] > GAMEPAD_REGISTER_ZONE) {
      if (!enterActive){
        //simulateKeyPress(display, "Return");
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

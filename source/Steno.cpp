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
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_keysyms.h>

using namespace std;

struct ButtonConfiguration {
  string description;
  unsigned long int keySymbol; // constants from /usr/include/X11/keysymdef.h
  unsigned int keyMask; // constants from /usr/include/X11/X.h None as default
};

//map<int, map<int, const char32_t*>> layerButtonMaps;
//map<int, map<int, const string>> layerButtonMaps;
map<int, map<int, ButtonConfiguration>> layerButtonMaps;
//map<int, map<int, const const*>> layerButtonMaps;
//map<int, LayerSetting> layerConfiguration;

//wchar_t character;
//std::map<int, std::function<bool(int, int)>> layerConditionsMap;

void showNotification(const char* message) {
  notify_init("Opensteno");
  NotifyNotification* notification = notify_notification_new("🎮 Opensteno", message, NULL);
  notify_notification_set_timeout(notification, 10000);
  notify_notification_show(notification, NULL);
  // Free the memory used by the notification object
  g_object_unref(G_OBJECT(notification));
}

void simulateKeyPress(Display* display, ButtonConfiguration button) {
  XEvent event;
  XKeyEvent keyEvent;
  Window focusedWindow;
  int revertTo;
  XGetInputFocus(display, &focusedWindow, &revertTo);
  // Set up the event structure
  event.xkey.type = KeyPress;
  event.xkey.window = focusedWindow;
  event.xkey.display = display;
  event.xkey.state = button.keyMask;
  KeyCode keycode;
  keycode = XKeysymToKeycode(display, button.keySymbol);
  event.xkey.keycode = keycode;

  keyEvent = event.xkey;
  keyEvent.state = event.xkey.state;
  // Send the event
  XSendEvent(display, event.xkey.window, True, KeyPressMask, &event);
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
  layerButtonMaps =
    {
      {1, {{Y_BUTTON, {"i", XK_i}},
           {X_BUTTON, {"u", XK_u, None}},
           {A_BUTTON, {"e", XK_e, None}},
           {B_BUTTON, {"a", XK_a, None}},
           {RB_BUTTON, {"BackSpace", XK_BackSpace, None}}
        }
      },
      {11, {{Y_BUTTON,  {"#", XK_numbersign, Mod5Mask}},
            {X_BUTTON,  {"|", XK_bar, Mod5Mask}},
            {A_BUTTON,  {"@", XK_at, Mod5Mask}},
            {B_BUTTON,  {"'", XK_quoteleft, Mod5Mask}}
        }
      },
      {12, {{Y_BUTTON, {"ß", XK_ssharp, None}},
            {X_BUTTON, {"ü", XK_udiaeresis, None}},
            {A_BUTTON, {"ä", XK_adiaeresis, None}},
            {B_BUTTON, {"ö", XK_odiaeresis, None}},
            {RB_BUTTON, {"BackSpace", XK_BackSpace, None}}
        }
      }
  };
  //   layerButtonMaps[11] = {{Y_BUTTON, {}},
  //                          {X_BUTTON, {}},
  //                          {A_BUTTON, {}},
  //                          {B_BUTTON, {}}};


  // layerButtonMaps[1]  = {{Y_BUTTON, {"i", XK_i}}, {X_BUTTON, {"u", XK_u}, {A_BUTTON, {"e", XK_e}}, {B_BUTTON, {"a", XK_a}}, {RB_BUTTON, {"BackSpace", XK_BackSpace}}};
  //   layerButtonMaps[2]  = {{Y_BUTTON, {"d", XK_d}}, {X_BUTTON, {"r", XK_r}}, {A_BUTTON, {"n", XK_n}}, {B_BUTTON, {"t", XK_t}}};
  //   layerButtonMaps[3]  = {{Y_BUTTON, {"q", XK_q}}, {X_BUTTON, {"o", XK_o}}, {A_BUTTON, {"s", XK_s}}, {B_BUTTON, {"y", XK_y}}};
  //   layerButtonMaps[4]  = {{Y_BUTTON, {"h", XK_h}}, {X_BUTTON, {"w", XK_w}}, {A_BUTTON, {"c", XK_c}}, {B_BUTTON, {"l", XK_l}}};
  //   layerButtonMaps[5]  = {{Y_BUTTON, {"f", XK_f}}, {X_BUTTON, {"v", XK_v}}, {A_BUTTON, {"g", XK_g}}, {B_BUTTON, {"x", XK_x}}};
  //   layerButtonMaps[6]  = {{Y_BUTTON, {"p", XK_p}}, {X_BUTTON, {"z", XK_z}}, {A_BUTTON, {"b", XK_b}}, {B_BUTTON, {"m", XK_m}}};
  //   layerButtonMaps[7]  = {{Y_BUTTON, {"?", XK_question}},
  //                          {X_BUTTON, {".", XK_period}},
  //                          {A_BUTTON, {"j", XK_j}},
  //                          {B_BUTTON, {"k", XK_k}}};
  //   layerButtonMaps[8]  = {{Y_BUTTON, {"=", XK_equal}},
  //                          {X_BUTTON, {"!", XK_exclam}}, {A_BUTTON, {"+", XK_plus}}, {B_BUTTON, {"-", XK_minus}}};
  //   layerButtonMaps[9]  = {{Y_BUTTON, {">", XK_greater}}, {X_BUTTON, {"<", XK_less}}, {A_BUTTON, {"(", XK_parenleft}}, {B_BUTTON, {")", XK_parenright}}};
  //   layerButtonMaps[10] = {{Y_BUTTON, {"&", XK_ampersand}},
  //                          {X_BUTTON, {"\"", XK_quotedbl}},
  //                          {A_BUTTON, {"{", XK_braceleft}},
  //                          {B_BUTTON, {"}", XK_braceright}}};
  //   layerButtonMaps[11] = {{Y_BUTTON, {"#", XK_numbersign}},
  //                          {X_BUTTON, {"|", XK_bar}},
  //                          {A_BUTTON, {"@", XK_at}},
  //                          {B_BUTTON, {"'", XK_quoteleft}}};
  //   layerButtonMaps[12] = {{Y_BUTTON, {"ß", XK_ssharp}},
  //                          {X_BUTTON, {"ü", XK_udiaeresis}},
  //                          {A_BUTTON, {"ä", XK_adiaeresis}},
  //                          {B_BUTTON, {"ö", XK_odiaeresis}}};
  //   layerButtonMaps[13] = {{Y_BUTTON, {"3", XK_3}}, {X_BUTTON, {"4", XK_4}}, {A_BUTTON, {"1", XK_1}}, {B_BUTTON, {"2", XK_2}}, {RB_BUTTON, {"5", XK_5}}};
  //   layerButtonMaps[14] = {{Y_BUTTON, {"8", XK_8}}, {X_BUTTON, {"9", XK_9}}, {A_BUTTON, {"6", XK_6}}, {B_BUTTON, {"7", XK_7}}, {RB_BUTTON, {"0", XK_0}}};

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

  unsigned char buttonStates[20]={0};
  int axisValues[20];
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
          //simulateKeyPress(display, layerButtonMaps[6].at(event.number));
        }
      } else if (isAnalogueTopRight(axisValues)) {
        if (layerButtonMaps[7].count(event.number) != 0) {
          //simulateKeyPress(display, layerButtonMaps[7].at(event.number));
        }
      } else if (isAnalogueBottomLeft(axisValues)) {
      } else if (isAnalogueBottomRight(axisValues)) {
      } else if (isAnalogueTop(axisValues)  && event.value == 1) {
        if (layerButtonMaps[3].count(event.number) != 0) {
          // simulateKeyPress(display, layerButtonMaps[3].at(event.number));
        }
      } else if (isAnalogueBottom(axisValues)  && event.value == 1) {
        if (layerButtonMaps[5].count(event.number) != 0) {
          //simulateKeyPress(display, layerButtonMaps[5].at(event.number));
        }
      } else if (isAnalogueLeft(axisValues) && event.value == 1) {
        switch (event.number) {
        case RB_BUTTON:
          //simulateKeyPress(display, "BackSpace");
          //simulateKeyPress(display, "BackSpace");
          //simulateKeyPress(display, "BackSpace");
          break;
        default:
          if (layerButtonMaps[2].count(event.number) != 0) {
            //simulateKeyPress(display, layerButtonMaps[2].at(event.number));
          }
          break;
        }
      } else if (isAnalogueRight(axisValues) && event.value == 1) {
        if (layerButtonMaps[4].count(event.number) != 0) {
          //simulateKeyPress(display, layerButtonMaps[4].at(event.number));
        }
      } else if (isPadTopLeft(axisValues) && event.value == 1) {
        if (layerButtonMaps[14].count(event.number) != 0) {
          //simulateKeyPress(display, layerButtonMaps[14].at(event.number));
        }
      } else if (isPadTopRight(axisValues) && event.value == 1) {
      } else if (isPadBottomRight(axisValues) && event.value == 1) {
      } else if (isPadBottomLeft(axisValues) && event.value == 1) {
      } else if (isPadTop(axisValues) && event.value == 1) {
        if (layerButtonMaps[10].count(event.number) != 0) {
          //simulateKeyPress(display, layerButtonMaps[10].at(event.number));
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
          //simulateKeyPress(display, layerButtonMaps[13].at(event.number));
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

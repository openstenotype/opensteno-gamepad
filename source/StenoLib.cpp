#include <string>
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
const char* UNICODE_SPACE = "U0020";

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


string help = "Layer 1\t\tLayer 2 ⬅️\t\tLayer 3 ⬆️\t\tLayer 4 ➡️\t\tLayer 5 ⬇️\t\tLayer 6 ↖️\n";

const char* getHelp(std::map<int, std::map<int, const char*>> layerButtonMaps){
  // Y Button
  help += "\t";
  for (int i = 1; i <= 6; i++) {
    help += layerButtonMaps[i].at(Y_BUTTON);
    help += "\t\t\t";
  }
  help += "\n";

  // X & B Button
  help += "\n";
  for (int i = 1; i <= 6; i++) {
    help += layerButtonMaps[i].at(X_BUTTON);
    help += "\t\t";
    help += layerButtonMaps[i].at(B_BUTTON);
    help += "\t";
  }
  help += "\n\n";

  // A Button
  help += "\t";
  for (int i = 1; i <= 6; i++) {
    help += layerButtonMaps[i].at(A_BUTTON);
    help += "\t\t\t";
  }
  help += "\n";
  return help.c_str();
}


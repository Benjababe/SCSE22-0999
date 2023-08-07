#ifndef H_MOTOR
#define H_MOTOR

#include <Arduino.h>

namespace ESP8266Hanger {
enum class MotorState { retracted,
                        extended };

class MotorControl {
private:
  MotorState state;
  int INA;
  int INB;

public:
  MotorControl();

  void initPins(const int, const int);

  MotorState getState();

  bool extendMotor();

  bool retractMotor();

  void stop();
};
}

#endif
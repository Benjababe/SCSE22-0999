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

  /** Retrieves current state of the hanger, retracted/extended */
  MotorState getState();

  /** Extends the clothes hanger if currently retracted. Returns whether motor made an action */
  bool extendMotor();

  /** Retracts the clothes hanger if currently extended. Returns whether motor made an action */
  bool retractMotor();

  /** Clears output pins to the motor to prevent overheating */
  void stop();
};
}

#endif
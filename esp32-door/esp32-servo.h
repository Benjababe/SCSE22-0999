#ifndef H_SERVO
#define H_SERVO

#include <Servo.h>

namespace ESP32Door {

class ServoControl {

private:
  Servo servo;

public:
  int LOCK_ANGLE;
  int UNLOCK_ANGLE;
  int SERVO_DATA_PIN;

  ServoControl();

  /** @brief Moves servo into locked position */
  void lock();

  /** @brief Moves servo into unlocked position */
  void unlock();

  /** @brief Set servo to a specific angle */
  void setAngle(int);

  /** @brief Reads the current angle of the servo */
  int getAngle();

  /** @brief Returns flag whether servo is currently locked */
  bool isLocked();
};
}

#endif
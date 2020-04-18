#include <Arduino.h>
#include "SoftwareSerial.h"
#include "ParallelMD-host.hpp"

#define chassisFront 1
#define chassisRight 2
#define chassisLeft 3

ParallelMD_Host MD(A1, 9600);

void setup()
{
  // put your setup code here, to run once:
  MD.activate(chassisFront);
  MD.activate(chassisRight);
  MD.activate(chassisLeft);
  MD.setPWM(chassisFront, 74);
  MD.setPWM(chassisRight, 170);
  MD.setPWM(chassisLeft, 180);
}

void loop()
{
  // put your main code here, to run repeatedly:
  static uint8_t pwm;
  MD.transmit();
  MD.setPWM(chassisRight, 170); //同じpwm値がセットされたとき送信をキャンセルするか検証
  MD.setPWM(chassisLeft, 180);
  MD.setPWM(chassisFront, pwm++); //pwm可変
  delayMicroseconds(2);
}
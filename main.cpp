#include <Arduino.h>
#include "SoftwareSerial.h"

SoftwareSerial MD_Slave_Bus(A0, A1);

#define robotArmRight 5
#define robotArmLeft 13
#define chassisFront 1
#define chassisRight 2
#define chassisLeft 3

typedef struct
{
  bool activated, isNecessaryToSend;
  int16_t latestPWM, previousPWM;
} slaveMD_Data;

slaveMD_Data MD[20];

void initialize();
void activate(const uint8_t);
void setPWM(const uint8_t, const int16_t);
void transmit();
void sendPacket(const int);

void setup()
{
  // put your setup code here, to run once:
  initialize();
  //activate(robotArmRight);
  //activate(robotArmLeft);
  activate(chassisFront);
  activate(chassisRight);
  activate(chassisLeft);
  //setPWM(robotArmLeft, -150);
  //setPWM(robotArmRight, 50);
  setPWM(chassisFront, 74);
  setPWM(chassisRight, 170);
  setPWM(chassisLeft, 180);
}

void loop()
{
  // put your main code here, to run repeatedly:
  transmit();
  delayMicroseconds(2);
  static uint8_t pwm;
  setPWM(chassisRight, 170);

  setPWM(chassisFront, pwm++);
}

void initialize() //コンストラクタに実装
{
  MD_Slave_Bus.begin(9600);
  for (int i = 0; i < 20; i++)
  {
    MD[i].activated = false;
    MD[i].isNecessaryToSend = false;
    MD[i].latestPWM = 0;
    MD[i].previousPWM = 0;
  }
}

void activate(const uint8_t recNum)
{
  MD[recNum].activated = true;
  MD[recNum].isNecessaryToSend = true;
}

void setPWM(const uint8_t recNum, const int16_t pwm)
{
  MD[recNum].latestPWM = pwm;
  if (MD[recNum].latestPWM == MD[recNum].previousPWM)
  {
    MD[recNum].isNecessaryToSend = false;
    return;
  }
  MD[recNum].isNecessaryToSend = true;
  MD[recNum].previousPWM = MD[recNum].latestPWM;
}

void transmit()
{
  for (int recNum = 0; recNum < 20; recNum++)
  {
    if (MD[recNum].activated && MD[recNum].isNecessaryToSend)
      sendPacket(recNum);
  }
}

void sendPacket(const int recNum)
{
  uint8_t dataPacket[1 + 2 + 1]; //recNum + CW,CCW DATA(PWM) + CHECKSUM
  dataPacket[0] = recNum;
  dataPacket[1] = MD[recNum].latestPWM > 0 ? MD[recNum].latestPWM : 0;
  dataPacket[2] = MD[recNum].latestPWM > 0 ? 0 : -MD[recNum].latestPWM;
  dataPacket[3] = dataPacket[0] ^ dataPacket[1] ^ dataPacket[2];
  MD_Slave_Bus.listen();
  for (int dataIndex = 0; dataIndex < 4; dataIndex++)
    MD_Slave_Bus.write(dataPacket[dataIndex]);

  Serial.begin(256000);
  Serial.print(dataPacket[0]);
  Serial.print('\t');
  Serial.print(dataPacket[1]);
  Serial.print('\t');
  Serial.print(dataPacket[2]);
  Serial.print('\t');
  Serial.print(dataPacket[3]);
  Serial.println("");
}

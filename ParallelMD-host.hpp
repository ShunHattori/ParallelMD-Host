#pragma once
#include <Arduino.h>
#include "SoftwareSerial.h"

class ParallelMD_Host
{
private:
    static const uint8_t maximumNumOfMD = 20;
    SoftwareSerial *MD_Slave_Bus;
    typedef struct
    {
        bool activated, isNecessaryToSend;
        int16_t latestPWM, previousPWM;
    } slaveMD_Data;
    slaveMD_Data MD[maximumNumOfMD];

    void sendPacket(const int);

public:
    ParallelMD_Host(const uint8_t, const uint32_t);
    ~ParallelMD_Host();

    void activate(const uint8_t);
    void setPWM(const uint8_t, const int16_t);
    void transmit();
};

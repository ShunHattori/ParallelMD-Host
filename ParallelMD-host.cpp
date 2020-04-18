#include "ParallelMD-host.hpp"

ParallelMD_Host::ParallelMD_Host(const uint8_t TX, const uint32_t baud)
{
    MD_Slave_Bus = new SoftwareSerial(0, TX);
    MD_Slave_Bus->begin(baud);
    for (int i = 0; i < maximumNumOfMD; i++)
    {
        MD[i].activated = false;
        MD[i].isNecessaryToSend = false;
        MD[i].latestPWM = 0;
        MD[i].previousPWM = 0;
    }
}

ParallelMD_Host::~ParallelMD_Host()
{
    delete[] MD_Slave_Bus;
}

void ParallelMD_Host::activate(const uint8_t recNum)
{
    MD[recNum].activated = true;
    MD[recNum].isNecessaryToSend = true;
}

void ParallelMD_Host::setPWM(const uint8_t recNum, const int16_t pwm)
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

void ParallelMD_Host::transmit()
{
    for (int recNum = 0; recNum < maximumNumOfMD; recNum++)
    {
        if (MD[recNum].activated && MD[recNum].isNecessaryToSend)
            sendPacket(recNum);
    }
}

void ParallelMD_Host::sendPacket(const int recNum)
{
    uint8_t dataPacket[1 + 2 + 1 + 1]; //recNum + CW,CCW DATA(PWM) + CHECKSUM(1) + CHECKSUM(2)
    dataPacket[0] = recNum;
    dataPacket[1] = MD[recNum].latestPWM > 0 ? MD[recNum].latestPWM : 0;
    dataPacket[2] = MD[recNum].latestPWM > 0 ? 0 : -MD[recNum].latestPWM;
    dataPacket[3] = dataPacket[0] ^ dataPacket[1] ^ dataPacket[2];
    dataPacket[4] = dataPacket[0] + dataPacket[1] + dataPacket[2];
    MD_Slave_Bus->listen();
    for (int dataIndex = 0; dataIndex < 5; dataIndex++)
        MD_Slave_Bus->write(dataPacket[dataIndex]);

    // Serial.begin(256000);
    // Serial.print(dataPacket[0]);
    // Serial.print('\t');
    // Serial.print(dataPacket[1]);
    // Serial.print('\t');
    // Serial.print(dataPacket[2]);
    // Serial.print('\t');
    // Serial.print(dataPacket[3]);
    // Serial.print('\t');
    // Serial.print(dataPacket[4]);
    // Serial.println("");
}

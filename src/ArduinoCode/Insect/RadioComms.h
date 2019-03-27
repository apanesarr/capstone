#ifndef RadioComms_h
#define RadioComms_h

#include "RF24.h"
#include "RF24Network.h"
#include "CarMotorCtrl.h"

#define MOTOR_TYPE 'M'
#define MOTOR_READY 'R'
#define INIT 'I'
#define TEMP_TYPE 'T'
#define FAILURE 'F'

struct payloadMsg {
    unsigned msg_id;
    char data [32];
};

struct SensorPayload {
  unsigned int msg_id;
  float temp;
};

struct InitPayload {
  unsigned int msg_id;
};

class RadioComms {
    public:
        RadioComms() : radio(7,8), network(radio) {};
        void init(unsigned int nodeID);
        void update();
        motorSettings_t newSettings;
    private:
        RF24 radio;
        RF24Network network;
        void handle_temp(RF24NetworkHeader *header, payloadMsg *payload);
        void handle_set_gyro (RF24NetworkHeader *header, payloadMsg *payload);
        void handle_get_gyro (RF24NetworkHeader *header, payloadMsg *payload);
        void handle_init(RF24NetworkHeader *header, payloadMsg *payload);
        void handle_stop_motor(RF24NetworkHeader *header, payloadMsg *payload);
        void handle_move_motor(RF24NetworkHeader *header, payloadMsg *payload);
        void safeSend(char type, payloadMsg *payload, int tryAgain);
        bool sendToMaster (char type, payloadMsg *payload);
};

#endif

#ifndef RadioComms_h
#define RadioComms_h

#include "RF24.h"
#include "RF24Network.h"

#define SET_GYRO_TYPE 'H'
#define MOTOR_TYPE 'M'
#define GET_GYRO_TYPE 'G'
#define STOP_TYPE 'S'
#define INIT 'I'
#define TEMP_TYPE 'T'

struct payloadMsg {
    unsigned msg_id;
    char data [32];
};

class RadioComms {
    public:
        RadioComms() : radio(7,8), network(radio) {};
        void init();
        void update();
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

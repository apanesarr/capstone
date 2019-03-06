#ifndef RadioComms_h
#define RadioComms_h

#include "CommsToolbox.h"
#include "RF24.h"
#include "RF24Network.h"

class Antenna {
    public:
        Antenna() : radio(7,8), network(radio) {};
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

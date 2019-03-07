#include <CarMotorCtrl.h>
#include <CommsToolbox.h>
#include "RF24.h"
#include "RF24Network.h"

#define nodeID 1
#define retry 10

MotorControl motor;

RF24 radio(7,8);
RF24Network network(radio);

bool init_conn = false;

void setup() {
  Serial.begin(115200);
  radio.begin();
  network.begin(100, nodeID);
  Serial.println("Starting insect...");
}

void loop() {
  network.update();
  payloadMsg payload;
  //safeSend('I', &payload, 10);
  while(network.available()){
    RF24NetworkHeader header;
    payloadMsg payload;
    network.read(header,&payload, sizeof(payloadMsg));
    switch (header.type){
      case INIT:
        handle_init(&header, &payload);
        break;
      case TEMP_TYPE:
        handle_temp(&header, &payload);
        break;
      case GET_GYRO_TYPE:
        handle_get_gyro(&header, &payload);
        break;
      case SET_GYRO_TYPE:
        handle_set_gyro(&header, &payload);
        break;
      case STOP_TYPE:
        handle_stop_motor(&header, &payload);
        break;
      case MOTOR_TYPE:
        handle_move_motor(&header, &payload);
        break;
      default:
        Serial.println("UNKNOWN MESSAGE TYPE");
        break;
    }
  }
}

void handle_temp(RF24NetworkHeader *header, payloadMsg *payload){
  Serial.println("Handling temp_type");
  float temp = 20.00;
  dtostrf(temp,12,20,(payload->data));
  safeSend(GET_GYRO_TYPE, payload, retry);
}

void handle_set_gyro (RF24NetworkHeader *header, payloadMsg *payload) {
  float degree = atof(payload->data);
  Serial.println(degree);
  
}

void handle_get_gyro (RF24NetworkHeader *header, payloadMsg *payload) {
  Serial.println("Handling get gyro");
  float degree = 90.0012312312313;
  dtostrf(degree,12,20,(payload->data));
  safeSend(SET_GYRO_TYPE,payload, retry);
}

void handle_init(RF24NetworkHeader *header, payloadMsg *payload){
  Serial.println("Handling init_type");
//  payload->data = "INIT";
  safeSend(INIT, payload, retry);
}

void handle_stop_motor(RF24NetworkHeader *header, payloadMsg *payload){
  Serial.println("Handling stop_type");
  motor.setMotor(STOP);
  safeSend(STOP_TYPE, payload, retry);
}

void handle_move_motor(RF24NetworkHeader *header, payloadMsg *payload){
  Serial.println("Handling move_type");
  motor.setMotor(FORWARD);
  safeSend(MOTOR_TYPE, payload, retry);
}

void safeSend(char type, payloadMsg *payload, int tryAgain ){
  while ( !sendToMaster(type,payload)){
    Serial.println("Failed to send");
    delay(100);
  }
}

bool sendToMaster (char type, payloadMsg *payload) {
  RF24NetworkHeader header(0,type);
  return network.write(header, payload, sizeof(*payload));
}

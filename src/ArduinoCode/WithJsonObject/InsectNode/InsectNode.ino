#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include <EEPROM.h>
#include "ArduinoJson.h"

#define NODE_ID 1

RF24 radio(7, 8); // CE , CS Pins 
RF24Network network(radio);
RF24Mesh mesh(radio, network);

StaticJsonBuffer<200> jsonBuffer;

float temp = 0;
float longitude = 0;
float latitude = 0;

JsonObject& createData(){
    JsonObject& data = jsonBuffer.createObject();
    data["nodeId"] = NODE_ID;
    data["longitude"] = longitude;
    data["latitude"] = latitude;
    return data;
}

void sendToMaster(JsonObject& data){
    mesh.update();
    if (!mesh.write(&data, 'M', sizeof(data))) {
        if ( ! mesh.checkConnection() ) {
            Serial.println("Renewing Address");
            mesh.renewAddress();
        } else {
            Serial.println("Send fail, Test OK");
        }
    } else {
      Serial.print("Send OK: ");
      data.printTo(Serial);
    }
}

void recieveFromMaster(){
 while (network.available()) {
    RF24NetworkHeader header;
    char data[] = "{ }";
    network.read(header, &data, sizeof(data));
    JsonObject& dataJson = jsonBuffer.parseObject(data);
    dataJson.printTo(Serial);
  }
}

void setup() {
  Serial.begin(115200);
  mesh.setNodeID(NODE_ID);
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();
}

void loop() {
    sendToMaster(createData());
    recieveFromMaster();
}

#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include "ArduinoJson.h"
#include <SPI.h>
#include <EEPROM.h>


#define NODE_ID 0

RF24 radio(7, 8); // CE , CS Pins 
RF24Network network(radio);
RF24Mesh mesh(radio, network);

StaticJsonBuffer<200> jsonBuffer;

float nextLongitude = 0 ;
float nextLatitude = 0;

JsonObject& createData( float longitude , float latitude){
    JsonObject& data = jsonBuffer.createObject();
    data["longitude"] = longitude;
    data["latitude"] = latitude;
    return data;
}

void sendToNode(int nodeId, JsonObject& payload){
    for (int i = 0; i < mesh.addrListTop; i++) {
        if (mesh.addrList[i].nodeID == nodeId) {  //Searching for node one from address list
            RF24NetworkHeader header(mesh.addrList[i].address, OCT); //Constructing a header
            Serial.println((network.write(header, &payload, sizeof(payload)) == 1 ? "Send OK" : "Send Fail")); //Sending an message
        }
    }
}

void recieveFromNodes(){
    if(network.available()){
        RF24NetworkHeader header;
        network.peek(header);
        Serial.println("Got : ");
        char recieved[] ="{}";
        switch(header.type){
            case 'M':
                network.read(header, &recieved,sizeof(recieved));
                JsonObject& recievedJson = jsonBuffer.parseObject(recieved);
                recievedJson.printTo(Serial);
            default:
                network.read(header,0,0);
                Serial.println(header.type);
                break;
        }
    }   
}

void setup() {
  Serial.begin(115200);
  mesh.setNodeID(NODE_ID);
  Serial.println(mesh.getNodeID());
  mesh.begin();
}

void loop() { 
    mesh.update();
    sendToNode(1,createData(nextLongitude,nextLatitude));
    recieveFromNodes();
    mesh.DHCP();
}

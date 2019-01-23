
#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include <EEPROM.h>
//#include <printf.h>


/**** Configure the nrf24l01 CE and CS pins ****/
RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

/**** Set Node ID 1-255 ****/
#define nodeID 3

uint32_t returnTemp = 26;
uint32_t returnX = 0;
uint32_t returnY = 0;
uint32_t displayTimer = 0;

struct payload_r {
  unsigned long longitude;
  unsigned long latitude;
};

struct payload_t {
  unsigned int InsectID;
  unsigned long longitude;
  unsigned long latitude;
  unsigned long temperature;
};

void setup() {

  Serial.begin(115200);
  // Set the nodeID manually
  mesh.setNodeID(nodeID);
  // Connect to the mesh
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();
}

void loop() {

  mesh.update();
  // Send to the master node every second
  if (millis() - displayTimer >= 1000) {
    displayTimer = millis();
    payload_t transmit = {nodeID, returnX, returnY, returnTemp};
    // Send an 'M' type message containing the current millis()
    if (!mesh.write(&transmit, 'M', sizeof(transmit))) {
      // If a write fails, check connectivity to the mesh network
      if ( ! mesh.checkConnection() ) {
        //refresh the network address
        Serial.println("Renewing Address");
        mesh.renewAddress();
      } else {
        Serial.println("Send fail, Test OK");
      }
    } else {
      Serial.print("Send OK: "); Serial.print(nodeID); Serial.print("|");Serial.print(transmit.longitude);Serial.print("|");Serial.print(transmit.latitude);Serial.print("|");Serial.println(transmit.temperature);
    }
  }

  while (network.available()) {
    RF24NetworkHeader header;
    payload_r payload;
    network.read(header, &payload, sizeof(payload));
    Serial.print("Next Location  x = ");
    Serial.print(payload.longitude);
    Serial.print(" y  =  ");
    Serial.println(payload.latitude);
    returnX = payload.longitude;
    returnY = payload.latitude;
  }
}

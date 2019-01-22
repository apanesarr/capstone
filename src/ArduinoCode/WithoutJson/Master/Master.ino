#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>
 
/***** Configure the chosen CE,CS pins *****/
RF24 radio(7,8);
RF24Network network(radio);
RF24Mesh mesh(radio,network);

uint32_t passX = 0;
uint32_t passY = 0;
uint32_t displayTimer = 0;
struct payload_r {
  unsigned long longitude;
  unsigned long latitude;
};

struct payload_t {
  unsigned int nodeID;
  unsigned long longitude;
  unsigned long latitude;
  unsigned long temperature;
};

void setup() {
  Serial.begin(115200);
  // Set the nodeID to 0 for the master node
  mesh.setNodeID(0);
  Serial.println(mesh.getNodeID());
  // Connect to the mesh
  mesh.begin();
 
}
 
 
void loop() {    
 
  // Call mesh.update to keep the network updated
  mesh.update();
 
  // In addition, keep the 'DHCP service' running on the master node so addresses will
  // be assigned to the sensor nodes
  mesh.DHCP();
 
 
  // Check for incoming data from the sensors
  if(network.available()){
    RF24NetworkHeader header;
    network.peek(header);
    Serial.print("Got ");
    payload_t recieved = {0,0,0,0};
    switch(header.type){
      // Display the incoming millis() values from the sensor nodes
    case 'M': 
      network.read(header, &recieved,sizeof(recieved));
      Serial.print(recieved.nodeID);
      Serial.print(" | ");
      Serial.print(recieved.longitude);
      Serial.print(" | ");
      Serial.print(recieved.latitude);
      Serial.print(" | ");
      Serial.print(recieved.temperature);
      Serial.print(" | ");
      Serial.print(" from RF24Network address 0");
      Serial.println(header.from_node, OCT);
      break;
    default: 
      network.read(header,0,0); 
      Serial.println(header.type);
      break;
    }
  }
 
 
  // Send each node a message every five seconds
  // Send a different message to node 1, containing another counter instead of millis()
  if(millis() - displayTimer > 5000){
    passX++;
    passY++;
    for (int i = 0; i < mesh.addrListTop; i++) {
      payload_r payload = {passX,passY};
      if (mesh.addrList[i].nodeID == 1) {  //Searching for node one from address list
        payload = {passX,passY/2};
      }
      RF24NetworkHeader header(mesh.addrList[i].address, OCT); //Constructing a header
      Serial.println((network.write(header, &payload, sizeof(payload)) == 1 ? "Send OK" : "Send Fail")); //Sending an message
      
    }
    displayTimer = millis();
  }
}

#include <SPI.h>  
#include "RF24.h"

RF24 myRadio (7, 8);
byte addresses[][6] = {"0"};
int lossCount;
int id_old = 0;
float timeVar = 0;
float delayMax = 20000;
int errorVar = 0;
int resetVar = 0;
int startVar = 0;

struct package {
  int id = 1;
  int distance;
  float x;
  float y;
  float z;
  float current;
  float voltage;
};


typedef struct package Package;
Package dataReceive;
Package dataTransmit;

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  myRadio.begin();  
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.setDataRate( RF24_250KBPS );
  
  myRadio.openReadingPipe(1, addresses[0]);
  myRadio.startListening();

  while (startVar == 0) {
  Serial.println("Waiting for the other transceiver...");
  if ( myRadio.available()) {
    while (myRadio.available()){
      myRadio.read( &dataReceive, sizeof(dataReceive) );
    startVar = 1;
    }
  }
    delay(200);
  myRadio.stopListening();
  myRadio.openWritingPipe(addresses[0]);
  myRadio.write(&dataTransmit, sizeof(dataTransmit));
  myRadio.openReadingPipe(1, addresses[0]);
  myRadio.startListening();
  }
  
}



void loop() {
  if ( myRadio.available()) {
    while (myRadio.available()){
      myRadio.read( &dataReceive, sizeof(dataReceive) );
    }
    Serial.println("Recieve: ");
    Serial.print("Package:");
    Serial.print(dataReceive.id);
    Serial.print("\n");
    Serial.print("Distance: ");
    Serial.print(dataReceive.distance);
    Serial.println("cm");
    Serial.print("\n");
    Serial.print("Voltage: ");
    Serial.print(dataReceive.voltage);
    Serial.println("mV");
    Serial.print("\n");
    Serial.print("Current: ");
    Serial.print(dataReceive.current);
    Serial.println("mA");
    Serial.print("\n");
    Serial.print("(x,y,z) position : (");
    Serial.print(dataReceive.x);
    Serial.print(",");
    Serial.print(dataReceive.y);
    Serial.print(",");
    Serial.print(dataReceive.z);
    Serial.println(")");
  }
  if (id_old < dataReceive.id) {
    timeVar = millis();
    resetVar = resetVar+1;
    }
  if (resetVar == 1){
    errorVar = 0;
    }

  if(millis() - timeVar>delayMax){
    errorVar = 1;
    }
  
  if (errorVar == 0) {
  delay(200);
  myRadio.stopListening();
  dataTransmit.id = dataTransmit.id + 1;
  Serial.println("Transmit: ");
  Serial.print("Package:");
  Serial.print(dataTransmit.id);
  Serial.print("\n");
  myRadio.openWritingPipe(addresses[0]);
  myRadio.write(&dataTransmit, sizeof(dataTransmit));
  myRadio.openReadingPipe(1, addresses[0]);
  myRadio.startListening();}
  else{
    Serial.println("Error: Too much loss of data");
    }
  Serial.print("Error variable: ");
  Serial.println(errorVar);
  Serial.print("Time delay: ");
  Serial.print(millis() - timeVar);
  Serial.println("ms");
  id_old = dataReceive.id;
 Serial.print("----------------");
  Serial.print("\n");
}

//Packages:
    #include <Adafruit_INA260.h>
    #include <Wire.h>
    #include <Adafruit_Sensor.h>
    #include <Adafruit_BNO055.h>
    #include <utility/imumaths.h>
  //Transceiver packages:
    #include <SPI.h>  
    #include "RF24.h"
  //ESC package:
  #include <Servo.h>
///////////////////////////////
  
Adafruit_BNO055 bno = Adafruit_BNO055(55);

#define echoPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 3 //attach pin D3 Arduino to pin Trig of HC-SR04
RF24 myRadio (7, 8); //Digital pins which are used for the transceiver data

// defines variables - Ultrasonic sensor test
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement

// Settings for transceivers
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

//ESC settings:
Servo ESC;

int potValue;

///////////////////////


Adafruit_INA260 ina260 = Adafruit_INA260();

void setup() {

  ESC.attach(9,1000,2000);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  // Wait until serial port is opened
  while (!Serial) { delay(10); }

  Serial.println("Adafruit INA260 + Ultrasonic Sensor HC-SR04 Test");

  if (!ina260.begin()) {
    Serial.println("Couldn't find INA260 chip");
    while (1);
  }
  Serial.println("Found INA260 chip");

  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  Serial.println("Found BNO055 chip");

  bno.setExtCrystalUse(true);
}

void loop() {
  wirelessWait();
  delay(50);
  getDistance();
  delay(50);
  getVoltage();
  delay(50);
  getIMU();
  delay(50);
  wirelessActive();
  delay(50);
  ESC_function();
  delay(200);
}

void getIMU() {
  /* Get a new sensor event */ 
  sensors_event_t event; 
  bno.getEvent(&event);
  
  /* Display the floating point data */
  Serial.print("X: ");
  Serial.print(event.orientation.x, 4);
  Serial.print("\tY: ");
  Serial.print(event.orientation.y, 4);
  Serial.print("\tZ: ");
  Serial.print(event.orientation.z, 4);
  Serial.println("");
}


void getDistance() {
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

void getVoltage() {
  Serial.print("Current: ");
  Serial.print(ina260.readCurrent());
  Serial.println(" mA");

  Serial.print("Bus Voltage: ");
  Serial.print(ina260.readBusVoltage());
  Serial.println(" mV");

  Serial.print("Power: ");
  Serial.print(ina260.readPower());
  Serial.println(" mW");
  
  Serial.println();
}

void wirelessWait() {
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

  /*Update the sensor values to transmit:*/
    /*Get distance:*/
     // Clears the trigPin condition
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      // Reads the echoPin, returns the sound wave travel time in microseconds
      duration = pulseIn(echoPin, HIGH);
      // Calculating the distance
      dataTransmit.distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    /* Get a new sensor event for the orientation: */ 
      sensors_event_t event; 
      bno.getEvent(&event);
      dataTransmit.x = event.orientation.x;
      dataTransmit.y = event.orientation.y;
      dataTransmit.z = event.orientation.z;
    /*Get the voltage and current:*/
      dataTransmit.current = ina260.readCurrent();
      dataTransmit.voltage = ina260.readBusVoltage();
    /*Send info about the sensors to the stationary unit:*/
    getIMU();
    getDistance();
    getVoltage();
    ESC_function();
  }
  Serial.println();
  Serial.println("Connection received!");
  Serial.println();
}

void wirelessActive(){
    if ( myRadio.available()) {
    while (myRadio.available()){
      myRadio.read( &dataReceive, sizeof(dataReceive) );
    }
    Serial.println("Recieve: ");
    Serial.print("Package:");
    Serial.print(dataReceive.id);
    Serial.print("\n");
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
    Serial.print("Package: ");
    Serial.print(dataTransmit.id);
    Serial.print("\n");
    Serial.print("Distance: ");
    Serial.print(dataTransmit.distance);
    Serial.println("cm");
    Serial.print("\n");
    Serial.print("Voltage: ");
    Serial.print(dataTransmit.voltage);
    Serial.println("mV");
    Serial.print("\n");
    Serial.print("Current: ");
    Serial.print(dataTransmit.current);
    Serial.println("mA");
    Serial.print("\n");
    Serial.print("(x,y,z) position: (");
    Serial.print(dataTransmit.x);
    Serial.print(",");
    Serial.print(dataTransmit.y);
    Serial.print(",");
    Serial.print(dataTransmit.z);
    Serial.println(")");

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
    Serial.println(" ms");
    id_old = dataReceive.id;
   Serial.print("----------------");
    Serial.print("\n");
  }
  
  void ESC_function() {
    potValue = analogRead(A0);
    potValue = map(potValue, 0, 1023, 0, 180);
    ESC.write(potValue);
    }
  

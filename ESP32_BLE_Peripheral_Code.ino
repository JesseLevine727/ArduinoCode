
 
/*
 * Bluetooth Low Energy (BLE) is a low-energy version of the normal version of Bluetooth everyone is familiar with.
 * BLE remains "asleep" until a connection is on, and is perfect for short distance transmission of small amounts of data
 * 
 * This program deals with the setup of a BLE server/peripheral;that is the device that contains the data being sent to a connecting device
 * 
 *The peripheral device is seperated into hierachies: at the top are the services. Under the services are the characteristics
 *Within the characteristics, is the actual data and descriptors. The services are then advertised.
 *
 * The setup is as follows:
 * (1) BLE Server setup->(2) Creation of BLE Service->(3) Creation of characteristic(s)-> (4)Create BLE descriptor(s) for the characteristic(s)
 * (5)Starting the server->(6) begin advertising to nearby devices
 */

//Libraries used
#include "Wire.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_BNO055.h"
#include "utility/imumaths.h"
#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEServer.h"
#include "BLE2902.h"

//set up server
bool deviceConnected = false;

class pServerCallbacks: public BLEServerCallbacks{
  void onConnect(BLEServer* pServer){
    deviceConnected = true;
  }
  void onDisconnect(BLEServer* pServer){
    deviceConnected = false;
  }
  
};

//defining services
#define SERVICE1_UUID           "8be5a5e3-82bd-4cb8-89c9-f3022fce303a"
#define SERVICE2_UUID           "2f95d50f-c124-45f2-825b-6c5ed1a35463"

//defining characteristics
BLECharacteristic *UltronCharacteristic;
BLECharacteristic *TouchPadCharacteristic;
BLECharacteristic *xCharacteristic;
BLECharacteristic *yCharacteristic;
BLECharacteristic *zCharacteristic;

#define UltronChar_UUID "8a90c654-f336-4957-8c01-223f1e4dd517"
#define TouchPadChar_UUID "ec62e45e-6835-4857-99ae-dc967774b166"
#define xChar_UUID "9dabffb9-5aa8-429d-ac32-1c872d45a7d6"
#define yChar_UUID "7c0ef830-a49d-47c3-b31c-bc2d5150dfbe"
#define zChar_UUID "00660493-2cb1-4a16-8e86-86cc3d6a1c9d"


float txValue1 = 0.00;
float txValue2 = 0.00;
float txValue3 = 0.00;
float txValue4 = 0.00;

//I2C devide address for IMU is by default 0x28
Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x28);

//pin locations for each sensor
const byte DistTrigger = 13;
const byte DistEcho = 12;
const byte LED = 14;
const byte touchPad = 27;
const byte Knx_LED = 32;

//Full BLE set up function, to be placed in the void setup.
void BLE_setup(){
  if(!bno.begin())
  {
    Serial.print("IMU not connected");
  }
  
  BLEDevice::init("General Kenobi");

  BLEServer *pServer = BLEDevice::createServer();
  
  pServer->setCallbacks(new pServerCallbacks());
  
  
  
  BLEService *pService = pServer->createService(SERVICE1_UUID);
  BLEService *IMUService = pServer->createService(SERVICE2_UUID);

  UltronCharacteristic = pService->createCharacteristic(
                       UltronChar_UUID,
                       BLECharacteristic::PROPERTY_NOTIFY|
                       BLECharacteristic::PROPERTY_WRITE |
                       BLECharacteristic::PROPERTY_READ
                    
                    );

  TouchPadCharacteristic = pService->createCharacteristic(
                       TouchPadChar_UUID,
                       BLECharacteristic::PROPERTY_NOTIFY|
                       BLECharacteristic::PROPERTY_WRITE |
                       BLECharacteristic::PROPERTY_READ
                    );
  xCharacteristic = IMUService->createCharacteristic(
                       xChar_UUID,
                       BLECharacteristic::PROPERTY_NOTIFY|
                       BLECharacteristic::PROPERTY_WRITE |
                       BLECharacteristic::PROPERTY_READ
                    );
  yCharacteristic = IMUService->createCharacteristic(
                       yChar_UUID,
                       BLECharacteristic::PROPERTY_NOTIFY|
                       BLECharacteristic::PROPERTY_WRITE |
                       BLECharacteristic::PROPERTY_READ
                    );
  zCharacteristic = IMUService->createCharacteristic(
                       zChar_UUID,
                       BLECharacteristic::PROPERTY_NOTIFY|
                       BLECharacteristic::PROPERTY_WRITE |
                       BLECharacteristic::PROPERTY_READ
                    );                
  
  UltronCharacteristic->addDescriptor(new BLE2902());
  TouchPadCharacteristic->addDescriptor(new BLE2902());
  xCharacteristic->addDescriptor(new BLE2902());
  yCharacteristic->addDescriptor(new BLE2902());
  zCharacteristic->addDescriptor(new BLE2902());  
  
  pService->start();
  IMUService->start();
  
  pServer->getAdvertising()->start();
  }


//Function that sets the value of the data. Placed in loop function to constantly update its value
void BLE_Data(){
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
   
  if (deviceConnected){
       
    txValue1 = readDistanceSensor();
    char txString_one[1];
    dtostrf(txValue1, 1, 2, txString_one);
    UltronCharacteristic->setValue(txString_one);
    UltronCharacteristic->notify();
    
    Serial.println("Sent val: " + String(txString_one));
    delay(500);

    if(digitalRead(touchPad)==HIGH){
      digitalWrite(LED,HIGH);
      TouchPadCharacteristic->setValue("1");
      TouchPadCharacteristic->notify(); 
    }
    else{
      digitalWrite(LED,LOW);
      TouchPadCharacteristic->setValue("0");
      TouchPadCharacteristic->notify();
    }
     txValue2 = euler.x();
     char txString_two[1];
     dtostrf(txValue2, 1, 2, txString_two);
     xCharacteristic->setValue(txString_two);
     xCharacteristic->notify();
     
     txValue3 = euler.y(); 
     char txString_three[1];
     dtostrf(txValue3, 1, 2, txString_three);
     yCharacteristic->setValue(txString_three);
     yCharacteristic->notify();

     txValue4 = euler.z();
     char txString_four[1];
     dtostrf(txValue4, 1, 2, txString_four);
     zCharacteristic->setValue(txString_four);
     zCharacteristic->notify();
  }
  else{
    digitalWrite(Knx_LED,HIGH);
    delay(500);
    digitalWrite(Knx_LED,LOW);
    delay(1000);
    }
 }

//Function for reading distance from ultrasonic sensor
float readDistanceSensor(){
  digitalWrite(DistTrigger,LOW);
  delayMicroseconds(2);
  digitalWrite(DistTrigger,HIGH);
  delayMicroseconds(10);
  digitalWrite(DistTrigger,LOW);

  float duration = pulseIn(DistEcho,HIGH); //duration is microseconds
  float distance = duration * 0.0343 / 2; //distance in centimeters
  
  return distance;
}


void Connect_LED(){
  if(deviceConnected){
    digitalWrite(Knx_LED,HIGH);
    }
  else{
    digitalWrite(Knx_LED,LOW); 
    }
  }
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial);
  
  pinMode(DistTrigger,OUTPUT);
  pinMode(DistEcho, INPUT);
  pinMode(touchPad,INPUT);
  pinMode(LED,OUTPUT);
  pinMode(Knx_LED,OUTPUT);

  BLE_setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  BLE_Data();
  Connect_LED();
}

//Libraries used0157
#include <ArduinoBLE.h>
#include "MIDIUSB.h"
#include "frequencyToNote.h"
#include "pitchToNote.h"
#include <Keyboard.h>

int newVal = 0;

//Functions used for MIDI connection
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}
void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

//Function that scans for nearby BLE devices
void BLEscan(){
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
  
    while (1);
  }

  Serial.println("BLE Central - Music Converter Proto 2");
  Serial.println("Make sure to turn on the device.");Serial.println(" ");delay(500); 

  // start scanning for peripheral
  BLE.scan();
  }

/*Function that searches through each scanned BLE device until "General Kenobi" is found.  
 *"Generel Kenobi" is the ESP32. 
 *This function then proceeds to call the ESP32Chars function
 */
void BLEsearch(){
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    // discovered a peripheral, print out address, local name, and advertised service
    Serial.print("Found ");
    Serial.print(peripheral.address());
    Serial.print(" '");
    Serial.print(peripheral.localName());
    Serial.print("' ");
    Serial.print(peripheral.advertisedServiceUuid());
    Serial.println();

    // Check if the peripheral is a SensorTag, the local name will be:
    // "General Kenobi"
    if (peripheral.localName() == "General Kenobi") {
      // stop scanning
      BLE.stopScan();

      ESP32Chars(peripheral);//calls for ESP32Chars function->produces music based on recieved data

      // peripheral disconnected, start scanning again
      BLE.scan();
    }
  }
 }

/*Function that finds each advertised service and characteristic, stores the data and maps the data to musical notes and sends
 * MIDI signals to a connected MIDI device.
 */
void ESP32Chars(BLEDevice peripheral) {
  // connect to the peripheral
  Serial.println("Connecting ...");
  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

  // discover peripheral attributes
  Serial.println("Discovering service 1...");
  if (peripheral.discoverService("8be5a5e3-82bd-4cb8-89c9-f3022fce303a")) {
    Serial.println("Service discovered");
  } else {
    Serial.println("Attribute discovery failed.");
    peripheral.disconnect();

    while (1);
    return;
  }
  Serial.println("Discovering service 2...");
  if (peripheral.discoverService("2f95d50f-c124-45f2-825b-6c5ed1a35463")) {
    Serial.println("Service discovered");
  } else {
    Serial.println("Attribute discovery failed.");
    peripheral.disconnect();

    while (1);
    return;
  }
  Serial.println("Discovering service 3...");
  if (peripheral.discoverService("05fb73dc-0066-11ec-9a03-0242ac130003")) {
    Serial.println("Service discovered");
  } else {
    Serial.println("Attribute discovery failed.");
    peripheral.disconnect();

    while (1);
    return;
  }

  //Retrieve the characteristics advertised from ESP32
  BLECharacteristic TouchPadChar = peripheral.characteristic("ec62e45e-6835-4857-99ae-dc967774b166"); //The touch pad characteristic
  BLECharacteristic Ultron = peripheral.characteristic("8a90c654-f336-4957-8c01-223f1e4dd517"); //The ultrasonic sensor characteristic
  BLECharacteristic Euler_x = peripheral.characteristic("9dabffb9-5aa8-429d-ac32-1c872d45a7d6"); //x-axis characteristic
  BLECharacteristic Euler_y = peripheral.characteristic("7c0ef830-a49d-47c3-b31c-bc2d5150dfbe"); //y-axis characteristic
  BLECharacteristic Euler_z = peripheral.characteristic("00660493-2cb1-4a16-8e86-86cc3d6a1c9d"); //z-axis characteristic
 
  Serial.print("Retrieving characteristics...");delay(500);
  
  Serial.println("The program will begin in...");Serial.println("3");delay(1000);Serial.println("2");delay(1000);Serial.println("1");
  delay(1000);Serial.println("");

  //while the client is connected to the server 
  while (peripheral.connected()) {
    // while the peripheral is connected
    
      char TouchBuff[1];

      TouchPadChar.readValue(TouchBuff,1);
      Serial.print("Touch pad: ");

      //convert TouchBuff to a string which then gets converted to a floating point number
      int Padsize = sizeof(TouchBuff) / sizeof(char);
      String TouchString = convertToString(TouchBuff,Padsize);
      Serial.print(convertToInt(TouchString));
      
      Serial.print('\n');
      
      //When the touch pad had been touched, the program will stop running
      if(convertToInt(TouchString) == 1){
        Serial.println("System - OFF");
        exit(0);
        }     
       
       
    if(Ultron.valueUpdated()){
      char UltronBuff[4];
    
      Ultron.readValue(UltronBuff,4);
      Serial.print("Ultron: ");

      int Usize = sizeof(UltronBuff) / sizeof(char);
      String Ustring = convertToString(UltronBuff,Usize);
      Serial.print(convertToInt(Ustring));
      
      LVLTrigger_Ultron(convertToInt(Ustring));
 
      Serial.print('\n');
      }
      
    if(Euler_x.valueUpdated()){
      char eulerXbuff[4];

      Euler_x.readValue(eulerXbuff,4);
      Serial.print("X-Axis: ");

      int Euler_Xsize = sizeof(eulerXbuff) / sizeof(char);
      String xString = convertToString(eulerXbuff,Euler_Xsize);
      Serial.print(convertToInt(xString));

      LVLTrigger_X(convertToInt(xString));      
      Serial.print('\n');
      }
      
   if(Euler_y.valueUpdated()){
      char eulerYbuff[4];

      Euler_y.readValue(eulerYbuff,4);
      Serial.print("Y-Axis: ");

      int Euler_Ysize = sizeof(eulerYbuff) / sizeof(char);
      String yString = convertToString(eulerYbuff,Euler_Ysize);
      Serial.print(convertToInt(yString));

      LVLTrigger_Y(convertToInt(yString));

      Serial.print('\n');
      }
      
    if(Euler_z.valueUpdated()){
      char eulerZbuff[4];

      Euler_z.readValue(eulerZbuff,4);
      Serial.print("Z-Axis: ");

      int Euler_Zsize = sizeof(eulerZbuff) / sizeof(char);
      String zString = convertToString(eulerZbuff,Euler_Zsize);
      Serial.print(convertToInt(zString));
     
      LVLTrigger_Z(convertToInt(zString));
      }    
     }
    }

String convertToString(char* a, int size){
  int i;
  String s = "";
  for (i= 0; i < size;i++){
    s = s + a[i];
    }
    return s;
  }

int convertToInt(String sensStr){
  float cnvrtInt = sensStr.toInt();
  return cnvrtInt;
  }

void LVLTrigger_Ultron(int sensorData){
  int UltronList[]={3,10,20,25,30,35,40,45};
  int notes[] = {24,36,48,60,72,84,96,108};

  for(int i = 0; i <= 7; i++){
    if(sensorData <= UltronList[i]){
      noteOn(0,notes[i],64);
      
      Serial.print('\n');
      Serial.println(notes[i]);
      
      MidiUSB.flush();
      delay(100);
      noteOff(0,notes[i],64);
      MidiUSB.flush();
      break;
      }
    }
  }

void LVLTrigger_X(int sensorData){
  int X_List[]={0,45,90,135,180,225,270,315};
  int notes[] = {28,40,52,64,76,88,100,21};

  for(int i = 0; i <= 7; i++){
    if(sensorData <= X_List[i]){
      noteOn(0,notes[i],64);
      
      Serial.print('\n');
      Serial.println(notes[i]);
      
      MidiUSB.flush();
      delay(100);
      noteOff(0,notes[i],64);
      MidiUSB.flush();
      break;
      }
    }
  }


void LVLTrigger_Y(int sensorData){
  int Y_List[]={-180,-135,-90,-45,0,45,90,135,180};
  int notes[] = {21,33,45,57,69,81,93,105,108};

  for(int i = 0; i <= 8; i++){
    if(sensorData <= Y_List[i]){
      noteOn(0,notes[i],64);
      
      Serial.print('\n');
      Serial.println(notes[i]);
      
      MidiUSB.flush();
      delay(100);
      noteOff(0,notes[i],64);
      MidiUSB.flush();
      break;
      }
    }
  }

void LVLTrigger_Z(int sensorData){
  int Z_List[]={-180,-135,-90,-45,0,45,90,135,180};
  int notes[] = {31,43,55,67,79,91,103,105,108};

  for(int i = 0; i <= 8; i++){
    if(sensorData <= Z_List[i]){
      noteOn(0,notes[i],64);
      
      Serial.print('\n');
      Serial.println(notes[i]);
      
      MidiUSB.flush();
      delay(100);
      noteOff(0,notes[i],64);
      MidiUSB.flush();
      break;
      }
    }
  }


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
    BLEscan();
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEsearch();
}

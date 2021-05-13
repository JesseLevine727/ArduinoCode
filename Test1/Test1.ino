#include "MIDIUSB.h"
#include "frequencyToNote.h"
#include "pitchToNote.h"

const int xInput = A0;
const int yInput = A1;
const int zInput = A2;
const int tempIn = A3;
const int grayScale = A4;


// initialize minimum and maximum Raw Ranges for each axis for accelerometer 
int RawMin = 0;
int RawMax = 1023;


void noteOn(byte channel, byte pitch, byte velocity){
  
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}
void noteOff(byte channel, byte pitch, byte velocity){
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}





void setup() 
{
  pinMode(grayScale, INPUT);
  pinMode(xInput,INPUT);
  pinMode(yInput,INPUT);
  pinMode(zInput,INPUT);
  pinMode(tempIn,INPUT);
  analogReference(EXTERNAL);
  
  Serial.begin(115200);
}

void controlChange(byte channel, byte control, byte value){
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control,value};
  MidiUSB.sendMIDI(event);
}

void loop() 
{
  //Read raw values
  long xRaw = analogRead(xInput);
  delay(5);
  long yRaw = analogRead(yInput);
  delay(5);
  long zRaw = analogRead(zInput);
  delay(5);

  // Convert raw values to Gs
  long xScaled = map(xRaw, RawMin, RawMax, -3, 3);
  long yScaled = map(yRaw, RawMin, RawMax, -3, 3);
  long zScaled = map(zRaw, RawMin, RawMax, -3, 3);

  
  Serial.print(" X, Y, Z ");
  Serial.print('\n');
  Serial.print( xScaled);
  Serial.print("G, ");
  Serial.print(yScaled);
  Serial.print("G, ");
  Serial.print(zScaled);
  Serial.println("G");

  delay(500);

  //temperature sensor
  uint16_t val;
  int dat;
  val = analogRead(tempIn);
  delay(5);
  dat = (int) val * (5/10.24);
  Serial.print("Temp:");
  Serial.print(dat);
  Serial.println('C');
  delay(500);

  //Grayscale
  int light;
  light = analogRead(grayScale);
  Serial.print(light);
  
  delay(500);
  Serial.print('\n');
  Serial.println("On");
  noteOn(0,dat,127);
  MidiUSB.flush();
  
  delay(500);
  Serial.println("Off");
  noteOff(0,dat,127);
  MidiUSB.flush();
  
  

  controlChange(0,10,65);


  
}

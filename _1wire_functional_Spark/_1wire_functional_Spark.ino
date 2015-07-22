// This #include statement was automatically added by the Spark IDE.
#include "OneWire/OneWire.h"

// OneWire DS18S20, DS18B20, DS1822, DS2438 Temperature Example
//
// https://github.com/Hotaman/OneWireSpark
//
// Thanks to all who have worked on this demo!
// I just made some minor tweeks for the spark core
// and added support for the DS2438 battery monitor
// 6/2014 - Hotaman

// Define the pins we will use
int ow = D3;    // put the onewire bus on D3 (or 3 for a regular Arduino)
int ledPin = D7;

//create the oneWire instance
OneWire  oneWire(ow);  // OneWire commands will be prefixed with oneWire

// define the 1-Wire addresses of the DS2438 battery monitors here (lsb first)
uint8_t DS2438_address1[] = { 0x26, 0x5B, 0xDD, 0xD6, 0x01, 0x00, 0x00, 0xA6 };       //device #1
uint8_t DS2438_address2[] = { 0x26, 0x69, 0xDD, 0xD6, 0x01, 0x00, 0x00, 0x25 };       //device #2
uint8_t DS2438_address3[] = { 0x26, 0x6D, 0xDD, 0xD6, 0x01, 0x00, 0x00, 0xF9 };       //device #3

//set up ADC voltage channels
#define V_AD 1
#define V_DD 0

int temp2 = 123;
int volt2 = 456;

void setup(void) 
{
  Serial.begin(9600);  // local hardware test only, for Spark set to 57600, for conventional set to 9600

  Spark.variable("temperature2", &temp2, INT);
  
  Spark.variable("voltage2", &volt2, INT);

}

void loop(void) 
{

  byte i;
  byte type_s = 2;
  byte data_T[12];
  byte data_V[12];
  byte addr[8];
  float voltage2, temperature2;

  temperature2 = MeasTemperature_2438(DS2438_address2);

  temp2 = temperature2;

  Serial.println();
  Serial.print("  Data T2 = ");
  Serial.print("  Temperature2 = ");
  Serial.print(temperature2);
  Serial.println();

  voltage2 = MeasADC(DS2438_address2, V_AD);

  volt2 = voltage2;

  Serial.println();
  Serial.print("  Data V2 = ");
  Serial.print("  Voltage2 = ");
  Serial.print(voltage2);
  Serial.println();

  flashLED();

}

//function to measure the two voltage channels of the DS2438
float MeasADC(uint8_t address[8], int source)
{
  int n, a[9]; 
  float v;
  
  oneWire.reset();
  oneWire.select(address);  
  oneWire.write( 0x4e, 0);   // write scratchpad
  oneWire.write( 0x00, 0);   // setup for Vdd or A/D
  if (source == V_AD)
  {
    oneWire.write( 0x00, 0);  // Vad
  }
  else
  {
    oneWire.write( 0x08, 0);  // Vdd
  }     

  delay(1000);     // maybe 750ms
  
  oneWire.reset();
  oneWire.select(address);  
  oneWire.write( 0xb4, 0);  // perform A/D
  delay(1000);  // wait for A/D to complete

  oneWire.reset();  //reset
  oneWire.select(address);  
  oneWire.write( 0xb8, 0);   //recall memory
  oneWire.write( 0x00, 0);  // recall to scratchpad  

  oneWire.reset();
  oneWire.select(address);  
  oneWire.write( 0xbe, 0);  //reac scratchpad
  oneWire.write( 0x00, 0);  // read scratchpad
  
  for (n=0; n<9; n++)
  {
    a[n] = oneWire.read(); 
  }  
       
  v = a[4] * 256 + a[3];    
  v = v / 100;
  return(v); 
}

//function to measure the temperature of the DS2438
float MeasTemperature_2438(uint8_t address[8]) 
{
  int data_T[12], i;
  float t;

  oneWire.reset();
  oneWire.select(address);      // Just do one at a time for testing
  oneWire.write(0x44);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms
      
  oneWire.reset();
  oneWire.select(address);    
  oneWire.write(0xB8,0);         // Recall Memory 0
  oneWire.write(0x00,0);         // Recall Memory 0

  oneWire.reset();
  oneWire.select(address);    
  oneWire.write(0xBE, 0);         // Read Scratchpad 0
  oneWire.write(0x00,0);         // Recall Memory 0

  for ( i = 0; i < 9; i++)      // we need 9 bytes
  {           
    data_T[i] = oneWire.read();
  }
  
  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data_T[1] << 8) | data_T[0];
  if (data_T[2] > 127) data_T[2]=0;
  data_T[1] = data_T[1] >> 3;
  t = (float)data_T[2] + ((float)data_T[1] * .03125);
    
  return(t);
}

//heartbeat function
void flashLED(){
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
}

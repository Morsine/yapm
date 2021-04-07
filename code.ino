int fanspeed = 4;
int overheat = 0;
int lastButtonState;
int currentButtonState;
int R4 = 2;
int R3 = 3;
int R2 = 4;
int R1 = 5;
int BUTTON_PIN = 12;
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 8
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;
#define I2C_ADDRESS 0x3C
#define RST_PIN -1

SSD1306AsciiAvrI2c oled;
void setup() {
    pinMode(R4, OUTPUT);  
    pinMode(R3, OUTPUT); 
    pinMode(R2, OUTPUT); 
    pinMode(R1, OUTPUT); 
    digitalWrite(R4, HIGH);
    digitalWrite(R3, HIGH);
    digitalWrite(R2, HIGH);
    digitalWrite(R1, HIGH);
    digitalWrite(R4, LOW);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    currentButtonState = digitalRead(BUTTON_PIN);
#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0
  // Call oled.setI2cClock(frequency) to change from the default frequency.

  oled.setFont(System5x7);
  oled.clear();
  oled.print("Loading...");
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  // locate devices on the bus
  Serial.print("Locating devices...");
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
   if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, 9);
 
  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC); 
  Serial.println();
}


void printTemperature(DeviceAddress deviceAddress)
{
  // method 1 - slower
  //Serial.print("Temp C: ");
  //Serial.print(sensors.getTempC(deviceAddress));
  //Serial.print(" Temp F: "); //unless you live in America
  //Serial.print(sensors.getTempF(deviceAddress)); // Makes a second call to getTempC and then converts to Fahrenheit

  // method 2 - faster
  float tempC = sensors.getTempC(deviceAddress);
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  Serial.print("Temp C: ");
  Serial.print(tempC);
  oled.setFont(System5x7);
  oled.clear();
  oled.print("Temp: ");
  oled.print(tempC);
  oled.print(" C");
  oled.print("\n");
  oled.print("Fan Speed: ");
  oled.print(fanspeed);
  oled.print("\n");
  oled.print("Int Temp: ");
  oled.print(GetTemp());
  if (tempC > 75) {
    oled.print("\n");
    oled.print("   WARNING!");
    oled.print("\n");
    oled.print("OVER HEAT");
    fanspeed = 4;
    overheat = 1;
    digitalWrite(R1, HIGH);
    digitalWrite(R3, HIGH);
    digitalWrite(R2, HIGH);
    digitalWrite(R4, LOW);
  }
}


void loop(void)
{ 
  lastButtonState    = currentButtonState;
  currentButtonState = digitalRead(BUTTON_PIN);
  if(lastButtonState == HIGH && currentButtonState == LOW) {
    oled.print("\n");
    oled.print("Changing fan speed...");
    if(fanspeed == 0){
      //ignore this
    }
    else if(fanspeed == 1){
      fanspeed = 2;
      digitalWrite(R4, HIGH);
    digitalWrite(R3, HIGH);
    digitalWrite(R1, HIGH);
    digitalWrite(R2, LOW);
    }
    else if(fanspeed == 2){
      fanspeed = 3;
      digitalWrite(R4, HIGH);
    digitalWrite(R1, HIGH);
    digitalWrite(R2, HIGH);
    digitalWrite(R3, LOW);
    }
    else if(fanspeed == 3){
      fanspeed = 4;
      digitalWrite(R1, HIGH);
    digitalWrite(R3, HIGH);
    digitalWrite(R2, HIGH);
    digitalWrite(R4, LOW);
    }
    else if(fanspeed == 4){
      fanspeed = 1;
      digitalWrite(R4, HIGH);
    digitalWrite(R3, HIGH);
    digitalWrite(R2, HIGH);
    digitalWrite(R1, LOW);
    }
  }
  if(overheat == 1){
    oled.print("\n");
    oled.print("Prv OVERHEAT!");
  }

  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");
  printTemperature(insideThermometer); 
}


double GetTemp(void)
{
  unsigned int wADC;
  double t;

  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
  ADCSRA |= _BV(ADEN);  // enable the ADC

  delay(20);            // wait for voltages to become stable.

  ADCSRA |= _BV(ADSC);  // Start the ADC

  // Detect end-of-conversion
  while (bit_is_set(ADCSRA,ADSC));

  // Reading register "ADCW" takes care of how to read ADCL and ADCH.
  wADC = ADCW;

  // The offset of 324.31 could be wrong. It is just an indication.
  t = (wADC - 324.31 ) / 1.22;

  // The returned temperature is in degrees Celsius.
  return (t);
}


void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

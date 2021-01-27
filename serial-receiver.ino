#include "Adafruit_VL53L0X.h"
#include <Adafruit_NeoPixel.h>

#define PIN        6
Adafruit_NeoPixel pixels(1, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500
int r,g,b;

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
int old_measure;
int measure;

const int switchPin = 2;      
const int xAxis = A0;         
const int yAxis = A1;         
int lastSwitchState = HIGH;
int xMap, yMap, old_xMap, old_yMap, xValue, yValue;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Ready!");
  
  lox.begin();
  pixels.begin();
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);
  old_xMap = 0;
  old_yMap = 0;
}

void loop() {
  if (Serial.available())
    {
      long int command = Serial.parseInt();
      String str_command = String(command);
      if (command == 1)  // Turns on built-in led
      {
       digitalWrite(LED_BUILTIN, HIGH);
       Serial.println("ON");
      }
      else if (command == 2)  // Turns off built-in led
      {
       digitalWrite(LED_BUILTIN, LOW);
       Serial.println("OFF");
      }
      else if (command == 3)  // Blinks built-in led three times
      {
       led_blink(3, 300);
       Serial.println("BLINK");
      }
      else if (str_command.startsWith("4")) // Parsing command(for 416 returns 256) and returns square of number
      {
       delay(50);
       long int number = str_command.substring(1).toInt();
       Serial.println(number*number);
      }
      else if (command == 5) // Continuously sends joystick location(in xy format) and button state(HIGH = * and LOW = +)
      {
       delay(50); 
       while(1){
        int stop_command = Serial.read();
          if(stop_command == 57){  //equals 9 in decimal, exits to receiving new commands
            old_xMap = 0; //reset old values for next requests
            old_yMap = 0;
            break;
          }
          int switchState = digitalRead(switchPin);
          if (switchState != lastSwitchState) {
            if (switchState == LOW) {
              Serial.println("+");
            }
            if (switchState == HIGH) {
              Serial.println("*");
            }
            delay(100);
          }else{
            xValue = analogRead(xAxis);
            yValue = analogRead(yAxis);
            xMap = map(xValue, 0,1023, 1, 9);
            yMap = map(yValue,0,1023,1,9);
            if(old_xMap != xMap || old_yMap != yMap){
              Serial.print(xMap);
              Serial.println(yMap);
            }
            old_xMap = xMap;
            old_yMap = yMap;
            delay(100);
          }
          lastSwitchState = switchState;
       }
       Serial.println("END");
      }
      else if (str_command.startsWith("6")){  // Parsing command(for 64 blinks 4 times) and blinks as ordered
        int times = str_command.substring(1).toInt();
        led_blink(times, 300);
        Serial.println("BLINK");
      }
      else if(command == 7){  // Continuously sends distance measure in milimeters
        delay(50);
        while(1){
          int stop_command = Serial.read();
          if(stop_command == 57){  //equals 9 in decimal, exits to receiving new commands
            old_measure = 0; //reset old values for next
            break;
          }
          
          VL53L0X_RangingMeasurementData_t measure_data;  
          lox.rangingTest(&measure_data, false);
        
          measure = int(measure_data.RangeMilliMeter);
          if(abs(measure - old_measure) >= 5){  
            if(measure > 20 && measure < 1200){
              Serial.println(measure);
              delay(100);
            }else{
              Serial.println("+");
            }
          }
          old_measure = measure;
        }
        Serial.println("END");
      }
      else if (command == 8){  // Continuously sends distance measure and sets rgb led color with measurement
        delay(50);
        while(1){
          int stop_command = Serial.read();
          if(stop_command == 57){  //equals 9 in decimal, exits to receiving new commands
            old_measure = 0; //reset old values for next
            break;
          }
          
          VL53L0X_RangingMeasurementData_t measure_data;  
          lox.rangingTest(&measure_data, false);
        
          measure = int(measure_data.RangeMilliMeter);
          if(abs(measure - old_measure) >= 5){  
            if(measure > 20 && measure < 1200){
              Serial.println(measure);
              long int hue = map(measure, 20, 1200, 0, 65535); 
              setColorHue(hue);
              delay(50);
            }else{
              Serial.println("+");
            }
          }
          old_measure = measure;
        }
        closePixels();
        Serial.println("END");
      }
    }
}

void led_blink(int times, int _delay){
  for(int i=0; i<times*2; i++){
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(_delay);
  }
}

void setColorHue(long int hue) {  // 0 to 65535
  pixels.setPixelColor(0, pixels.ColorHSV(hue, 255, 255));
  pixels.show();   // Send the updated pixel colors to the hardware.
}

void closePixels() {
  pixels.clear();
  pixels.show();
}

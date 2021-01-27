const int switchPin = 2;      
const int xAxis = A0;         
const int yAxis = A1;         
int lastSwitchState = LOW;
int xMap, yMap, old_xMap, old_yMap, xValue, yValue;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);
  old_xMap = 0;
  old_yMap = 0;
  
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Ready!");
}

void loop() {
  if (Serial.available())
    {
      long int command = Serial.parseInt();
      String str_command = String(command);
      if (command == 1)
      {
       digitalWrite(LED_BUILTIN, HIGH);
       Serial.println("ON");
      }
      else if (command == 2)
      {
       digitalWrite(LED_BUILTIN, LOW);
       Serial.println("OFF");
      }
      else if (command == 3)
      {
       led_blink(3, 300);
       Serial.println("blinked");
      }
      else if (str_command.startsWith("4"))
      {
       delay(50);
       long int number = str_command.substring(1).toInt();
       Serial.println(number*number);
      }
      else if (command == 5)
      {
       delay(50); 
       Serial.println("BONUS!!!!");
       while(1){
        int stop_command = Serial.read();
          if(stop_command == 57){  //equals 9 in decimal
            old_xMap = 0; //reset old values for next requests
            old_yMap = 0;
            break;
          }
          int switchState = digitalRead(switchPin);
          if (switchState != lastSwitchState) {
            if (switchState == LOW) {
              Serial.println("+");
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
       Serial.println("end of bonus");
      }
      else if (str_command.startsWith("6")){
        int times = str_command.substring(1).toInt();
        led_blink(times, 300);
        Serial.println("blinked");
      }
      else{
        if(command != 0){
          Serial.print("Invalid:  ");
          Serial.println(command);
        }
      }
    }
}

void led_blink(int times, int _delay){
  for(int i=0; i<times*2; i++){
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(_delay);
  }
}

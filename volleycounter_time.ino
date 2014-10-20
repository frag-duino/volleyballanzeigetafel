// DS1307 time module

char in;
String inputString = "";
const char helpstring[] = "\n - gettime\n - settime hh:mm:ss";

void loop_clock(){

  if(currentMillis > wakeTime_clock){
    getTime();
    wakeTime_clock = currentMillis + 1000;
  }

  while (Serial.available()) {
    // Input lesen
    delay(3);
    in = Serial.read();
    inputString += in;
  } 

  if (inputString.length() > 0) {
    if(inputString.substring(0,7) == "settime"){
      Serial.print("settime> ");
      printTime();
      hour=inputString.substring(8,10).toInt();
      minute=inputString.substring(11,13).toInt();
      second=inputString.substring(14,16).toInt();
      setTime();
      delay(100);
      getTime();
      printTime();
    }
    else if(inputString.substring(0,7) == "gettime"){
      Serial.print("gettime> ");
      printTime();
    } 
    else if(inputString.substring(0,4) == "help"){
      Serial.println(helpstring);

      // Initialize Demo:
      for(int i=0; i < 99; i++){
        set_digit(LEFT, i);
        set_digit(RIGHT, i);
        print_matrix(INVERTED);
        delay(500);
      }
    }
    else
    {
      Serial.print("Unknown (help): ");
      Serial.println(inputString);
    }
    inputString = "";
  }
}

void getTime(){
  // Initialize
  Wire.beginTransmission(DS1307_ADRESS);
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADRESS, 7);

  // Read values
  second = bcdToDec(Wire.read());
  minute = bcdToDec(Wire.read());
  hour = bcdToDec(Wire.read() & 0b111111);
}

void printTime(){
  // Output
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);
}

// Sets the current time and date at the module
void setTime(){
  Wire.beginTransmission(DS1307_ADRESS);
  Wire.write(0x00);
  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(1); // Weekday
  Wire.write(1); // Day
  Wire.write(1); // Month
  Wire.write(1); // Year
  Wire.write(0x00);
  Wire.endTransmission();
}

void displayClock(){
  displayClockUntil = currentMillis + TIME_SHOWCLOCK;
}

// Helper
byte decToBcd(byte val) {
  return ((val/10*16) + (val%10));
}
byte bcdToDec(byte val) {
  return ((val/16*10) + (val%16));
}








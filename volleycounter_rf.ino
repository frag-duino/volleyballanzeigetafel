// Handles the RF-Inputs

// RF-Values for 10101
long VALUES_10101_ON[]={
  1115473,1118545, 1119313, 1119505,1119553}; // AN: A,B,C,D,E
long VALUES_10101_OFF[] = {
  1115476, 1118548, 1119316, 1119508,1119556}; // AUS: A,B,C,D,E

long VALUES_MINIRF_LEFT = 5330188;
long VALUES_MINIRF_RIGHT = 5592076;
// long VALUES_MINIRF = 5592332; // Without soldering

void setup_rf(){
  // RF-Receiver on Interrupt 1 = PIN 3
  mySwitch.enableReceive(INTERRUPT_RF);

  if(INVERTED){
    long templong = VALUES_MINIRF_LEFT;
    VALUES_MINIRF_LEFT = VALUES_MINIRF_RIGHT;
    VALUES_MINIRF_RIGHT = templong;
  }
}

void loop_rf(){
  if (mySwitch.available()) {
    long value = mySwitch.getReceivedValue();
    Serial.print("RF> ");
    Serial.print(value);
    Serial.print(": ");

    if(check_if_operation_is_allowed()){ // Debounce

      if (value == VALUES_10101_ON[2])  // C on = Reset
        aktuator_reset(DEBOUNCE_LONG);
      else if (value == VALUES_10101_OFF[2])  // C off = Clock
        displayClock();
      else if (value == VALUES_10101_ON[3])  // D on = Display Akku
        displayAkku();
      else if(value == VALUES_10101_ON[0] || value == VALUES_MINIRF_LEFT) // A on = Left++
        aktuator_plus(LEFT, DEBOUNCE_LONG);
      else if(value == VALUES_10101_OFF[0] || value == VALUES_MINIRF_RIGHT) // A off = Right++
        aktuator_plus(RIGHT, DEBOUNCE_LONG);
      else if(value == VALUES_10101_ON[1]) // B on = Left--
        aktuator_minus(LEFT, DEBOUNCE_LONG);
      else if(value == VALUES_10101_OFF[1]) // B off = Right--
        aktuator_minus(RIGHT, DEBOUNCE_LONG);
      else
        Serial.println(" (Unknown)");  
    }

    mySwitch.resetAvailable();
  }
}






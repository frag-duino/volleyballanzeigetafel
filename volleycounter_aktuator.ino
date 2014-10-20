// Handles the commands
int lastPointFor = LEFT;

// Start the debounce counter
void start_debounce(int debounceTime){
  wakeTime_debounce = currentMillis + debounceTime;
  // Serial.print("CurrentMillis: ");
  // Serial.print(currentMillis);
  // Serial.print(" Debounce at: ");
  // Serial.println(wakeTime_debounce);
}

// Checks if there is debounce
boolean check_if_operation_is_allowed(){
  if(currentMillis > wakeTime_debounce)
    return true; // Allowed
  else
    return false; // Blocking
}

// Undos the last point
void undoPoint(){
  if(lastPointFor == LEFT)
    aktuator_minus(LEFT, DEBOUNCE_MEDIUM);
  else
    aktuator_minus(RIGHT, DEBOUNCE_MEDIUM);
}

void aktuator_reset(int debounceTime){
  start_debounce(debounceTime);
  counter_left = counter_right = 0;
  Serial.println("RESET");
  blink_and_sound(BOTH);
  play_voice('0');
  play_voice('P');
  play_voice('0');
}

void aktuator_plus(int LEFTRIGHT, int debounceTime){
  start_debounce(debounceTime);
  if(LEFTRIGHT == LEFT){
    counter_left++;
    lastPointFor = LEFT;
    Serial.print("Left++ = ");
    Serial.println(counter_left);
    blink_and_sound(LEFT);
  }
  else{    
    counter_right++;
    lastPointFor = RIGHT;
    Serial.print("Right++ = ");
    Serial.println(counter_right);
    blink_and_sound(RIGHT);
  }
}

void aktuator_minus(int LEFTRIGHT, int debounceTime){
  start_debounce(debounceTime);
  if(LEFTRIGHT == LEFT){
    counter_left--;
    Serial.print("Left-- = ");
    Serial.println(counter_left);
    blink_and_sound(LEFT);
  }
  else
  {
    counter_right--;
    Serial.print("Right-- = ");
    Serial.println(counter_right);
    blink_and_sound(RIGHT);
  }
}

void blink_and_sound(int left_right){
  // Play Sound 
  play_sound(left_right);

  // Set Status LEDs
  if(left_right == LEFT){
    state_LED_R = LOW;
    state_LED_L = HIGH;
    current_blink_amount_L = BLINK_AMOUNT;
  } 
  else if(left_right == RIGHT){
    state_LED_R = HIGH;
    state_LED_L = LOW;
    current_blink_amount_R = BLINK_AMOUNT;
  } 
  else { // Both
    state_LED_R = HIGH;
    state_LED_L = HIGH;
    current_blink_amount_R = BLINK_AMOUNT;
    current_blink_amount_L = BLINK_AMOUNT;
  }
}


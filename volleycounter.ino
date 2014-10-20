// Volleyballanzeigetafel
// Version 1.0
// http://frag-duino.de/index.php/projekte/48-volleyball-anzeigetafel-arduino-wtv020sd

// Includes
#include <CapSense.h>
#include <RCSwitch.h>
#include <SoftwareSerial.h>
#include <Wtv020sd16p.h>
#include "LedControl.h"
#include <Wire.h>

// Pins SOUND
#define PIN_TON_RESET 2
#define PIN_TON_CLK A2
#define PIN_TON_DATA A0
#define PIN_TON_BUSY A1

// Pins RF
#define INTERRUPT_RF 1 // = PIN3

// Pins LED-Matrix
#define PIN_MATRIX_DIN 6
#define PIN_MATRIX_CLK 7
#define PIN_MATRIX_CS 8

// Config
#define THRESHOLD 100
#define FADE_DELAY 10
#define SAMPLES 100
#define VOLTAGE_WARNING_THRESHOLD 3800 // mV
#define BLINK_AMOUNT 26 // Must be even
#define DS1307_ADRESS 0x68 // I2C address of time module
#define INVERTED false // Matrix is upside-down

// Timers debounce
#define DEBOUNCE_SHORT 500 // IR
#define DEBOUNCE_MEDIUM 1000 // Button+
#define DEBOUNCE_LONG 2500 // Button-, RF
#define INTERVAL_BLINK 150
#define INTERVAL_VOLTAGE 30000
#define TIME_PUSH_LONG 1500 // Shortpush --> Longpush
#define TIME_SHOWCLOCK 5000


// Pins LEDs and Buttons
int PIN_TEAM_L = 5;
int PIN_TEAM_R = 4;
int PIN_BUTTON_L = 12;
int PIN_BUTTON_R = 13;

// Timers voice:
int DELAY_SHORT = 1000; // z.B. "Acht"
int DELAY_MEDIUM = 1200; // z.B. "Acht"
int DELAY_LONG = 1500; // z.B. "Dreiundzwanzig"

// Constants
int LEFT = 1;
int RIGHT = 2;
int ON = 3;
int OFF = 4;
int BOTH = 5;

// State
int counter_left = 0;
int counter_right = 0;
int current_blink_amount_L = 0;
int current_blink_amount_R = 0;

// Battery
boolean low_battery = false;
boolean batterywarning_had_been_told = false; // Only one warning
long status_battery;

// Time
unsigned long currentMillis = 0;
unsigned long previousMillis_voltage = 0;
unsigned long wakeTime_voice = 0;
unsigned long wakeTime_debounce = 0;
unsigned long wakeTime_clock = 0;
unsigned long wakeTime_blink_L = 0; // Blink of the Left LED
unsigned long wakeTime_blink_R = 0; // Blink of the Right LED
unsigned long previousMillis_L = 0;
unsigned long previousMillis_R = 0;
unsigned long displayClockUntil = 0;

// Clock
int second, minute, hour, month, day, weekday, year;

// Input
RCSwitch mySwitch = RCSwitch();
int value_bt_R = HIGH;
int value_bt_L = HIGH;
int value_bt_R_before, value_bt_L_before ;
int temp;

// Output
Wtv020sd16p wtv020sd16p(PIN_TON_RESET,PIN_TON_CLK,PIN_TON_DATA,PIN_TON_BUSY);
LedControl lc = LedControl(PIN_MATRIX_DIN,PIN_MATRIX_CLK,PIN_MATRIX_CS,2);
int state_LED_R = HIGH;
int state_LED_L = HIGH;

void setup() 
{
  // Setup voice first, it needs time
  setup_voice();

  // Initialize outputs
  pinMode(PIN_TEAM_R, OUTPUT);
  pinMode(PIN_TEAM_L, OUTPUT);

  if(INVERTED){
    // Invert outputs
    temp = PIN_TEAM_R;
    PIN_TEAM_R = PIN_TEAM_L;
    PIN_TEAM_L = temp;

    // Invert inputs
    temp = PIN_BUTTON_R;
    PIN_BUTTON_R = PIN_BUTTON_L;
    PIN_BUTTON_L = temp;
  }

  // Initialize inputs and pullups
  pinMode(PIN_BUTTON_R, INPUT);
  pinMode(PIN_BUTTON_L, INPUT);
  digitalWrite(PIN_BUTTON_R, HIGH);
  digitalWrite(PIN_BUTTON_L, HIGH);

  // RF
  setup_rf();

  Serial.begin(9600);
  Serial.println("Volleyballanzeigetafel  2.1");
  Serial.println("2014 Marcel Imig - www.frag-duino.de");
  previousMillis_voltage = millis() + INTERVAL_VOLTAGE;

  // Time-Module
  Wire.begin();

  // LEDs on and off
  for(int i=0; i<4; i++){
    delay(50);
    digitalWrite(PIN_TEAM_L, LOW);
    digitalWrite(PIN_TEAM_R, LOW);
    delay(50);
    digitalWrite(PIN_TEAM_L, HIGH);
    digitalWrite(PIN_TEAM_R, HIGH);
  }

  // Sound and LED-Matrix
  setup_matrix();

  // Test voice
  play_voice('0');
  play_voice('P');
  play_voice('0');
}

void loop() 
{
  currentMillis = millis();
  loop_matrix();
  loop_rf();
  loop_voice();
  loop_clock();

  // Read the buttons
  value_bt_R_before = value_bt_R;
  value_bt_L_before = value_bt_L;
  value_bt_R = digitalRead(PIN_BUTTON_R);
  value_bt_L = digitalRead(PIN_BUTTON_L);

  if(value_bt_R == LOW && value_bt_L == LOW){ // Reset
    aktuator_reset(DEBOUNCE_LONG);
    return;
  } 

  if(check_if_operation_is_allowed() && value_bt_L_before == HIGH && value_bt_L == LOW){ // Left first down
    Serial.println(currentMillis);
    previousMillis_L = millis();
  }

  if(check_if_operation_is_allowed() && value_bt_R_before == HIGH && value_bt_R == LOW){ // Right first down
    Serial.println(currentMillis);
    previousMillis_R = millis();
  }

  // Check the time for long push left
  if(check_if_operation_is_allowed() && value_bt_L_before == LOW && value_bt_L == LOW && currentMillis - previousMillis_L > TIME_PUSH_LONG){
    Serial.println(currentMillis - previousMillis_L);
    aktuator_minus(LEFT, DEBOUNCE_LONG);
  } 
  else // Check if only short push left
  if(check_if_operation_is_allowed() && value_bt_L_before == LOW && value_bt_L == HIGH && currentMillis - previousMillis_L < TIME_PUSH_LONG){
    Serial.println(currentMillis - previousMillis_L);
    aktuator_plus(LEFT, DEBOUNCE_MEDIUM);  
  }

  // Check the time for long push right
  if(check_if_operation_is_allowed() && value_bt_R_before == LOW && value_bt_R == LOW && currentMillis - previousMillis_R > TIME_PUSH_LONG){
    Serial.println(currentMillis - previousMillis_R);
    aktuator_minus(RIGHT, DEBOUNCE_LONG);
  } 
  else // Check if only short push right
  if(check_if_operation_is_allowed() && value_bt_R_before == LOW && value_bt_R == HIGH && currentMillis - previousMillis_R < TIME_PUSH_LONG){
    Serial.println(currentMillis - previousMillis_L);
    aktuator_plus(RIGHT, DEBOUNCE_MEDIUM);  
  }

  // Check voltage
  if((unsigned long) (millis() - previousMillis_voltage) > INTERVAL_VOLTAGE){
    Serial.print("Voltage: ");
    Serial.println(readVcc(), DEC);
    previousMillis_voltage = millis();
  }

  // Check Battery
  if(low_battery && !batterywarning_had_been_told){
    displayAkku();
    batterywarning_had_been_told = true;
  }

  // Check range of counters
  if(counter_left < 0)
    counter_left = 0;
  if(counter_right < 0)
    counter_right = 0;
  if(counter_left > 39)
    counter_left = 39;
  if(counter_right > 39)
    counter_right = 39;

  // Set Status LEDs blinking
  if(currentMillis > wakeTime_blink_L){
    if(current_blink_amount_L-- > 0)
      if(state_LED_L == HIGH)
        state_LED_L = LOW;
      else 
        state_LED_L = HIGH;
    wakeTime_blink_L = currentMillis + INTERVAL_BLINK;

  }

  if(currentMillis > wakeTime_blink_R){
    if(current_blink_amount_R-- > 0)
      if(state_LED_R == HIGH)
        state_LED_R = LOW;
      else 
        state_LED_R = HIGH;
    wakeTime_blink_R = currentMillis + INTERVAL_BLINK;

  }
  digitalWrite(PIN_TEAM_L, state_LED_L);
  digitalWrite(PIN_TEAM_R, state_LED_R);
}

long readVcc() {
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  status_battery = ADCL;
  status_battery |= ADCH<<8;
  status_battery = 1126400L / status_battery; // Back-calculate AVcc in mV
  if(status_battery < VOLTAGE_WARNING_THRESHOLD)
    low_battery = true;
  else
    low_battery = false;

  return status_battery;
}


void displayAkku(){
  readVcc();
  play_voice('R');
  playInteger(status_battery);
}







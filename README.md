volleyballanzeigetafel
======================

http://frag-duino.de/index.php/projekte/48-volleyball-anzeigetafel-arduino-wtv020sd

Es handelt sich um das passende Repository zu dem Artikel "Volleyball-Anzeigetafel mit Sprachausgabe (Arduino und WTV020SD)":
http://frag-duino.de/index.php/projekte/48-volleyball-anzeigetafel-arduino-wtv020sd

Pinbelegung der Arduino-Pins:

// Pins SOUND
PIN_TON_RESET 2
PIN_TON_CLK A2
PIN_TON_DATA A0
PIN_TON_BUSY A1

// Pins RF
INTERRUPT_RF 3 (= Interrupt 1!)

// Pins LED-Matrix
PIN_MATRIX_DIN 6
PIN_MATRIX_CLK 7
PIN_MATRIX_CS 8

// Pins LEDs and Buttons
PIN_TEAM_L = 5;
PIN_TEAM_R = 4;
PIN_BUTTON_L = 12;
PIN_BUTTON_R = 13;
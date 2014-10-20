// Handles the Matrix-Output

// First digit
byte digit1_0[8] = { 
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000 };
byte digit1_1[8] = { 
  B00100000, B01100000, B10100000, B00100000, B00100000, B00100000, B00100000, B00100000 };
byte digit1_2[8] = { 
  B01100000, B10100000, B00100000, B00100000, B01000000, B10000000, B10000000, B11100000 };
byte digit1_3[8] = { 
  B11000000, B00100000, B00100000, B11000000, B00100000, B00100000, B00100000, B11000000 };
byte digit1_4[8] = { 
  B00100000, B01000000, B10000000, B10100000, B11100000, B01000000, B01000000, B01000000 };
byte digit1_5[8] = { 
  B11100000, B10000000, B10000000, B11100000, B00100000, B00100000, B00100000, B11000000 };
byte digit1_6[8] = { 
  B01100000, B10000000, B10000000, B11000000, B10100000, B10100000, B10100000, B01000000 };
byte digit1_7[8] = { 
  B11100000, B00100000, B01000000, B01000000, B11100000, B01000000, B01000000, B01000000 };
byte digit1_8[8] = { 
  B01000000, B10100000, B10100000, B01000000, B10100000, B10100000, B10100000, B01000000 };
byte digit1_9[8] = {  
  B01000000, B10100000, B10100000, B01100000, B01100000, B00100000, B00100000, B11000000 };

// Second digit
byte digit2_0[8] = { 
  B01100000, B10010000, B10010000, B10010000, B10010000, B10010000, B10010000, B01100000 };
byte digit2_1[8] = { 
  B00010000, B00110000, B01010000, B00010000, B00010000, B00010000, B00010000, B00010000 };
byte digit2_2[8] = { 
  B01100000, B10010000, B00010000, B00100000, B01000000, B10000000, B10000000, B11110000 };
byte digit2_3[8] = { 
  B11100000, B00010000, B00010000, B11100000, B00010000, B00010000, B00010000, B11100000 };
byte digit2_4[8] = { 
  B00100000, B01000000, B10000000, B10100000, B11110000, B00100000, B00100000, B00100000 };
byte digit2_5[8] = { 
  B11110000, B10000000, B10000000, B11110000, B00010000, B00010000, B00010000, B11100000 };
byte digit2_6[8] = { 
  B01100000, B10010000, B10000000, B11100000, B10010000, B10010000, B10010000, B01100000 };
byte digit2_7[8] = { 
  B11110000, B00010000, B00100000, B00100000, B01110000, B00100000, B00100000, B00100000 };
byte digit2_8[8] = { 
  B01100000, B10010000, B10010000, B01100000, B01100000, B10010000, B10010000, B01100000 };
byte digit2_9[8] = { 
  B01100000, B10010000, B10010000, B10010000, B01110000, B00010000, B10010000, B01100000 };

byte display_left[8] ={
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000 };
byte display_right[8] ={
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000 };

void setup_matrix(){
  for(int index=0; index < 2; index++) {
    lc.shutdown(index, false); 
    lc.setIntensity(index, 1); // Set Brightness
    lc.clearDisplay(index);
  }

  // Initialize Demo:
  for(int i=10; i >= 0; i--){
    set_digit(LEFT, i);
    set_digit(RIGHT, i);
    print_matrix(INVERTED);
    delay(100);
  }
}

void loop_matrix() { 
  if(currentMillis < displayClockUntil){
    set_digit(LEFT, hour);
    set_digit(RIGHT, minute);
  }
  else { 
    set_digit(LEFT, counter_left);
    set_digit(RIGHT, counter_right);
  }
  print_matrix(INVERTED);
}

// Returns the byte-array for a digit
byte *getDigit(int leftright, int number){
  if(leftright == LEFT){  // 0,1,2 --> 3 dots
    // if(true){
    if(number == 0)
      return digit1_0;
    else if(number == 1)
      return digit1_1;
    else if(number == 2)
      return digit1_2;
    else if(number == 3)
      return digit1_3;
    if(number == 4)
      return digit1_4;
    else if(number == 5)
      return digit1_5;
    else if(number == 6)
      return digit1_6;
    else if(number == 7)
      return digit1_7;
    if(number == 8)
      return digit1_8;
    else if(number == 9)
      return digit1_9;
  }
  else
  {
    if(number == 0) // 0-9 --> 4 dots
      return digit2_0;
    else if(number == 1)
      return digit2_1;
    else if(number == 2)
      return digit2_2;
    else if(number == 3)
      return digit2_3;
    else if(number == 4)
      return digit2_4;
    else if(number == 5)
      return digit2_5;
    else if(number == 6)
      return digit2_6;
    else if(number == 7)
      return digit2_7;
    else if(number == 8)
      return digit2_8;
    else if(number == 9)
      return digit2_9;
  }

  return digit1_0;
}

void set_digit(int leftright, int value){
  int lastdigit = value % 10;
  int firstdigit = value / 10;

  if((leftright == RIGHT && INVERTED) || (leftright == LEFT && !INVERTED) ){
    for(int i=0; i<8; i++){
      byte *temp = getDigit(LEFT, firstdigit);
      display_left[i] = temp[i] ;
      byte* temp_R = getDigit(RIGHT, lastdigit);      
      display_left[i] = display_left[i] | (temp_R[i] >> 4);
    }
  }
  else{
    for(int i=0; i<8; i++){
      byte *temp3 = getDigit(LEFT, firstdigit);
      display_right[i] = temp3[i] ;
      byte* temp4 = getDigit(RIGHT, lastdigit);      
      display_right[i] = display_right[i] | (temp4[i] >> 4);
    }
  }

}

void print_matrix(boolean is_inverted){
  if(is_inverted)
    for(int r=0; r < 8;r++){
      lc.setRow(0, 7-r, reverseByte(display_left[r])); // Left
      lc.setRow(1, 7-r, reverseByte(display_right[r])); // Right
    }
  else
    for(int r=0; r<8;r++){
      lc.setRow(0, r, display_left[r]); // Left
      lc.setRow(1, r, display_right[r]); // Right
    }
}



byte reverseByte(byte num) {
  byte var = 0;     
  int i, x, y, p;
  int s = 8;    // number of bits in 'num'.

  for (i = 0; i < (s / 2); i++) {
    // extract bit on the left, from MSB
    p = s - i - 1;
    x = num & (1 << p);
    x = x >> p;  
    // extract bit on the right, from LSB
    y = num & (1 << i);
    y = y >> i;

    var = var | (x << i);       // apply x
    var = var | (y << p);       // apply y
  }
  return var;
}
















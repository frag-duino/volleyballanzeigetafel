// handles the Sound Output

/*
(ASCII 48)
 0 Null
 1 Eins
 ...
 9 Neun
 : Zehn
 ; Elf
 < Zwoelf
 = Dreizehn
 > Vierzehn
 ? Fuenfzehn
 @ Sechzehn
 A Siebzehn
 B Achzehn
 ...
 N Dreissig
 O Satzball
 P Zu
 Q Es steht 
 R Akku leer
 S Beide
 
 V Test von 0-32
 X = NULL
 */

// Constants
const char voice_NULL = voice_NULL;

// Variables
char voice_queue[] = {
  voice_NULL,voice_NULL,voice_NULL,voice_NULL,voice_NULL}; // X == NULL
char temp_queue[] = {
  voice_NULL,voice_NULL,voice_NULL,voice_NULL,voice_NULL};
char c = 0;
int number=0;

void setup_voice(){
  wtv020sd16p.reset();
}

void loop_voice(){
  // Check the time for long push left
  if(currentMillis > wakeTime_voice && voice_queue[0] != voice_NULL){
    c=-1;
    // Get the last element
    for(int i=1; i < sizeof(voice_queue); i++)
      if(voice_queue[i] == voice_NULL){
        c = voice_queue[i-1];
        voice_queue[i-1]=voice_NULL;
        break;
      }
    if(c==-1){ // Overflow, no X
      c = voice_queue[sizeof(voice_queue)-1];
      voice_queue[sizeof(voice_queue)-1] = voice_NULL;
    }

    number = c-48; // char->int
    Serial.print("Input: ");
    Serial.print(c);
    Serial.print("\tVoice:");
    Serial.print(number);

    if(c == 'P'){ // Zu
      play_sample(number, 800);
      Serial.println("\t\tDelay: 800");
    }
    else if((c >= '0' && c <='<')){ // 0-12
      play_sample(number, DELAY_SHORT);
      Serial.print("\t\tDelay:");
      Serial.println(DELAY_SHORT);
    }
    else if((c >= '=' && c <='D') || c == 'N' || c == 'O' || c=='Q' || c == 'R' || c== 'S'){ // 13-20 or 30 or Voice-commands
      play_sample(number, DELAY_MEDIUM);
      Serial.print("\t\tDelay:");
      Serial.println(DELAY_MEDIUM);
    }
    else if(c >= 'E' && c <='M'){ // 21-29
      play_sample(number, DELAY_LONG);
      Serial.print("\t\tDelay:");
      Serial.println(DELAY_LONG);
    } 
    else if(c == 'V')
      for (int i='0'; i <= 'R'; i++)
        play_voice((char)i);
    else{
      Serial.print("\tUnkown: ");
      Serial.println(c);
    }
  } 
}

// Public aktuator
void play_sound(int left_right){
  if(counter_left != 0 && counter_left == counter_right){
    play_voice((char)(counter_left + 48));
    play_voice('S'); // both
  }  
  else if(left_right == LEFT){
    play_voice((char)(counter_left + 48));
    play_voice('P'); // to
    play_voice((char)(counter_right + 48));
  }
  else if(left_right == RIGHT){
    play_voice((char)(counter_right + 48));
    play_voice('P'); // to
    play_voice((char)(counter_left + 48));
  }

  if(counter_left >= 24 or counter_right >= 24){
    if(abs(counter_left - counter_right) >= 1)
      play_voice('O'); // Setpoint
  }
}

void play_voice(char c){
  //  Serial.print("Queue before: ");
  //  for(int i=0; i < sizeof(voice_queue); i++){
  //    Serial.print(voice_queue[i]);
  //    Serial.print(" ");
  //  }
  //  Serial.println(" ");

  // Check for buffer overflow
  if(voice_queue[sizeof(voice_queue)-1] != voice_NULL){
    Serial.print("Overflow! Dropping queue");
    for(int i=0; i < sizeof(voice_queue); i++)
      voice_queue[i] = voice_NULL;
  }
  else   // shift array the left
  shiftVoiceArrayTo(LEFT);

  // Set the first element
  voice_queue[0] = c;

  //  Serial.print("Queue after: ");
  //  for(int i=0; i < sizeof(voice_queue); i++){
  //    Serial.print(voice_queue[i]);
  //    Serial.print(" ");
  //  }
  //  Serial.println(" ");

}

// Helper-Method to shift the array to the right
void shiftVoiceArrayTo(int leftright){
  for(int i=0; i < sizeof(voice_queue); i++)
    temp_queue[i] =  voice_queue[i];  // Copy array 
  if(leftright==LEFT)
    for(int i=0; i < sizeof(voice_queue)-1; i++)
      voice_queue[i+1] = temp_queue[i];   // Shift left
  else
    for(int i=1; i < sizeof(voice_queue); i++)
      voice_queue[i] = temp_queue[i-1];   // Shift right
}

void play_sample(int id, int delaytime){
  wtv020sd16p.asyncPlayVoice(id);
  wakeTime_voice= currentMillis + delaytime;
}

void playInteger(int digit){
  if(digit>=1000)
    play_voice(48 + (status_battery / 1000)); // 1000er
  if(digit>=100)
    play_voice(48 +(status_battery / 100)%10); // 100er
  if(digit>=10)
    play_voice(48 +(status_battery / 10)%10); // 10er
  play_voice(48 +(status_battery / 1)%10); // 1er
}

// void sayClock(){
//  play_voice(hour);
//  playInteger(minute);
//}


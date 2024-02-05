//Configuration: 
//1. 5v LED yellow ==> light_A (house light)
//2. 5v LED yellow (big) ==> light_B1 (left cue light)
//3. 3v LED yellow (small) ==> light_B2 (left nosepoke light)
//4. 3v LED yellow (small) ==> light_C (deliver light)
//5. 5v led yellow (big) ==> light_D1 (right cue light)
//6. 3v LED yellow (small) ==> light_D2 (right nosepoke light)

int Relay_Light_A = 4; //(relay 4)
int Relay_Light_B2 = 7;//(relay 1)
int Relay_Light_C = 9;//(relay 2)
int Relay_Light_B1 = 5;//(relay 5)
int Relay_Light_D1 = 6;//(relay 6)
int Relay_Light_D2 = 3;//(relay 3)
int Relay_solenoid = 8;//

boolean Nosepoke_In_B = false;
boolean Nosepoke_In_C = false;
boolean Nosepoke_In_D = false;

boolean Lock_A = false;
boolean Lock_C = false;
boolean Lock_B1 = false;
boolean Lock_B2 = false;
boolean Lock_D1 = false;
boolean Lock_D2 = false;
boolean Lock_timer5 = false;
boolean Lock_side = false;
boolean nosepoke_B_voltage = true;
boolean nosepoke_C_voltage = true;
boolean nosepoke_D_voltage = true;


char msg1[] = "trial start";
char msg3[] = "trial end";
char msg8[] = "np_B in";
char msg9[] = "np_B out";
char msg10[] = "np_C in";
char msg11[] = "np_C out";
char msg13[] = "np_D in";
char msg14[] = "np_D out";
char msg15[] = "cen tri";
char msg16[] = "center omi";
char msg17[] = "side omi";
char msg18[] = "timeout";
char msg19[] = "B1_";
char msg20[] = "B2_";
char msg21[] = "B3_";
char msg22[] = "B4_";
char msg23[] = "B5_";
char msg24[] = "B6_";
char msg25[] = "B7_";
char msg26[] = "B8_";
char msg27[] = "B9_";
char msg29[] = "s reward delivery";
char msg30[] = "session ends";
char msg31[] = "l reward delivery";
char msg32[] = "B10_";
char msg33[] = "B11_";
char msg34[] = "B12_";
char msg35[] = "B13_";
char msg36[] = "B14_";
char msg37[] = "B15_";


unsigned long trial_count = 1;
unsigned long current_trial = 0;
unsigned long max_trial = 1;
unsigned long phase = 1;
unsigned long tempnum = 0;

boolean New_TrialL = false;
boolean phase_lock = false;

unsigned long block_number = 1;
unsigned long current_block = 1;

unsigned long StartTime = 0;
unsigned long timer_1 = 0; // #1 10 sec timer
unsigned long timer_2 = 0; // #1 5 sec timer
unsigned long timer_3 = 0; // #2 10 sec timer
unsigned long timer_4 = 0; // #3 10 sec timer
unsigned long timer_5 = 0; // inter block interval
unsigned long timer_trial = 0;
unsigned long current_time = 0;
unsigned long elapsed_time = 0;
unsigned long timed_1 = 0;
unsigned long timed_2 = 0;

unsigned long randNumber;
unsigned long randN;


long current_state = 1; 
unsigned long side = 1;
unsigned long ticket = 1;

char tempMsg[] = "";

boolean nosepoke_voltage (int pin1){

  float sensorV1 = analogRead(pin1); //read voltage value from analog pin A0 (A0 for left nosepoke)

  if (sensorV1 * (5.0 / 1023.0) < 1) {
    return true;
    }
  else if (sensorV1 * (5.0 / 1023.0) > 4) {
    return false;
    }
}

void timestamp(char msg[]) {
  Serial.println(msg);
  unsigned long CurrentTime = millis();
  unsigned long ElapsedTime = CurrentTime - StartTime;
  unsigned long allSeconds= ElapsedTime/1000;
  int runHours= allSeconds/3600;
  int secsRemaining=allSeconds%3600;
  int runMinutes=secsRemaining/60;
  int runSeconds=secsRemaining%60;
  int runMilisec = ElapsedTime - runHours*60*60*1000 - runMinutes*60*1000 - runSeconds*1000;
  char buf[21];
  sprintf(buf,"Timestamp: %02d:%02d:%02d:%02d",runHours,runMinutes,runSeconds,runMilisec);
  Serial.println(buf);
} 

#include "Wire.h"
#include "RTClib.h"
RTC_PCF8523 RTC;
#include <SPI.h>
#include <SD.h>

// change this to match your SD shield or module;
// Adafruit SD shields and modules: pin 10
const int chipSelect = 10;

// the logging file
File logfile;

String year, month, day, hour, minute, second, date, time;
void logData(char msg[]) {
 Serial.println(msg);
 DateTime datetime = RTC.now();
 year = String(datetime.year(), DEC);
 month = String(datetime.month(), DEC);
 day = String(datetime.day(), DEC);
 hour = String(datetime.hour(), DEC);
 minute = String(datetime.minute(), DEC);
 second = String(datetime.second(), DEC);

 unsigned long CurrentTime = millis();
 unsigned long ElapsedTime = CurrentTime - StartTime;
 unsigned long allSeconds= ElapsedTime/1000;
 int runHours= allSeconds/3600;
 int secsRemaining=allSeconds%3600;
 int runMinutes=secsRemaining/60;
 int runSeconds=secsRemaining%60;
 int runMilisec = ElapsedTime - runHours*60*60*1000 - runMinutes*60*1000 - runSeconds*1000;
 char buf[21];
 sprintf(buf,"%02d:%02d:%02d:%02d",runHours,runMinutes,runSeconds,runMilisec);
 
 Serial.println(buf);
 logfile.print('"');
 logfile.print(year);
 logfile.print("/");
 logfile.print(month);
 logfile.print("/");
 logfile.print(day);
 logfile.print(" ");
 logfile.print(hour);
 logfile.print(":");
 logfile.print(minute);
 logfile.print(":");
 logfile.print(second);
 logfile.print('"');
 logfile.print(", ");
 logfile.print(msg);
 logfile.print(", ");
 logfile.print(buf);
 logfile.println();
 logfile.flush();
}
/* ******************
 * FORCED block1
 *********************/

void forced_left(unsigned long trial_n, unsigned long delay_t) {
unsigned long temp_trial = 1;

timer_2 = 0;
timer_3 = 0;

current_state = 1;
trial_count = 0;
while (temp_trial <= trial_n) {
if (temp_trial > trial_count) {
   digitalWrite(Relay_Light_A, HIGH);
   digitalWrite(Relay_Light_B1, HIGH);
   digitalWrite(Relay_Light_B2, HIGH);
   digitalWrite(Relay_Light_C, HIGH);
   digitalWrite(Relay_Light_D1, HIGH);
   digitalWrite(Relay_Light_D2, HIGH);
   digitalWrite(Relay_solenoid, HIGH);
   nosepoke_D_voltage == true;
   
   timer_1 = millis();
   timer_trial = millis();
   timer_2 = 0;
   timer_3 = 0;
   Lock_A = false;
   Lock_C = false;
   Lock_B1 = false;
   Lock_B2 = false;
   Lock_D1 = false;
   Lock_D2 = false;
   trial_count = temp_trial;
   current_state = 1;
   logData(msg1);
   
}
// detect nosepoke_C voltage
   int PIN_1 = A2;
   nosepoke_C_voltage = nosepoke_voltage (PIN_1);

// detect nosepoke_B voltage
   int PIN_3 = A0;
   nosepoke_B_voltage = nosepoke_voltage (PIN_3);

   int PIN_4 = A1;
   nosepoke_D_voltage = nosepoke_voltage (PIN_4);

// printout nosepoke_B in/out  
   if (nosepoke_B_voltage == false ) {
      if (Nosepoke_In_B == false) {
        logData(msg8);
      }
      Nosepoke_In_B = true;

    }
    else if (nosepoke_B_voltage == true) {
        if (Nosepoke_In_B == true) {
        logData(msg9);
      }
      Nosepoke_In_B = false;
      }

   if (nosepoke_C_voltage == false ) {
      if (Nosepoke_In_C == false) {
        logData(msg10);
      }
      Nosepoke_In_C = true;

    }
    else if (nosepoke_C_voltage == true) {
        if (Nosepoke_In_C == true) {
        logData(msg11);
      }
      Nosepoke_In_C = false;
      }
//printout nosepoke_D in/out  

   if (nosepoke_D_voltage == false ) {
      if (Nosepoke_In_D == false) {
        logData(msg13);
      }
      Nosepoke_In_D = true;

    }
    else if (nosepoke_D_voltage == true) {
        if (Nosepoke_In_D == true) {
        logData(msg14);
      }
      Nosepoke_In_D = false;
      }

if (current_state ==1 && millis() - timer_1 < 10000) { //less than 10 sec
if (nosepoke_C_voltage == false){ //c triggered
    logData(msg15);
    current_state = 2;
    timer_2 = millis();
   
  }
}
else if (current_state == 1 && millis() - timer_1 >= 10000) {
  logData(msg16);
  logData(msg18);
  current_state = 3; // end state
  
}
if (current_state ==2 && millis() - timer_2 > 10000) {

  logData(msg17);
  logData(msg18);
  current_state = 3;
  }
else if (current_state ==2 && millis() - timer_2 <= 10000) {
  if (nosepoke_D_voltage == false ) { // b triggered
      logData(msg29);
      digitalWrite(Relay_solenoid, LOW);
      delay (100);
      digitalWrite(Relay_solenoid, HIGH);
      current_state = 5;
      timer_3 = millis();
    }
  }

if (current_state == 5 && millis() - timer_3 > 10000) { 
  logData(msg18);

  current_state = 3;
  }
// states' actions
   if (current_state == 1) {
      Lock_A = false; // turn on A
      Lock_C = false;
      Lock_B1 = true; // turn off B1
      Lock_B2 = true;
      Lock_D1 = true;
      Lock_D2 = true;     
    }

   if (current_state == 2) {
        Lock_A = false;
        Lock_C = true;
        Lock_B1 = true;
        Lock_B2 = true;
        Lock_D1 = false;
        Lock_D2 = false;         
    }

if (current_state == 3) {
      Lock_A = true;
      Lock_C = true;
      Lock_B1 = true;
      Lock_B2 = true;
      Lock_D1 = true;
      Lock_D2 = true;
      if (millis() - timer_trial > 55000) {
        temp_trial += 1;
      }
      else{
        }     
    }

if (current_state == 5) { //wait for 10 sec or until C is poked
    Lock_A = false;
    Lock_C = false;
    Lock_B1 = true;
    Lock_B2 = true;
    Lock_D1 = true;
    Lock_D2 = true;
  }
// --------> decide lights on or off
if (Lock_A == false){
   digitalWrite(Relay_Light_A, LOW);
   //Lock_A = true;
  }
  else {
    digitalWrite(Relay_Light_A, LOW);
    }

if (Lock_C == false){
   digitalWrite(Relay_Light_C, LOW);
   //Lock_C = true;
  }
  else {
    digitalWrite(Relay_Light_C, HIGH);
    }

if (Lock_B1 == false){
   digitalWrite(Relay_Light_B1, LOW);
   //Lock_B1 = true;
  }
  else {
    digitalWrite(Relay_Light_B1, HIGH);
    }

if (Lock_B2 == false){
   digitalWrite(Relay_Light_B2, LOW);
 //  Lock_B2 = true;
  }
  else {
    digitalWrite(Relay_Light_B2, HIGH);
    }

if (Lock_D1 == false){
   digitalWrite(Relay_Light_D1, LOW);
   //Lock_D1 = true;
  }
  else {
    digitalWrite(Relay_Light_D1, HIGH);
    }

if (Lock_D2 == false){
   digitalWrite(Relay_Light_D2, LOW);
   //Lock_D2 = true;
  }
  else {
    digitalWrite(Relay_Light_D2, HIGH);
    }

delay(1);
} // end while
}

/* ******************
 * FORCED block2
 *********************/
void forced_right(unsigned long trial_n, unsigned long delay_t) {
Serial.println(delay_t);
unsigned long temp_trial = 1;

timer_2 = 0;
timer_3 = 0;

current_state = 1;

trial_count = 0;
while (temp_trial <= trial_n) {

if (temp_trial > trial_count) {
   digitalWrite(Relay_Light_A, HIGH);
   digitalWrite(Relay_Light_B1, HIGH);
   digitalWrite(Relay_Light_B2, HIGH);
   digitalWrite(Relay_Light_C, HIGH);
   digitalWrite(Relay_Light_D1, HIGH);
   digitalWrite(Relay_Light_D2, HIGH);
   digitalWrite(Relay_solenoid, HIGH);
   timer_1 = millis();
   timer_trial = millis();
   timer_2 = 0;
   timer_3 = 0;
   timer_4 = 0;
   Lock_A = false;
   Lock_C = false;
   Lock_B1 = false;
   Lock_B2 = false;
   Lock_D1 = false;
   Lock_D2 = false;
  
   trial_count = temp_trial;
   current_state = 1;
   logData(msg1);
}
// detect nosepoke_C voltage
   int PIN_1 = A2;
   nosepoke_C_voltage = nosepoke_voltage (PIN_1);

// detect nosepoke_B voltage
   int PIN_3 = A0;
   nosepoke_B_voltage = nosepoke_voltage (PIN_3);

   int PIN_4 = A1;
   boolean nosepoke_D_voltage = nosepoke_voltage (PIN_4);

// printout nosepoke_B in/out  
   if (nosepoke_B_voltage == false ) {
      if (Nosepoke_In_B == false) {
        logData(msg8);
      }
      Nosepoke_In_B = true;

    }
    else if (nosepoke_B_voltage == true) {
        if (Nosepoke_In_B == true) {
        logData(msg9);
      }
      Nosepoke_In_B = false;
      }

// printout nosepoke_C in/out  
   if (nosepoke_C_voltage == false ) {
      if (Nosepoke_In_C == false) {
        logData(msg10);
        
      }
      Nosepoke_In_C = true;

    }
    else if (nosepoke_C_voltage == true) {
        if (Nosepoke_In_C == true) {
        logData(msg11);
      }
      Nosepoke_In_C = false;
      }

//printout nosepoke_D in/out  

   if (nosepoke_D_voltage == false ) {
      if (Nosepoke_In_D == false) {
        logData(msg13);
      }
      Nosepoke_In_D = true;

    }
    else if (nosepoke_D_voltage == true) {
        if (Nosepoke_In_D == true) {
        logData(msg14);
      }
      Nosepoke_In_D = false;
      }

if (current_state ==1 && millis() - timer_1 < 10000) { //less than 10 sec
if (nosepoke_C_voltage == false){ //c triggered
    logData(msg15);
    current_state = 2;
    timer_2 = millis();
   
  }
}
else if (current_state == 1 && millis() - timer_1 >= 10000) {

  logData(msg16);
  logData(msg18);
  current_state = 3; // end state
}
if (current_state ==2 && millis() - timer_2 > 10000) {

  logData(msg17);
  logData(msg18);
  current_state = 3;
  }
else if (current_state == 2 && millis() - timer_2 <= 10000) {
  if (nosepoke_B_voltage == false ) { // b triggered
      current_state = 4;
      Serial.println(delay_t);
      timer_4 = millis();
    }
  }

if (current_state == 4 && millis() - timer_4 > delay_t) {//delay time discounting
    current_state = 5;
      logData(msg31);
      digitalWrite(Relay_solenoid, LOW);
      delay (100);
      digitalWrite(Relay_solenoid, HIGH);
      delay (250);
      digitalWrite(Relay_solenoid, LOW);
      delay (100);
      digitalWrite(Relay_solenoid, HIGH);
      delay (250);
      digitalWrite(Relay_solenoid, LOW);
      delay (100);
      digitalWrite(Relay_solenoid, HIGH); 
  //current_state = 5;
  timer_3 = millis();
  }

if (current_state == 5 && millis() - timer_3 > 10000) { 
  logData(msg18);
  current_state = 3;
  }

// states' actions
   if (current_state == 1) {
      Lock_A = false; // turn on A
      Lock_C = false;
      Lock_B1 = true; // turn off B1
      Lock_B2 = true;
      Lock_D1 = true;
      Lock_D2 = true;     
    }
   if (current_state == 2) {
        Lock_A = false;
        Lock_C = true;
        Lock_B1 = false;
        Lock_B2 = false;
        Lock_D1 = true;
        Lock_D2 = true;         
    }

if (current_state == 3) {
      Lock_A = true;
      Lock_C = true;
      Lock_B1 = true;
      Lock_B2 = true;
      Lock_D1 = true;
      Lock_D2 = true;
      if (millis() - timer_trial > 55000) {
        temp_trial += 1;
      }
      else{
        }      
    }

if (current_state == 4) { //wait for 10 sec or until C is poked
    Lock_A = false;
    Lock_C = true;
    Lock_B1 = true;
    Lock_B2 = true;
    Lock_D1 = true;
    Lock_D2 = true;  
  }
  
if (current_state == 5) { //wait for 10 sec or until C is poked
    Lock_A = false;
    Lock_C = false;
    Lock_B1 = true;
    Lock_B2 = true;
    Lock_D1 = true;
    Lock_D2 = true;
  }

// --------> decide lights on or off
if (Lock_A == false){
   digitalWrite(Relay_Light_A, LOW);
   //Lock_A = true;
  }
  else {
    digitalWrite(Relay_Light_A, LOW);
    }

if (Lock_C == false){
   digitalWrite(Relay_Light_C, LOW);
   //Lock_C = true;
  }
  else {
    digitalWrite(Relay_Light_C, HIGH);
    }

if (Lock_B1 == false){
   digitalWrite(Relay_Light_B1, LOW);
   //Lock_B1 = true;
  }
  else {
    digitalWrite(Relay_Light_B1, HIGH);
    }

if (Lock_B2 == false){
   digitalWrite(Relay_Light_B2, LOW);
 //  Lock_B2 = true;
  }
  else {
    digitalWrite(Relay_Light_B2, HIGH);
    }
if (Lock_D1 == false){
   digitalWrite(Relay_Light_D1, LOW);
   //Lock_D1 = true;
  }
  else {
    digitalWrite(Relay_Light_D1, HIGH);
    }

if (Lock_D2 == false){
   digitalWrite(Relay_Light_D2, LOW);
   //Lock_D2 = true;
  }
  else {
    digitalWrite(Relay_Light_D2, HIGH);
    }
delay(1);
} // end while
}

/* ******************
 * free choice 
 *********************/

void free_choice (unsigned long trial_n, unsigned long delay_t) {
Serial.println(delay_t);

unsigned long temp_trial = 1;

timer_2 = 0;
timer_3 = 0;

current_state = 1;

trial_count = 0;
while (temp_trial <= trial_n) {

if (temp_trial > trial_count) {
   digitalWrite(Relay_Light_A, HIGH);
   digitalWrite(Relay_Light_B1, HIGH);
   digitalWrite(Relay_Light_B2, HIGH);
   digitalWrite(Relay_Light_C, HIGH);
   digitalWrite(Relay_Light_D1, HIGH);
   digitalWrite(Relay_Light_D2, HIGH);
   digitalWrite(Relay_solenoid, HIGH);
   timer_1 = millis();
   timer_trial = millis();
   timer_2 = 0;
   timer_3 = 0;
   timer_4 = 0;
   Lock_A = false;
   Lock_C = false;
   Lock_B1 = false;
   Lock_B2 = false;
   Lock_D1 = false;
   Lock_D2 = false;
  
   trial_count = temp_trial;
   current_state = 1;
   logData(msg1);
}

// detect nosepoke_C voltage
   int PIN_1 = A2;
   nosepoke_C_voltage = nosepoke_voltage (PIN_1);

// detect nosepoke_B voltage
   int PIN_3 = A0;
   nosepoke_B_voltage = nosepoke_voltage (PIN_3);

   int PIN_4 = A1;
   boolean nosepoke_D_voltage = nosepoke_voltage (PIN_4);

// printout nosepoke_B in/out  
   if (nosepoke_B_voltage == false ) {
      if (Nosepoke_In_B == false) {
        logData(msg8);
      }
      Nosepoke_In_B = true;

    }
    else if (nosepoke_B_voltage == true) {
        if (Nosepoke_In_B == true) {
        logData(msg9);
      }
      Nosepoke_In_B = false;
      }

// printout nosepoke_C in/out  
   if (nosepoke_C_voltage == false ) {
      if (Nosepoke_In_C == false) {
        logData(msg10);
        
      }
      Nosepoke_In_C = true;

    }
    else if (nosepoke_C_voltage == true) {
        if (Nosepoke_In_C == true) {
        logData(msg11);
      }
      Nosepoke_In_C = false;
      }

//printout nosepoke_D in/out  
   if (nosepoke_D_voltage == false ) {
      if (Nosepoke_In_D == false) {
        logData(msg13);
      }
      Nosepoke_In_D = true;

    }
    else if (nosepoke_D_voltage == true) {
        if (Nosepoke_In_D == true) {
        logData(msg14);
      }
      Nosepoke_In_D = false;
      }

if (current_state ==1 && millis() - timer_1 < 10000) { //less than 10 sec
if (nosepoke_C_voltage == false){ //c triggered
    logData(msg15);
    current_state = 2;
    timer_2 = millis();
  }
}
else if (current_state == 1 && millis() - timer_1 >= 10000) {

  logData(msg16);
  logData(msg18);
  current_state = 3; // end state
}

if (current_state ==2 && millis() - timer_2 > 10000) {

  logData(msg17);
  logData(msg18);

  current_state = 3;
  }
else if (current_state ==2 && millis() - timer_2 <= 10000) {
  if (nosepoke_D_voltage == false ) { // d triggered

      logData(msg29);
      digitalWrite(Relay_solenoid, LOW);
      delay (100);
      digitalWrite(Relay_solenoid, HIGH); 
      current_state = 5;
      timer_3 = millis();
    }
else if (nosepoke_B_voltage == false ) { // b triggered
      current_state = 4;
      timer_4 = millis();
      Serial.println(delay_t);
    }
  }
if (current_state == 4 && millis() - timer_4 > delay_t) {//delay time discounting 

      current_state = 5;
          logData(msg31);
          digitalWrite(Relay_solenoid, LOW);
          delay (100);
          digitalWrite(Relay_solenoid, HIGH); 
          delay(250);
          digitalWrite(Relay_solenoid, LOW);
          delay (100);
          digitalWrite(Relay_solenoid, HIGH); 
          delay(250);
          digitalWrite(Relay_solenoid, LOW);
          delay (100);
          digitalWrite(Relay_solenoid, HIGH); 
          
      timer_3 = millis();
  }

if (current_state == 5 && millis() - timer_3 > 10000) { 
  logData(msg18);

  current_state = 3;
  }

// states' actions
   if (current_state == 1) {
      Lock_A = false; // turn on A
      Lock_C = false;
      Lock_B1 = true; // turn off B1
      Lock_B2 = true;
      Lock_D1 = true;
      Lock_D2 = true;     
    }
   if (current_state == 2) {
        Lock_A = false;
        Lock_C = true;
        Lock_B1 = false;
        Lock_B2 = false;
        Lock_D1 = false;
        Lock_D2 = false;         
    }
if (current_state == 3) {
      Lock_A = true;
      Lock_C = true;
      Lock_B1 = true;
      Lock_B2 = true;
      Lock_D1 = true;
      Lock_D2 = true;
      if (millis() - timer_trial > 55000) {
        temp_trial += 1;
      }
      else{
        }     
    }
if (current_state == 4) { //wait for 10 sec or until C is poked
    Lock_A = false;
    Lock_C = true;
    Lock_B1 = true;
    Lock_B2 = true;
    Lock_D1 = true;
    Lock_D2 = true;  
  }

if (current_state == 5) { //wait for 10 sec or until C is poked
    Lock_A = false;
    Lock_C = false;
    Lock_B1 = true;
    Lock_B2 = true;
    Lock_D1 = true;
    Lock_D2 = true;
  }

// --------> decide lights on or off
if (Lock_A == false){
   digitalWrite(Relay_Light_A, LOW);
   //Lock_A = true;
  }
  else {
    digitalWrite(Relay_Light_A, LOW);
    }

if (Lock_C == false){
   digitalWrite(Relay_Light_C, LOW);
   //Lock_C = true;
  }
  else {
    digitalWrite(Relay_Light_C, HIGH);
    }

if (Lock_B1 == false){
   digitalWrite(Relay_Light_B1, LOW);
   //Lock_B1 = true;
  }
  else {
    digitalWrite(Relay_Light_B1, HIGH);
    }

if (Lock_B2 == false){
   digitalWrite(Relay_Light_B2, LOW);
 //  Lock_B2 = true;
  }
  else {
    digitalWrite(Relay_Light_B2, HIGH);
    }

if (Lock_D1 == false){
   digitalWrite(Relay_Light_D1, LOW);
   //Lock_D1 = true;
  }
  else {
    digitalWrite(Relay_Light_D1, HIGH);
    }

if (Lock_D2 == false){
   digitalWrite(Relay_Light_D2, LOW);
   //Lock_D2 = true;
  }
  else {
    digitalWrite(Relay_Light_D2, HIGH);
    }

delay(1);
} // end while
}

/* ++++++++++
 * MAIN PROGRAM
 +++++++++++++ */ 
  void setup() {
  // put your setup code here, to run once:
  // put your setup code here, to run once:
  pinMode (Relay_Light_A, OUTPUT);
  pinMode (Relay_Light_B1, OUTPUT);
  pinMode (Relay_Light_B2, OUTPUT);
  pinMode (Relay_Light_C, OUTPUT);
  pinMode (Relay_Light_D1, OUTPUT);
  pinMode (Relay_Light_D2, OUTPUT);
  pinMode (Relay_solenoid, OUTPUT);
  
  StartTime = millis();
  Serial.begin(9600); 

  // initialize the SD card
  Serial.println(F("Initializing SD card..."));
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(chipSelect, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println(F("Card failed, or not present"));
  }
  Serial.println(F("card initialized."));
  
  // create a new file
  char filename[] = "DELAYD00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }

  if (! logfile) {
   char error("couldnt create file");
  }
  Serial.print(F("Logging to: "));
  Serial.println(filename);

  logfile.println("Date, Event Name, Event Time");  

  // connect to RTC
  Wire.begin();  
  if (!RTC.begin()) {
    logfile.println("RTC failed");
#if ECHO_TO_SERIAL
    Serial.println(F("RTC failed"));
#endif  //ECHO_TO_SERIAL
  } 
}

void loop() {
if (current_block < block_number) {
  Serial.println("new section");
  timer_5 = millis();
  while (millis() - timer_5 < 40000) {
    }
  current_block = block_number;
}

if (block_number == 1){

 logData(msg19);
  forced_right(5, 0);
 block_number = 2;
  }
else if (block_number == 2){ 

 logData(msg20);
  forced_left(5, 0);
  block_number = 3;
  }
else if (block_number == 3){

  logData(msg21);
  free_choice(20, 0); 
  block_number = 4;
 }
else if (block_number == 4){

  logData(msg22);
  forced_right(5, 3000);
  block_number = 5;
 }
else if (block_number == 5){

  logData(msg23);
  forced_left(5, 0);
  block_number = 6;
 }
else if (block_number == 6){

  logData(msg24);
  free_choice(20, 3000);
  block_number = 7;
 }

 else if (block_number == 7){

  logData(msg25);
  forced_right(5, 6000);
  block_number = 8;
 }
 else if (block_number == 8){

  logData(msg26);
  forced_left(5, 0);
  block_number = 9;
 }
else if (block_number == 9){

  logData(msg27);
  free_choice(20, 6000);
  block_number = 10;
}
else if (block_number == 10){

 logData(msg32);
  forced_right(5, 9000);
  block_number = 11;
 }
 else if (block_number == 11){

logData(msg33);
 forced_left(5, 0);
 block_number = 12;
 }
else if (block_number == 12){

  logData(msg34);
  free_choice(20, 9000);
  logData(msg30);
  digitalWrite(Relay_Light_A, HIGH);
  while (1){} 
}
}

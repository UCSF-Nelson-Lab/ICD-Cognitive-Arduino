//Configuration:
//1. 5v LED yellow ==> light_A (house light)
//2. 5v LED yellow (big) ==> light_B1 (left cue light)
//3. 3v LED yellow (small) ==> light_B2 (left nosepoke light)
//4. 3v LED yellow (small) ==> light_C (deliver light)
//5. 5v led yellow (big) ==> light_D1 (right cue light)
//6. 3v LED yellow (small) ==> light_D2 (right nosepoke light)

int Relay_Light_A = 4; //(relay 4)
int Relay_Light_B2 = 7;//(relay 1)
int Relay_Light_C = 9;//(relay 2) (DIFFERENT IN THIS SYSTEM)
int Relay_Light_B1 = 5;//(relay 5)
int Relay_Light_D1 = 6;//(relay 6)
int Relay_Light_D2 = 3;//(relay 3)
int Relay_solenoid = 8;//(relay 8) (different in this system)


boolean Nosepoke_In_B = false;
boolean Nosepoke_In_C = false;
boolean Nosepoke_In_D = false;

boolean Lock_A = false;
boolean Lock_C = false;
boolean Lock_B1 = false;
boolean Lock_B2 = false;
boolean Lock_D1 = false;
boolean Lock_D2 = false;
boolean Lock_solenoid = false;

boolean Lock_side = false;

boolean nosepoke_D_voltage;

char msg1[] = "trial start";
char msg2[] = "houselight on";
char msg3[] = "trial end, timeout";
char msg4[] = "left nosepoke";
char msg5[] = "left side assigned";
char msg6[] = "right nosepoke";
char msg7[] = "right side assigned";
char msg8[] = "nosepoke_B in";
char msg9[] = "nosepoke_B out";
char msg10[] = "nosepoke_C in";
char msg11[] = "nosepoke_C out";
char msg13[] = "nosepoke_D in";
char msg14[] = "nosepoke_D out";
char msg15[] = "center triggered";
char msg16[] = "center omission";
char msg17[] = "side omission";
char msg18[] = "session ends";


int trial_count = 1;
int current_trial = 0;

boolean New_TrialL = false;
unsigned long StartTime = 0;
unsigned long timer_1 = 0; // #1 10 sec timer
unsigned long timer_2 = 0; // #1 5 sec timer
unsigned long timer_3 = 0; // #2 10 sec timer
unsigned long timer_4 = 0;
unsigned long current_time = 0;
unsigned long elapsed_time = 0;
unsigned long timed_1 = 0;
unsigned long timed_2 = 0;

unsigned long randNumber;
unsigned long randN;

unsigned long current_state = 1; 
unsigned long side = 1;
unsigned long ticket = 1;

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
  int runMilisec = ElapsedTime - runHours*60*60*100 - runMinutes*60*1000 - runSeconds*1000;
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

void setup() {
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
  char filename[] = "CENTRI00.CSV";
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
  // put your main code here, to run repeatedly:
 if (current_trial < trial_count){
  //make relay off in the begining
   digitalWrite(Relay_Light_A, HIGH);
   digitalWrite(Relay_Light_B1, HIGH);
   digitalWrite(Relay_Light_B2, HIGH);
   digitalWrite(Relay_Light_C, HIGH);
   digitalWrite(Relay_Light_D1, HIGH);
   digitalWrite(Relay_Light_D2, HIGH);
   digitalWrite(Relay_solenoid, HIGH);
   timer_1 = millis();
   Lock_A = false;
   Lock_C = false;
   Lock_B1 = false;
   Lock_B2 = false;
   Lock_D1 = false;
   Lock_D2 = false;
   Lock_solenoid = false;

   Lock_side = false;
   current_trial = trial_count; // now start state 1
   current_state = 1;
   logData(msg1);

   if ( ticket != 1 and ticket != 2) { //when no ticket
     if (side == 1 ){ // when left
        side = 0; // switch to right
        ticket = random(0, 3);
        
      }
      else if ( side == 0) {
        side = 1;
        ticket = random(0, 3);
        }
   }
  }

// detect nosepoke_C voltage
   int PIN_1 = A2;
   boolean nosepoke_C_voltage = nosepoke_voltage (PIN_1);

   int PIN_3 = A0;
   boolean nosepoke_B_voltage = nosepoke_voltage (PIN_3);

   int PIN_4 = A1;
   boolean nosepoke_D_voltage = nosepoke_voltage (PIN_4);
   
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

// printout nosepoke_D in/out  
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


// -------> decide state number
if (current_state == 1 && millis() - timer_1 < 10000) { //less than 10 sec
  if (nosepoke_C_voltage == false){ //c is triggered
      logData(msg15);
      current_state = 2;
      timer_2 = millis();
    }
  }
  
 else if (current_state == 1 && millis() - timer_1 >= 10000) { // no trigger within 10s
    logData(msg16);
    current_state = 3;// end state
    logData(msg3);
  }

if (current_state ==2 && millis() - timer_2 > 10000) {// no trigger within 10s
    logData(msg17);
    current_state = 3;
    logData(msg3);
  }
  
else if (current_state ==2 && millis() - timer_2 <= 10000) {
  if (nosepoke_B_voltage == false && side == 1) { // b triggered
      logData(msg5);
      logData(msg4);
      digitalWrite(Relay_solenoid, LOW);
      delay (100);
      digitalWrite(Relay_solenoid, HIGH);
      delay (250);
      digitalWrite(Relay_solenoid, LOW);
      delay (100);
      digitalWrite(Relay_solenoid, HIGH);
      current_state = 4;
      timer_3 = millis();
    }
   else if (nosepoke_D_voltage == false && side == 0) { // d triggered

      logData(msg7);
      logData(msg6);
      digitalWrite(Relay_solenoid, LOW);
      delay (100);
      digitalWrite(Relay_solenoid, HIGH);
      delay (250);
      digitalWrite(Relay_solenoid, LOW);
      delay (100);
      digitalWrite(Relay_solenoid, HIGH);
      
      current_state = 5;
      timer_3 = millis();
    }
  }

if (current_state == 4 && millis() - timer_3 > 10000) { // state 4 ==> after B nosepoke under state 2
  current_state = 3;
  logData(msg3);
  }

if (current_state == 5 && millis() - timer_3 > 10000) {// state 5 ==> after C nosepoke under state 2
  current_state = 3;
  logData(msg3);
  }

  // states' actions
   if (current_state == 1) {
      Lock_A = false; // turn on A
      Lock_C = false;
      Lock_B1 = true; // turn off B1
      Lock_B2 = true;
      Lock_D1 = true;
      Lock_D2 = true;  
      //Lock_solenoid = true;  
    }

   if (current_state == 2) {
      if (side == 1 && Lock_side == false) {
        Lock_A = false;
        Lock_C = true;
        Lock_B1 = false;
        Lock_B2 = false;
        Lock_D1 = true;
        Lock_D2 = true;

        Lock_side = true;
        
        if (ticket == 2) {
          ticket = 1;
         }
        else if (ticket == 1) {
          ticket = 0;
         }
        }
        else if (side == 0 && Lock_side == false) {
        Lock_A = false;
        Lock_C = true;
        Lock_B1 = true;
        Lock_B2 = true;
        Lock_D1 = false;
        Lock_D2 = false;

        Lock_side = true;
        if (ticket == 2) {
          ticket = 1;
         }
         else if (ticket == 1) {
          ticket = 0;
         }
        }
    }

if (current_state == 3) {
      timer_4 = millis();
      Lock_A = true;
      Lock_C = true;
      Lock_B1 = true;
      Lock_B2 = true;
      Lock_D1 = true;
      Lock_D2 = true;  

      trial_count += 1;
               
    }

if (current_state == 4) {
    Lock_A = false;
    Lock_C = false;
    Lock_B1 = true;
    Lock_B2 = true;
    Lock_D1 = true;
    Lock_D2 = true;
    //Lock_side = true;
  }

if (current_state == 5) {
    Lock_A = false;
    Lock_C = false;
    Lock_B1 = true;
    Lock_B2 = true;
    Lock_D1 = true;
    Lock_D2 = true;

    Lock_side = true;
  }

// --------> decide lights on or off
if (Lock_A == false){
   digitalWrite(Relay_Light_A, LOW);
  }
  else {
    digitalWrite(Relay_Light_A, LOW);
    }

if (Lock_C == false){
   digitalWrite(Relay_Light_C, LOW);
  }
  else {
    digitalWrite(Relay_Light_C, HIGH);
    }

if (Lock_B1 == false){
   digitalWrite(Relay_Light_B1, LOW);
  }
  else {
    digitalWrite(Relay_Light_B1, HIGH);
    }

if (Lock_B2 == false){
   digitalWrite(Relay_Light_B2, LOW);
  }
  else {
    digitalWrite(Relay_Light_B2, HIGH);
    }

if (Lock_D1 == false){
   digitalWrite(Relay_Light_D1, LOW);
  }
  else {
    digitalWrite(Relay_Light_D1, HIGH);
    }

if (Lock_D2 == false){
   digitalWrite(Relay_Light_D2, LOW);
  }
  else {
    digitalWrite(Relay_Light_D2, HIGH);
    }

   current_time = millis();
   timed_1 = millis()-timer_1;
   
   delay(1); // 
// delay module
if (trial_count > current_trial) {

  randNumber = random(2, 4)*10000;
  while(millis() - timer_4 < randNumber) {
    // detect nosepoke_C voltage
   int PIN_1 = A2;
   boolean nosepoke_C_voltage = nosepoke_voltage (PIN_1);

   int PIN_3 = A0;
   boolean nosepoke_B_voltage = nosepoke_voltage (PIN_3);

   int PIN_4 = A1;
   boolean nosepoke_D_voltage = nosepoke_voltage (PIN_4);
    
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

// printout nosepoke_D in/out  
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
    }
if (millis()-StartTime > 5400000){
  Serial.println ("session end");
  logData(msg18);
  digitalWrite(Relay_Light_A, HIGH);
  digitalWrite(Relay_Light_B1, HIGH);
  digitalWrite(Relay_Light_B2, HIGH);
  digitalWrite(Relay_Light_C, HIGH);
  digitalWrite(Relay_Light_D1, HIGH);
  digitalWrite(Relay_Light_D2, HIGH);

  while (1){}
  }
}      
}

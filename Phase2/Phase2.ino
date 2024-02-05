//onfiguration:
//1. 5v LED yellow ==> light_A (house light)
//2. 5v LED yellow (big) ==> light_B1 (left cue light)
//3. 2v LED yellow (small) ==> light_B2 (left nosepoke light)
//4. 2v LED yellow (small) ==> light_C (deliver light)
//5. 2v led yellow (big) ==> light_D1 (right cue light)
//6. 2v LED yellow (small) ==> light_D2 (right nosepoke light)

// recording suggestion: record all device status each timestamp
int Relay_Light_A = 4; //(relay 4)
int Relay_Light_B1 = 5;//(relay 5)
int Relay_Light_B2 = 7; //(relay 1)
int Relay_Light_C = 9;//(relay 2)(change from pin10 to pin9)
int Relay_Light_D1 = 6; //(relay 6)
int Relay_Light_D2 = 3; //(relay 3)

boolean NOSEPOKE_IN_B = false;
boolean NOSEPOKE_IN_C = false;
boolean NOSEPOKE_IN_D = false;

unsigned long StartTime = 0;

boolean LIGHT_A_STATUS = false;
boolean LIGHT_B_STATUS = false;
boolean LIGHT_C_STATUS = false;
boolean LIGHT_D_STATUS = false;

boolean LOCK_A = false;
boolean LOCK_B = false;
boolean LOCK_C = false;
boolean LOCK_D = false;
boolean LOCK_BC = false;
boolean LOCK_DC = false;

unsigned long section = 1;


char msg1[] = "trial start";
char msg2[] = "houselight on";
char msg4[] = "light_B on";
char msg5[] = "light_B off";
char msg6[] = "light_C on";
char msg7[] = "light_C off";
char msg8[] = "nosepoke_B in";
char msg9[] = "nosepoke_B out";
char msg10[] = "nosepoke_C in";
char msg11[] = "nosepoke_C out";
char msg12[] = "section 1 end";
char msg13[] = "nosepoke_D in";
char msg14[] = "nosepoke_D out";
char msg15[] = "light_D on";
char msg16[] = "light_D off";
char msg17[] = "section 2 end";
char msg18[] = "trial end, timeout";
char msg19[] = "reward delivery";
char msg20[] = "omission";
char msg21[] = "session 2 start";

int trial_count = 1;
int current_trial = 0;

boolean NEW_TRIAL = false;

unsigned long timer_1 = 0;
unsigned long timer_2 = 0;
unsigned long timer_3 = 0;
unsigned long current_time = 0;
unsigned long elapsed_time = 0;
unsigned long timed_1 = 0;
unsigned long timed_2 = 0;

unsigned long randNumber;

unsigned long TrialTime = 0;

boolean nosepoke_D_voltage;
int Relay_solenoid =8;

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
  // Set display type as 16 char, 2 rows
  //lcd.begin(16,2);

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
  char filename[] = "NOSEPO00.CSV";
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
 if (section == 1){
  if (current_trial < trial_count ){
    digitalWrite(Relay_Light_B1, HIGH); // make relay off in the begining
    digitalWrite(Relay_Light_B2, HIGH);
    digitalWrite(Relay_Light_C, HIGH);
    digitalWrite(Relay_Light_D1, HIGH); // make relay off in the begining
    digitalWrite(Relay_Light_D2, HIGH);
    digitalWrite(Relay_solenoid, HIGH);
   
    timer_1 = 0;
    timer_2 = 0;
    timer_3 = 0;

    LOCK_D = false;
    LOCK_C = false;
    LOCK_DC = false;

    LIGHT_C_STATUS = false;
    current_trial = trial_count; // now start state 1
    LIGHT_A_STATUS = true;
    LIGHT_D_STATUS = true;

    logData(msg1);
    logData(msg15);
  }

// detect nosepoke_C voltage
   int PIN_1 = A2;
   boolean nosepoke_C_voltage = nosepoke_voltage (PIN_1);

   int PIN_3 = A0;
   boolean nosepoke_B_voltage = nosepoke_voltage (PIN_3);

   int PIN_4 = A1;
   boolean nosepoke_D_voltage = nosepoke_voltage (PIN_4);

// printout nosepoke_D in/out  
   if (nosepoke_D_voltage == false ) {
      if (NOSEPOKE_IN_D == false) {
        //timestamp(msg13);
        logData(msg13);
      }
      NOSEPOKE_IN_D = true;

    }
    else if (nosepoke_D_voltage == true) {
        if (NOSEPOKE_IN_D == true) {
        //timestamp(msg14);
        logData(msg14);
      }
      NOSEPOKE_IN_D = false;
      }
// printout nosepoke_B in/out
    if (nosepoke_B_voltage == false ) {
      if (NOSEPOKE_IN_B == false) {
         //timestamp(msg8);
         logData(msg8);
      }
      NOSEPOKE_IN_B = true;

    }
    else if (nosepoke_B_voltage == true) {
        if (NOSEPOKE_IN_B == true) {
           //timestamp(msg9);
           logData(msg9);
      }
      NOSEPOKE_IN_B = false;
      }

// printout nosepoke_C in/out  
   if (nosepoke_C_voltage == false  ) {
      if (NOSEPOKE_IN_C == false) {
        //timestamp(msg10);
        logData(msg10);
      }
      NOSEPOKE_IN_C = true;

    }
    else if (nosepoke_C_voltage == true) {
        if (NOSEPOKE_IN_C == true) {
        //timestamp(msg11);
        logData(msg11);
      }
      NOSEPOKE_IN_C = false;
      }

  */
  // decide if light A needs to be turned on
  if (LIGHT_A_STATUS == true ){
    digitalWrite(Relay_Light_A, LOW);
  }
  
// decide if light B needs to be turned on
  if (LIGHT_D_STATUS == true && LOCK_D == false){
    digitalWrite(Relay_Light_D1, LOW);
    digitalWrite(Relay_Light_D2, LOW);
    timer_1 = millis();
    timer_3 = millis();
    LOCK_D = true; // prevent coming into this loop again
  }
  
  current_time = millis();
  timed_1 = millis() - timer_1;
  
  if (timed_1 <= 20000) {   
    if (nosepoke_D_voltage == false) {//enter into state 2
       LIGHT_C_STATUS = true; // light c for 10 sec      
      if (LIGHT_C_STATUS == true && LOCK_C == false) {
        digitalWrite(Relay_Light_C, LOW);// turn on light c
        digitalWrite(Relay_solenoid, LOW);
        delay (100);
        digitalWrite(Relay_solenoid, HIGH );
        delay (250);
        digitalWrite(Relay_solenoid, LOW);
        delay (100);
        digitalWrite(Relay_solenoid, HIGH );
        //timestamp(msg19);
        logData(msg19);
        timer_2 = millis();
        digitalWrite(Relay_Light_D1, HIGH);// turn off light b
        digitalWrite(Relay_Light_D2, HIGH);
        //timestamp(msg5);
        logData(msg16);
        LIGHT_D_STATUS = false;
        LOCK_C = true;
        LOCK_DC = true; // prevent double turn-off from both B and C
        
        }
      }
  }
  if (timed_1 > 20000 && LOCK_DC == false) {
    //A B C off
    digitalWrite(Relay_Light_A, LOW);
    digitalWrite(Relay_Light_D1, HIGH); 
    digitalWrite(Relay_Light_D2, HIGH);
    digitalWrite(Relay_Light_C, HIGH);
    digitalWrite(Relay_solenoid, HIGH);

    logData(msg20);
    trial_count += 1;

    logData(msg18);
    randNumber = random(30000, 50000);

    delay(randNumber);
    }

  if (LIGHT_C_STATUS == true){
    timed_2 = millis() - timer_2;
    if (timed_2 > 10000) {
    // A B C off
    digitalWrite(Relay_Light_A, LOW);
    digitalWrite(Relay_Light_D1, HIGH);
    digitalWrite(Relay_Light_D2, HIGH);
    digitalWrite(Relay_Light_C, HIGH);
    trial_count += 1;

    logData(msg18);
    LIGHT_C_STATUS = false;
    randNumber = random(30000, 50000);
 
    delay(randNumber);
    }
  }
   if (millis()-StartTime > 3000000){
    digitalWrite(Relay_Light_A, HIGH);
    digitalWrite(Relay_Light_D1, HIGH);
    digitalWrite(Relay_Light_D2, HIGH);
    digitalWrite(Relay_Light_C, HIGH);

    logData(msg12);
    int trial_count = 1;
    int current_trial = 0;
    
    delay (120000);
    section = 2; 
    logData(msg21);
}
 }
// start of session #2   
if (section == 2){

 if (current_trial < trial_count ){
    
    digitalWrite(Relay_Light_A, HIGH);  
    digitalWrite(Relay_Light_B1, HIGH); // make relay off in the begining
    digitalWrite(Relay_Light_B2, HIGH);
    digitalWrite(Relay_Light_C, HIGH);
    digitalWrite(Relay_Light_D1, HIGH); 
    digitalWrite(Relay_Light_D2, HIGH);
   
    timer_1 = 0;
    timer_2 = 0;
    timer_3 = 0;

    LOCK_B = false;
    LOCK_C = false;
    LOCK_BC = false;
    LOCK_D = false;
    LOCK_DC = false;

    LIGHT_C_STATUS = false;
    current_trial = trial_count; // now start state 1
    LIGHT_A_STATUS = true;
    LIGHT_B_STATUS = true;
    
    logData(msg1);
    logData(msg4);
  }
// detect nosepoke_C voltage
   int PIN_1 = A2;
   boolean nosepoke_C_voltage = nosepoke_voltage (PIN_1);

   int PIN_3 = A0;
   boolean nosepoke_B_voltage = nosepoke_voltage (PIN_3);

   int PIN_4 = A1;
  boolean nosepoke_D_voltage = nosepoke_voltage (PIN_4);


// printout nosepoke_B in/out
    if (nosepoke_B_voltage == false) {
      if (NOSEPOKE_IN_B == false) {
         logData(msg8);
      }
      NOSEPOKE_IN_B = true;

    }
    else if (nosepoke_B_voltage == true) {
        if (NOSEPOKE_IN_B == true) {
           logData(msg9);
      }
      NOSEPOKE_IN_B = false;
      }

// printout nosepoke_C in/out  
   if (nosepoke_C_voltage == false ) {
      if (NOSEPOKE_IN_C == false) {
        logData(msg10);
      }
      NOSEPOKE_IN_C = true;

    }
    else if (nosepoke_C_voltage == true) {
        if (NOSEPOKE_IN_C == true) {
        logData(msg11);
      }
      NOSEPOKE_IN_C = false;
      }
      
  // printout nosepoke_D in/out  
   if (nosepoke_D_voltage == false ) {
      if (NOSEPOKE_IN_D == false) {
        logData(msg13);
      }
      NOSEPOKE_IN_D = true;

    }
    else if (nosepoke_D_voltage == true) {
        if (NOSEPOKE_IN_D == true) {
        logData(msg14);
      }
      NOSEPOKE_IN_D = false;
      }

 */
  // decide if light A needs to be turned on
  if (LIGHT_A_STATUS == true ){
    digitalWrite(Relay_Light_A, LOW);
  }
  
// decide if light B needs to be turned on
  if (LIGHT_B_STATUS == true && LOCK_B == false){
    digitalWrite(Relay_Light_B1, LOW);
    digitalWrite(Relay_Light_B2, LOW);
    timer_1 = millis();
    timer_3 = millis();
    LOCK_B = true; // prevent coming into this loop again
  }

  current_time = millis();
  timed_1 = millis() - timer_1;
  
  if (timed_1 <= 20000) {   
    if (nosepoke_B_voltage == false ) {//enter into state 2
       LIGHT_C_STATUS = true; // light c for 10 sec
      if (LIGHT_C_STATUS == true && LOCK_C == false) {
        digitalWrite(Relay_Light_C, LOW);// turn on light c  
        digitalWrite(Relay_solenoid, LOW); 
        delay (100); 
        digitalWrite(Relay_solenoid, HIGH);
        delay (250); 
        digitalWrite(Relay_solenoid, LOW); 
        delay (100); 
        digitalWrite(Relay_solenoid, HIGH);
        logData(msg19);
        timer_2 = millis();
        digitalWrite(Relay_Light_B1, HIGH);// turn off light d
        digitalWrite(Relay_Light_B2, HIGH);

        logData(msg5);
        LIGHT_B_STATUS = false;
        LOCK_C = true;
        LOCK_BC = true; // prevent double turn-off from both B and C
        }
      }
  }

  if (timed_1 > 20000 && LOCK_BC == false) {
    //A B C off
    digitalWrite(Relay_Light_A, LOW);
    digitalWrite(Relay_Light_B1, HIGH);
    digitalWrite(Relay_Light_B2, HIGH);
    digitalWrite(Relay_Light_C, HIGH);
    logData(msg20);
    trial_count += 1;
    logData(msg18);
    randNumber = random(30000, 50000);

    delay(randNumber);
    }

 if (LIGHT_C_STATUS == true){
    timed_2 = millis() - timer_2;
    if (timed_2 > 10000) {
    // A B C off
    digitalWrite(Relay_Light_A, LOW);
    digitalWrite(Relay_Light_B1, HIGH);
    digitalWrite(Relay_Light_B2, HIGH);
    digitalWrite(Relay_Light_C, HIGH);
    trial_count += 1;
    logData(msg18);
    LIGHT_C_STATUS = false;
    randNumber = random(30000, 50000);

    delay(randNumber);
    }
  }
    if (millis()-StartTime > 6120000){
    digitalWrite(Relay_Light_A, HIGH);
    digitalWrite(Relay_Light_B1, HIGH);
    digitalWrite(Relay_Light_B2, HIGH);
    digitalWrite(Relay_Light_C, HIGH);
    
    logData(msg17);
    while (1);   
    }

 delay(50);

}
}

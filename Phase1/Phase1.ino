//Configuration:
//1. 5v yellow LED ==> light_A: houselight
//2. 2v yellow LED (small) ==> light_C: delivery cue light
//3. center nosepoke 

// behavioral paradigm:
// state 1:C on for 10 s (reward delivery)
// state 2: C off
// ITI: 60 +/- 20 s
// center nosepoke detect all the time

int Relay_Light_A = 4; //(Relay 4)
int Relay_Light_C = 9;//(relay 2)

//int TTL_Output_2 = 24; //nosepoke C
//int TTL_Output_11 = 48;//ir leak sensor - lick on
//int TTL_Output_10 = 28;// reward delivery
int Relay_solenoid = 8; //( change in this system)

int PIN_1 = A2; //Detection: nosepoke C pin

int trial_count = 1;
int current_trial = 0;
int temp_trial = 0;

boolean Light_A_Status = false;
boolean Light_C_Status = false;

boolean Lock_A = false;
boolean Lock_C = false;
boolean Nosepoke_In_C = false;
boolean nosepoke_C_voltage;

// NOSEPOKE_C : A2

unsigned long current_time = 0;
unsigned long elapsed_time = 0;
unsigned long StartTime = 0;
unsigned long timer_1 = 0;
unsigned long timed_1 = 0;
unsigned long timer_2 = 0;

char msg1[] = "trial start";
char msg2[] = "houselight on";
char msg3[] = "hosuelight off";
char msg4[] = "reward delivery";
char msg10[] = "nosepoke_C in";
char msg11[] = "nosepoke_C out";
char msg12[] = "time out";
char msg13[] = "session ends";

unsigned long randNumber;


// Function: convert raw value to voltage value,  
// then decide if the voltage is below 1 or over 4
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
  Serial.println (buf);
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

// create a function called logData (void means no return value for this function)
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

//---------------------------------------------------------------------------------
// Function: to set up Arduino board
void setup() {
  pinMode (Relay_Light_A, OUTPUT);
  pinMode (Relay_Light_C, OUTPUT);
  pinMode (Relay_solenoid, OUTPUT);
  Serial.begin (9600);
  StartTime = millis();
  
  // initialize the SD card
  Serial.println(F("Initializing SD card..."));
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
  }
  Serial.println("card initialized.");
  
  // create a new file
  char filename[] = "MAGENT00.CSV";
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
  Serial.print("Logging to: ");
  Serial.println(filename);

  logfile.println("Date, Event Name, Event Time");  

  // connect to RTC
  Wire.begin();  
  if (!RTC.begin()) {
    logfile.println("RTC failed");
#if ECHO_TO_SERIAL
    Serial.println("RTC failed");
#endif  //ECHO_TO_SERIAL
  }
}

//----------------------------------------------------------------------
//Main Function that will be looping
void loop() {
  // put your main code here, to run repeatedly:
 if (current_trial <= 40){
    if (Lock_C == false){

        // reward delivery cue light
        digitalWrite(Relay_Light_A, LOW);
        digitalWrite(Relay_Light_C, LOW);

        digitalWrite(Relay_solenoid, LOW);
        delay (100);
        digitalWrite(Relay_solenoid, HIGH);
        delay (250);
        digitalWrite(Relay_solenoid, LOW);
        delay (100);
        digitalWrite(Relay_solenoid, HIGH);
        
        timer_1 = millis();        
        logData(msg1);
        logData(msg4);
        Lock_C = true;      
      }    
//--------------------------------------------------------------------------
    // detect nosepoke_C voltage
      nosepoke_C_voltage = nosepoke_voltage (PIN_1);

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
//-----------------------------------------------------------------------            
      if (millis()-timer_1 > 10000 ){
        digitalWrite(Relay_Light_A, LOW);
        digitalWrite(Relay_Light_C, HIGH);
        logData(msg3); 
        logData(msg12);
              
        timer_2 = millis();
        randNumber = random(4, 8)*10000;
        
        while (millis()-timer_2 < randNumber){
        
            // printout nosepoke_C in/out 
            nosepoke_C_voltage = nosepoke_voltage (PIN_1);
            if (nosepoke_C_voltage == false ) {
              if (Nosepoke_In_C == false) {
                logData(msg10);
                //digitalWrite(TTL_Output_2, HIGH);
                //delay (1);
                //digitalWrite(TTL_Output_2, LOW);
              }
              Nosepoke_In_C = true;
            }
            else if (nosepoke_C_voltage == true) {
                if (Nosepoke_In_C == true) {
                logData(msg11);
              }
              Nosepoke_In_C = false;
              }
        } 
 //---------------------------------------------------end of while loop         
        Lock_C = false;
        current_trial += 1;           
        }  
 }
        else if (current_trial >= 41){
        digitalWrite(Relay_Light_A, HIGH);
        logData(msg13);
        while (1);
 }
 
 delay(1);
}

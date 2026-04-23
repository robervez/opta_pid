#include <AdvancedPID.h>

#include "variables.h"
#include "utility.h"
#include "CLI.h"
#include "FSM.h"


void LettureSensori(){
  // lettura temperatura corrente ed aggiunta a somma
  int icurval = analogRead(PIN_INPUT_TEMP);
  iSommaTemp += icurval;  
  iNumTemp += 1;
}



void setupPID(){
  // configurazione PID
  myPID.setOutputLimits(0, 255);
  // Enable derivative filter (0.8 = strong smoothing, 1.0 = OFF)
  myPID.setDerivativeFilter(0.8);
  // Protect actuator: Output can change max 100 units per second
  myPID.setOutputRampRate(100.0);
}


void GestionePID(){
  
  // TODO: input e setpoint 

  // Optional: Read feed-forward (e.g. battery voltage compensation)
  //float feedForward = 10.0; 
  // Optional: Read external derivative (e.g. from gyroscope)
  //float gyroRate = 0.0; // Read from sensor...

  // Run PID with advanced features
  
  float output = myPID.run(lastPIDinput, iCurrentSetPoint);
  
  analogWrite(PIN_OUTPUTPID, output);
  
}



//-----------------------------------------------------------------

void setup() {
  // put your setup code here, to run once:
  Serial.begin(SERIALBAUD);
  iTimePID = 0;
  iTimeInput= 0;
  iTimeOutput = 0;

  bOutputEnable = false;

  // valori di default
  TCAPTURE = 10; // campionamento input ogni 10 ms
  TSEND = 1000; // output valori medi ogni secondo
  TPID = 10; // PID opera a 10 ms


  iCurrentState=0;

  Serial.println("#initializing pid...");

  setupPID();
  Serial.println("#done.");
  
  delay(100);

}


void loop() {
  // put your main code here, to run repeatedly:
  if (millis()-iTimePID > TPID)
  {
    iTimePID = millis();
    lastPIDinput = 1024- analogRead(PIN_INPUTPID);
    
    GestionePID();
  }

  if (millis()-iTimeInput > TCAPTURE)
  {
    iTimeInput = millis();
    LettureSensori();
  }

  if (bOutputEnable && (millis()-iTimeOutput > TSEND))
  {
    iTimeOutput = millis();
    InviaPacchettoDati();
  }

  runFSM();

  GestioneComandiSeriale();

  //DEBUG delay(10);

}

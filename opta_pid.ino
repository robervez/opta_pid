#include <AdvancedPID.h>

#include "variables.h"
#include "utility.h"
#include "CLI.h"



void LettureSensori(){
  // lettura temperatura corrente ed aggiunta a somma
  int icurval = analogRead(PIN_INPUT_TEMP);
  iSommaTemp += icurval;  
  iNumTemp += 1;
}


void GestioneOutputs(){
  // invio output con valori dei sensori
  Serial.print("A;");
  Serial.print("2;");  // numero di valori stampati
  Serial.print((float)iSommaTemp/iNumTemp);  // todo, splittare i due valori 
  Serial.print(";");  
  Serial.println(0);

  // resetto numero media valori input
    iSommaTemp =0;
    iNumTemp =0;
}



void setupPID(){
  // configurazione PID
  myPID.setOutputLimits(0, 255);
  // Enable derivative filter (0.8 = strong smoothing, 1.0 = OFF)
  myPID.setDerivativeFilter(0.8);
  // Protect actuator: Output can change max 100 units per second
  myPID.setOutputRampRate(100.0);
}

bool IsStable(){
  
  int cursetpoint = vSetpoints[iIndexSetPoint];
  return (abs(cursetpoint-lastPIDinput)<THRES); 
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


/*
stati:
0 - idle
1 - inizio FSM
2 - setta set point
3 - attendi stabilità
4 - stabilità
5 - cattura 
6 - invio dati step corrente [poi vai a 2 o 7,]
7 - invio dati finali [poi vai a 0]
*/
long int lastchange;

void runFSM(){
  int iFutureState=iCurrentState; // default
  // se non attivata, esci

  if (iCurrentState==1) iFutureState=2;

  if (iCurrentState==2) {
    // cambio set point
    iCurrentSetPoint = vSetpoints[iIndexSetPoint];
    iFutureState=3;
  }

  if (iCurrentState==3) {
    // controllo stabilità
    if (IsStable())  iFutureState=4; // vado a stabile
  }

  if (iCurrentState==4) {
    // controllo fine tempo stabilita
    if (millis() - lastchange > THOLD)  iFutureState=5;
    if (! IsStable()) iFutureState = 3;  // torno a instabile
  }


  if (iCurrentState==5) {
    // acquisizione
    if (millis() - lastchange > TCAPTURE)  iFutureState=6;
    
  }


  if (iCurrentState==6) {
      Serial.println("End step");
      iIndexSetPoint++;
      if (iIndexSetPoint>=iNumSetpoints)  // fine 
          iFutureState=7;
      else
          iFutureState=2;   // prossimo elemento
    }

  if (iCurrentState==7) {
    Serial.println("End all");
    iIndexSetPoint=0;
    iFutureState=0;
    }

  // entry actions
  if (iCurrentState==4 && iFutureState==5)    bOutputEnable=true;  // inizia cattura
  if (iCurrentState==5 && iFutureState==6)    bOutputEnable=false;  // fine cattura


  // transizione di stato
  if (iFutureState !=iCurrentState )
  { 
     lastchange=millis();
     iCurrentState=iFutureState;
  }

}


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

  setupPID();
  
  delay(100);

}


void loop() {
  // put your main code here, to run repeatedly:
  if (millis()-iTimePID > TPID)
  {
    iTimePID = millis();
    lastPIDinput = analogRead(PIN_INPUTPID);
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
    GestioneOutputs();
  }

  runFSM();

  GestioneComandiSeriale();

  //DEBUG delay(10);

}

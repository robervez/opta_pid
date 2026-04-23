
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


// check stabilità del PID, per passare da stato 3 a stato 4, ovvero per iniziare a campionare i dati
bool IsStable(){
  
  int cursetpoint = vSetpoints[iIndexSetPoint];
  return (abs(cursetpoint-lastPIDinput)<THRES); 
}


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


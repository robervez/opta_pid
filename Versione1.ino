#include <AdvancedPID.h>

/* variabili per gli input e gli output
*/


int iFreqPID;  // millis tra ogni lettura PID
int iFreqInput;  // millis tra ogni lettura valori input per medie
int iFreqOutput;  // millis tra ogni  output media
int iNumValoriInput; // numero valori input prima di media


// timers per ultima operazione
static long int iTimePID;
static long int iTimeOutput;
static long int iTimeInput;

/*
Variabili temporanee
*/

long int iSommaTemp;  
int iNumTemp;

bool bOutputEnable;

int vSetpoints[30];
int iNumSetpoints;
int iIndexSetPoint;


// Kp, Ki, Kd, Kb (Back-calculation coefficient)
AdvancedPID myPID(2.0, 5.0, 1.0, 0.0); 



#define SERIALBAUD 460800
static char buffer [100]="";
static int iCnt=0;

int StringSplit(String &strIn, String *strsOut, char separator){
  
  int StringCount = 0;
  // Split the string into substrings
  while (strIn.length() > 0)
  {
    int index = strIn.indexOf(separator);
    if (index == -1) // No space found
    {
      strsOut[StringCount++] = strIn;
      break;
    }
    else
    {
      strsOut[StringCount++] = strIn.substring(0, index);
      strIn = strIn.substring(index+1);
    }
  }
  return StringCount;
}



void IniziaCattura(){
  bOutputEnable=true;
}

void FineCattura(){
  bOutputEnable=false;
}

void ImpostaCicloCompleto(String &tmp){
  static String strs[50];
  int nparam = StringSplit(tmp, strs, ';');
  //DEBUG Serial.println(nparam);
  if (nparam > 30) {
    iNumSetpoints=0;
    Serial.println("KO");
    return;
    }

  iNumSetpoints = nparam-1; // tranne il primo 
  for (int i =0; i<iNumSetpoints; i++)
    vSetpoints[i]=strs[i+1].toInt();    //TODO: se non int, cambiare
  Serial.println("OK");
}

void ImpostaParametro(String &tmp){
 
  static String strs[10];
  int nparam = StringSplit(tmp, strs, ';');
  //DEBUG Serial.println(nparam);
  if (nparam != 3) return;
  if (strs[1].startsWith("FREQPID")) iFreqPID = strs[2].toInt();
  Serial.println("OK");

    
}


void CallbackSerial (){
  // stringa errata
  if (strlen(buffer)<2) return;

  // TODO: inserire checksum controllo
  String tmp(buffer);

  if (tmp.startsWith("START")) IniziaCattura();
  if (tmp.startsWith("STOP")) FineCattura();
  if (tmp.startsWith("SETCYCLE")) ImpostaCicloCompleto(tmp);
  if (tmp.startsWith("SETPARAM")) ImpostaParametro(tmp);

}


void GestioneComandiSeriale(){
  while (Serial.available())
  {
    buffer[iCnt]=Serial.read();
    if (buffer[iCnt]=='\n')
    {
      // callback
      CallbackSerial();
      // vuoto buffer
      iCnt=0;
    }
    else
      {
        iCnt++;
      }
  }
}


void GestioneInputs(){
;
}

void GestioneOutputs(){
  // invio output con valori dei sensori
  Serial.print("A;");
  Serial.print("2;");  // numero di valori stampati
  Serial.print((float)iSommaTemp/iNumTemp);  // todo, splittare i due valori 
  Serial.print(";");  
  Serial.println(0);
}


void GestionePID(){
  // TODO: input e setpoint 
  
  // Optional: Read feed-forward (e.g. battery voltage compensation)
  //float feedForward = 10.0; 
  // Optional: Read external derivative (e.g. from gyroscope)
  //float gyroRate = 0.0; // Read from sensor...

  // Run PID with advanced features
  
  float output = 3; // TODO:  myPID.run(input, setpoint);
  
  //analogWrite(3, output);
  
}


/*
stati:
0 - idle
1 - inizio FSM
2 - setta set point
3 - attendi stabilità
4 - cattura 
5 - invio dati step corrente [poi vai a 2 o 6,]
6 - invio dati finali [poi vai a 0]
*/
int iCurrentState=0;
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
    if (1==1)  // TODO: condizione di stabilità
    iFutureState=4;
  }

  if (iCurrentState==4) {
    // controllo fine tempo
    if (millis() - lastchange > TIMEOUT)
      iFutureState=5;
  }

  if (iCurrentState==5) {
      Serial.println("End step");
      iIndexSetPoint++;
      if (iIndexSetPoint>=iNumSetpoints)
          iFutureState=6;
      else
          iFutureState=2;
    }

  if (iCurrentState==6) {
    Serial.println("End all");
    iIndexSetPoint=0;
    iFutureState=0;
    }


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
  iTimePID = millis();
  bOutputEnable = false;

  // valori di default
  iFreqInput = 10; // campionamento input ogni 10 ms
  iFreqOutput = 1000; // output valori medi ogni secondo
  iFreqPID = 10; // PID opera a 10 ms




  // configurazione PID
  myPID.setOutputLimits(0, 255);
  // Enable derivative filter (0.8 = strong smoothing, 1.0 = OFF)
  myPID.setDerivativeFilter(0.8);
  // Protect actuator: Output can change max 100 units per second
  myPID.setOutputRampRate(100.0);

  delay(100);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis()-iTimePID > iFreqPID)
  {
    iTimePID = millis();
    GestionePID();
  }

  if (millis()-iTimeInput > iFreqInput)
  {
    iTimeInput = millis();
    GestioneInputs();
  }

  if (bOutputEnable && (millis()-iTimeOutput > iFreqOutput))
  {
    iTimeOutput = millis();
    GestioneOutputs();
  }

  runFSM();

  GestioneComandiSeriale();

  //DEBUG delay(10);

}

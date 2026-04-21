

static char bufferCLI [100]="";
static int iCnt=0;



int InizioCiclo(){
  // Solo se ciclo è spento
  if (iCurrentState)  return 0;
  // porto FSM su primo stato
  iCurrentState=1;
  
  return 1;

}

int FineCiclo(){
  // se ciclo attivo lo porto all'ultimo stato
  if (iCurrentState)
    iCurrentState=7;
  return 1;
  
}

int ImpostaCicloCompleto(String &tmp){
  static String strs[50];
  int nparam = StringSplit(tmp, strs, ';');
  //DEBUG Serial.println(nparam);
  if (nparam > 30) {
    iNumSetpoints=0;
    return 0;
    }

  iNumSetpoints = nparam-1; // tranne il primo 
  for (int i =0; i<iNumSetpoints; i++)
    vSetpoints[i]=strs[i+1].toInt();    //TODO: se non int, cambiare
  return 1;
}


/*
int TPID;  // millis tra ogni lettura PID
int TCAPTURE;  // millis tra ogni lettura valori input per medie
int TSEND;  // millis tra ogni  output media
int THOLD; // millis di stabilità richiesta 
int THRES; // errore stabilità input PID TODO
*/

int ImpostaParametro(String &tmp){
 
  static String strs[10];
  int nparam = StringSplit(tmp, strs, ';');
  //DEBUG Serial.println(nparam);
  if (nparam != 3) return 0;
  if (strs[1].equals("TPID"))     {TPID = strs[2].toInt(); return 1;}
  if (strs[1].equals("TCAPTURE")) {TCAPTURE = strs[2].toInt(); return 1;}
  if (strs[1].equals("TSEND"))    {TSEND = strs[2].toInt(); return 1;}
  if (strs[1].equals("THOLD"))    {THOLD = strs[2].toInt(); return 1;}
  if (strs[1].equals("THRES"))    {THRES = strs[2].toInt(); return 1;}

  return 0;

    
}


int ImpostaSetPoint(String &tmp){
 
  static String strs[10];
  int nparam = StringSplit(tmp, strs, ';');
  //DEBUG Serial.println(nparam);
  if (nparam != 2) return 0;
   iCurrentSetPoint = strs[1].toInt(); 
   return 1;

   
}

int StampaValori(String &tmp){
 
  static String strs[10];
  int nparam = StringSplit(tmp, strs, ';');
  //DEBUG Serial.println(nparam);
  if (nparam != 2) return 0;
  if (strs[1].startsWith("SETPOINT"))     {Serial.println(iCurrentSetPoint); return 1;}
  if (strs[1].startsWith("PIDINPUT"))     {Serial.println(lastPIDinput); return 1;}
   return 0;

   
}



int CallbackSerial (){
  

  // stringa errata
  if (strlen(bufferCLI)<2) return 0;

  // TODO: inserire checksum controllo
  String tmp(bufferCLI);

  if (tmp.startsWith("START")) return InizioCiclo();
  if (tmp.startsWith("STOP")) return FineCiclo();

  if (tmp.startsWith("SETCYCLE")) return ImpostaCicloCompleto(tmp);
  if (tmp.startsWith("SETPARAM")) return ImpostaParametro(tmp);
  if (tmp.startsWith("SETPOINT")) return ImpostaSetPoint(tmp);
  if (tmp.startsWith("READ")) return StampaValori(tmp);
  
  return 0;
}



void GestioneComandiSeriale(){
  while (Serial.available())
  {
    bufferCLI[iCnt]=Serial.read();
    if (bufferCLI[iCnt]=='\n')
    {
      // callback
      int ris= CallbackSerial();
      if (ris) Serial.println("OK");
        else  Serial.println("KO");
      // vuoto buffer
      iCnt=0;
    }
    else
      {
        iCnt++;
      }
  }
}

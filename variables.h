/* variabili per gli input e gli output
 e PARAMETRI DI CONFIGURAZIONE STATICI
*/

#define PIN_INPUTPID 0
#define PIN_INPUT_TEMP 1

#define PIN_OUTPUTPID 5

// parametri impostabili da seriale
int TPID;  // millis tra ogni lettura PID
int TCAPTURE;  // millis tra ogni lettura valori input per medie
int TSEND;  // millis tra ogni  output media
int THOLD; // millis di stabilità richiesta 
int THRES; // soglia stabilita -->>> TODO se assoluta o ratio


// timers usati nel loop principale 
static long int iTimePID;
static long int iTimeOutput;
static long int iTimeInput;

// accumulatori letture dei sensori per medie
long int iSommaTemp;  
int iNumTemp;

// enables 
bool bOutputEnable;  // invio output durante ciclo


//elenco, indici e setpoint corrente
int vSetpoints[30];
int iNumSetpoints;
int iIndexSetPoint;
int iCurrentSetPoint;

// FSM principale
int iCurrentState;

// PID
// Kp, Ki, Kd, Kb (Back-calculation coefficient)
AdvancedPID myPID(2.0, 5.0, 1.0, 0.0); 
int lastPIDinput;



#define SERIALBAUD 9600

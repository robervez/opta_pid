# OPTA PID - Control System

## Descrizione
Sistema di controllo PID per gestione di flussometro e acquisizione contestuale di dati da sensori diversi, come ad esempio della temperatura. Il sistema è basato su Arduino OPTA. Il progetto implementa un controllore PID avanzato con acquisizione dati da sensori, gestione via interfaccia seriale (CLI) e una macchina a stati finiti (FSM) per l'automazione di cicli di controllo.

## Funzionalità Principali
- Controllo PID avanzato con protezione dell'attuatore e filtro derivativo
- Acquisizione e media di letture sensori (e.g., temperatura)
- Interfaccia CLI seriale per configurazione e controllo
- Macchina a stati finiti per automazione cicli 
- Monitoraggio stabilità con timeout configurabili

## Comandi CLI

I comandi sono inviati via seriale UART e terminati da newline (`\n`).

| Comando | Parametri | Descrizione | Esempio |
|---------|-----------|-------------|---------|
| `START` | - | Avvia il ciclo di controllo | `START\n` |
| `STOP` | - | Arresta il ciclo | `STOP\n` |
| `SETPOINT` | `<valore>` | Imposta il setpoint corrente | `SETPOINT;512\n` |
| `SETPARAM` | `<parametro>;<valore>` | Configura parametri di sistema | `SETPARAM;TPID;10\n` |
| `SETCYCLE` | `<sp1>;<sp2>;...;<spN>` | Imposta ciclo completo con lista di setpoint | `SETCYCLE;512;768;1024\n` |
| `READ` | `<parametro>` | Legge il valore di un parametro | `READ;SETPOINT\n` |
| `OUTPUT` | `ENABLE\|DISABLE` | Abilita/disabilita output dati | `OUTPUT;ENABLE\n` |

### Parametri configurabili (SETPARAM)
- `TPID` - Intervallo lettura PID [ms] (default: 10)
- `TCAPTURE` - Intervallo lettura sensori per media [ms] (default: 10)
- `TSEND` - Intervallo invio dati medi [ms] (default: 1000)
- `THOLD` - Tempo di stabilità richiesto [ms]
- `THRES` - Soglia stabilità (errore massimo)

### Parametri leggibili (READ)
- `SETPOINT` - Valore setpoint corrente
- `PIDINPUT` - Ultimo valore lettura PID

## Risposta Comandi
- `#OK` - Comando eseguito con successo
- `#KO` - Comando non riconosciuto o parametri errati
- `#<valore>` - Risposta a comando READ

## Dati Output
Quando output è abilitato, il sistema invia pacchetti dati con formato:
A;2;<temperatura_media>;<pidinput>\n



# Struttura del progetto

## opta_pid.ino Main Arduino sketch
Sensor reading function (LettureSensori)
PID setup and management
Main loop timing logic for PID, sensor input, and data output

## CLI.h - Command-line interface 
Serial data packet transmission
Cycle start/stop functions
Parameter setting and configuration functions
Setpoint management
Serial command processing and callbacks

## FSM.h - Finite State Machine

All 8 states (idle, setup, waiting, stable, capturing, reporting, final)
Stability checking logic
State transitions and conditions
Output enable/disable entry actions


## variables.h  - global variables and parameters

Pin definitions for input/output
Configurable timing parameters (TPID, TCAPTURE, TSEND, THOLD)
Sensor accumulators for averaging
PID coefficients and state variables
FSM state management


## utility.h - Utility functions

StringSplit function for parsing serial commands

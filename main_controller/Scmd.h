#ifndef Scmd_h
#define Scmd_h

#include "Arduino.h"
#include "Cmd.h"
char scmd_buffer[100];

void sCmdSetup(Stream *);
void cmdHelp(int, char);
void cmdProgram(int, char);
void cmdFreq(int, char);
void cmdFeederPause(int, char);
void cmdFeederCont(int, char);
void cmdStatus(int, char);
void cmdTopSpin(int, char);
void cmdUnderSpin(int, char);


#endif

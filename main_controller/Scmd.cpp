#include "Scmd.h"
#include "Arduino.h"
#include "Cmd.h"

void sCmdSetup(Stream *str) {
  cmdInit(str);
  cmdAdd("help", cmdHelp);
  cmdAdd("pgm", cmdProgram);
  cmdAdd("freq", cmdFreq);
  cmdAdd("pause", cmdFeederPause);
  cmdAdd("cont", cmdFeederCont);
  cmdAdd("status", cmdStatus);
  cmdAdd("top", cmdTopSpin);
  cmdAdd("under", cmdUnderSpin);
  //cmdAdd("elev", cmdElevation);
  //cmdAdd("azim", cmdAzimute);
}

void cmdHelp(int arg_cnt, char **args)
{
  Serial.println("status");
  Serial.println("pgm 1..9");
  Serial.println("pause");
  Serial.println("cont");
  sprintf(scmd_buffer, "freq %i..%i (bolas por minuto)", BALL_FREQ_MIN, BALL_FREQ_MAX);
  Serial.println(scmd_buffer);
  Serial.println("top 0..100 (%)");
  Serial.println("under 0..100 (%)");

  // futuros
  Serial.println("elev -30..30 (graus)");
  Serial.println("azim -30..30 (graus)");
}

void cmdProgram(int arg_cnt, char **args)
{
  int pgm = 1;
  if (arg_cnt > 1)
  {
    pgm = cmdStr2Num(args[1], 10);
  }
  Serial.print("pgm ");
  Serial.println(pgm);
}

void cmdFreq(int arg_cnt, char **args)
{
  if (arg_cnt > 1)
  {
    ball_freq = max(min(cmdStr2Num(args[1], 10), BALL_FREQ_MAX), BALL_FREQ_MIN); // limita entre BALL_FREQ_MIN e BALL_FREQ_MAX
    ball_interval = 60000 / ball_freq;
  }
  Serial.print("freq ");
  Serial.println(ball_freq);
}

void cmdTopSpin(int arg_cnt, char **args)
{
  if (arg_cnt > 1)
  {
    top.write(max(min(cmdStr2Num(args[1], 10), 100), 0));
  }
  sprintf(buffer, "Top %i%%", top.read());
  Serial.println(buffer);
}

void cmdUnderSpin(int arg_cnt, char **args)
{
  if (arg_cnt > 1)
  {
    under.write(max(min(cmdStr2Num(args[1], 10), 100), 0));
  }
  sprintf(buffer, "Under %i%%", under.read());
  Serial.println(buffer);
}

void cmdFeederPause(int arg_cnt, char **args)
{
  ball_soft_pause = true;
  sound.pause();
}

void cmdFeederCont(int arg_cnt, char **args)
{
  ball_soft_pause = false;
  sound.pause();
}

void cmdStatus(int arg_cnt, char **args)
{
  vccMonitor();
  sprintf(buffer, "run: %i; count %i; freq %i; top %i%%; under %i%%; vcc %i",
          !ball_soft_pause, ball_counter, ball_freq,
          top.read(),
          under.read(),
          vcc_monitor
         );
  Serial.println(buffer);
}

void cmdStatus() {
  cmdStatus(0, 0);
}
